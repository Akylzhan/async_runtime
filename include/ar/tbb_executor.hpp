#ifndef AR_TBB_EXECUTOR_H
#define AR_TBB_EXECUTOR_H

#include "ar/executor.hpp"
#include "ar/metricer.hpp"
#include "ar/cpu_helper.hpp"
#include <oneapi/tbb.h>
#include "tbb/tbb_delayed_scheduler.h"
#include "tbb/tbb_stream.h"

namespace AsyncRuntime {

    class TBBExecutor : public IExecutor {
    public:
        TBBExecutor(const std::string &name_,
                    int numa_node,
                    const std::vector<AsyncRuntime::CPU> &cpus,
                    std::vector<WorkGroupOption> work_groups_option = {});

        ~TBBExecutor() noexcept override;


        TBBExecutor(const TBBExecutor &) = delete;

        TBBExecutor(TBBExecutor &&) = delete;

        TBBExecutor &operator=(const TBBExecutor &) = delete;

        TBBExecutor &operator=(TBBExecutor &&) = delete;

        uint16_t AddEntity(void *ptr) final;

        void DeleteEntity(uint16_t id) final;

        void SetIndex(int index) final;

        /**
         * @brief
         * @param task
         */
        void Post(Task *task) final;

        /**
         * @brief
         * @return
         */
        std::vector<std::thread::id> GetThreadIds();
    private:
        void PostToStream(Task *task, EntityTag tag);
        
        TBBDelayedScheduler main_delayed_scheduler;
        std::vector<std::unique_ptr<TBBDelayedScheduler>> delayed_schedulers;
        oneapi::tbb::concurrent_queue<TBBStream*> free_streams;
        TBBStream streams[MAX_ENTITIES];
        TBBStream main_stream;
        tbb::task_arena task_arena;
    };
}

#endif //AR_TBB_EXECUTOR_H