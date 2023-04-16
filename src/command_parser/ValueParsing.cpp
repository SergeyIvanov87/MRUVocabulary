#include "common/base_command_parser/TypedValue.hpp"
#include "command_parser/SourceValue.h"

SourceValue::SourceValue(std::string str) :
    base(std::move(str))
{
}

std::shared_ptr<SourceValue> SourceValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<SourceValue> ret = ValueBase::create<SourceValue>(argv[arg_offset + processed_arguments]);
    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
