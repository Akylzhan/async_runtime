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
    class aw_co_notify;

    class aw_notifier : tmc::detail::AwaitTagNoGroupAsIs {
        friend class notifier;
        friend class aw_co_notify;

        notifier &parent;
        tmc::detail::waiter_list_waiter waiter;

        inline aw_notifier(notifier &parent) noexcept : parent(parent) {}

    public:
        bool await_ready() noexcept;
        bool await_suspend(std::coroutine_handle<> Outer) noexcept;
        int await_resume() const noexcept;
    };

    class
    [[nodiscard("You must co_await aw_atomic_condvar_co_notify for it to have any effect.")]]
    aw_co_notify : tmc::detail::AwaitTagNoGroupAsIs {
        friend class notifier;
        notifier &parent;

        inline aw_co_notify(notifier& parent) : parent{parent} {}

    public:
        inline bool await_ready() const noexcept {
            return false;
        }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> Outer) noexcept;
        inline void await_resume() const noexcept {}
    };

    class notifier {
    friend class aw_notifier;
    friend class aw_co_notify;

    std::atomic_int32_t value{0};
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
            toWake->waiter.resume();
        }
    }

    inline aw_co_notify co_notify_all() noexcept {
        return aw_co_notify{*this};
    }

    inline aw_notifier await() noexcept { return aw_notifier(*this); }
    };

    inline bool aw_notifier::await_ready() noexcept {
        return parent.value.load(std::memory_order_seq_cst) > 0;
    }

    inline bool aw_notifier::await_suspend(std::coroutine_handle<> Outer) noexcept {
        waiter.continuation = Outer;
        waiter.continuation_executor = tmc::detail::this_thread::executor();
        waiter.continuation_priority = tmc::detail::this_thread::this_task().prio;

        std::scoped_lock<std::mutex> l{parent.mutex};
        if (parent.value.load(std::memory_order_seq_cst) > 0) {
            return false;
        } else {
            parent.waiters.push(this);
            return true;
        }
    }

    inline int aw_notifier::await_resume() const noexcept {
        return parent.value.load(std::memory_order_seq_cst);
    }

    inline std::coroutine_handle<> aw_co_notify::await_suspend(std::coroutine_handle<> Outer) noexcept {
        std::lock_guard lock{parent.mutex};
        if (parent.waiters.size() == 0) {
            return Outer;
        }

        auto *front = parent.waiters.front();
        parent.waiters.pop();

        while (!parent.waiters.empty()) {
            auto *toWake = parent.waiters.front();
            parent.waiters.pop();
            toWake->waiter.resume();
        }

        return front->waiter.try_symmetric_transfer(Outer);
    }
}

namespace AsyncRuntime::Dataflow {

    class Notifier {
    public:
        Notifier() = default;

        void Notify(int state);

        tmc::task<void> co_notify(int state);

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
