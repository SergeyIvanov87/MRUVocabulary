#ifndef VALUEBASE_VALUE_H
#define VALUEBASE_VALUE_H

#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <optional>
#include <vector>

struct ValueBase : public std::enable_shared_from_this<ValueBase>
{
    using modifiers_t = std::optional<std::vector<std::string>>;

    virtual const char *name() const noexcept = 0;

    std::shared_ptr<ValueBase> get_ptr();

    template<class Value, class ...Args>
    static std::shared_ptr<Value> create(Args &&...args);

    template<class Value>
    static std::shared_ptr<Value> deserialize(int &argc, int &arg_offset, const void* argv[]);

    template<class Value>
    static size_t pack(const Value &param, std::ostream &out);

    template<class Value>
    static size_t pack(const std::shared_ptr<Value> &param, std::ostream &out);

    template<class Value>
    static std::shared_ptr<Value> unpack(std::istream &in);
protected:
    ValueBase() = default;
    ~ValueBase() = default;
};


template<class ValueImpl>
struct TypedValue : public ValueBase
{
    using value_t = ValueImpl;

    TypedValue(value_t v);
    value_t getValue() const;
protected:
    value_t val;
};

template<>
struct TypedValue<std::string> : public ValueBase
{
    using value_t = std::string;

    TypedValue(value_t v);
    value_t getValue() const;
private:
    value_t val;
};


class ParamCount final : public TypedValue<size_t>
{
public:
    using base = TypedValue<size_t>;
    using value_t = typename base::value_t;

    static constexpr const char* class_name()
    {
        return "param_count";
    }

    static const std::set<std::string> &modifiers();

    ParamCount(size_t count);

    template<class ...Params>
    ParamCount(const std::shared_ptr<Params> &...params) :
        ParamCount(sizeof...(params))
    {
    }

    const char *name() const noexcept override;

    static std::shared_ptr<ParamCount> create(int &argc, int &arg_offset, const char* argv[]);
};


template<class ...Values>
static size_t pack_values(std::ostream &out, const std::shared_ptr<Values>&...params)
{
    ParamCount param_count(sizeof...(params));
    size_t expander[]
    {
        (ValueBase::pack(param_count, out), size_t(0)),
        (params ? ValueBase::pack(params, out) : size_t(0)) ...
    };
    return std::accumulate(std::begin(expander), std::end(expander), 0);
}

#endif //VALUEBASE_VALUE_H
