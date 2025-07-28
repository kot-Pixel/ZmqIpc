#include "ZSocketIpcUtils.hpp"

int get_cpu_core_count() {
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    return (cores > 0) ? (int)cores : 1;
}