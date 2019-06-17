#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>

class Entry{
private:
    int since;
    bool valid;
    std::string tag;

public:
    Entry(){};

    Entry(int tagSize){
        valid = false;
        since = 0;
    }

    void setTag(std::string binStr){
        valid = true;
        tag = binStr;
        //std::cout << binStr << "\n" << tag << std::endl;
    }

    std::string getTag(){
        return tag;
    }

    bool isValid(){
        return valid;
    }

    int getSince(){
        return since;
    }

    void setSince(int newSince){
        since = newSince;
    }
};