#pragma once

#include <type_traits>

namespace CompileSort {

template <typename T, std::size_t N>
struct IsBiggerEq : std::bool_constant<(sizeof(T) >= N)> {};

template <typename T, std::size_t N>
struct IsLess : std::bool_constant<(sizeof(T) < N)> {};

// for readable message after invalid sorting with templates like vector
template <template <typename...> typename Tmpl, typename... Args>
concept CanBeInstantiatedWith = requires {
    typename Tmpl<Args...>;

    // force instantiation - but it is a little overkill
    //   - on one side, makes error of instatioation vector<allocator, int> much more
    //   readable
    //   - on the other side, makes some code non-compilable, see last line in main.cpp
    // so i decided to remove it: c++ programmers must be able to get through errors
    // sizeof(Tmpl<Args...>);
};

// to make specialization with 0 params
template <typename... Args>
struct MyTuple;

template <typename Arg, typename... Args>
struct MyTuple<Arg, Args...> {
    using T = Arg;
    using Child = MyTuple<Args...>;

    template <template <typename...> typename T>
        requires CanBeInstantiatedWith<T, Arg, Args...>
    using UnWrap = T<Arg, Args...>;
};

template <>
struct MyTuple<> {
    template <template <typename...> typename T>
        requires CanBeInstantiatedWith<T>
    using UnWrap = T<>;
};

template <typename, typename>
struct Cat;
template <typename... First, typename... Second>
struct Cat<MyTuple<First...>, MyTuple<Second...>> {
    using T = MyTuple<First..., Second...>;
};

namespace CompileSortImpl {

template <typename Tuple, template <typename> typename Pred>
struct GetIfImpl;

template <typename Tuple, template <typename> typename Pred, typename Ok>
struct GetIfImplCond {
    using T = typename GetIfImpl<typename Tuple::Child, Pred>::T;
};

template <typename Tuple, template <typename> typename Pred>
struct GetIfImplCond<Tuple, Pred, std::true_type> {
    using T = typename Cat<MyTuple<typename Tuple::T>,
                           typename GetIfImpl<typename Tuple::Child, Pred>::T>::T;
};

// repeating for removing ambiguity
template <template <typename> typename Pred>
struct GetIfImplCond<MyTuple<>, Pred, std::true_type> {
    using T = MyTuple<>;
};

template <template <typename> typename Pred>
struct GetIfImplCond<MyTuple<>, Pred, std::false_type> {
    using T = MyTuple<>;
};

template <typename Tuple, template <typename> typename Pred>
struct GetIfImpl {
    using T =
        typename GetIfImplCond<Tuple, Pred, typename Pred<typename Tuple::T>::type>::T;
};

template <template <typename> typename Pred>
struct GetIfImpl<MyTuple<>, Pred> {
    using T = MyTuple<>;
};

template <typename Tuple>
struct SortImpl {
    using SortedSub = typename SortImpl<typename Tuple::Child>::T;
    using X = typename Tuple::T;

    template <typename T>
    using Less = IsLess<T, sizeof(X)>;
    template <typename T>
    using More = IsBiggerEq<T, sizeof(X)>;

    using Left = typename GetIfImpl<SortedSub, Less>::T;
    using Right = typename GetIfImpl<SortedSub, More>::T;

    using T = typename Cat<Left, typename Cat<MyTuple<X>, Right>::T>::T;
};

template <>
struct SortImpl<MyTuple<>> {
    using T = MyTuple<>;
};

}  // namespace CompileSortImpl

template <typename T>
concept SizeOfAble = requires() { sizeof(T); };

template <typename Tuple>
struct Sort {
    // dangerous but works in modern c++
    static_assert(false, "Non-tuple-like types are not allowed");
};

template <template <typename...> typename Tuple, typename... Args>
struct Sort<Tuple<Args...>> {
    static_assert(false, "non-sizeofable objects are not allowed");
};

template <template <typename...> typename Tuple, typename... Args>
    requires(SizeOfAble<Args> && ...)
struct Sort<Tuple<Args...>> {
    using T =
        typename CompileSortImpl::SortImpl<MyTuple<Args...>>::T::template UnWrap<Tuple>;
};

template <typename Tuple, template <typename> typename Pred>
struct GetIf {
    static_assert(false, "Non-tuple-like types are not allowed");
};

template <template <typename...> typename Tuple, template <typename> typename Pred,
          typename... Args>
    requires(SizeOfAble<Args> && ...)
struct GetIf<Tuple<Args...>, Pred> {
    using T = typename CompileSortImpl::GetIfImpl<MyTuple<Args...>,
                                                  Pred>::T::template UnWrap<Tuple>;
};

template <typename Tuple>
struct ToMyTuple;

template <template <typename...> typename Tuple, typename... Args>
struct ToMyTuple<Tuple<Args...>> {
    using T = MyTuple<Args...>;
};

}  // namespace CompileSort
