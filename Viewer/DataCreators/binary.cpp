#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(int argc,char*argv[]) {
    if(argc<2)
    {
        std::cerr <<"Provide binary file"<<std::endl;
        return 1;
    }
    std::string strInputFile = argv[1];
    std::ifstream binaryFile(strInputFile, std::ios::binary);
    if (!binaryFile.is_open()) {
        std::cerr << "Could not open the file." << std::endl;
        return 1;
    }

    std::vector<unsigned char> bytes(
        (std::istreambuf_iterator<char>(binaryFile)),
        (std::istreambuf_iterator<char>())
    );

    binaryFile.close();
    strInputFile.erase(std::remove_if(strInputFile.begin(), strInputFile.end(), [](char c) {
        return c == '/';
    }), strInputFile.end());

    std::string strOutputFile = strInputFile+".txt";
    std::cout << "Opening "<<strOutputFile<<std::endl;
    std::ofstream textFile(strOutputFile);   

    if (!textFile.is_open()) {
        std::cerr << "Could not open the text file for writing." << std::endl;
        return 1;
    }

    // Write each byte as an integer to the text file.
    for (unsigned char byte : bytes) {
        textFile << static_cast<unsigned int>(byte) << std::endl;
    }

    textFile.close();

    std::cout << "Dataset creation complete." << std::endl;

    return 0;
}
