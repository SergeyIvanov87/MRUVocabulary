#ifndef FB2_DECODER_H
#define FB2_DECODER_H

#define FB2_DECODER_PLUGIN_NAME         "fb2"
#define FB2_DECODER_CURRENT_VERSION     0

#include <map>
#include <string>
#include <unordered_map>
#include "DecoderExport.h"
#include "CommandArguments.h"
#include <txml/applications/fb2/fb2_fwd.h>

struct fb2_context_v0
{
    std::string filePath;

    std::unique_ptr<CommandArgumentsBase> property_holder;

    std::unique_ptr<txml::TextReaderWrapper> xml_reader;
};

#endif //FB2_DECODER_H
