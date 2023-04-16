#ifndef TYPED_VALUE_HPP
#define TYPED_VALUE_HPP

#include "common/base_command_parser/ValueBase.hpp"

template<class ValueImpl>
inline TypedValue<ValueImpl>::TypedValue(value_t v) :
    val(std::move(v))
{
}

template<class ValueImpl>
inline typename TypedValue<ValueImpl>::value_t TypedValue<ValueImpl>::getValue() const
{
    /*std::stringstream ss;
    ss << val;
    value_t ret;
    ss >> ret;
    return ret;
    */
    return val;
}



inline TypedValue<std::string>::TypedValue(value_t v) :
    val(std::move(v))
{
}

inline typename TypedValue<std::string>::value_t TypedValue<std::string>::getValue() const
{
    return val;
}

#endif
