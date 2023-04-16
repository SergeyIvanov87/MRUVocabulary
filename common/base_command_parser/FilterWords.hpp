#ifndef FILTER_WORDS_PARAM_H
#define FILTER_WORDS_PARAM_H
#include <set>

#include "common/base_command_parser/ValueBase.h"

class FilterWordsValue final: public TypedValue<std::set<std::string>>
{
public:
    using base = TypedValue<std::set<std::string>>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "filter_words";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    FilterWordsValue(std::set<std::string> str);
    ~FilterWordsValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<FilterWordsValue> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //FILTER_WORDS_PARAM_H
