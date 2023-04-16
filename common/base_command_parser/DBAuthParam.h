#ifndef DB_AUTH_PARAM_H
#define DB_AUTH_PARAM_H

#include "common/base_command_parser/ValueBase.h"

class DBUserParamValue final : public TypedValue<std::string>
{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "dbUser";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    DBUserParamValue(std::string str);
    ~DBUserParamValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<DBUserParamValue> create(int &argc, int &arg_offset, const char* argv[]);
};

class DBPwdParamValue : public TypedValue<std::string>
{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "dbPwd";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    DBPwdParamValue(std::string str);
    ~DBPwdParamValue() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<DBPwdParamValue> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //DB_AUTH_PARAM_H
