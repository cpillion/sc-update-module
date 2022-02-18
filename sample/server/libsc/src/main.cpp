#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>

int StoreSample(const std::string&, const std::string&);

void Usage();

int
main(int argc, char** argv)
{

    if (argc < 2) {
        Usage();
        return 1;
    }
    
    std::string model_path = argv[1];
    std::string modelname = argv[2];       
    StoreSample(model_path, modelname); 

}

void
Usage()
{
    std::cout << "Usage: libsc_sample model_folder modelname" << std::endl;
    std::cout << "model_folder\tName of output directory." << std::endl;
    std::cout << "modelname\tName of model to author or edit." << std::endl;

}
