#include "db.hpp"

#include <utility>
#include "Option.hpp"

using namespace templatedb;

// Get one value from db.
Value DB::get(int key)
{
    Value v;
    // Check bloomfiler for memtbale and level0;
    if (!bf.query(to_string(key))) {
        v.visible = false;
        return v;
    }

    // If key is in memory table.
    if (table.count(key))
        v = table[key];
    else
        v = disk.search(key);
    int64_t t = deleteTable.getTimeInt(key);
    // Compare to check if is deleted using range del.
    if (t > v.timestamp) {
        v.visible = false;
    }
	return v;
}

// Insert a value to db.
void DB::put(int key, Value val)
{
    // Put key in bloomfilter.
    bf.program(to_string(key));

    min_key = std::min(key, min_key);
    max_key = std::max(key, max_key);
    table[key] = std::move(val);
    // Merge memory table to level 0 if memory table is full. (Further merge in disk.add() function)
    if (table.size() >= Option::MEM_SPACE) {
		disk.add(table);
		table.clear();
	}

}


// Scan every entry in the db.
std::vector<Value> DB::scan()
{
    return scan(min_key, max_key);
}


// Scan for value in a range of keys.
std::vector<Value> DB::scan(int min_key, int max_key)
{
    std::map<int, Value> return_map;
    std::vector<Value> return_vector;

    // Search memory table.
    for (auto pair: table)
    {
        if ((pair.first >= min_key) && (pair.first <= max_key))
            return_map[pair.first] = pair.second;
    }
    // Search disk.
    return_map.merge(disk.search(min_key, max_key));
    // Search delete table to remove deleted keys.
    for (auto & it : return_map) {
        unsigned long int t = deleteTable.getTimeInt(it.first);
        if (t <= it.second.timestamp && it.second.visible){
            return_vector.push_back(it.second);
        }
    }
    return return_vector;
}

// Put a tombstone for single key deletion.
void DB::del(int key)
{
    if (table.count(key))
        table.erase(key);
    else {
        this->put(key,Value(false));
    }
    
}

// Insert to delete table for range deletion.
void DB::del(int min_key, int max_key)
{
    deleteTable.del(min_key,max_key);
}

// Return size of memory table.
size_t DB::size()
{
    return table.size();
}

/* --------------------
 * No change under
 * -------------------- */
std::vector<Value> DB::execute_op(Operation op)
{
    std::vector<Value> results;
    if (op.type == GET)
    {
        results.push_back(this->get(op.key));
    }
    else if (op.type == PUT)
    {
        this->put(op.key, Value(op.args));
    }
    else if (op.type == SCAN)
    {
        results = this->scan(op.key, op.args[0]);
    }
    else if (op.type == DELETE)
    {
        if ( op.args.size()>0 ){
            this->del(op.key, op.args[0]);
        }
        else
            this->del(op.key);
    }

    return results;
}


bool DB::load_data_file(std::string & fname)
{
    std::ifstream fid(fname);
    if (fid.is_open())
    {
        int key;
        int line_num = 0;
        std::string line;
        std::getline(fid, line); // First line is rows, col
        while (std::getline(fid, line))
        {
            line_num++;
            std::stringstream linestream(line);
            std::string item;

            std::getline(linestream, item, ' ');
            std::string op_code = item;

            std::getline(linestream, item, ' ');
            key = stoi(item);
            std::vector<int> items;
            while(std::getline(linestream, item, ' '))
            {
                items.push_back(stoi(item));
            }
            this->put(key, Value(items));
        }
    }
    else
    {
        fprintf(stderr, "Unable to read %s\n", fname.c_str());
        return false;
    }

    return true;
}


db_status DB::open(std::string & fname)
{
    this->file.open(fname, std::ios::in | std::ios::out);
    if (file.is_open())
    {
        this->status = OPEN;
        // New file implies empty file
        if (file.peek() == std::ifstream::traits_type::eof())
            return this->status;

        int key;
        bool visible;
        int64_t timestamp;
        std::string line;
        std::getline(file, line); // First line is rows, col
        while (std::getline(file, line))
        {
            std::stringstream linestream(line);
            std::string item, _key, _visible, _timestamp;

            std::getline(linestream, _key, ',');
            std::getline(linestream, _visible, ',');
            std::getline(linestream, _timestamp, ',');
            key = stoi(_key);
            visible = stoi(_visible);
            timestamp = stoll(_timestamp);

            std::vector<int> items;
            Value v;
            if (visible) {
                while (std::getline(linestream, item, ',')) {
                    items.push_back(stoi(item));
                }
                v = Value(items);
                v.visible = visible;
                v.timestamp = timestamp;
            }
            else
                v = Value(false);
            this->put(key, v);
            if (value_dimensions == 0)
                value_dimensions = items.size();
        }
    }
    else if (!file) // File does not exist
    {
        this->file.open(fname, std::ios::out);
        this->status = OPEN;
    }
    else
    {
        file.close();
        this->status = ERROR_OPEN;
    }

    return this->status; 
}


bool DB::close()
{
    if (file.is_open())
    {
        this->write_to_file();
        file.close();
    }
    deleteTable.close();
    bf.save(dir+"_bf"); //save the bf for level0 and memory
    disk.close();//save the bloom filters for other levels
    this->status = CLOSED;

    return true;
}


bool DB::write_to_file()
{
    file.clear();
    file.seekg(0, std::ios::beg);

    std::string header = std::to_string(table.size()) + ',' + std::to_string(value_dimensions) + '\n';
    file << header;
    for(auto item: table)
    {
        std::ostringstream line;
        if (item.second.visible) {
            std::copy(item.second.items.begin(), item.second.items.end() - 1, std::ostream_iterator<int>(line, ","));
            line << item.second.items.back();
        }
        std::string value(line.str());
        file << item.first << ',' << item.second.visible << ',' << item.second.timestamp << ',' << value << '\n';
    }

    return true;
}
