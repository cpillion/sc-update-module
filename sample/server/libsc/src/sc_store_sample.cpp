
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "hoops_license.h"
#include "sc_store.h"
#include "sc_assemblytree.h"
#include <gason.h>

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
        printf("%s\n", modelName);

        SC::Store::AssemblyTree assembly_tree(logger);
        // Load/Author assembly tree.
        {
            if (assembly_tree.DeserializeFromXML(xml_output_path.c_str()))
            {
                printf("Read and Loaded XML Assembly\n");
                // assembly_tree.SetNodeName(0, "chris overwrite");
                // // Add an attribute on that node.
                // assembly_tree.AddAttribute(
                //     34, "chris's attribute", SC::Store::AssemblyTree::AttributeTypeString,
                //     "dope if this works");

                ///// PROCESS JSON IMPORT
                char *source = new char[json_input_string.length() + 1];
                strcpy(source, json_input_string.c_str());
                // strcpy(source, "{\"colors\":[{\"nodeId\":20,\"color\":{\"r\":229,\"g\":158,\"b\":16},\"scInstanceId\":1}]}");

                // do not forget terminate source string with 0
                char *endptr;
                JsonValue value;
                JsonAllocator allocator;
                int status = jsonParse(source, &endptr, &value, allocator);
                if (status != JSON_OK)
                {
                    fprintf(stderr, "%s at %zd\n", jsonStrError(status), endptr - source);
                }
                else
                {
                    for (auto changeRequestItem : value)
                    {
                        if (strcmp(changeRequestItem->key, "attributes") == 0)
                        {
                            /*"attributes":[
                                {"nodeId":67,"Material":"Inconel"},
                                {"nodeId":28,"Material":"Steel"},
                                {"nodeId":59,"Material":"Wood"},
                                {"nodeId":95,"Manufacture Date":"10/22/2021"}]
                            */
                            // The attributes will always be stored in an array so access the array in the "value" of the first child and then get the node of the array
                            for (auto attributes : changeRequestItem->value)
                            {
                                auto attribute = attributes->value.toNode();
                                // for(auto attribute: attributePair->value){
                                if (strcmp(attribute->key, "nodeId") == 0)
                                {
                                    auto nodeId = (int)attribute->value.toNumber();
                                    auto attributeName = attribute->next->key;
                                    auto attributeValue = attribute->next->value.toString();
                                    printf("Attribute Name: %s  :::  Value: %s . \n", attributeName, attributeValue);
                                    if (!assembly_tree.AddAttribute(nodeId, attributeName, SC::Store::AssemblyTree::AttributeTypeString, attributeValue))
                                    {
                                        printf("ERROR: Failed to add attribute %s on node %i . \n", attributeName, nodeId);
                                    }
                                }
                                //}
                            }
                        }
                        else if (strcmp(changeRequestItem->key, "colors") == 0)
                        {
                            /*"colors":[{"nodeIds":[8,9,10,11],"color":{"r":255,"g":0,"b":0}}]}*/
                            float red, green, blue = 0.0;

                            // The colors will always be stored in an array so access the array in the "value" of the first child and then get the node of the array
                            for (auto colors : changeRequestItem->value)
                            {
                                auto colorNode = colors->value.toNode();
                                if (strcmp(colorNode->key, "nodeIds") == 0)
                                {
                                    // auto color = colorNode->next;
                                    // if (strcmp(color->key, "color") == 0)
                                    // {
                                    //     for (auto rgbValues : color->value)
                                    //     {
                                    //         if (strcmp(rgbValues->key, "r") == 0)
                                    //         {
                                    //             red = (int)rgbValues->value.toNumber();
                                    //         }
                                    //         else if (strcmp(rgbValues->key, "g") == 0)
                                    //         {
                                    //             green = (int)rgbValues->value.toNumber();
                                    //         }
                                    //         else if (strcmp(rgbValues->key, "b") == 0)
                                    //         {
                                    //             blue = (int)rgbValues->value.toNumber();
                                    //         }
                                    //     }
                                    // }

                                    // if (colorNode->value.getTag() == JSON_ARRAY)
                                    // {
                                    //     for (auto nodeIdsItem : colorNode->value)
                                    //     {
                                    //         // TODO publish color update.
                                    //         auto nodeId = (int)nodeIdsItem->value.toNumber();
                                    //         auto material = SC::Store::Material(SC::Store::Color(red, green, blue, 1.0));
                                    //         auto materialKey = model.Insert(material);
                                    //         if (!assembly_tree.SetNodeMaterial(nodeId, material))
                                    //         {
                                    //             printf("ERROR: Failed to set color on node %i . \n", nodeId);
                                    //         }
                                    //     }
                                    // }
                                }
                                else if (strcmp(colorNode->key, "nodeId") == 0)
                                {
                                    auto color = colorNode->next;
                                    if (strcmp(color->key, "color") == 0)
                                    {
                                        for (auto rgbValues : color->value)
                                        {
                                            if (strcmp(rgbValues->key, "r") == 0)
                                            {
                                                red = (rgbValues->value.toNumber()) / 255.0;
                                            }
                                            else if (strcmp(rgbValues->key, "g") == 0)
                                            {
                                                green = (rgbValues->value.toNumber()) / 255.0;
                                            }
                                            else if (strcmp(rgbValues->key, "b") == 0)
                                            {
                                                blue = (rgbValues->value.toNumber()) / 255.0;
                                            }
                                        }
                                    }
                                    SC::Store::InstanceKey scInstanceKey;
                                    auto scInstanceId = color->next;
                                    if (strcmp(scInstanceId->key, "scInstanceId") == 0)
                                    {
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
                                    model.Set(scInstanceKey, inputMaterialKey, materialKeyBlack, materialKeyBlack);
                                }
                            }
                        }
                        else if (strcmp(changeRequestItem->key, "defaultCamera") == 0)
                        {
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
                            for (auto cameraSettings : changeRequestItem->value)
                            {
                                if (strcmp(cameraSettings->key, "position") == 0)
                                {
                                    for (auto xyzVals : cameraSettings->value)
                                    {
                                        if (strcmp(xyzVals->key, "x") == 0)
                                        {
                                            defaultCamera.position.x = xyzVals->value.toNumber();
                                        }
                                        else if (strcmp(xyzVals->key, "y") == 0)
                                        {
                                            defaultCamera.position.y = xyzVals->value.toNumber();
                                        }
                                        else if (strcmp(xyzVals->key, "z") == 0)
                                        {
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
                            model.Set(defaultCamera);
                        }
                        else
                        {
                            // Unhandled JSON top level item
                            printf("ERROR: Unknown change insertion in JSON file\n");
                        }
                    }
                }

                delete[] source;

                printf("%s\n", json_update.c_str());
                ///// END JSON IMPORT

                // Serialize authored content to model and xml output
                auto passed = assembly_tree.SerializeToModel(model);
                passed = assembly_tree.SerializeToXML(xml_output_path.c_str());

                // Prepare the model for streaming.
                model.PrepareStream();

                model.GenerateSCSFile(scs_output_path.c_str());
                model.GenerateSCZFile(scz_output_path.c_str());
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
