#ifndef LEVEL_ZERO_H
#define LEVEL_ZERO_H
#include "Value.hpp"
#include "SSTable.hpp"
#include <string>
#include <cstdint>
#include <vector>

class LevelZero {
public:
    explicit LevelZero(const std::string &dir);
    Value search(int key) const;
    std::map<int, Value> search(int min_key, int max_key) const;
    void add(const std::map<int, Value> &mem, uint64_t &no);
    std::vector<SSTable> extract(uint64_t &no);
    void clear();
    uint64_t space() const;
    void save() const;
private:
    std::string dir;
    uint64_t size;
    uint64_t byteCnt;
    std::vector<SSTable> ssts;

};

#endif