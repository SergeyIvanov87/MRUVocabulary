#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <memory>
#include <ostream>
#include <string>
#include "interfaces/PluginCtx.h"
#include "BasePluginExports.h"

struct ValueBase;
using PluginCtxPtr = std::unique_ptr<plugin_ctx_t, release_plugin_ptr>;
struct IPlugin
{
     virtual ~IPlugin() noexcept {};

    virtual const std::string &getName() const = 0;
    virtual PluginCtxPtr initPluginCtx(const u_int8_t *data, size_t size) = 0;
    virtual bool setParam(PluginCtxPtr &ctx, const ValueBase& param, const void *data) = 0;
};
#endif
