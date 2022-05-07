#ifndef UTIL_H
#define UTIL_H

#include "Value.hpp"
#include "Option.hpp"
#include "SSTable.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Util {
    // Compact several unordered SSTables to ordered SSTables and delete duplicated items.
    inline std::vector<SSTable> compact(const std::vector<SSTable> &inputs, const std::string& dir, uint64_t &no) {
        std::vector<SSTable> ret;
        int size = 0;
        std::vector<std::ifstream> ifs;
        std::vector<std::string> lines;
        std::vector<int> keys;
        std::ofstream file;
        // If there's only one table, no need to compact.
        if (inputs.size() == 1)
            return inputs;
        // Create ifstream for each SSTable, save the first line and its key.
        for (const auto& t : inputs) {
            std::ifstream ifile(t.sstbId.name());
            ifs.emplace_back(std::move(ifile));
            std::string line;
            std::getline(ifile, line);
            std::stringstream linestream(line);
            lines.emplace_back(line);
            int key;
            linestream >> key;
            keys.emplace_back(key);
        }
        // Flag indicates if all tables are looped to the end.
        bool flag = true;
        while (flag) {
            int min_idx = 0;
            // Create a new SSTable if previous is full, or it's start the loop.
            if (size == 0) {
                SSTable t(SSTableId(dir, no++));
                ret.emplace_back(t);
                file.open(t.sstbId.name());
            }
            // Get the smallest key index from all SSTables.
            // From back to front since tables at the end is the newest.
            min_idx = int(std::min_element(keys.rbegin(), keys.rend()) - keys.rend() - 1);
            // Write the line to new file.
            // If minimum is INT_MAX, all files are read to the end.
            if (keys[min_idx] != INT_MAX) {
                file << lines[min_idx];
                ++size;
                // If size reach limit
                if (size == Option::MEM_SPACE) {
                    ret.rbegin()->setSpace(size);
                    size = 0;
                }
            }
            else {
                ret.rbegin()->setSpace(size);
                flag = false;
            }
            // Loop over the lines to drop duplicated keys and update the vectors.
            for (int i = 0; i < keys.size(); ++i) {
                std::string line;
                int key;
                if (keys[i] == keys[min_idx]) {
                    std::getline(ifs[i], line);
                    // If file is not end
                    std::stringstream linestream(line);
                    lines[i] = line;
                    linestream >> key;
                    // Reading the same key indicating reading the same line,
                    // which means it's the last line of the file.
                    if (keys[i] == key)
                        keys[i] = INT_MAX;
                    else
                        keys[i] = key;
                }
            }
        }

        return ret;
    }
}

#endif
