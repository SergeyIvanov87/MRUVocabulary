#ifndef TRANSLATOR_SHARED_DATA_IMPL_V0_HPP
#define TRANSLATOR_SHARED_DATA_IMPL_V0_HPP
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "ISharedTranslatedData.h"
#include <txml/applications/xdxf/xdxf_fwd.h>

#include "../translators/vers/TranslatedDataStructure_v0.h"
namespace v0
{
    using OutputSessionCtx = TranslatedDataStructure;

    void format_serialize(const OutputSessionCtx& data, std::ostream &out, const std::string &format,txml::EmptyTracer &tracer);
    void format_serialize(const OutputSessionCtx& data, std::ostream &out, const std::string &format, txml::StdoutTracer &tracer);
}
#endif // TRANSLATOR_SHARED_DATA_IMPL_V0_HPP
