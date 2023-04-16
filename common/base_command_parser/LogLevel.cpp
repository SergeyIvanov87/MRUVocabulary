#include <stdlib.h>

#include "common/base_command_parser/LogLevel.h"
#include "common/base_command_parser/TypedValue.hpp"

LogLevel::LogLevel(int level) :
    base((eLogLevel)level)
{
}

std::shared_ptr<LogLevel> LogLevel::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<LogLevel> ret = ValueBase::create<LogLevel>(atoi(argv[arg_offset + processed_arguments]));

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
