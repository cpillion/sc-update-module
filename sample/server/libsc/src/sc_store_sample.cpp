
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

int StoreSample(const std::string &model_output_path, const std::string &model_name = "sc-model-default")
{

    ApplicationLogger logger;

    try
    {
        // Open the cache.
        SC::Store::Database::SetLicense(HOOPS_LICENSE);
        SC::Store::Cache cache = SC::Store::Database::Open(logger);

        std::string output_path = model_output_path;
        output_path += "/";
        output_path += model_name;
        std::string scz_output_path = output_path + ".scz";
        std::string xml_output_path = output_path + ".xml";

        // // Does the model in question exist?
        // if (cache.Exists(file_path_string.c_str())) {
        //     // delete the model (if desired).
        //     recursiveDeleteDirectory(file_path_string);
        // }


        // Check if model SCZ and XML already exists
        auto scz_status = static_cast<std::string>(SC::Store::Database::QuerySCZ(scz_output_path.c_str(), logger));

        SC::Store::Database::DecompressSCZ(scz_output_path.c_str(), output_path.c_str(), logger);

        // Query existing models.
        size_t count;
        const char *const *names = cache.GetModelPaths(count);
        for (size_t i = 0; i < count; ++i)
        {
            printf("%u:\t%s\n", (unsigned int)i, names[i]);
        }

        // Open (or Create) the model we care about.
        SC::Store::Model model = cache.Open(output_path.c_str());
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
            if (assembly_tree.DeserializeFromXML(xml_output_path.c_str()))
            {
                assembly_tree.SetNodeName(0, "chris overwrite");
                // Add an attribute on that node.
                assembly_tree.AddAttribute(
                    0, "chris's attribute", SC::Store::AssemblyTree::AttributeTypeString,
                    "dope if this works");

                // Serialize authored content to model and xml output
                assembly_tree.SerializeToXML(xml_output_path.c_str());
                assembly_tree.SerializeToModel(model);
            }
            else
            {
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
