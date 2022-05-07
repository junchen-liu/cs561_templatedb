#include "DiskStorage.hpp"
//#include "Util.h"
#include "Option.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

DiskStorage::DiskStorage() : DiskStorage("./diskdata") {}

// Check for existing data and create directory if it doesn't exist.
DiskStorage::DiskStorage(const std::string &dir): dir(dir), level0(dir + Option::Z_NAME) {
    if (std::filesystem::exists(std::filesystem::path(dir + "/meta"))) {
        std::ifstream ifs(dir + "/meta", std::ios::binary);
        ifs.read((char*) &no, sizeof(uint64_t));
        ifs.close();
    } else {
        no = 0;
        save();
    }
    // Create levels.
    for (uint64_t i = 0; i < Option::NZ_NUM; ++i)
        levels.emplace_back(dir + Option::NZ_NAMES[i]);
}

// Push memory table to disk.
void DiskStorage::add(const std::map<int, Value> &mem) {
    level0.add(mem, no);
    // Compact and merge to lv1 if lv0 is full
    if (level0.space() >= Option::Z_SPACE){
        levels[0].merge(level0.extract(no), no);
        level0.clear();
        level0.save();
    }
    // Loop through to merge.
    for (uint64_t i = 0; i + 1 < Option::NZ_NUM; ++i)
        if (levels[i].space() >= Option::NZ_SPACES[i]) {
            levels[i + 1].merge(levels[i].extract(no), no); //append to the next level
            levels[i].clear();
            levels[i].save();
        }
    save();
}

// Single search
Value DiskStorage::search(int key) {
    Value searchResult = level0.search(key);
    if (!searchResult.visible)
        for (uint64_t i = 0; !searchResult.visible && i < Option::NZ_NUM; ++i){
            if(!levels[i].bf.query(to_string(key))){ //Check the bloomfilter of this level
                continue;
            }
            searchResult = levels[i].search(key);
        }

    return searchResult;
}

// Range search
std::map<int, Value> DiskStorage::search(int min_key, int max_key) {
    std::map<int, Value> ret_map;
    ret_map.merge(level0.search(min_key, max_key));

    // Search lower level first
    for (auto lv : levels) {
        std::map<int, Value> v = lv.search(min_key, max_key);
        ret_map.merge(v);
    }
    // Then update with current level newer entries.
    for (auto it = ret_map.cbegin(); it != ret_map.cend();)
        if (!it->second.visible)
            ret_map.erase(it++);
        else
            ++it;
    return ret_map;
}

// Clear the whole database.
void DiskStorage::clear() {
    level0.clear();
    for (uint64_t i = 0; i < Option::NZ_NUM; ++i)
        levels[i].clear();
    no = 0;
    save();
}

// Save metadata.
void DiskStorage::save() const {
    std::ofstream ofs(dir + "/meta", std::ios::binary);
    ofs.write((char*) &no, sizeof(uint64_t));
    ofs.close();
}

void DiskStorage::close() {

    for (auto level : levels)
    {
        level.close();
    }
}
