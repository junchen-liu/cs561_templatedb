#include "LevelNonZero.hpp"
#include "Util.hpp"
#include "SSTableId.hpp"
#include "Value.hpp"
#include <filesystem>
#include <fstream>


LevelNonZero::LevelNonZero(const std::string &dir): dir(dir) {
    if (!std::__fs::filesystem::exists(std::__fs::filesystem::path(dir))) {
        std::__fs::filesystem::create_directories(std::__fs::filesystem::path(dir));
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
            ifs.read((char*) &no, sizeof(uint64_t));
            ssts.emplace_back(SSTableId(dir, no));
        }
        ifs.close();
    }
}

SearchResult LevelNonZero::search(uint64_t key) const {
    for (const SSTable &sst : ssts) {
        SearchResult res = sst.search(key);
        if (res.success)
            return res;
    }
    return false;
}

std::map<int, Value> LevelNonZero::extract() {
    auto itr = ssts.begin();
    while (itr != ssts.end() && itr->load().rbegin()->first <= lastKey)
        ++itr;
    if (itr == ssts.end())
        itr = ssts.begin();
    byteCnt -= itr->getSpace();
    lastKey = itr->load().rbegin()->first;
    std::map<int, Value> ret = itr->load();
    itr->remove();
    ssts.erase(itr);
    --size;
    save();
    return ret;
}

void LevelNonZero::merge(std::map<int, Value> &&entries1, uint64_t &no) {
    uint64_t lo = entries1.begin()->first;
    uint64_t hi = entries1.rbegin()->first;
    std::map<int, Value> entries0;
    auto itr = ssts.begin();
    while (itr != ssts.end() && itr->load().rbegin()->first < lo)
        ++itr;
    while (itr != ssts.end() && itr->load().begin()->first <= hi) {
        std::map<int, Value> loadmap = itr->load();
        entries0.insert(loadmap.begin(), loadmap.end());
        byteCnt -= itr->getSpace();
        itr->remove();
        itr = ssts.erase(itr);
        --size;
    }
    std::vector<std::map<int, Value>> v;
    v.emplace_back(entries0);
    v.emplace_back(entries1);
    std::map<int, Value> entries = Util::compact(v);
    size_t n = entries.size();
    size_t pos = 0;
    while (pos < n) {
        byteCnt += ssts.emplace(itr, entries, SSTableId(dir, no++))->getSpace();
        ++size;
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
    }
    ofs.close();
}