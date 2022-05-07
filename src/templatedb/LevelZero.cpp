#include "LevelZero.hpp"
#include "Value.hpp"
#include "Util.hpp"
#include "Option.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>


// Check for existing data and create directory if it doesn't exist.
LevelZero::LevelZero(const std::string &dir): dir(dir){
    if (!std::filesystem::exists(std::filesystem::path(dir))) {
        std::filesystem::create_directories(std::filesystem::path(dir));
        size = 0;
        byteCnt = 0;
        save();
    } else {
        std::ifstream ifs(dir + "/index", std::ios::binary);
        ifs.read((char*) &size, sizeof(uint64_t));
        ifs.read((char*) &byteCnt, sizeof(uint64_t));
        for (uint64_t i = 0; i < size; ++i) {
            uint64_t no;
            int min_key, max_key;
            ifs.read((char*) &no, sizeof(uint64_t));
            ifs.read((char*) &min_key, sizeof(int));
            ifs.read((char*) &max_key, sizeof(int));
            ssts.emplace_back(SSTableId(dir, no), min_key, max_key);
        }
        ifs.close();
    }
}

Value LevelZero::search(int key) const {
    for (uint64_t i = 1; i <= size; ++i) {
        Value res = ssts[size - i].search(key);
        if (res.visible)
            return res;
    }
    return {false};
}

std::map<int, Value> LevelZero::search(int min_key, int max_key) const {
    std::map<int, Value> ret_map;
    for (auto it = ssts.rbegin(); it != ssts.rend(); ++it) {
        ret_map.merge(it->search(min_key, max_key));
    }
    return ret_map;
}

// Add memory table to lv0.
void LevelZero::add(const std::map<int, Value> &mem, uint64_t &no) {
    if (Option::LEVELING) { // Leveling
        ssts.emplace_back(mem, SSTableId(dir, no++));
        if (ssts.size() > 1) {
            std::vector<SSTable> newTables = Util::compact(ssts, dir, no, nullptr);
            clear();
            ssts = newTables;
            size = ssts.size();
            byteCnt = 0;
            for (const auto &t: ssts)
                byteCnt += t.getSpace();
            save();
        }
    }
    else {  // Tiering
        ssts.emplace_back(mem, SSTableId(dir, no++));
        ++size;
        byteCnt += mem.size();
    }
    save();
}

std::vector<SSTable> LevelZero::extract(uint64_t &no) {
    return ssts;
}

void LevelZero::clear() {
    while (!ssts.empty()) {
        ssts.back().remove();
        ssts.pop_back();
    }
    size = 0;
    byteCnt = 0;
    save();
}

uint64_t LevelZero::space() const {
    return byteCnt;
}

void LevelZero::save() const {
    std::ofstream ofs(dir + "/index", std::ios::binary);
    ofs.write((char*) &size, sizeof(uint64_t));
    ofs.write((char*) &byteCnt, sizeof(uint64_t));
    for (const SSTable &sst : ssts) {
        uint64_t no = sst.number();
        ofs.write((char*) &no, sizeof(uint64_t));
        ofs.write((char*) &sst.min_key, sizeof(int));
        ofs.write((char*) &sst.max_key, sizeof(int));
    }
    ofs.close();
}