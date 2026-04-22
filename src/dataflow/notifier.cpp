#include "ar/dataflow/notifier.hpp"

using namespace AsyncRuntime::Dataflow;

void Notifier::Notify(int state) {
    std::lock_guard<std::mutex> lock(mutex);
    notify_state |= state;
    if (watch_any || ((watch_state & state) == state) == 1) {
        notifier.notify(notify_state);
    }
}

tmc::task<void> Notifier::co_notify(int state) { // TODO: test in kernel terminate
    bool notify = false;
    int out = 0;
    {
        std::lock_guard<std::mutex> lock(mutex);
        notify_state |= state;
        if (watch_any || ((watch_state & state) == state) == 1) {
            notify = true;
            out = notify_state;
        }
    }

    if (notify) {
        co_await notifier.co_notify_all();
    }

    co_return;
}

void Notifier::CheckNotifications(int &notifications, int state) const {
    if (((notify_state & state) == state) == 1) {
        notifications |= state;
    }
}

tmc::task<int> Notifier::AsyncWatchAny() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        watch_any = true;
        int notifications = notify_state;
        watch_state = 0;
        notify_state = 0;
        if (notifications != 0) {
            co_return notifications;
        }
    }

    co_return co_await notifier.await();
}


