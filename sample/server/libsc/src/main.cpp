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
    //Test JSON String for executing changes to model file.
    std::string json_update = "{\"meshes\":[{\"nodeId\":-64,\"parentNodeId\":-2,\"faces\":[{\"position\":[-10,10,10,10,10,10,-10,-10,10,10,10,10,10,-10,10,-10,-10,10,10,10,-10,-10,10,-10,-10,-10,-10,10,10,-10,-10,-10,-10,10,-10,-10,-10,10,-10,10,10,-10,10,10,10,-10,10,-10,10,10,10,-10,10,10,-10,-10,-10,10,-10,10,10,-10,-10,-10,-10,-10,-10,-10,10,10,-10,10,-10,10,-10,-10,10,10,-10,-10,-10,-10,10,10,-10,-10,10,-10,-10,-10,10,10,10,10,10,-10,10,-10,-10,10,10,10,10,-10,-10,10,-10,10],\"normal\":[0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0],\"rgba\":[],\"uv\":[]}],\"lines\":[],\"points\":[],\"winding\":\"clockwise\",\"isTwoSided\":0,\"isManifold\":0}]}";
    
    if(argc > 3) {
        json_update = argv[3];
    }

    //json_update = ""; //Uncomment this and it will simply revert the files back to their original and import/export.

    StoreSample(model_path, modelname, json_update); 

}

void
Usage()
{
    std::cout << "Usage: libsc_sample model_folder modelname" << std::endl;
    std::cout << "model_folder\tName of output directory." << std::endl;
    std::cout << "modelname\tName of model to author or edit." << std::endl;

}
