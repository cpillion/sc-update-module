
#include <assert.h>
#include <string>

#include "hoops_license.h"
#include "sc_store.h"
#include "sc_assemblytree.h"

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

int StoreSample(const std::string &model_path)
{

    std::string model_name = "microengine";

    ApplicationLogger logger;

    try
    {
        // Open the cache.
        SC::Store::Database::SetLicense(HOOPS_LICENSE);
        SC::Store::Cache cache = SC::Store::Database::Open(logger);

        std::string file_path_string = model_path;
        file_path_string += "/";
        file_path_string += model_name;
        std::string scz_file_path_string = file_path_string + ".scz";
        std::string xml_file_path_string = file_path_string + ".xml";

        // // Does the model in question exist?
        // if (cache.Exists(file_path_string.c_str())) {
        //     // delete the model (if desired).
        //     recursiveDeleteDirectory(file_path_string);
        // }

        SC::Store::Database::DecompressSCZ(scz_file_path_string.c_str(), file_path_string.c_str(), logger);

        // Query existing models.
        size_t count;
        const char *const *names = cache.GetModelPaths(count);
        for (size_t i = 0; i < count; ++i)
        {
            printf("%u:\t%s\n", (unsigned int)i, names[i]);
        }

        // Open (or Create) the model we care about.
        SC::Store::Model model = cache.Open(file_path_string.c_str());
        auto modelName = model.GetName();
        printf("%s\n", modelName);

        // These keys will get populated later in this function.
        SC::Store::MeshKey mesh_key;
        SC::Store::MaterialKey red_material_key;
        SC::Store::MaterialKey green_material_key;
        SC::Store::MaterialKey blue_material_key;
        SC::Store::MatrixKey matrix_key;
        SC::Store::InstanceKey instance_key_1;
        SC::Store::InstanceKey instance_key_2;
        SC::Store::InstanceKey instance_key_3;
        SC::Store::InstanceKey instance_key_4;
        SC::Store::AssemblyTree assembly_tree(logger);

        // Load/Author assembly tree.
        {
            if (assembly_tree.DeserializeFromXML(xml_file_path_string.c_str()))
            {
                assembly_tree.SetNodeName(0, "chris overwrite");
                // Add an attribute on that node.
                assembly_tree.AddAttribute(
                    0, "chris's attribute", SC::Store::AssemblyTree::AttributeTypeString,
                    "dope if this works");

                // Serialize authored content to model.
                assembly_tree.SerializeToModel(model);
            }
            else
            {
                // 02/14/2022: Bug? Apparently the returned major version in 2022 is 9, whereas the 2022 xml from converter
                // authors Major Version 22!
                printf("Could not load XML. Assembly Tree Major Version must be lower than: %u\n", SC::Store::AssemblyTree::MAJOR_VERSION);
            }
        }

        // Prepare the model for streaming.
        model.PrepareStream();
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
