#ifndef XDXF_PARAM_H
#define XDXF_PARAM_H

#include "common/base_command_parser/ValueBase.h"

class XDXFTranslatorPath final: public TypedValue<std::string>

{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "xdxf_file";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    XDXFTranslatorPath(std::string str);
    ~XDXFTranslatorPath() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<XDXFTranslatorPath> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //XDXF_PARAM_H
