#include "SSTable.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <vector>

#include <string>

using std::ofstream;
using std::ifstream;


SSTable::SSTable(const SSTableId &id)
    : SSTable(id, INT_MAX, INT_MAX) {
}

SSTable::SSTable(SSTableId id, int min, int max)
        : sstbId(std::move(id)), min_key(min), max_key(max), space(0) {
}

SSTable::SSTable(const std::map<int, Value> &entries, SSTableId id)
    : sstbId(std::move(id)), space(int(entries.size())), min_key(entries.begin()->first), max_key(entries.rbegin()->first) {
    save(entries);
}


Value SSTable::search(int key) const {
    if (min_key <= key && max_key >= key) {
        std::map<int, Value> entries = load();
        if (entries.count(key)) {
            return entries[key];
        }
    }
    return {false};
}

std::map<int, Value> SSTable::search(int min, int max) const {
    std::map<int, Value> ret;
    if (min_key >= min || max_key <= max) {
        std::map<int, Value> entries = load();
        if (min_key >= min || max_key <= max) {
            auto lower = entries.lower_bound(min);
            auto upper = entries.upper_bound(max);
            for (auto it = lower; it != upper; ++it) {
                ret[it->first] = it->second;
            }
        }
    }
    return ret;
}

std::map<int, Value> SSTable::load() const {
    std::map<int, Value> entries;
    ifstream file(sstbId.name());

    std::string line, item, op_string, key_str, visible_str,timestamp_str;
    while (std::getline(file, line))
    {
        std::vector<int> items = std::vector<int>();

        Value v;

        std::stringstream linestream(line);
        std::getline(linestream, key_str, ' '); // First argument is a key
        std::getline(linestream, visible_str, ' '); // Second argument is if visible
        int key = stoi(key_str);
        int ifVisible = stoi(visible_str);

        if (ifVisible == 0){
            v.visible = false;
        } else{
            v.visible = true;
            std::getline(linestream, timestamp_str, ' '); // Third argument is timestamp
            int64_t t = stol(timestamp_str);
            v.timestamp = t;
            while(std::getline(linestream, item, ' '))
            {
                items.push_back(stoi(item));
            }
            v.items = items;
        }

        entries[key] = v;
        // std::cout << key << std::endl;
    }
    return entries;
}


int SSTable::getSpace() const {
    return space;
}

void SSTable::setSpace(int s) {
    space = s;
}

void SSTable::remove() const {
    std::filesystem::remove(std::filesystem::path(sstbId.name()));
}

uint64_t SSTable::number() const {
    return sstbId.no;
}

void SSTable::save(const std::map<int, Value> &entries) {
    std::ofstream file(sstbId.name());
    min_key = entries.begin()->first;
    max_key = entries.rbegin()->first;
    for (const auto &i : entries) {
        file << i.first << ' ';
        if(i.second.visible){
            file << 1 << ' '; //Visible == True
        } else{
            file << 0 << ' ';
        }
        file << i.second.timestamp << ' ';
        for (const auto &j : i.second.items){
            file << j<< ' ';
        }
        file << std::endl;
    }
    file.close();
}