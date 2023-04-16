#ifndef XDXF_DICTIONARY_H
#define XDXF_DICTIONARY_H

#define XDXF_DICTIONARY_PLUGIN_NAME         "xdxf"
#define XDXF_DICTIONARY_CURRENT_VERSION     0

#include <map>
#include <string>
#include <unordered_map>
#include "TranslatorExport.h"
#include "CommandArguments.h"
#include <txml/applications/xdxf/xdxf_fwd.h>

struct SharedTranslatedData;
struct xdxf_dictionary_context_v0
{
    std::string filePath;

    std::unique_ptr<CommandArgumentsBase> property_holder;

    std::unique_ptr<txml::TextReaderWrapper> xml_reader;
    std::map<std::string, std::optional<xdxf::XDXFArticle>> dictionary;

    std::unique_ptr<SharedTranslatedData> shared_data_ptr;
};

#endif //XDXF_DICTIONARY_H
