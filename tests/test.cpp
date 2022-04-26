//
// Created by Light on 4/25/22.
//
#include "templatedb/db.hpp"
#include "templatedb/Value.hpp"

#include <chrono>
#include <thread>

using namespace std;

int main() {
    templatedb::DB db;
    for (int i = 0; i < 100; ++i) {
        db.put(i+1, Value({i, i+1, i+2}));
    }
    this_thread::sleep_for(std::chrono::milliseconds(1000));
}