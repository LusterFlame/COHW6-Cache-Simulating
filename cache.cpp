#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]){
    std::ifstream input(argv[1], std::ifstream::in);
    std::ofstream output(argv[2], std::ofstream::trunc);

    output.close();
    input.close();

    return 0;
}