//
// Created by Junchen Liu on 3/31/2022.
//

#include "Level.h"
#include "SSTableId.hpp"
#include "Value.hpp"


Level::Level(const std::string &dir): levelNo(dir) {
    table = SSTable(SSTableId(levelNo,-1));
}

SearchResult Level::search(int key) const {
    return table.search(key);
}

std::map<int, Value> Level::extract() {
    return table.load();
}

void Level::merge(std::map<int, Value> &&entries1, int no) {
    entries1.insert(table.load().begin(), table.load().end());
    SSTableId tid = SSTableId(levelNo,no);
    table = SSTable(entries1, tid);
}

void Level::clear() {
    table = SSTable(SSTableId(levelNo,-1););
}

int Level::space() const {
    return table.getSpace();
}

void Level::add(const std::map<int, Value> &map) {
    SSTableId tid = SSTableId(levelNo,-1);
    table = SSTable(map, tid);
}
