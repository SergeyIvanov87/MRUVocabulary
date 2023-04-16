#ifndef REGEX_FILTER_PARAM_H
#define REGEX_FILTER_PARAM_H

#include "common/base_command_parser/ValueBase.h"

class RegexFilterValue final: public TypedValue<std::string>
{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "regex";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    RegexFilterValue(std::string str);
    ~RegexFilterValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<RegexFilterValue> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //REGEX_FILTER_PARAM_H
