#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>

int StoreSample(const std::string&);

void Usage();

int
main(int argc, char** argv)
{

    if (argc < 2) {
        Usage();
        return 1;
    }
    
    std::string model_path = argv[1];

    StoreSample(model_path); 

}

void
Usage()
{
    std::cout << "Usage: libsc_sample model_folder" << std::endl;
    std::cout << "model_folder\tName of output directory." << std::endl;
}
