#ifndef VALUE_H
#define VALUE_H

#include <vector>
#include "TimeUtil.h"
class Value {
public:
    std::vector<int> items;
    bool visible = true;
//    unsigned long int timestamp = time(nullptr);
    int64_t timestamp = TimeUtil::getCurrentLocalTimeStamp();

    Value() = default;
    Value(bool _visible): visible(_visible) {}
    Value(std::vector<int> _items): items(_items) {}

    bool operator ==(Value const & other) const
    {
        return (visible == other.visible) && (items == other.items);
    }

};

#endif