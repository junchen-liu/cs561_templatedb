//
// Created by Junchen Liu on 3/31/2022.
//

#ifndef TEMPLATEDB_LEVEL_H
#define TEMPLATEDB_LEVEL_H


#include <string>
#include <map>
#include "Value.hpp"
#include "SearchResult.hpp"
#include "SSTable.hpp"

class Level {
public:
    explicit Level(const std::string &dir);
    SearchResult search(int key) const;
    std::map<int, Value> extract();
    void clear();
    int space() const;

    void merge(std::map<int, Value> &&entries1, int no);

    void add(const std::map<int, Value> &map);

private:
    std::string levelNo;
    SSTable table = SSTable(SSTableId());
};


#endif //TEMPLATEDB_LEVEL_H
