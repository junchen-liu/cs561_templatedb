#include "Disk.h"

//
// Created by Junchen Liu on 3/31/2022.
//
void Disk::add(const std::map<int, Value> &mem) {
    if(levels.size() == 0){
        std::string dir = std::to_string(levels.size());
        Level level = Level(dir);
        level.add(mem);
        levels.push_back(level);
    }


    //Compaction
    for (int i = 0; i < levels.size() - 1; i++){
        if (levels[i].space() > (i + 1) * 10){
            levels[i + 1].merge(levels[i].extract(), -1);
            levels[i].clear();
        }
    }

    int lastIndex = levels.size() - 1;
    if (levels[lastIndex].space() > (lastIndex + 1) * 10){
        std::string dir = std::to_string(lastIndex);
        Level level = Level(dir);
        level.add(mem);
        levels[lastIndex - 1].clear();
        levels.push_back(level);
    }

}

Value Disk::search(int key) {
    SearchResult searchResult = SearchResult(false);
    for (auto & level : levels) {
        searchResult = level.search(key);
        if (searchResult.success) {
            return searchResult.value;
        }
    }
    return searchResult.value;
}
#include "Disk.h"
