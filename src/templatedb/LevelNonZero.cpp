#include "LevelNonZero.hpp"
#include "Util.hpp"
#include "SSTableId.hpp"
#include "Value.hpp"
#include "Option.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

// Check for existing data and create directory if it doesn't exist.
LevelNonZero::LevelNonZero(const std::string &dir): dir(dir), bf(dir+"_bf", 1024, 10) {
    if (!std::filesystem::exists(std::filesystem::path(dir))) {
        std::filesystem::create_directories(std::filesystem::path(dir));
        size = 0;
        byteCnt = 0;
        lastKey = 0;
        save();
    } 
    else {
        std::ifstream ifs(dir + "/index", std::ios::binary);
        ifs.read((char*) &size, sizeof(uint64_t));
        ifs.read((char*) &byteCnt, sizeof(uint64_t));
        ifs.read((char*) &lastKey, sizeof(uint64_t));
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

Value LevelNonZero::search(int key) const {
    for (uint64_t i = 1; i <= size; ++i) {
        Value res = ssts[size - i].search(key);
        if (res.visible)
            return res;
    }
    return {false};
}

std::map<int, Value> LevelNonZero::search(int min_key, int max_key) const {
    std::map<int, Value> ret_map;
    for (auto it = ssts.rbegin(); it != ssts.rend(); ++it) {
        ret_map.merge(it->search(min_key, max_key));
    }
    return ret_map;
}

std::vector<SSTable> LevelNonZero::extract(uint64_t &no) {
    return ssts;
}

// Merge higher level to this level.
void LevelNonZero::merge(std::vector<SSTable> upper, uint64_t &no) {
    if (Option::LEVELING) { // Leveling
        ssts.insert(std::end(ssts), std::begin(upper), std::end(upper));
        if (ssts.size() > 1) {
            std::vector<SSTable> newTables = Util::compact(ssts, dir, no, &bf);
            clear();
            ssts = newTables;
            size = ssts.size();
            byteCnt = 0;
            for (const auto &t: ssts)
                byteCnt += t.getSpace();
        }
    }
    else {  // Tiering
        upper = Util::compact(upper, dir, no, &bf);
        ssts.insert(std::end(ssts), std::begin(upper), std::end(upper));
        size = ssts.size();
        byteCnt = 0;
        for (auto t : ssts)
            byteCnt += t.getSpace();
    }
    save();
}

void LevelNonZero::clear() {
    while (!ssts.empty()) {
        ssts.back().remove();
        ssts.pop_back();
    }
    size = 0;
    byteCnt = 0;
    lastKey = 0;
    save();
}

uint64_t LevelNonZero::space() const {
    return byteCnt;
}

void LevelNonZero::save() const {
    std::ofstream ofs(dir + "/index", std::ios::binary);
    ofs.write((char*) &size, sizeof(uint64_t));
    ofs.write((char*) &byteCnt, sizeof(uint64_t));
    ofs.write((char*) &lastKey, sizeof(uint64_t));
    for (const SSTable &sst : ssts) {
        uint64_t no = sst.number();
        ofs.write((char*) &no, sizeof(uint64_t));
        ofs.write((char*) &sst.min_key, sizeof(int));
        ofs.write((char*) &sst.max_key, sizeof(int));
    }
    ofs.close();
}

void LevelNonZero::close() {
    bf.save(dir+"_bf");
}
