#ifndef SSTABLE_H
#define SSTABLE_H

#include "Value.hpp"
#include "SSTableId.hpp"
#include <string>
#include <vector>
#include <map>

class SSTable {
public:
    explicit SSTable(const SSTableId &sstbId);
    explicit SSTable(SSTableId sstbId, int min, int max);
    explicit SSTable(const std::map<int, Value> &mem, SSTableId id);
    Value search(int key) const;
    std::map<int, Value> search(int min_key, int max_key) const;
    std::map<int, Value> load() const;
    int getSpace() const;
    void setSpace(int);
    void remove() const;
    uint64_t number() const;
    int min_key;
    int max_key;
    SSTableId sstbId;

private:
    int space;
    void save(const std::map<int, Value> &entries);

};

#endif