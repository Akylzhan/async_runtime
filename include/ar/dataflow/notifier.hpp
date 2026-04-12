#ifndef AR_DATAFLOW_NOTIFYER_H
#define AR_DATAFLOW_NOTIFYER_H

#include "tmc/detail/concepts_awaitable.hpp"
#include "tmc/detail/waiter_list.hpp"
#include "tmc/task.hpp"

#include <atomic>
#include <coroutine>
#include <mutex>
#include <queue>
#include <iostream>

namespace tmc {
    class notifier;

    class aw_notifier : tmc::detail::AwaitTagNoGroupAsIs {
    friend class notifier;

    std::atomic_int32_t value{0};
    notifier &parent;
    tmc::detail::waiter_list_waiter waiter;

    inline aw_notifier(notifier &parent) noexcept : parent(parent) {}

    inline void set(int v) { value.store(v, std::memory_order_seq_cst); }

    public:
    bool await_ready() noexcept;
    bool await_suspend(std::coroutine_handle<> Outer) noexcept;
    inline int await_resume() const noexcept {
        return value.load(std::memory_order_seq_cst);
    }
    };

    class notifier {
    friend class aw_notifier;
    std::mutex mutex;
    std::queue<aw_notifier *> waiters;

    inline aw_notifier *get_one_waiter() {
        if (waiters.empty()) {
            return nullptr;
        }
        aw_notifier *toWake = waiters.front();
        waiters.pop();
        return toWake;
    }

    public:
    /// On destruction, any awaiters will be resumed.
    inline ~notifier() {
        std::scoped_lock<std::mutex> l{mutex};
        // No need to unlock before resuming here - it's not valid for resumers to
        // access the destroyed mutex anyway.
        while (!waiters.empty()) {
            auto toWake = waiters.front();
            waiters.pop();
            toWake->waiter.resume();
        }
    }

    inline void notify(int v) {
        std::scoped_lock<std::mutex> l{mutex};

        auto *toWake = get_one_waiter();
        if (toWake != nullptr) {
            toWake->set(v);
            toWake->waiter.resume();
        }
    }

    inline aw_notifier await() noexcept { return aw_notifier(*this); }
    };

    inline bool aw_notifier::await_ready() noexcept {
        return value.load(std::memory_order_seq_cst) > 0;
    }

    inline bool aw_notifier::await_suspend(std::coroutine_handle<> Outer) noexcept {
    waiter.continuation = Outer;
    waiter.continuation_executor = tmc::detail::this_thread::executor();
    waiter.continuation_priority = tmc::detail::this_thread::this_task().prio;

    std::scoped_lock<std::mutex> l{parent.mutex};
    if (value.load(std::memory_order_seq_cst) > 0) {
        return false;
    } else {
        parent.waiters.push(this);
        return true;
    }
    }
}

namespace AsyncRuntime::Dataflow {

    class Notifier {
    public:
        Notifier() = default;

        void Notify(int state);

        template<typename... Arguments>
        tmc::task<int> AsyncWatch(Arguments &&... args);
        tmc::task<int> AsyncWatchAny();

        template <typename T>
        static bool HasState(int states, T state) { return ((states & (int)state) == (int)state) == 1; }
    private:
        void CheckNotifications(int & notifications, int state) const;

        std::mutex mutex;
        tmc::notifier notifier;
        bool watch_any = false;
        uint8_t watch_state = {0};
        uint8_t notify_state = {0};
    };

    template<typename... Arguments>
    tmc::task<int> Notifier::AsyncWatch(Arguments &&... args) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            watch_any = false;
            int notifications = 0;
            ((void) CheckNotifications(notifications, (int)std::forward<Arguments>(args)), ...);
            watch_state = 0;
            notify_state = 0;
            ((watch_state |= (int)(std::forward<Arguments>(args))), ...);
            if (notifications != 0) {
                co_return notifications;
            }
        }
        co_return co_await notifier.await();
    }
}

#endif //AR_DATAFLOW_NOTIFYER_H
