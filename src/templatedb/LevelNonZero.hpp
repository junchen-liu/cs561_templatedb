#ifndef LEVEL_NON_ZERO_H
#define LEVEL_NON_ZERO_H

#include "SSTable.hpp"
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include "BloomFilter/BloomFilter.h"

class LevelNonZero {
public:
    explicit LevelNonZero(const std::string &dir);
    Value search(int key) const;
    std::map<int, Value> search(int min_key, int max_key) const;
    std::vector<SSTable> extract(uint64_t &no);
    void merge(std::vector<SSTable> upper, uint64_t &no);
    void clear();
    uint64_t space() const;
    void close();
    void save() const;
    BF::BloomFilter bf = BF::BloomFilter(dir+"_bf", 1024, 10); // number of keys, bits per element
private:
    std::string dir;
    uint64_t size;
    uint64_t byteCnt;
    uint64_t lastKey;
    std::vector<SSTable> ssts;

};

#endif