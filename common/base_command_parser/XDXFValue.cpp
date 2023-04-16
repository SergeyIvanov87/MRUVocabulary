#include "common/base_command_parser/XDXFValue.h"
#include "common/base_command_parser/TypedValue.hpp"

XDXFTranslatorPath::XDXFTranslatorPath(std::string str) :
    base(std::move(str))
{
}

std::shared_ptr<XDXFTranslatorPath> XDXFTranslatorPath::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<XDXFTranslatorPath> ret = ValueBase::create<XDXFTranslatorPath>(argv[arg_offset + processed_arguments]);

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
