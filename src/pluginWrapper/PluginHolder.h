#ifndef PLUGIN_HOLDER_H
#define PLUGIN_HOLDER_H
#include <memory>
#include <string>
#include "plugins/interfaces/PluginInterface.h"

#include "common/base_command_parser/ValueBase.h"


class PluginHolder : virtual public IPlugin
{
public:
    using PluginHolderPtr = std::shared_ptr<PluginHolder>;
    using PluginCtxPtr = std::unique_ptr<plugin_ctx_t, release_ptr>;

    PluginHolder(void *handle);
    PluginHolder(PluginHolder &&src);
    virtual ~PluginHolder() noexcept;

    const std::string &getName() const override;
    PluginCtxPtr initPluginCtx(const u_int8_t *data, size_t size) override;

    template<class ValueImpl>
    bool setTypedParam(PluginCtxPtr &ctx, const TypedValue<ValueImpl> &param)
    {
        return setParam(ctx, param, &param);
    }


    template<class ExportFunction>
    ExportFunction exportFunction(const std::string &functionName) const
    {
        ExportFunction f = nullptr;
        exportImpl(reinterpret_cast<void**>(&f), functionName);
        return f;
    }

    static PluginHolderPtr loadPlugin(const std::string &fileName);
private:
    void exportImpl(void **exported, const std::string &name) const;
    void *dl_handle;
    init_ptr init_function;
    set_typed_params_ptr set_typed_params_function;
    release_ptr releases_function;

    std::string name;
    bool setParam(PluginCtxPtr &ctx, const ValueBase& param, const void *data) override;
};
#endif //PLUGIN_HOLDER_H
