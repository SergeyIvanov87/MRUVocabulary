#ifndef OUTPUT_PLUGIN_WRAPPER_H
#define OUTPUT_PLUGIN_WRAPPER_H
#include <plugins/output/OutputInterface.h>
#include <plugins/output/OutputExport.h>
#include "PluginHolder.h"

class OutputPluginWrapper : public IOutputPlugin,
                            public PluginHolder
{
public:
    using OutputPluginPtr = std::shared_ptr<OutputPluginWrapper>;

    OutputPluginWrapper(void *handle);
    OutputPluginWrapper(OutputPluginWrapper &&src) = default;
    OutputPluginWrapper(PluginHolder &&src);
    ~OutputPluginWrapper() noexcept;

    long long writeTranslatedData(PluginCtxPtr &ctx, SessionPtr &translated_ctx) override;

    static OutputPluginPtr loadPlugin(const std::string &fileName);
private:
    write_translated_data_ptr write_decoded_data_function;

};
#endif //OUTPUT_PLUGIN_WRAPPER_H
