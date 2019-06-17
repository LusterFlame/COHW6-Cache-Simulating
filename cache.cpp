#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <climits>
#include "Entry.h"

#define WORD_LEN 32

int getOldestBlock4(int baseIndex, std::vector<Entry>* cache);
int getOldestBlockAll(std::vector<Entry>* cache);

int main(int argc, char* argv[]){
    std::ifstream input(argv[1], std::ifstream::in);
    std::ofstream output(argv[2], std::ofstream::trunc);

    int cacheSize = 0;  // In KB
    int blockSize = 0;  // In B
    int entryCount = 0; // Total slot for the cache
    int tagSize = 0;    // Size of tag in an entry
    int offsetSize = 0;
    int asso = 0;       // 0 = direct, 1 = 4-way, 2 = fully
    int policy = 0;     // 0 = FIFO, 1 = LRU, 2 = Custom

    input >> cacheSize >> blockSize >> asso >> policy;
    {
        std::string trash;
        getline(input, trash);
    }

    entryCount = (cacheSize * 1024) / blockSize;
    int entryLength = log2(entryCount);
    tagSize = WORD_LEN - log2(blockSize) - entryLength;
    offsetSize = log2(blockSize);

    if(asso == 1){
        entryLength -= 2;
        tagSize += 2;
    }
    if(asso == 2){
        tagSize += entryLength;
        entryLength = 0;
    }

    // Creating the cache vector
    std::vector<Entry> cache(entryCount, tagSize);

    int instIndex = 0;
    std::string inString;
    std::stringstream ss;
    unsigned long long temp;
    unsigned index;
    while(std::getline(input, inString)){
        // Getting new address as binary(32 bit)
        ss.clear();
        ss << std::hex << inString;
        ss >> temp;
        std::bitset<32> btemp(temp);
        inString = btemp.to_string<char, std::string::traits_type, std::string::allocator_type>();
        //std::cout << inString << std::endl;

        // Set cache according to association type
        switch (asso){
            case 0:     // case for direct mapped
                index = std::stoi(inString.substr(tagSize, entryLength), nullptr, 2);
                //std::cout << index << std::endl;
                if(!cache.at(index).isValid()){
                    cache.at(index).setTag(inString.substr(0, tagSize));
                    output << -1 << std::endl;
                }else{
                    if(cache.at(index).getTag() == inString.substr(0, tagSize)){
                        output << -1 << std::endl;
                    }else{
                        output << std::strtoull(cache.at(index).getTag().c_str(), nullptr, 2) << std::endl;
                        cache.at(index).setTag(inString.substr(0, tagSize));
                    }
                }
                break;
            case 1:     // case for 4-way assoc
                index = std::stoi(inString.substr(tagSize, entryLength), nullptr, 2);
                for(int temp1 = 0;temp1 < 4;++temp1){
                    if(temp1 == 0){
                        bool set = false;
                        for(int temp2 = 0;temp2 < 4;++temp2){
                            if(cache.at(index + temp2).getTag() == inString.substr(0, tagSize)){
                                if(policy == 1 || policy == 2){
                                    cache.at(index + temp2).setSince(instIndex);
                                }
                                output << -1 << std::endl;
                                set = true;
                                break;
                            }
                        }
                        if(set) break;
                    }
                    if(!cache.at(index + temp1).isValid()){
                        cache.at(index + temp1).setTag(inString.substr(0, tagSize));
                        cache.at(index + temp1).setSince(instIndex);
                        output << -1 << std::endl;
                        break;
                    }
                    // If all 4 slot is filled, go into this if block
                    if(temp1 == 3){
                        temp1 = getOldestBlock4(index, &cache);
                        output << std::strtoull(cache.at(index + temp1).getTag().c_str(), nullptr, 2) << std::endl;
                        cache.at(index + temp1).setTag(inString.substr(0, tagSize));
                        cache.at(index + temp1).setSince(instIndex);
                        break;
                    }
                }
                break;
            case 2:
                for(int temp1 = 0;temp1 < entryCount;++temp1){
                    if(temp1 == 0){
                        bool set = false;
                        for(int temp2 = 0;temp2 < entryCount;++temp2){
                            if(cache.at(temp2).getTag() == inString.substr(0, tagSize)){
                                if(policy == 1 || policy == 2){
                                    cache.at(temp2).setSince(instIndex);
                                }
                                output << -1 << std::endl;
                                set = true;
                                break;
                            }
                        }
                        if(set) break;
                    }
                    if(!cache.at(temp1).isValid()){
                        cache.at(temp1).setTag(inString.substr(0, tagSize));
                        cache.at(temp1).setSince(instIndex);
                        output << -1 << std::endl;
                        break;
                    }
                    if(temp1 == entryCount - 1){
                        temp1 = getOldestBlockAll(&cache);
                        output << std::strtoul(cache.at(temp1).getTag().c_str(), nullptr, 2) << std::endl;
                        cache.at(temp1).setTag(inString.substr(0, tagSize));
                        cache.at(temp1).setSince(instIndex);
                        break;
                    }
                }
                break;
        }
        ++instIndex;
    }

    // for(int temp1 = 0;temp1 < entryCount;++temp1){
    //     output << cache.at(temp1).getTag() << std::endl;
    // }


    output.close();
    input.close();
    return 0;
}

int getOldestBlock4(int baseIndex, std::vector<Entry>* cache){
    int time = INT_MAX;
    int oldest;
    for(int temp = 0;temp < 4;++temp){
        if(cache->at(baseIndex + temp).getSince() < time){
            oldest = temp;
            time = cache->at(baseIndex + temp).getSince();
        }
    }

    return oldest;
}

int getOldestBlockAll(std::vector<Entry>* cache){
    int time = INT_MAX;
    int oldest;
    for(int temp = 0;temp < cache->size();++temp){
        if(cache->at(temp).getSince() < time){
            oldest = temp;
            time = cache->at(temp).getSince();
        }
    }

    return oldest;
}
