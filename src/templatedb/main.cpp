#include "db.hpp"
#include "Value.hpp"
using namespace std;

int main (int argc, char *argv[])
{
    templatedb::DB db;
    Value v1 = Value({1,2,3});
    Value v2 = Value({4,5,6});
    db.put(1, v1);
    db.put(2, v1);
    db.put(3, v1);
    db.put(4, v1);
    db.del(4);
    db.put(8, v1);
    db.put(7, v1);
    db.put(6, v1);
    db.put(5, v1);
    db.put(9, v1);

    auto val = db.get(4);
    if (val.visible)
        cout << val.items[1] << endl;
    else
        cout << "Not found" << endl;
}