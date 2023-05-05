#ifndef SETTINGS_H
#define SETTINGS_H
#include "common/base_command_parser/CommandArguments.h"

class Settings : public Resources::IBaseResource<Settings>,
                 public ISerializableIntrusive<Settings>,
                 public CommandArgumentsBase
{
public:
    using Base = Resources::IBaseResource<Settings>;
    using Base::ResourcesMap;


    ~Settings();

    //Interfaces Decl
    //ISerializable
    size_t onSerialize(std::ostream &out);
    size_t onDeserialize(std::istream &out);

    //IBaseResource
    constexpr static const char *getResourceTypeDescriptionImpl()
    {
        return "Settings";
    }
    template<class Tracer = Tracer<EmptyTracerImpl>>
    static ResourcesMap loadResourcesImpl(const std::string &resourcePath, Tracer tracer);
};
#endif //SETTINGS_H
