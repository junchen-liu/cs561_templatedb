#ifndef VALUE_H
#define VALUE_H

#include <vector>
#include <boost/serialization/vector.hpp>

class Value
{
public:
    std::vector<int> items;
    bool visible = true;

    Value() {}
    Value(bool _visible);
    Value(std::vector<int> _items);

    bool operator ==(Value const & other) const
    {
        return (visible == other.visible) && (items == other.items);
    }
};

#endif