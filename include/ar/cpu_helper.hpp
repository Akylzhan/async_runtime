#ifndef AR_CPU_HELPER_HPP
#define AR_CPU_HELPER_HPP

#include "ar/object.hpp"
#include <thread>

namespace AsyncRuntime {

    struct CPU {
        size_t      id;
        size_t      numa_node_id;
    };

    struct NumaNode {
        size_t              id;
        std::vector<CPU>    cpus = {};
    };

    int SetAffinity(std::thread & thread, const AsyncRuntime::CPU &affinity_cpu);

    std::vector<CPU> GetCPUs();
    std::vector<NumaNode> GetNumaNodes();
}

#endif //AR_CPU_HELPER_HPP
