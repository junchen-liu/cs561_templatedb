#include <iostream>
#include <chrono>

#include "BloomFilter/BloomFilter.h"

using namespace BF;

void queryTest( BloomFilter *bf,  string key ){
	bool result = bf->query( key );
	string result_str = (result)? "positive" : "negative";
	cout << "Bloom Filter query result of " << key  << " is " << result_str << endl;
}

int main(int argc, char * argv[])
{
    BloomFilter bf( 1024, 20 ); // number of keys, bits per element
//
    // Enpty filter is not match, at this level
    queryTest(&bf, "hello");
    queryTest(&bf, "world");

    // insert two string
    bf.program("hello");
    bf.program("world");

    queryTest(&bf, "hello");
    queryTest(&bf, "world");
    queryTest(&bf, "x");
    queryTest(&bf, "foo");

    bf.save("test-bf");

    BloomFilter bf2( "test-bf",1024, 20 );
    queryTest(&bf2, "hello");
    queryTest(&bf2, "world");
    queryTest(&bf2, "x");
    queryTest(&bf2, "foo");
    return 0;
;
}
