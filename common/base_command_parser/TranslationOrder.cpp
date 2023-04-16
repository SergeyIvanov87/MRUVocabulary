#include <stdlib.h>

#include "common/base_command_parser/TranslationOrder.h"
#include "common/base_command_parser/TypedValue.hpp"

TranslationOrder::TranslationOrder(const std::string& order) :
    base(order)
{
}

std::shared_ptr<TranslationOrder> TranslationOrder::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::shared_ptr<TranslationOrder> ret = ValueBase::create<TranslationOrder>(argv[arg_offset + processed_arguments]);

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
