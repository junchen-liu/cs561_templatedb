#include <string>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "templatedb/db.hpp"
#include "templatedb/Value.hpp"

TEST(PersistenceTest, BasicOpenClose)
{
    std::string fname = "test_db1.db";
    std::string dbname = "db1";
    std::string deltablename = dbname + "DelTable";
    templatedb::DB db("./" + dbname);
    Value v1({1, 2});
    db.open(fname);
    db.put(5, v1);
    db.close();

    db.open(fname);
    Value v2 = db.get(5);
    db.close();
    remove(fname.c_str());
    ASSERT_EQ(v1, v2);
}

//TEST(PersistenceTest, DeleteOpenClose)
//{
//    std::string fname = "test_db2.db";
//    templatedb::DB db("./db2");
//    Value v1({1, 2});
//    Value v2({12, 21});
//    db.open(fname);
//    db.put(5, v1);
//    db.put(1, v2);
//    db.del(5);
//    db.close();
//
//
//    db.open(fname);
//    Value v3 = db.get(5);
//    Value v4 = db.get(1);
//    db.close();
//    remove(fname.c_str());
//    remove("./db2");
//    remove("./db2DelTable");
//    ASSERT_EQ(v3, Value(false));
//    ASSERT_EQ(v2, v4);
//}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}