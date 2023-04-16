#include "common/base_command_parser/DBAuthParam.h"
#include "common/base_command_parser/TypedValue.hpp"

DBUserParamValue::DBUserParamValue(std::string str) :
    base(std::move(str))
{
}

std::shared_ptr<DBUserParamValue> DBUserParamValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<DBUserParamValue> ret = ValueBase::create<DBUserParamValue>(argv[arg_offset + processed_arguments]);

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}

DBPwdParamValue::DBPwdParamValue(std::string str) :
    base(std::move(str))
{
}

std::shared_ptr<DBPwdParamValue> DBPwdParamValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<DBPwdParamValue> ret = ValueBase::create<DBPwdParamValue>(argv[arg_offset + processed_arguments]);

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
