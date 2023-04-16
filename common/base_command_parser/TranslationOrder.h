#ifndef TRANSLATION_ORDER_VALUE_H
#define TRANSLATION_ORDER_VALUE_H

#include "common/base_command_parser/ValueBase.h"


class TranslationOrder final: public TypedValue<std::string>

{
public:
    using base = TypedValue<std::string>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "tr_order";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    TranslationOrder(const std::string&);
    ~TranslationOrder() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<TranslationOrder> create(int &argc, int &arg_offset, const char* argv[]);
};
#endif //TRANSLATION_ORDER_VALUE_H
