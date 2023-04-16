#include "common/base_command_parser/FilterWords.hpp"
#include "common/base_command_parser/TypedValue.hpp"

FilterWordsValue::FilterWordsValue(std::set<std::string> str) :
    base(std::move(str))
{
}

std::shared_ptr<FilterWordsValue> FilterWordsValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::stringstream ss{argv[arg_offset + processed_arguments]};

    std::set<std::string> result;
    while (ss.good())
    {
        std::string substr;
        std::getline (ss, substr, ',');
        result.insert(std::move(substr));
    }

    std::shared_ptr<FilterWordsValue> ret = ValueBase::create<FilterWordsValue>(std::move(result));

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
