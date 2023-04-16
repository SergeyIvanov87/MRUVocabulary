#ifndef DESTINATION_FORMAT_H
#define DESTINATION_FORMAT_H

#include "common/base_command_parser/ValueBase.h"

class DestinationFormat final : public TypedValue<std::string>
{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "-dst_format";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    DestinationFormat(std::string str);
    ~DestinationFormat() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<DestinationFormat> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif
