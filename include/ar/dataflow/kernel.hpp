#ifndef AR_DATAFLOW_KERNEL_H
#define AR_DATAFLOW_KERNEL_H

#include "ar/dataflow/sink.hpp"
#include "ar/dataflow/source.hpp"
#include "ar/dataflow/notifier.hpp"
#include "ar/dataflow/port.hpp"
#include "ar/dataflow/kernel_events.hpp"
#include "ar/allocators.hpp"

#include "tmc/aw_yield.hpp"
#include "tmc/sync.hpp"
#include "tmc/task.hpp"
#include "tmc/ex_cpu.hpp"

#include <config.hpp>
#include <string>

namespace AsyncRuntime::Dataflow {

    enum KernelProcessResult : int {
        kNEXT  =0,
        kEND   =1,
        kERROR =2
    };

    enum KernelState : int {
        kREADY = 0,
        kINITIALIZED = 1,
        kRUNNING = 2,
        kTERMINATED = 3
    };

    /**
     * @brief
     */
    class KernelContext {
    public:
        KernelContext() = default;
        virtual ~KernelContext() = default;

        void SetErrorCode(int code) { error_code = code; }
        void SetInterruptCallback(const std::function<int(void*)> & callback, void *opaque) {
            interrupt_callback = callback;
            interrupt_callback_opaque = opaque;
        }

        int GetErrorCode() const { return error_code; }

        void SetResource(resource_pool *res) { resource = res; }

        resource_pool *GetResource() { return resource; }
    protected:
        int Interrupt() {
            if (interrupt_callback) {
                return interrupt_callback(interrupt_callback_opaque);
            } else {
                return 0;
            }
        }

      resource_pool *resource = nullptr;

    private:
        std::function<int(void*)> interrupt_callback;
        void *interrupt_callback_opaque = nullptr;
        int error_code = 0;
    };

    /**
     * @brief
     * @tparam KernelContextT
     */
    template< class KernelContextT >
    class Kernel {
        static_assert(std::is_base_of<KernelContext, KernelContextT>::value,
                      "KernelContextT must derive from KernelContext");
    public:
        explicit Kernel(const std::string &name);
        Kernel(resource_pool *resource, const std::string &name);

        virtual ~Kernel();

        tmc::task<int> AsyncInit();

        bool Run(const std::function<void(int)> &terminated_callback);

        tmc::task<int> AsyncTerminate();

        void SetWorkGroup(AsyncRuntime::ObjectID wg) {  }

        void SetEntityTag(AsyncRuntime::EntityTag tag) {  }

        const std::string &GetName() const { return name; }

        const Source &GetSource() const { return source; }

        const Sink &GetSink() const { return sink; }

        Sink &GetSink() { return sink; }

        template<class T>
        std::shared_ptr<SourcePort<T>> GetSourcePort( const std::string & name ) { return source.template At<T>(name); };

        template<class T>
        std::shared_ptr<SinkPort<T>> GetSinkPort( const std::string & name ) { return sink.template At<T>(name); }
#if defined(MEASURE_CPU_TIME)
        size_t GetCpuTime() {
            // TODO:
            // return coroutine->get_cpu_time();
            return 0;
        }
#endif

    protected:
        virtual tmc::task<int> OnInit(KernelContextT *context) = 0;

        virtual tmc::task<KernelProcessResult> OnProcess(KernelContextT *context) = 0;

        virtual tmc::task<KernelProcessResult> OnSinkSubscription(KernelContextT *context);

        virtual tmc::task<KernelProcessResult> OnWaitSinkSubscription(KernelContextT *context);

        virtual tmc::task<KernelProcessResult> OnSinkUnsubscription(KernelContextT *context);

        virtual tmc::task<KernelProcessResult> OnTerminate(KernelContextT *context);

        virtual tmc::task<KernelProcessResult> OnUpdate(KernelContextT *context);

        virtual void OnDispose(KernelContextT *context) { };

        tmc::task<int> AsyncLoop(std::function<void(int)> terminated_callback);
        tmc::task<int> AsyncLoopBody();

        Source source;
        Sink sink;
        Notifier process_notifier;
        resource_pool *resource = nullptr;
    private:
        std::atomic<KernelState> state;
        std::string name;
        std::future<int> loop_future;
        std::unique_ptr<KernelContextT> kernel_context{nullptr};
        //shared_future_t<int> future_res;
        //std::shared_ptr<AsyncRuntime::coroutine<int>> coroutine;
    };

    template<class KernelContextT>
    Kernel<KernelContextT>::Kernel(const std::string &name)
            : source(&process_notifier)
            , sink(&process_notifier)
            , name(name)
            , state{kREADY} {
    }

    template<class KernelContextT>
    Kernel<KernelContextT>::Kernel(resource_pool *res, const std::string &name)
            : source(res, &process_notifier)
            , sink(&process_notifier)
            , name(name)
            , state{kREADY}
            , resource(res) {
    }

    template<class KernelContextT>
    Kernel<KernelContextT>::~Kernel() {
        source.Flush();
        sink.DisconnectAll();
    }

    template<class KernelContextT>
    tmc::task<int> Kernel<KernelContextT>::AsyncLoop(std::function<void(int)> terminated_callback) {
        auto result = co_await AsyncLoopBody();
        if (terminated_callback) {
            terminated_callback(result);
        }
        co_return result;
    }

    template<class KernelContextT>
    tmc::task<int> Kernel<KernelContextT>::AsyncLoopBody() {
        try {
            if (state.load(std::memory_order_relaxed) != kTERMINATED) {
                state.store(kRUNNING, std::memory_order_relaxed);

                auto res = kNEXT;
                while (res == kNEXT) {
                    co_await tmc::reschedule();
                    if (state.load(std::memory_order_relaxed) == kTERMINATED) {
                        break;
                    }
                    res = co_await OnUpdate(kernel_context.get());
                }
            }

            state.store(kTERMINATED, std::memory_order_relaxed);
            OnDispose(kernel_context.get());
        } catch (std::exception & ex) {
            std::cerr << ex.what() << std::endl;
            state.store(kTERMINATED, std::memory_order_relaxed);
            OnDispose(kernel_context.get());
            co_return -1;
        }

        co_return kernel_context->GetErrorCode();
    }

    template<class KernelContextT>
    tmc::task<int> Kernel<KernelContextT>::AsyncInit() {
        try {
            kernel_context = std::make_unique<KernelContextT>();
            kernel_context->SetResource(resource);
            int init_error = co_await OnInit(kernel_context.get());
            if (init_error != 0) {
                OnDispose(kernel_context.get());
                state.store(kTERMINATED, std::memory_order_relaxed);
                co_return init_error;
            } else {
                state.store(kINITIALIZED, std::memory_order_relaxed);
            }
            co_return init_error;
        } catch (...) {
            co_return -1;
        }
    }

    template<class KernelContextT>
    bool Kernel<KernelContextT>::Run(const std::function<void(int)> &terminated_callback) {
        if (state.load(std::memory_order_relaxed) != kINITIALIZED) {
            return false;
        }

        state.store(kRUNNING, std::memory_order_relaxed);
        loop_future = tmc::post_waitable(tmc::cpu_executor(), AsyncLoop(terminated_callback));

        return true;
    }

    template<class KernelContextT>
    tmc::task<int> Kernel<KernelContextT>::AsyncTerminate() {
        if (loop_future.valid()) {
            process_notifier.Notify((int) KernelEvent::kKERNEL_EVENT_TERMINATE);
            state.store(kTERMINATED, std::memory_order_relaxed);
            co_return loop_future.get();
        } else if (state.load(std::memory_order_relaxed) == kINITIALIZED) {
            state.store(kTERMINATED, std::memory_order_relaxed);
            process_notifier.Notify((int) KernelEvent::kKERNEL_EVENT_TERMINATE);
            co_return 0;
        } else {
            state.store(kTERMINATED, std::memory_order_relaxed);
            process_notifier.Notify((int) KernelEvent::kKERNEL_EVENT_TERMINATE);
            co_return 0;
        }
    }

    template<class KernelContextT>
    tmc::task<KernelProcessResult>
    Kernel<KernelContextT>::OnUpdate(KernelContextT *context) {
        KernelProcessResult res = kNEXT;
        if (sink.SubscribersEmpty()) {
            res = co_await OnWaitSinkSubscription(context);
            if (res != kNEXT) {
                co_return res;
            }
        }
        int events = co_await process_notifier.AsyncWatchAny();

        if (Dataflow::Notifier::HasState(events, KernelEvent::kKERNEL_EVENT_READ_SOURCE)) {
            res = co_await OnProcess(context);
            if (res != kNEXT) {
                co_return res;
            }
        }

        if (Dataflow::Notifier::HasState(events, KernelEvent::kKERNEL_EVENT_TERMINATE)) {
            res = co_await OnTerminate(context);
            if (res != kNEXT) {
                co_return res;
            }
        }

        if (Dataflow::Notifier::HasState(events, KernelEvent::kKERNEL_EVENT_SINK_SUBSCRIPTION)) {
            res = co_await OnSinkSubscription(context);
            if (res != kNEXT) {
                co_return res;
            }
        }

        if (Dataflow::Notifier::HasState(events, KernelEvent::kKERNEL_EVENT_SINK_UNSUBSCRIPTION)) {
            res = co_await OnSinkUnsubscription(context);
            if (res != kNEXT) {
                co_return res;
            }
        }

        co_return res;
    }

    template<class KernelContextT>
    tmc::task<KernelProcessResult>
    Kernel<KernelContextT>::OnTerminate(KernelContextT *context) {
        source.Deactivate();
        source.Flush();
        sink.UnsubscribeAll();
        co_return kEND;
    }

    template<class KernelContextT>
    tmc::task<KernelProcessResult>
    Kernel<KernelContextT>::OnSinkSubscription(KernelContextT *context) {
        source.Activate();
        co_return kNEXT;
    }

    template<class KernelContextT>
    tmc::task<KernelProcessResult>
    Kernel<KernelContextT>::OnSinkUnsubscription(KernelContextT *context) {
        KernelProcessResult res = kNEXT;
        if (sink.SubscribersEmpty()) {
            res = co_await OnWaitSinkSubscription(context);
        }

        co_return res;
    }

    template<class KernelContextT>
    tmc::task<KernelProcessResult>
    Kernel<KernelContextT>::OnWaitSinkSubscription(KernelContextT *context) {
        source.Flush();
        source.Deactivate();

        KernelProcessResult res = kNEXT;
        int events = co_await process_notifier.AsyncWatch(KernelEvent::kKERNEL_EVENT_SINK_SUBSCRIPTION, KernelEvent::kKERNEL_EVENT_TERMINATE);
        if (Dataflow::Notifier::HasState(events, KernelEvent::kKERNEL_EVENT_TERMINATE)) {
            res = co_await OnTerminate(context);
            if (res != kNEXT) {
                co_return res;
            }
        }

        if (Dataflow::Notifier::HasState(events, KernelEvent::kKERNEL_EVENT_SINK_SUBSCRIPTION)) {
            res = co_await OnSinkSubscription(context);
            source.Flush();
            if (res != kNEXT) {
                co_return res;
            }
        }

        co_return res;
    }
}

#endif //AR_DATAFLOW_KERNEL_H
