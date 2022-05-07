#include "BloomFilter.h"
#include "murmurhash.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

using namespace std;
using namespace BF;

BloomFilter::BloomFilter(){
	numElement = 1024;
	bitsPerElement = 10;

	numIndex = (int)floor(0.693*bitsPerElement+ 0.5);
	size = numElement * bitsPerElement;
}

BloomFilter::BloomFilter( int numElement_, int bitsPerElement_ ){
	numElement = numElement_;
	bitsPerElement = bitsPerElement_;
	numIndex = (int)floor(0.693*bitsPerElement+ 0.5);
	size = numElement * bitsPerElement;

	makeBloomFilter();
}

void BloomFilter::makeBloomFilter(){
	bf_vec.resize(size, 0);
}

void BloomFilter::program( string key ){
	vector<int> index( numIndex, 0 );
	getIndex( key, &index );

	for(int i=0; i<numIndex; i++){
		bf_vec[index[i]] = 1;
	}
}

bool BloomFilter::query( string key ){
	vector<int> index( numIndex, 0 );
	getIndex( key, &index );

	for(int i=0; i<numIndex; i++){
		if( bf_vec[index[i]] == 0 )
			return false;
	}

	return true; // positive
}

void BloomFilter::getIndex( string key, vector<int>* index ){
	unsigned int h = MurmurHash2( key.c_str(), key.size(), 0xbc9f1d34 );

	const unsigned int delta = (h>>17) | (h<<15); // Rorate right 17 bits
	for( int i=0 ; i<numIndex ; i++ ){
		index->at(i) = h % size;
		h += delta;
	}

	return;
}

int BloomFilter::getIndexNum(){
	return numIndex;
}

int BloomFilter::getSize(){
	return size + 1;
}

void BloomFilter::save(string filename) {
    ofstream file(filename, std::ios::binary);
    file.write((char*) &numElement, sizeof(int));
    file.write((char*) &bitsPerElement, sizeof(int));
    for (const auto &i : bf_vec) {
        if (i){
            file.write((char*) 1, sizeof(int));
        }
        else{
            file.write((char*) 0, sizeof(int));
        }
    }
    file.close();
//    writeVec(bf_vec, filename+"_vec");

}

void BloomFilter::load(string filename) {
    vector<bool> vec;

    std::ifstream rfile(filename, std::ios::binary);
    rfile.read((char*) &numElement, sizeof(int));
    rfile.read((char*) &bitsPerElement, sizeof(int));
    numIndex = (int)floor(0.693*bitsPerElement+ 0.5);
    size = numElement * bitsPerElement;
    for (int i = 0; i < size; ++i) {
        int b;
        rfile.read((char*) &b, sizeof(int));
        if (b == 1){
            vec.push_back(true);

        }
        else{
            vec.push_back(false);
        }
    }
    rfile.close();
    bf_vec = vec;
//    bf_vec = readVec(filename+"_vec");
//    std::string line, item, op_string, numElement_str, bitsPerElement_str;
//    if(std::getline(file, line)){
//        std::stringstream linestream(line);
//        std::getline(linestream, numElement_str, ' ');
//        std::getline(linestream, bitsPerElement_str, ' ');
//        numElement= stoi(numElement_str);
//        bitsPerElement= stoi(bitsPerElement_str);
//        numIndex = (int)floor(0.693*bitsPerElement+ 0.5);
//        size = numElement * bitsPerElement;
//        vec.resize(size, 0);
//    }
//    if(std::getline(file, line)){
//        std::stringstream linestream(line);
//        std::getline(linestream, item);
//
//        for (size_t i = 0; i < item.size(); i++)
//        {
//            if(item[i] == '0'){
//                vec.push_back(false);
//            }else{
//                vec.push_back(true);
//            }
//        }
//    }

}

BloomFilter::BloomFilter(string filename,int numElement_, int bitsPerElement_ ) {
    load(filename);
    //Find if there exist dt file, if so load
    if (std::filesystem::exists(std::filesystem::path(filename))){
        this->load(filename);
    }
    else{
        BloomFilter( numElement_, bitsPerElement_ );
    }

}

