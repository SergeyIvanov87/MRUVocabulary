#ifndef DB_SERVICE_PARAM_H
#define DB_SERVICE_PARAM_H

#include "common/base_command_parser/ValueBase.h"

class DBHostParamValue final : public TypedValue<std::string>
{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "dbHost";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    DBHostParamValue(std::string str);
    ~DBHostParamValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<DBHostParamValue> create(int &argc, int &arg_offset, const char* argv[]);
};

class DBPortParamValue : public TypedValue<unsigned short>
{
public:
    using base = TypedValue<unsigned short>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "dbPort";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    DBPortParamValue(unsigned short port);
    ~DBPortParamValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<DBPortParamValue> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //DB_SERVICE_PARAM_H
