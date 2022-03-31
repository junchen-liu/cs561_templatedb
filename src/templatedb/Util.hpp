#ifndef UTIL_H
#define UTIL_H

#include "Value.hpp"
#include <vector>
#include <map>
namespace Util {
    std::map<int, Value> compact(const std::vector<std::map<int, Value> > &inputs);
}

#endif
