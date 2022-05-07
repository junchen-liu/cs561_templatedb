#ifndef _TEMPLATEDB_DB_H_
#define _TEMPLATEDB_DB_H_

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "DiskStorage.hpp"
#include "operation.hpp"
#include "Value.hpp"
#include "DeleteTable.h"
#include "BloomFilter/BloomFilter.h"


using namespace BF;
namespace templatedb
{

typedef enum _status_code
{
    OPEN = 0,
    CLOSED = 1,
    ERROR_OPEN = 100,
} db_status;

class DB
{
public:
    db_status status;

    DB() : DB("./diskdata") {}
    DB(std::string dir) : disk(dir), deleteTable(dir.erase(0, 2) + "DelTable") {
        this->dir=dir;
    }
    ~DB() {close();};

    Value get(int key);
    void put(int key, Value val);
    std::vector<Value> scan();

    std::vector<Value> scan(int min_key, int max_key);
    void del(int key);
    void del(int min_key, int max_key);
    size_t size();

    db_status open(std::string & fname);
    bool close();

    bool load_data_file(std::string & fname);

    std::vector<Value> execute_op(Operation op);




private:
    std::fstream file;
    std::string dir;
    std::map<int, Value> table;
    size_t value_dimensions = 0;
    DiskStorage disk;
    int min_key;
    int max_key;
    bool write_to_file();
    DeleteTable deleteTable;
    BF::BloomFilter bf = BF::BloomFilter(dir+"_bf", 1024, 10); //bloomfilter for level0 and the Memtable

};

}   // namespace templatedb

#endif /* _TEMPLATEDB_DB_H_ */