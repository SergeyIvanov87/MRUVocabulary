#ifndef SETTINGS_H
#define SETTINGS_H
#include "common/base_command_parser/CommandArguments.h"

class Settings : public Resources::IBaseResource<Settings>,
                 public ISerializable<Settings>,
                 public CommandArgumentsBase
{
public:
    using Base = Resources::IBaseResource<Settings>;
    using Base::ResourcesMap;

    enum
    {
        isSerializableSupport = true,
        isDumpObjectSupport = false
    };


    ~Settings();

    //Interfaces Decl
    //ISerializable
    bool serializeImpl(std::ostream &out);
    bool deserializeImpl(std::istream &out);

    //IBaseResource
    constexpr static const char *getResourceTypeDescriptionImpl()
    {
        return "Settings";
    }
    template<class Tracer = Tracer<EmptyTracerImpl>>
    static ResourcesMap loadResourcesImpl(const std::string &resourcePath, Tracer tracer);
};
#endif //SETTINGS_H
