#include "common/base_command_parser/RegexFilterWords.hpp"
#include "common/base_command_parser/TypedValue.hpp"

RegexFilterWordsValue::RegexFilterWordsValue(std::set<std::string> str) :
    base(std::move(str))
{
}

std::shared_ptr<RegexFilterWordsValue> RegexFilterWordsValue::create(int &argc, int &arg_offset, const char* argv[])
{
    int processed_arguments = 0;

    processed_arguments++; //skip cmd
    (void)processed_arguments; //no modifier

    std::stringstream ss{argv[arg_offset + processed_arguments]};

    std::set<std::string> result;

    std::string substr;
    while (ss.good())
    {
        std::string tmp;
        std::getline (ss, tmp, ',');

        if (tmp.empty())
        {
            continue;
        }

        if (*tmp.rbegin() == '\\')
        {
            tmp.pop_back();
            substr += (tmp + ',');
            continue;
        }

        if (!substr.empty())
        {
            substr += tmp;
            tmp.swap(substr);
            substr.clear();
        }


        result.insert(std::move(tmp));
    }

    std::shared_ptr<RegexFilterWordsValue> ret = ValueBase::create<RegexFilterWordsValue>(std::move(result));

    processed_arguments++; //processed value

    argc -= processed_arguments;
    arg_offset += processed_arguments;

    return ret;
}
