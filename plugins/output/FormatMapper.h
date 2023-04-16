#ifndef FORMAT_MAPPER_H
#define FORMAT_MAPPER_H

#include <tuple>
#include <type_traits>

template<class F1, class ...F>
struct FormatList : public FormatList<F...>
{
    using base_t = FormatList<F...>;
    using impl_t = std::tuple<F1, F...>;

    static constexpr int elapsed_types_count()
    {
        return sizeof...(F);
    }

    template<class SearchType, int CurIndex = 0>
    static constexpr int index()
    {
        if constexpr(std::is_same_v<SearchType, F1>)
        {
            return CurIndex;
        }
        else
        {
            return base_t::template index<SearchType, CurIndex + 1>();
        }

    }
};

template<class Last>
struct FormatList<Last>
{
    static constexpr int elapsed_types_count()
    {
        return 0;
    }

    template<class SearchType, int CurIndex = 0>
    static constexpr int index()
    {
        static_assert(std::is_same_v<SearchType, Last>, "Invalid Type");
        return CurIndex;
    }
};


template<class Impl, class InList, class OutList>
struct FormatMapper
{
    using in_list_t = InList;
    using out_list_t = OutList;
/*
    template<class In, class Tracer>
    void map(In&& in_val, Tracer tracer)
    {
        using mapped_type = std::tuple_element_t<
                                    in_list_t::template index<
                                        std::remove_cv_t<std::remove_reference_t<In>>>(),
                                    typename out_list_t::impl_t>;
        static_cast<Impl*>(this)->template map_impl<mapped_type>(std::forward<In>(in_val), tracer);
    }
*/

    template<class In, class Tracer>
    void map(const In& in_val, Tracer tracer)
    {
        using mapped_type = std::tuple_element_t<
                                    in_list_t::template index<
                                        std::remove_cv_t<std::remove_reference_t<In>>>(),
                                    typename out_list_t::impl_t>;
        static_cast<Impl*>(this)->template map_impl(in_val,
                                                    std::integral_constant<int,
                                                                in_list_t::template index<
                                                                                          std::remove_cv_t<std::remove_reference_t<In>>>()>{},
                                                    tracer);
    }
};
#endif //FORMAT_MAPPER_H
