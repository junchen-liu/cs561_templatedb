#ifndef SSTABLE_H
#define SSTABLE_H

#include "SearchResult.hpp"
#include "Value.hpp"
#include "SSTableId.hpp"
#include <string>
#include <vector>
#include <map>

class SSTable {
public:
    explicit SSTable(const SSTableId &sstbId);
    explicit SSTable(const std::map<int, Value> &mem, const SSTableId &id);
    SearchResult search(int key) const;
    std::map<int, Value> load() const;
    int getSpace() const;
    void remove() const;
    void number() const;

private:
    SSTableId sstbId;
    int space;
    void save(const std::map<int, Value> &entries);

};

#endif