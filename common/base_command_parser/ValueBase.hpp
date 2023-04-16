#ifndef VALUEBASE_VALUE_HPP
#define VALUEBASE_VALUE_HPP

#include "common/base_command_parser/ValueBase.h"
#include <Framework/SerializeFramework/SerializeUtilsCommon.hpp>

inline std::shared_ptr<ValueBase> ValueBase::get_ptr()
{
    return shared_from_this();
}

template<class Value, class ...Args>
inline std::shared_ptr<Value> ValueBase::create(Args &&...args)
{
    return std::make_shared<Value>(std::forward<Args>(args)...);
}

template<class Value>
inline std::shared_ptr<Value> ValueBase::deserialize(int &argc, int &arg_offset, const void* argv[])
{
    int processed_arguments = 0;

    const std::string got_name(static_cast<const char*>(argv[arg_offset]));
    if(got_name != Value::class_name())
    {
        throw std::runtime_error(std::string("Invalid deserialize attributes! Expected: " ) + Value::class_name() +
                                 ", got: " + got_name);
    }

    processed_arguments++; //skip name
    Value* ptr = reinterpret_cast<Value*>(const_cast<void*>(argv[arg_offset + processed_arguments]));
    processed_arguments++; //processed ptr

    std::shared_ptr<Value> ret = std::dynamic_pointer_cast<Value>(ptr->get_ptr());

    if (!ret)
    {
        throw std::runtime_error(std::string("Cannot obtain shared pointer from deserialized base! For derived type: " ) +
                                 Value::class_name());
    }
    argc -= processed_arguments;
    arg_offset += processed_arguments;
    return ret;
}


template<class Value>
size_t ValueBase::pack(const Value &param, std::ostream &out)
{
    //static_assert(not std::is_class_v<typename Value::value_t>, "Only primitive types can be packed");
    auto val = param.getValue();
    serializeParams(out, val);
    return sizeof(val);
}

template<class Value>
size_t ValueBase::pack(const std::shared_ptr<Value> &param, std::ostream &out)
{
    //static_assert(not std::is_class_v<typename Value::value_t>, "Only primitive types can be packed");
    if (!param)
    {
        return 0;
    }
    auto val = param->getValue();
    serializeParams(out, val);
    return sizeof(val);
}

template<class Value>
std::shared_ptr<Value> ValueBase::unpack(std::istream &in)
{
    //static_assert(not std::is_class_v<typename Value::value_t>, "Only primitive types can be unpacked");
    typename Value::value_t val{};
    deserializeUnit(in, val);

    std::shared_ptr<Value> param = std::make_shared<Value>(val);
    return param;
}





inline const std::set<std::string> &ParamCount::modifiers()
{
    static const std::set<std::string> mods;
    return mods;
}

inline ParamCount::ParamCount(size_t count) :
    base(count)
{
}

inline const char *ParamCount::name() const noexcept
{
    return class_name();
}

inline std::shared_ptr<ParamCount> ParamCount::create(int &argc, int &arg_offset, const char* argv[])
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

#endif //VALUEBASE_VALUE_H
