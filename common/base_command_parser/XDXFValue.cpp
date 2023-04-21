#include <iterator>
#include <cstring>

#include "common/base_command_parser/XDXFValue.h"
#include "common/base_command_parser/TypedValue.hpp"

XDXFTranslatorPath::XDXFTranslatorPath(std::vector<std::string> &&str) :
    base(std::move(str))
{
}

std::shared_ptr<XDXFTranslatorPath> XDXFTranslatorPath::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<XDXFTranslatorPath> ret;
    {
        const char* path_arg_ptr = argv[arg_offset + processed_arguments];
        const char* it = path_arg_ptr;
        const char separator = ':';
        std::vector<std::string> pathes;
        do
        {
            path_arg_ptr = it;
            it = strchr(path_arg_ptr, separator);
            if (it)
            {
                pathes.emplace_back(path_arg_ptr, it);
            }
            else
            {
                pathes.emplace_back(path_arg_ptr);
                break;
            }
        } while(*++it);
        ret = ValueBase::create<XDXFTranslatorPath>(std::move(pathes));
    }

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
