#ifndef AR_CHANNEL_H
#define AR_CHANNEL_H


#include "ar/task.hpp"
#include "ar/object.hpp"
#include "ar/array.hpp"
#include "ar/work_steal_queue.hpp"

#include <iostream>
#include <map>
#include <cstdint>
#include <cstring>
#include <atomic>

#ifdef USE_TESTS
class CHANNEL_TEST_FRIEND;
#endif


namespace AsyncRuntime {

    template<typename T>
    class Channel;

    /**
     * @brief
     * @class Channel
     */
    template<typename T>
    class Watcher : public BaseObject {
        friend Channel<T>;
    public:
        Watcher() = default;
        ~Watcher() override;

        std::optional<T> Receive();
        std::optional<T> TryReceive();
        future_t<void>   AsyncWait();
    private:
        void Send(const T& msg);

        WorkStealQueue<T>                               queue;
        std::condition_variable                         cv;
        std::mutex                                      mutex;
        promise_t<void>                                 promise;
    };


    template<typename T>
    class Channel {
        static_assert(std::is_copy_constructible_v<T>);
#ifdef USE_TESTS
        friend CHANNEL_TEST_FRIEND;
#endif
    public:
        typedef Watcher<T>  WatcherType;

        void Send(const T& msg);

        std::shared_ptr<WatcherType> Watch();

        void UnWatch(const std::shared_ptr<WatcherType>& watcher);
    private:
        std::mutex                                             mutex;
        std::map<ObjectID, std::shared_ptr<WatcherType>>       watchers;
    };


    template<typename T>
    std::shared_ptr<Watcher<T>> Channel<T>::Watch() {
        std::lock_guard<std::mutex> lock(mutex);
        auto watcher = std::make_shared<Watcher<T>>();
        watchers.insert(std::make_pair(watcher->GetID(), watcher));
        return watcher;
    }


    template<typename T>
    void Channel<T>::UnWatch(const std::shared_ptr<WatcherType> &watcher) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = watchers.find(watcher->GetID());
        if(it != watchers.end())
            watchers.erase(it);
    }


    template<typename T>
    void Channel<T>::Send(const T& msg) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto it = watchers.begin(); it != watchers.end(); ) {
            const std::shared_ptr<WatcherType>& watcher = it->second;

            if (watcher.use_count() > 1) {
                T new_msg(msg);
                watcher->Send(new_msg);
                ++it;
            } else {
                it = watchers.erase(it);
            }
        }
    }


    template<typename T>
    Watcher<T>::~Watcher() {
        while (!queue.empty()) {
            auto v = queue.steal();
            if(v) {
                delete v.value();
            }
        }
    }


    template<typename T>
    void Watcher<T>::Send(const T& msg) {
        promise.set_value();
        queue.push(msg);
        cv.notify_one();
    }


    template<typename T>
    std::optional<T> Watcher<T>::Receive() {
        std::unique_lock<std::mutex>  lock(mutex);
        while(queue.empty()) {
            cv.wait(lock);
        }
        return queue.steal();
    }


    template<typename T>
    std::optional<T> Watcher<T>::TryReceive() {
        return queue.steal();
    }


    template<typename T>
    future_t<void> Watcher<T>::AsyncWait() {
        auto v = queue.steal();
        if(!v) {
            promise = {};
            return promise.get_future();
        } else {
            return make_resolved_future();
        }
    }
}

#endif //AR_CHANNEL_H
