#include "common/base_command_parser/DBServiceParam.h"
#include "common/base_command_parser/TypedValue.hpp"

DBHostParamValue::DBHostParamValue(std::string str) :
    base(std::move(str))
{
}

std::shared_ptr<DBHostParamValue> DBHostParamValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<DBHostParamValue> ret = ValueBase::create<DBHostParamValue>(argv[arg_offset + processed_arguments]);

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}

DBPortParamValue::DBPortParamValue(unsigned short port) :
    base(port)
{
}

std::shared_ptr<DBPortParamValue> DBPortParamValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    unsigned short port = atoi(argv[arg_offset + processed_arguments]);
    std::shared_ptr<DBPortParamValue> ret = ValueBase::create<DBPortParamValue>(port);

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
