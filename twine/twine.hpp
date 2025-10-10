#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

#include "overloaded.hpp"

namespace HomeworkTwine {

template <class CharT, class Traits = std::char_traits<CharT>>
struct BasicTwine {
    class EmptyTwineChild {};

    using CharPtrT = const CharT *;
    using StrViewT = std::basic_string_view<CharT, Traits>;
    using StrT = std::basic_string<CharT, Traits>;

    using TwineChildBase =
        std::variant<CharPtrT, StrViewT, const BasicTwine *, EmptyTwineChild>;

    struct TwineChild : TwineChildBase {
        template <typename Arg>
        explicit TwineChild(const Arg &arg) : TwineChildBase(arg) {}

        explicit TwineChild(const BasicTwine &arg) : TwineChildBase(&arg) {}

        std::ostream &dump_impl(std::ostream &out) const;

        template <typename T>
        bool is_type() const {
            return std::holds_alternative<T>(*this);
        }

        size_t len() const {
            return std::visit(
                HomeworkCommon::overloaded{
                    [&](CharPtrT val) -> size_t { return Traits::length(val); },
                    [&](StrViewT val) -> size_t { return val.size(); },
                    [&](const BasicTwine *val) -> size_t { return val->len(); },
                    [&](EmptyTwineChild) -> size_t { return 0; }},
                *this);
        }

        void add_to_str(StrT &res) const {
            return std::visit(HomeworkCommon::overloaded{
                                  [&](CharPtrT val) { res.append(val); },
                                  [&](StrViewT val) { res.append(val); },
                                  [&](const BasicTwine *val) { val->add_to_str(res); },
                                  [&](EmptyTwineChild) {}},
                              *this);
        }
    };

    TwineChild lhs{EmptyTwineChild()};
    TwineChild rhs{EmptyTwineChild()};

    template <typename LT = EmptyTwineChild, typename RT = EmptyTwineChild>
    explicit BasicTwine(const LT &lhs_ = EmptyTwineChild(),
                        const RT &rhs_ = EmptyTwineChild())
        : lhs(lhs_), rhs(rhs_) {}

    std::ostream &dump_impl(std::ostream &out) const {
        lhs.dump_impl(out << "lhs: {") << '}';
        return rhs.dump_impl(out << "rhs: {") << '}';
    }

    BasicTwine operator+(const BasicTwine &rhs) const { return BasicTwine(*this, rhs); }

    size_t len() const { return lhs.len() + rhs.len(); }

    StrT str() const {
        StrT res;
        res.reserve(len());
        add_to_str(res);
        return res;
    }

    void add_to_str(StrT &res) const {
        lhs.add_to_str(res);
        rhs.add_to_str(res);
    }
};

template <class CharT, class Traits>
inline std::ostream &BasicTwine<CharT, Traits>::TwineChild::dump_impl(
    std::ostream &out) const {
    return std::visit(
        HomeworkCommon::overloaded{
            [&](CharPtrT val) -> std::ostream & {
                return out << "char*: " << (void *)val << " [" << val << "] ";
            },
            [&](StrViewT val) -> std::ostream & {
                return out << "string_view: [" << val << "] ";
            },
            [&](const BasicTwine *val) -> std::ostream & {
                return val->dump_impl(out << "twine: [" << &val << "\n") << "\n] ";
            },
            [&](EmptyTwineChild) -> std::ostream & { return out << "empty: []"; }},
        *this);
}

template <class CharT, class Traits>
std::ostream &operator<<(std::ostream &out,
                         const typename BasicTwine<CharT, Traits>::TwineChild &rhs);

template <class CharT, class Traits>
inline std::ostream &operator<<(std::ostream &out, const BasicTwine<CharT, Traits> &rhs) {
    return out << rhs.lhs << rhs.rhs;
}

template <class CharT, class Traits>
inline std::ostream &operator<<(
    std::ostream &out, const typename BasicTwine<CharT, Traits>::TwineChild &rhs) {
    auto functor = [&](auto &arg) -> std::ostream & {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T,
                                     typename BasicTwine<CharT, Traits>::EmptyTwineChild>)
            return out;
        else if constexpr (std::is_same_v<T, const BasicTwine<CharT, Traits> *>)
            return out << *arg;
        else
            return out << arg;
    };

    return std::visit(functor, rhs);
}

using Twine = BasicTwine<char>;
using WTwine = BasicTwine<wchar_t>;

}  // namespace HomeworkTwine
