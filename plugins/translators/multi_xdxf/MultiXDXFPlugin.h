#ifndef MULTI_XDXF_DICTIONARY_H
#define MULTI_XDXF_DICTIONARY_H

#define MULTI_XDXF_DICTIONARY_PLUGIN_NAME         "multi_xdxf"
#define MULTI_XDXF_DICTIONARY_CURRENT_VERSION     0

#include <map>
#include <string>
#include <unordered_map>
#include "TranslatorExport.h"
#include "CommandArguments.h"
#include <txml/applications/xdxf/xdxf_fwd.h>

struct SharedTranslatedData;
struct multi_xdxf_dictionary_context_v0
{
    std::map<std::string, std::string> filePathes;

    std::unique_ptr<CommandArgumentsBase> property_holder;

    using XMLReaderPtr = std::unique_ptr<txml::TextReaderWrapper>;
    std::map<std::string, XMLReaderPtr> xml_readers;

    using Article = std::optional<xdxf::XDXFArticle>;
    using Dictionary = std::map<std::string/*word*/, Article/*meaning*/>;
    using Volume = std::map<std::string/*language*/, Dictionary>;
    Volume multidictionary;

    std::unique_ptr<SharedTranslatedData> shared_data_ptr;
};

#endif //MULTI_XDXF_DICTIONARY_H
