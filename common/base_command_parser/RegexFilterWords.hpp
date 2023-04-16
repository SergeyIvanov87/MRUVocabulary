#ifndef REGEX_FILTER_WORDS_PARAM_H
#define REGEX_FILTER_WORDS_PARAM_H
#include <set>

#include "common/base_command_parser/ValueBase.h"

class RegexFilterWordsValue final: public TypedValue<std::set<std::string>>
{
public:
    using base = TypedValue<std::set<std::string>>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "regex_filter_words";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    RegexFilterWordsValue(std::set<std::string> str);
    ~RegexFilterWordsValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<RegexFilterWordsValue> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //REGEX_FILTER_WORDS_PARAM_H
