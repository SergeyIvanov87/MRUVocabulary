#ifndef SOURCE_VALUE_H
#define SOURCE_VALUE_H

#include "common/base_command_parser/ValueBase.h"

class SourceValue final : public TypedValue<std::string>
{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "-src";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    SourceValue(std::string str);
    ~SourceValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<SourceValue> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif
