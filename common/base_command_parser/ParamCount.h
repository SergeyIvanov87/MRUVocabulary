#if 0
#ifndef PARAM_COUNT_H
#define PARAM_COUNT_H

#include <stdlib.h>
#include "common/base_command_parser/ValueBase.h"

class ParamCount final: public TypedValue<size_t>

{
public:
    using base = TypedValue<size_t>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "param_count";
    }

    static const std::set<std::string> &modifiers()
    {
        static const std::set<std::string> mods;
        return mods;
    }

    template<class ...Params>
    ParamCount(const std::shared_ptr<Params> &...params) :
        base(0)
    {
        bool expander[]
        {
            ((bool)params)...
        };
        val = std::count(std::begin(expander), std::end(expander), true);
    }

    ParamCount(size_t count) :
        base(count)
    {
    }

    ~ParamCount() = default;

    const char *name() const noexcept override
    {
        return class_name();
    }

    static std::shared_ptr<ParamCount> create(int &argc, int &arg_offset, const char* argv[])
    {
        int processed_arguments = 0;

        processed_arguments++; //skip cmd
        (void)processed_arguments; //no modifier

        std::shared_ptr<ParamCount> ret = ValueBase::create<ParamCount>(atoll(argv[arg_offset + processed_arguments]));

        processed_arguments++; //processed value

        argc -= processed_arguments;
        arg_offset += processed_arguments;

        return ret;
    }
};

#endif //PARAM_COUNT_H

#endif
