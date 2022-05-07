#include "SSTableId.hpp"

SSTableId::SSTableId(const std::string &dir, uint64_t no) : dir(dir), no(no) {}

std::string SSTableId::name() const {
    return dir + '/' + std::to_string(no);
}