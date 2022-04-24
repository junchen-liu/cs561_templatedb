#include "SSTable.hpp"
#include <fstream>
#include <filesystem>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using std::ofstream;
using std::ifstream;


SSTable::SSTable(const SSTableId &id)
    : sstbId(id){
}

SSTable::SSTable(const std::map<int, Value> &entries, const SSTableId &id)
    : sstbId(id),space(entries.size()){
    save(entries);
}


Value SSTable::search(int key) const {
    std::map<int, Value> entries = load();
    if (entries.count(key)) {
        return entries[key];
    }
    return Value(false);
}

std::map<int, Value> SSTable::load() const {
    std::map<int, Value> entries;
    ifstream file(sstbId.name());

    std::string line, item, op_string, key_str;
    while (std::getline(file, line))
    {
        std::vector<int> items = std::vector<int>();

        std::stringstream linestream(line);
        std::getline(linestream, key_str, ' '); // First argument is a key
        int key = stoi(key_str);
        while(std::getline(linestream, item, ' '))
        {
            items.push_back(stoi(item));
        }
        Value v = Value(items);
        entries[key] = v;
        // std::cout << key << std::endl;
    }
    return entries;
}


int SSTable::getSpace() const {
    return space;
}

void SSTable::remove() const {
    std::__fs::filesystem::remove(std::__fs::filesystem::path(sstbId.name()));
}

uint64_t SSTable::number() const {
    return sstbId.no;
}

void SSTable::save(const std::map<int, Value> &entries) {
    std::ofstream file(sstbId.name());
    for (const auto &i : entries) {
        file << i.first << ' ';
        for (const auto &j : i.second.items){
            file << j<< ' ';
        }
        file << std::endl;
    }
}