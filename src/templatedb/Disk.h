//
// Created by Junchen Liu on 3/31/2022.
//

#ifndef TEMPLATEDB_DISK_H
#define TEMPLATEDB_DISK_H


#include <map>
#include "Value.hpp"
#include "Level.h"

class Disk {
public:
    explicit Disk();
    void add(const std::map<int, Value> &mem);
    Value search(int key);
private:
    std::vector<Level> levels;
};


#endif //TEMPLATEDB_DISK_H
