#ifndef COMMANDARGUMENTS_VALUE_H
#define COMMANDARGUMENTS_VALUE_H

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

struct ValueBase;

class CommandArgumentsBase
{
public:
    using Argument = std::string;
    using ValuePtr = std::shared_ptr<ValueBase>;
    using AVP = std::map<Argument, ValuePtr>;
    using Filters = std::map<std::string, std::function<ValuePtr(int &argc, int &offset, const char *argv[])>>;
    using Deserializers = std::map<std::string, std::function<ValuePtr(int &argc, int &offset, const void *data[])>>;

    template<class CommandsImpl, class...Arguments>
    static std::unique_ptr<CommandsImpl> create_with_filters();

    template<class CommandsImpl, class...Arguments>
    static std::unique_ptr<CommandsImpl> create_with_deserialize();

    template<class CommandsImpl, class...Arguments>
    static std::unique_ptr<CommandsImpl> create();


    template<class PrimitiveType>
    size_t makeParamsExtract(int argc, const PrimitiveType *argv[]);

    template<class PrimitiveType>
    size_t makeParamsDeserialize(int argc, const PrimitiveType *argv[]);

    template<class ValueType>
    std::shared_ptr<ValueType> get() const;
private:

    template<class...Arguments>
    static Filters prepareFilters();

    template<class ...Arguments>
    static Deserializers prepareDeserializers();

    template<class RegisteredParams, class PrimitiveType>
    size_t parseParams(RegisteredParams f, int argc, const PrimitiveType *argv[]);

    AVP storage;

    Filters parseFilters;
    Deserializers deserializersFilters;
};
#endif //COMMANDARGUMENTS_VALUE_H
