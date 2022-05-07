#ifndef OPTION_H
#define OPTION_H

#include <cstdint>

//For tuning.
namespace Option {
    // True for Leveling, False for Tiering.
    const bool LEVELING = true;
    // Size limit of memory table and each sstable.
    const uint64_t MEM_SPACE = (uint64_t) 1 * 1024 * 1024;
    // Number of non-zero levels.
    const uint64_t NZ_NUM = 3;
    // Size limit of level 0.
    const uint64_t Z_SPACE = (uint64_t) 4 * 1024 * 1024;
    // Size limit of non-zero levels.
    const uint64_t NZ_SPACES[] = {
        (uint64_t) 16 * 1024 * 1024,
        (uint64_t) 64 * 1024 * 1024,
        UINT64_MAX
    };
    // Name of level directories.
    const char *const Z_NAME = "/L0";
    const char *const NZ_NAMES[] = {"/L1", "/L2", "/L3"};
}

#endif
