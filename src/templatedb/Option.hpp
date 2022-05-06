#ifndef OPTION_H
#define OPTION_H

#include <cstdint>

namespace Option {
    const bool LEVELING = true;
    const uint64_t MEM_SPACE = (uint64_t) 2 * 1000;
    const uint64_t NZ_NUM = 3;
    const uint64_t Z_SPACE = (uint64_t) 8 * 1000;
    const uint64_t NZ_SPACES[] = {
        (uint64_t) 16 * 1000,
        (uint64_t) 64 * 1000,
        UINT64_MAX
    };
    const char *const Z_NAME = "/L0";
    const char *const NZ_NAMES[] = {"/L1", "/L2", "/L3"};
}

#endif
