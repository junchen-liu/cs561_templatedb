#ifndef SEARCH_RESULT_H
#define SEARCH_RESULT_H

#include <string>
#include "Value.hpp"

struct SearchResult {
    bool success;
    Value value;
    SearchResult(bool success);
};

#endif