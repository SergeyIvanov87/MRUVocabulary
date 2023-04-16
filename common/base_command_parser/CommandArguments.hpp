#ifndef COMMANDARGUMENTS_VALUE_HPP
#define COMMANDARGUMENTS_VALUE_HPP

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "common/base_command_parser/CommandArguments.h"
#include "common/base_command_parser/TypedValue.hpp"

template<class...Arguments>
CommandArgumentsBase::Filters CommandArgumentsBase::prepareFilters()
{
    Filters filters
    {
        {
            Arguments::class_name(), [](int &argc, int &offset, const char *argv[]) -> std::shared_ptr<ValueBase>
                                     {
                                          return Arguments::create(argc, offset, argv);
                                     }
        }...
    };
    return filters;
}

template<class ...Arguments>
CommandArgumentsBase::Deserializers CommandArgumentsBase::prepareDeserializers()
{
    Deserializers deser
    {
        {
            Arguments::class_name(), [](int &argc, int &offset, const void *data[]) -> std::shared_ptr<ValueBase>
                                     {
                                         return ValueBase::deserialize<Arguments>(argc, offset, data);
                                     }
        }...
    };

    return deser;
}

template<class CommandsImpl, class...Arguments>
std::unique_ptr<CommandsImpl> CommandArgumentsBase::create_with_filters()
{
    static_assert(std::is_base_of<CommandArgumentsBase, CommandsImpl>::value,
                  "CommandsImpl should inherit CommandArgumentsBase");

    std::unique_ptr<CommandsImpl> args(new CommandsImpl);
    args->parseFilters = CommandArgumentsBase::prepareFilters<Arguments...>();

    return args;
}

template<class CommandsImpl, class...Arguments>
std::unique_ptr<CommandsImpl> CommandArgumentsBase::create_with_deserialize()
{
    static_assert(std::is_base_of<CommandArgumentsBase, CommandsImpl>::value,
                      "CommandsImpl should inherit CommandArgumentsBase");

    std::unique_ptr<CommandsImpl> args(new CommandsImpl);
    args->deserializersFilters = CommandArgumentsBase::prepareDeserializers<Arguments...>();

    return args;
}


template<class CommandsImpl, class...Arguments>
std::unique_ptr<CommandsImpl> CommandArgumentsBase::create()
{
    static_assert(std::is_base_of<CommandArgumentsBase, CommandsImpl>::value,
                  "CommandsImpl should inherit CommandArgumentsBase");

    std::unique_ptr<CommandsImpl> args(new CommandsImpl);
    args->parseFilters = CommandArgumentsBase::prepareFilters<Arguments...>();
    args->deserializersFilters = CommandArgumentsBase::prepareDeserializers<Arguments...>();

    return args;
}

template<class ValueType>
std::shared_ptr<ValueType> CommandArgumentsBase::get() const
{
    if(auto it = storage.find(ValueType::class_name()); it != storage.end())
    {
        return std::static_pointer_cast<ValueType>(it->second);
    }
    return {};
}


template<class PrimitiveType>
size_t CommandArgumentsBase::makeParamsExtract(int argc, const PrimitiveType *argv[])
{
    return parseParams(parseFilters, argc, argv);
}

template<class PrimitiveType>
size_t CommandArgumentsBase::makeParamsDeserialize(int argc, const PrimitiveType *argv[])
{
    return parseParams(deserializersFilters, argc, argv);
}


template<class RegisteredParams, class PrimitiveType>
size_t CommandArgumentsBase::parseParams(RegisteredParams f, int argc, const PrimitiveType *argv[])
{
    int argc_tmp = argc;
    int arg_offset = 0;

    size_t count = 0;

    while(argc_tmp and !f.empty())
    {
        if(auto it = f.find(static_cast<const char*>(argv[arg_offset])); it != f.end())
        {
            storage.emplace(it->first, it->second(argc_tmp, arg_offset, argv));
            f.erase(it);
            count ++;
        }
        else
        {
            throw std::runtime_error(std::string("Invalid argument: ") +
                                     static_cast<const char*>(argv[arg_offset]));
        }
    }

    return count;
}

#endif //COMMANDARGUMENTS_VALUE_HPP
