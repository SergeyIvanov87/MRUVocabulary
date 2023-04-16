#include "common/base_command_parser/TypedValue.hpp"
#include "common/base_command_parser/DestinationFormat.h"


DestinationFormat::DestinationFormat(std::string str) :
    base(std::move(str))
{
}

std::shared_ptr<DestinationFormat> DestinationFormat::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<DestinationFormat> ret = ValueBase::create<DestinationFormat>(argv[arg_offset + processed_arguments]);
    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;


    return ret;
}
