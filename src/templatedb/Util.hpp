#ifndef UTIL_H
#define UTIL_H

#include "Value.hpp"
#include "Option.hpp"
#include "SSTable.hpp"
#include "BloomFilter/BloomFilter.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
typedef std::pair<std::string, int> pi;

namespace Util {

    // Get first integer from string, which is the key.
    inline int getKey(const std::string& s) {
        std::stringstream stream(s);
        std::string _key;
        int key = -1;
        stream >> key;
        return key;
    }

    // Comparison for heap. First compare key, if keys are same, larger index means later insertion time.
    struct comp {
        constexpr bool operator() (pi const& a, pi const& b) const noexcept {
            int keya = getKey(a.first);
            int keyb = getKey(b.first);
            if (keya != keyb)
                return keya > keyb;
            else
                return a.second < b.second;
        }
    };

    // Compact several unordered SSTables to ordered SSTables and delete duplicated items.
    inline std::vector<SSTable> compact(const std::vector<SSTable> &inputs, const std::string& dir,
                                        uint64_t &no, BF::BloomFilter* bf = nullptr) {
        std::priority_queue<pi, std::vector<pi>, comp> heap;
        std::vector<SSTable> ret;
        int size = 0;
        int previous_min = -1;   // Check if there are two same keys from different tables.
        std::vector<std::ifstream*> ifs;
        std::ofstream file;
        // If there's only one table, no need to compact.
        if (inputs.size() == 1)
            return inputs;
        // Create ifstream for each SSTable, save the first line and its key.
        for (int i = 0; i < inputs.size(); ++i) {
            auto* ifile = new std::ifstream(inputs[i].sstbId.name());
            ifs.emplace_back(ifile);
            std::string line;
            std::getline(**ifs.rbegin(), line);
            // First element is key, second is the table it from.
            heap.push(std::make_pair(line, i));
        }

        while (!heap.empty()) {
            // Create new sstable if previous one is full.
            if (size == 0) {
                file.close();
                SSTable t(SSTableId(dir, no++));
                ret.emplace_back(t);
                file.open(t.sstbId.name());
            }
            pi cur = heap.top();
            int key = getKey(cur.first);
            // Write to file
            if (key != previous_min) {
                if (bf != nullptr)
                    bf->program(std::to_string(key));
                file << cur.first << std::endl;
                if (size == 0)
                    ret.rbegin()->min_key = key;
                ++size;
                if (size == Option::MEM_SPACE) {
                    ret.rbegin()->setSpace(size);
                    ret.rbegin()->max_key = key;
                    size = 0;
                }
            }
            previous_min = key;
            heap.pop();
            // Read a new line to heap
            std::string line;
            std::getline(*(ifs[cur.second]), line);
            if (!line.empty())
                heap.push(std::make_pair(line, cur.second));
        }

        ret.rbegin()->setSpace(size);

        return ret;
    }
}

#endif
