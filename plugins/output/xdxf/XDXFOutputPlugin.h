#ifndef XDXF_OUTPUT_PLUGIN_H
#define XDXF_OUTPUT_PLUGIN_H

#define XDXF_OUTPUT_PLUGIN_NAME         "xdxf_output"
#define XDXF_OUTPUT_CURRENT_VERSION     1

#include <map>
#include <string>
#include <unordered_map>
#include "OutputExport.h"
#include "CommandArguments.h"
#include <txml/applications/xdxf/xdxf_fwd.h>

class ISharedTranslatedData;
struct xdxf_output_context_v0
{
    std::string filePath;
    std::string fileFormat;

    std::unique_ptr<CommandArgumentsBase> property_holder;

    std::unique_ptr<txml::TextReaderWrapper> xml_reader;

    std::unique_ptr<ISharedTranslatedData> translated_data_ptr;

    void merge(shared_ctx_t *new_data);
};

#endif //XDXF_OUTPUT_PLUGIN_H
