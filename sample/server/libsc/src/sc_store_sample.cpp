
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "hoops_license.h"
#include "sc_store.h"
#include "sc_assemblytree.h"
#include <gason.h>
#include "sc_json_helper.h"

#if 0
#include "tc_io.h"
void recursiveDeleteDirectory(std::string file_system_directory_path)
{
	using namespace TC::IO;
	path file_path = absolute(utf8_to_path(file_system_directory_path));
	remove_all(file_path);
}
#else
void recursiveDeleteDirectory(std::string file_system_directory_path)
{
}
#endif

class ApplicationLogger : public SC::Store::Logger
{
public:
    virtual void
    Message(const char *message) const
    {
        printf("%s\n", message);
    }
};

// Helper function to literally just save a file copy to file.orig
void backupFiletoOrig(std::string &file_path)
{
    std::string orig_file_path = file_path + ".orig";
    std::ofstream orig_file_stream(orig_file_path, std::ios::binary);
    std::ifstream file_path_stream(file_path, std::ios::binary);
    orig_file_stream << file_path_stream.rdbuf();
}

// Helper function to literally just save a file.orig back to the original file name
void revertFilesFromOrig(std::string &file_path)
{
    std::string orig_file_path = file_path + ".orig";
    std::ifstream orig_file_stream(orig_file_path, std::ios::binary);
    std::ofstream file_path_stream(file_path, std::ios::binary);
    file_path_stream << orig_file_stream.rdbuf();
}

static void
AddSquareFace(
    SC::Store::Mesh &mesh,
    uint32_t const *point_indices,
    uint32_t normal_index,
    uint32_t rgba_index)
{
    mesh.face_elements.emplace_back();
    SC::Store::MeshElement &face = mesh.face_elements.back();

    // Add first face triangle
    for (size_t i = 0; i < 3; ++i)
    {
        face.indices.push_back(point_indices[i]);
        face.indices.push_back(normal_index);
        face.indices.push_back(rgba_index);
    }

    // Add second face triangle
    for (size_t i = 0; i < 3; ++i)
    {
        face.indices.push_back(point_indices[(i + 2) % 4]);
        face.indices.push_back(normal_index);
        face.indices.push_back(rgba_index);
    }

    // Add face edges
    for (size_t i = 0; i < 4; ++i)
    {
        mesh.polyline_elements.emplace_back();
        SC::Store::MeshElement &edges = mesh.polyline_elements.back();
        edges.indices.push_back(point_indices[i % 4]);
        edges.indices.push_back(rgba_index);
        edges.indices.push_back(point_indices[(i + 1) % 4]);
        edges.indices.push_back(rgba_index);
        // Set bits on edges so that they are selectable for measurement
        mesh.polyline_elements_bits.push_back(SC::Store::SelectionBitsEdgeHasMeasurementData);
    }
    // Set bits on faces so that they are selectable for measurement
    mesh.face_elements_bits.push_back(
        SC::Store::SelectionBitsFaceHasMeasurementData | SC::Store::SelectionBitsFacePlanar);
}

