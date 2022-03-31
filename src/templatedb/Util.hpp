#ifndef UTIL_H
#define UTIL_H

#include "Value.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
namespace Util {
    std::map<int, Value> compact(const std::vector<std::map<int, Value> > &inputs) {
        size_t n = inputs.size();
        std::map<int, Value> ret;
        std::vector<size_t> poses(n);
        size_t nonEmpty = 0;
        for (size_t i = 0; i < n; i++) {
            if (!inputs[i].empty())
                nonEmpty++;
        }

        while (nonEmpty) {
        int key = INT_MAX;
        for (size_t i = 0; i < n; ++i)
            if (poses[i] < inputs[i].size()) {
                auto it = inputs[i].begin();
                std::advance(it, poses[i]);
                key = std::min(key, it->first);
            }
        Value value;
        for (size_t i = 0; i < n; ++i) {
            auto it = inputs[i].begin();
            std::advance(it, poses[i]);
            if (poses[i] < inputs[i].size() && it->first == key) {
                value = it->second;
                if (poses[i] == inputs[i].size())
                    --nonEmpty;
            }
        }
        ret.emplace(std::make_pair(key, value));
    }
    return ret;
    }
}

#endif
