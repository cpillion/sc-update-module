#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>

int StoreSample(const std::string&, const std::string&, const std::string&);

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
    std::string json_update = "{\"attributes\":[{\"nodeId\":67,\"Material\":\"Inconel\"},{\"nodeId\":28,\"Material\":\"Steel\"},{\"nodeId\":59,\"Material\":\"Wood\"},{\"nodeId\":95,\"Manufacture Date\":\"10/22/2021\"}],\"defaultCamera\":{\"_position\":{\"x\":81.22082242242087,\"y\":-99.85364263567925,\"z\":-14.745490335642312},\"_target\":{\"x\":42.01403360616819,\"y\":28.500000953674316,\"z\":-45.15500047683717},\"_up\":{\"x\":0.014413796198195017,\"y\":0.23468066183916042,\"z\":0.9719656523961587},\"_width\":137.61020125980392,\"_height\":137.61020125980392,\"_projection\":0,\"_nearLimit\":0.01,\"_cameraFlags\":0},\"colors\":[{\"nodeIds\":[8,9,10,11],\"color\":{\"r\":255,\"g\":0,\"b\":0}}],\"colors\":[{\"nodeId\":8,\"color\":{\"r\":0,\"g\":255,\"b\":0}},{\"nodeId\":9,\"color\":{\"r\":0,\"g\":255,\"b\":0}},{\"nodeId\":10,\"color\":{\"r\":0,\"g\":255,\"b\":0}},{\"nodeId\":11,\"color\":{\"r\":0,\"g\":255,\"b\":0}}]}";

    if(argc > 3) {
        json_update = argv[3];
    }

    StoreSample(model_path, modelname, json_update); 

}

void
Usage()
{
    std::cout << "Usage: libsc_sample model_folder modelname" << std::endl;
    std::cout << "model_folder\tName of output directory." << std::endl;
    std::cout << "modelname\tName of model to author or edit." << std::endl;

}