int StoreSample(const std::string &model_output_path, const std::string &model_name = "sc-model-default", const std::string &json_update = "")
{
    std::string json_input_string = json_update;
    json_input_string.erase(std::remove_if(json_input_string.begin(), json_input_string.end(), isspace), json_input_string.end());

    ApplicationLogger logger;

    try
    {
        // Open the cache and clean up files that we are using.
        SC::Store::Database::SetLicense(HOOPS_LICENSE);
        SC::Store::Cache cache = SC::Store::Database::Open(logger);

        std::string output_path = model_output_path;
        output_path += "/";
        output_path += model_name;
        std::string scs_output_path = output_path + ".scs";
        std::string scz_output_path = output_path + ".scz";
        std::string xml_output_path = output_path + ".xml";
        std::ifstream scs_orig_file(scs_output_path + ".orig", std::ios::binary);

        std::__fs::filesystem::remove_all(output_path);

        if (scs_orig_file.good())
        {
            // Start with a fresh copy on each instantiation fort he time being if an original file exists.
            revertFilesFromOrig(scs_output_path);
            revertFilesFromOrig(scz_output_path);
            revertFilesFromOrig(xml_output_path);
        }
        else
        {
            // If the original file does not exists, copy the assumed baseline files to the ".orig" extension
            backupFiletoOrig(scs_output_path);
            backupFiletoOrig(scz_output_path);
            backupFiletoOrig(xml_output_path);
        }

        // // Does the model in question exist?
        // if (cache.Exists(file_path_string.c_str())) {
        //     // delete the model (if desired).
        //     recursiveDeleteDirectory(file_path_string);
        // }

        // Check if model SCZ and XML already exists
        // auto scz_status = static_cast<std::string>(SC::Store::Database::QuerySCZ(scz_output_path.c_str(), logger));

        if (!std::__fs::filesystem::exists(output_path))
        {
            SC::Store::Database::DecompressSCZ(scz_output_path.c_str(), output_path.c_str(), logger);
        }

        // Open (or Create) the model we care about.
        SC::Store::Model model = cache.Open(output_path.c_str());
        auto modelName = model.GetName();
        printf("Opened and Loaded SC Model. Model Name: %s\n", modelName);

        SC::Store::AssemblyTree assembly_tree(logger);
        // Load/Author assembly tree.
        {
            if (assembly_tree.DeserializeFromXML(xml_output_path.c_str()))
            {
                printf("Successfully Read and Loaded XML Assembly\n");
                // assembly_tree.SetNodeName(0, "chris overwrite");
                // // Add an attribute on that node.
                // assembly_tree.AddAttribute(
                //     34, "chris's attribute", SC::Store::AssemblyTree::AttributeTypeString,
                //     "dope if this works");

                ///// PROCESS JSON IMPORT
                char *source = new char[json_input_string.length() + 1];
                strcpy(source, json_input_string.c_str());

                // do not forget terminate source string with 0
                char *endptr;
                JsonValue value;
                JsonAllocator allocator;
                int status = jsonParse(source, &endptr, &value, allocator);
                if (status != JSON_OK) {
                    fprintf(stderr, "%s at %zd\n", jsonStrError(status), endptr - source);
                } else {
                    for (auto changeRequestItem : value) {
                        if (strcmp(changeRequestItem->key, "attributes") == 0) {
                            /*"attributes":[
                                {"nodeId":67,"Material":"Inconel"},
                                {"nodeId":28,"Material":"Steel"},
                                {"nodeId":59,"Material":"Wood"},
                                {"nodeId":95,"Manufacture Date":"10/22/2021"}]
                            */
                            // The attributes will always be stored in an array so access the array in the "value" of the first child and then get the node of the array
                            for (auto attributes : changeRequestItem->value) {
                                auto attribute = attributes->value.toNode();
                                // for(auto attribute: attributePair->value){
                                if (strcmp(attribute->key, "nodeId") == 0) {
                                    auto nodeId = (int)attribute->value.toNumber();
                                    auto attributeName = attribute->next->key;
                                    auto attributeValue = attribute->next->value.toString();
                                    printf("Attribute written to node %i  ::  Attribute Name: %s  ::  Attribute Value: %s \n", nodeId, attributeName, attributeValue);
                                    if (!assembly_tree.AddAttribute(nodeId, attributeName, SC::Store::AssemblyTree::AttributeTypeString, attributeValue)) {
                                        printf("ERROR: Failed to add attribute %s on node %i . \n", attributeName, nodeId);
                                    }
                                }
                            }
                        } else if (strcmp(changeRequestItem->key, "nodeNames") == 0) {
                            /*"nodeNames":[
                                {"nodeId":0,"nodeName":"HC Node"},
                                {"nodeId":2,"nodeName":"HC Node 2"},
                            ]
                            */
                            // The nodeNames will always be stored in an array so access the array in the "value" of the first child and then get the node of the array
                            for (auto nodeNames : changeRequestItem->value) {
                                auto nodeName = nodeNames->value.toNode();
                                // for(auto attribute: attributePair->value){
                                if (strcmp(nodeName->key, "nodeId") == 0) {
                                    auto nodeId = (int)nodeName->value.toNumber();
                                    if (strcmp(nodeName->next->key, "nodeName") == 0) {
                                        auto nodeNameValue = nodeName->next->value.toString();
                                        printf("Node %i  was renamed to %s. \n", nodeId, nodeNameValue);
                                        if (!assembly_tree.SetNodeName(nodeId, nodeNameValue)) {
                                            printf("ERROR: Failed to rename node %i to %s. \n", nodeId, nodeNameValue);
                                        }
                                    }
                                }
                            }
                        } else if (strcmp(changeRequestItem->key, "colors") == 0) {
                            /*"colors":[{"nodeIds":[8,9,10,11],"color":{"r":255,"g":0,"b":0}}]}*/
                            float red, green, blue = 0.0;

                            // The colors will always be stored in an array so access the array in the "value" of the first child and then get the node of the array
                            for (auto colors : changeRequestItem->value) {
                                auto colorNode = colors->value.toNode();
                                if (strcmp(colorNode->key, "nodeId") == 0) {
                                    auto color = colorNode->next;
                                    if (strcmp(color->key, "color") == 0) {
                                        for (auto rgbValues : color->value) {
                                            if (strcmp(rgbValues->key, "r") == 0) {
                                                red = (rgbValues->value.toNumber()) / 255.0;
                                            } else if (strcmp(rgbValues->key, "g") == 0) {
                                                green = (rgbValues->value.toNumber()) / 255.0;
                                            } else if (strcmp(rgbValues->key, "b") == 0) {
                                                blue = (rgbValues->value.toNumber()) / 255.0;
                                            }
                                        }
                                    }
                                    SC::Store::InstanceKey scInstanceKey;
                                    auto scInstanceId = color->next;
                                    if (strcmp(scInstanceId->key, "scInstanceId") == 0) {
                                        scInstanceKey = scInstanceId->value.toNumber();
                                    }

                                    auto nodeId = (int)colorNode->value.toNumber();
                                    auto inputMaterial = SC::Store::Material(SC::Store::Color(red, green, blue, 1.0));
                                    auto inputMaterialKey = model.Insert(inputMaterial);
                                    auto materialBlack = SC::Store::Material(SC::Store::Color(0, 0, 0, 1.0));
                                    auto materialKeyBlack = model.Insert(materialBlack);
                                    // if (!assembly_tree.SetNodeMaterial(13, material))
                                    // {
                                    //     printf("ERROR: Failed to set color on instance %i . \n", nodeId);
                                    // } // TODO: publish color updates
                                    // Need to send over scInstanceId from client. Passing 13 for now.
                                    printf("Setting color to node %i  ::  ScInstanceId: %i  ::  Color: %f %f %f  \n", nodeId, (int)scInstanceId->value.toNumber(), red, green, blue);
                                    model.Set(scInstanceKey, inputMaterialKey, materialKeyBlack, materialKeyBlack);
                                }
                            }
                        } else if (strcmp(changeRequestItem->key, "defaultCamera") == 0) {
                            SC::Store::Camera defaultCamera;
                            /*"defaultCamera":
                            {"_position":{"x":81.22082242242087,"y":-99.85364263567925,"z":-14.745490335642312},
                            "_target":{"x":42.01403360616819,"y":28.500000953674316,"z":-45.15500047683717},
                            "_up":{"x":0.014413796198195017,"y":0.23468066183916042,"z":0.9719656523961587},
                            "_width":137.61020125980392,
                            "_height":137.61020125980392,
                            "_projection":0,
                            "_nearLimit":0.01,
                            "_cameraFlags":0},
                            */
                            for (auto cameraSettings : changeRequestItem->value) {
                                if (strcmp(cameraSettings->key, "position") == 0) {
                                    for (auto xyzVals : cameraSettings->value) {
                                        if (strcmp(xyzVals->key, "x") == 0) {
                                            defaultCamera.position.x = xyzVals->value.toNumber();
                                        } else if (strcmp(xyzVals->key, "y") == 0) {
                                            defaultCamera.position.y = xyzVals->value.toNumber();
                                        } else if (strcmp(xyzVals->key, "z") == 0) {
                                            defaultCamera.position.z = xyzVals->value.toNumber();
                                        }
                                    }
                                }
                                else if (strcmp(cameraSettings->key, "target") == 0)
                                {
                                    double xTarget, yTarget, zTarget = 0.0;
                                    for (auto xyzVals : cameraSettings->value)
                                    {
                                        if (strcmp(xyzVals->key, "x") == 0)
                                        {
                                            defaultCamera.target.x = xyzVals->value.toNumber();
                                        }
                                        else if (strcmp(xyzVals->key, "y") == 0)
                                        {
                                            defaultCamera.target.y = xyzVals->value.toNumber();
                                        }
                                        else if (strcmp(xyzVals->key, "z") == 0)
                                        {
                                            defaultCamera.target.z = xyzVals->value.toNumber();
                                        }
                                    }
                                }
                                else if (strcmp(cameraSettings->key, "up") == 0)
                                {
                                    for (auto xyzVals : cameraSettings->value)
                                    {
                                        if (strcmp(xyzVals->key, "x") == 0)
                                        {
                                            defaultCamera.up_vector.x = xyzVals->value.toNumber();
                                        }
                                        else if (strcmp(xyzVals->key, "y") == 0)
                                        {
                                            defaultCamera.up_vector.y = xyzVals->value.toNumber();
                                        }
                                        else if (strcmp(xyzVals->key, "z") == 0)
                                        {
                                            defaultCamera.up_vector.z = xyzVals->value.toNumber();
                                        }
                                    }
                                }
                                else if (strcmp(cameraSettings->key, "width") == 0)
                                {
                                    defaultCamera.field_width = cameraSettings->value.toNumber();
                                }
                                else if (strcmp(cameraSettings->key, "height") == 0)
                                {
                                    defaultCamera.field_height = cameraSettings->value.toNumber();
                                }
                                else if (strcmp(cameraSettings->key, "projection") == 0)
                                {
                                    auto clientProjectionEnum = (int)cameraSettings->value.toNumber();
                                    auto libScProjectionEnum = SC::Store::Camera::Projection::Invalid;
                                    // {0: 'Orthographic', 1: 'Perspective', Orthographic: 0, Perspective: 1}
                                    if (clientProjectionEnum == 0)
                                    {
                                        libScProjectionEnum = SC::Store::Camera::Projection::Orthographic;
                                    }
                                    if (clientProjectionEnum == 1)
                                    {
                                        libScProjectionEnum = SC::Store::Camera::Projection::Perspective;
                                    }
                                    defaultCamera.projection = libScProjectionEnum;
                                }
                                else if (strcmp(cameraSettings->key, "nearLimit") == 0)
                                {
                                    // defaultCamera.nearLimit = cameraSettings->value.toNumber();
                                }
                                else if (strcmp(cameraSettings->key, "cameraFlags") == 0)
                                {
                                    // cameraFlags = (int)cameraSettings->value.toNumber();
                                }
                            }
                            // TODO: Write the default camera settings to the file.
                            printf("Default Camera Overwritten\n");
                            model.Set(defaultCamera);
                        } else if (strcmp(changeRequestItem->key, "meshes") == 0) {
                            SC::Store::Mesh mesh;
                            int nodeId, parentNodeId = 0;

                            // The meshes will always be stored in an array so access the array in the "value" of the first child and then get the node of the array
                            for (auto meshTemplates : changeRequestItem->value) {
                                std::vector<SC::Store::MeshElement> faceMeshElements;
                                auto meshTemplate = meshTemplates->value;
                                for(auto meshTemplate : meshTemplates->value){
                                    // Set the default, if any values change, we will update them in the code below...
                                    mesh.flags = (SC::Store::Mesh::Bits)(
                                        SC::Store::Mesh::ClockwiseWinding | SC::Store::Mesh::FaceNormals | SC::Store::Mesh::FaceRGBA32s | SC::Store::Mesh::LineRGBA32s | SC::Store::Mesh::Manifold);

                                    // for(auto attribute: attributePair->value){
                                    if (strcmp(meshTemplate->key, "nodeId") == 0) {
                                        nodeId= (int)meshTemplate->value.toNumber();
                                    } else if (strcmp(meshTemplate->key, "parentNodeId") == 0) {
                                        parentNodeId = (int)meshTemplate->value.toNumber();                                        
                                    } else if (strcmp(meshTemplate->key, "faces") == 0) {
                                        /*
                                            faces: meshElementData = {
                                                        position: [],
                                                        normal: [],
                                                        rgba: [],
                                                        uv: [],
                                                    }
                                            }*/
                                        auto meshFacesNode = meshTemplate->value.toNode();

                                        std::vector<SC::Store::Point> points;
                                        std::vector<SC::Store::Normal> normals;
                                        std::vector<SC::Store::RGBA32> rgba32s;
                                        std::vector<SC::Store::UV> uvs;
                                        int point_count, normal_count, rgba_count, uv_count = 0;
                                        bool processed = processMeshPosNormRGBaUV(meshFacesNode, points, normals, rgba32s, uvs);

                                        if(points.size()){
                                            SC::Store::Point points_arr[points.size()];
                                            int point_count = 0;
                                            for(auto point : points){
                                                points_arr[point_count] = point;
                                                point_count++;
                                            }
                                            mesh.points = points_arr;
                                            mesh.point_count = point_count;
                                        }

                                        if(normals.size()){
                                            SC::Store::Normal normals_arr[normals.size()];
                                            int normals_count = 0;
                                            for(auto normal : normals){
                                                normals_arr[normals_count] = normal;
                                                normals_count++;
                                            }
                                            mesh.normals = normals_arr;
                                            mesh.normal_count = normal_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::FaceNormals);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::FaceNormals);

                                        if(rgba32s.size()){
                                            SC::Store::RGBA32 rgba32s_arr[rgba32s.size()];
                                            int rgba32s_count = 0;
                                            for(auto rgba32 : rgba32s){
                                                rgba32s_arr[rgba32s_count] = rgba32;
                                                rgba32s_count++;
                                            }
                                            mesh.rgba32s = rgba32s_arr;
                                            mesh.rgba32_count = rgba32s_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::FaceRGBA32s);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::FaceRGBA32s);

                                        if(uvs.size()){
                                            SC::Store::UV uvs_arr[uvs.size()];
                                            int uvs_count = 0;
                                            for(auto uv : uvs){
                                                uvs_arr[uvs_count] = uv;
                                                uvs_count++;
                                            }
                                            mesh.uvs = uvs_arr;
                                            mesh.uv_count = uvs_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::FaceUVs);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::FaceUVs);

                                    } else if (strcmp(meshTemplate->key, "lines") == 0) {
                                        /*
                                            lines: meshElementData = {
                                                        position: [],
                                                        normal: [],
                                                        rgba: [],
                                                        uv: [],
                                                    },
                                            }*/
                                        auto meshFacesNode = meshTemplate->value.toNode();
                                        std::vector<SC::Store::Point> points;
                                        std::vector<SC::Store::Normal> normals;
                                        std::vector<SC::Store::RGBA32> rgba32s;
                                        std::vector<SC::Store::UV> uvs;
                                        int point_count, normal_count, rgba_count, uv_count = 0;
                                        bool processed = processMeshPosNormRGBaUV(meshFacesNode, points, normals, rgba32s, uvs);

                                        if(points.size()){
                                            SC::Store::Point points_arr[points.size()];
                                            int point_count = 0;
                                            for(auto point : points){
                                                points_arr[point_count] = point;
                                                point_count++;
                                            }
                                            mesh.points = points_arr;
                                            mesh.point_count = point_count;
                                        }

                                        if(normals.size()){
                                            SC::Store::Normal normals_arr[normals.size()];
                                            int normals_count = 0;
                                            for(auto normal : normals){
                                                normals_arr[normals_count] = normal;
                                                normals_count++;
                                            }
                                            mesh.normals = normals_arr;
                                            mesh.normal_count = normal_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::LineNormals);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::LineNormals);

                                        if(rgba32s.size()){
                                            SC::Store::RGBA32 rgba32s_arr[rgba32s.size()];
                                            int rgba32s_count = 0;
                                            for(auto rgba32 : rgba32s){
                                                rgba32s_arr[rgba32s_count] = rgba32;
                                                rgba32s_count++;
                                            }
                                            mesh.rgba32s = rgba32s_arr;
                                            mesh.rgba32_count = rgba32s_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::LineRGBA32s);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::LineRGBA32s);

                                        if(uvs.size()){
                                            SC::Store::UV uvs_arr[uvs.size()];
                                            int uvs_count = 0;
                                            for(auto uv : uvs){
                                                uvs_arr[uvs_count] = uv;
                                                uvs_count++;
                                            }
                                            mesh.uvs = uvs_arr;
                                            mesh.uv_count = uvs_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::LineUVs);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::LineUVs);

                                        // TODO: Add lines...?
                                    } else if (strcmp(meshTemplate->key, "points") == 0){
                                        /*
                                            points: meshElementData = {
                                                        position: [],
                                                        normal: [],
                                                        rgba: [],
                                                        uv: [],
                                                    },
                                            }*/
                                        auto meshFacesNode = meshTemplate->value.toNode();
                                        std::vector<SC::Store::Point> points;
                                        std::vector<SC::Store::Normal> normals;
                                        std::vector<SC::Store::RGBA32> rgba32s;
                                        std::vector<SC::Store::UV> uvs;
                                        int point_count, normal_count, rgba_count, uv_count = 0;
                                        bool processed = processMeshPosNormRGBaUV(meshFacesNode, points, normals, rgba32s, uvs);

                                        if(points.size()){
                                            SC::Store::Point points_arr[points.size()];
                                            int point_count = 0;
                                            for(auto point : points){
                                                points_arr[point_count] = point;
                                                point_count++;
                                            }
                                            mesh.points = points_arr;
                                            mesh.point_count = point_count;
                                        }

                                        if(normals.size()){
                                            SC::Store::Normal normals_arr[normals.size()];
                                            int normals_count = 0;
                                            for(auto normal : normals){
                                                normals_arr[normals_count] = normal;
                                                normals_count++;
                                            }
                                            mesh.normals = normals_arr;
                                            mesh.normal_count = normal_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::PointNormals);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::PointNormals);

                                        if(rgba32s.size()){
                                            SC::Store::RGBA32 rgba32s_arr[rgba32s.size()];
                                            int rgba32s_count = 0;
                                            for(auto rgba32 : rgba32s){
                                                rgba32s_arr[rgba32s_count] = rgba32;
                                                rgba32s_count++;
                                            }
                                            mesh.rgba32s = rgba32s_arr;
                                            mesh.rgba32_count = rgba32s_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::PointRGBA32s);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::PointRGBA32s);

                                        if(uvs.size()){
                                            SC::Store::UV uvs_arr[uvs.size()];
                                            int uvs_count = 0;
                                            for(auto uv : uvs){
                                                uvs_arr[uvs_count] = uv;
                                                uvs_count++;
                                            }
                                            mesh.uvs = uvs_arr;
                                            mesh.uv_count = uvs_count;
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::PointUVs);
                                        } else mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::PointUVs);
                                        // TODO: Add Points...?

                                    } else if (strcmp(meshTemplate->key,"winding") == 0){
                                        /*winding: meshData.winding*/
                                        if(strcmp(meshTemplate->value.toString(),"clockwise") == 0){
                                            // Add clockwise bit and remove counter clockwise bit...
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::ClockwiseWinding);
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::CounterClockwiseWinding);
                                        } else {
                                            // Add Counter clockwise bit and remove clockwise bit...
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::CounterClockwiseWinding);
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::ClockwiseWinding);
                                        }
                                    } else if (strcmp(meshTemplate->key,"isTwoSided") == 0){
                                        /*isTwoSided: meshData.isTwoSided*/
                                        if(meshTemplate->value.toNumber() == 0){
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::TwoSided);
                                        } else {
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::Manifold);
                                        }
                                    } else if (strcmp(meshTemplate->key,"isManifold") == 0){
                                        /*isManifold: meshData.isManifold,*/
                                        if(meshTemplate->value.toNumber() == 0){
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags | SC::Store::Mesh::Bits::Manifold);
                                        } else {
                                            mesh.flags = (SC::Store::Mesh::Bits) (mesh.flags & ~SC::Store::Mesh::Bits::Manifold);
                                        }
                                    }
                                }
                            }
                            //Process the mesh at the end here.
                            auto meshKey = model.Insert(mesh);
                            auto instanceKey = model.Instance(meshKey);
                            SC::Store::NodeId childNodeId = nodeId;
                            SC::Store::NodeId bodyInstanceNode = 0;
                            assembly_tree.CreateChild(parentNodeId, childNodeId);
                            assembly_tree.CreateAndAddBodyInstance(childNodeId, bodyInstanceNode);
                        } else {
                            // Unhandled JSON top level item
                            printf("ERROR: Unknown change insertion in JSON file\n");
                        }
                    }
                }

                delete[] source;

                // printf("%s\n", json_update.c_str());
                ///// END JSON IMPORT

                // Serialize authored content to model and xml output
                auto passed = assembly_tree.SerializeToModel(model);
                passed = assembly_tree.SerializeToXML(xml_output_path.c_str());
                printf("Serialized Assembly Tree to Model and XML\n");

                // Prepare the model for streaming.
                model.PrepareStream();
                printf("Preparing Stream and authoring SCZ and SCS models.\n");

                model.GenerateSCSFile(scs_output_path.c_str());
                model.GenerateSCZFile(scz_output_path.c_str());
                printf("Authoring Complete.\n");
            }
            else
            {
                printf("Could not load XML. Assembly Tree Major Version must be >= %u\n", SC::Store::AssemblyTree::MAJOR_VERSION);
            }
        }
    }
    catch (std::exception const &e)
    {
        std::string message("Exception: ");
        message.append(e.what());
        message.append("\n");
        logger.Message(message.c_str());
        return 1;
    }

    return 0;
}
