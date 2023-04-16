#ifndef LOG_LEVEL_VALUE_H
#define LOG_LEVEL_VALUE_H

#include "common/base_command_parser/ValueBase.h"

enum eLogLevel : int
{
    ERROR_LEVEL,
    INFO_LEVEL,
    DEBUG_LEVEL,
    TRACE_LEVEL
};

inline std::ostream& operator<< (std::ostream& out, eLogLevel val)
{
    out << (int)val;
    return out;
}

inline constexpr int to_underlying(eLogLevel e) noexcept
{
    return static_cast<int>(e);
}

inline std::istream& operator>>(std::istream& in, eLogLevel &val)
{
    int tmp;
    in >> tmp;

    val = static_cast<eLogLevel>(tmp);
    return in;
}

class LogLevel final: public TypedValue<eLogLevel>

{
public:
    using base = TypedValue<eLogLevel>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "log_level";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    LogLevel(int);
    ~LogLevel() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<LogLevel> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //LOG_LEVEL_VALUE_H
