#ifndef TXT_DECODER_H
#define TXT_DECODER_H

#define TXT_DECODER_PLUGIN_NAME         "txt"
#define TXT_DECODER_CURRENT_VERSION     0

#include <map>
#include <string>
#include <unordered_map>
#include "DecoderExport.h"
#include "PluginDecodedData.h"
#include "CommandArguments.h"

struct txt_context_v0
{
    std::string filePath;

    std::unique_ptr<CommandArgumentsBase> property_holder;

    SharedDecodedData decoded;
};

#endif //TXT_DECODER_H
