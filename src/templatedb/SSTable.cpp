#include "SSTable.hpp"
#include "Util.hpp"
#include "Option.hpp"
#include <fstream>
#include <filesystem>
using std::ofstream;
using std::ifstream;
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 


SSTable::SSTable(const SSTableId &id)
    : sstbId(id){
}

SSTable::SSTable(const std::map<int, Value> &entries, const SSTableId &id)
    : sstbId(id),space(entries.size()){
    save(entries);
}


SearchResult SSTable::search(int key) const {
    std::map<int, Value> entries = load();
    SearchResult searchResult = SearchResult(false);
    if (entries.count(key)){
        searchResult.value = entries[key];
        searchResult.success = true;
    }
    return searchResult;
}

std::map<int, Value> SSTable::load() const {
    std::map<int, Value> entries;
    std::ifstream file(sstbId.name()); 
    boost::archive::text_iarchive ia(file); 
    ia >> entries; 
    space <- entries.size();
    return entries;
}


int SSTable::getSpace() const {
    return space;
}

void SSTable::save(const std::map<int, Value> &entries) {
    std::ofstream file(sstbId.name());

    // save data to archive
    boost::archive::text_oarchive oa(file);
    // write map instance to archive
    oa << entries;
    // archive and stream closed when destructors are called

}