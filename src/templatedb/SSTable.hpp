#ifndef SSTABLE_H
#define SSTABLE_H

#include "SearchResult.hpp"
#include "Value.hpp"
#include "SSTableId.hpp"
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

class SSTable {
public:
    explicit SSTable(const SSTableId &sstbId);
    explicit SSTable(const std::map<int, Value> &mem, const SSTableId &id);
    SearchResult search(int key) const;
    std::map<int, Value> load() const;
    void remove() const;
    uint64_t number() const;
    uint64_t lower() const;
    uint64_t upper() const;
    uint64_t space() const;
private:
    int sstbId;

};

#endif