#include "db.hpp"
#include "Value.hpp"
using namespace std;

int main (int argc, char *argv[])
{
    templatedb::DB db;
    Value v1 = Value({1,2,3});
    Value v2 = Value({1,2});
    Value v3 = Value({1,2,3});
    db.put(1, v1);
    db.put(2, v2);
    db.put(3, v3);
    cout << db.get(4).visible << endl;
}