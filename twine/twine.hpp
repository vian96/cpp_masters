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

struct Twine {
    class EmptyTwineChild {};

    using TwineChildBase =
        std::variant<const char *, std::string_view, const Twine *, EmptyTwineChild>;

    struct TwineChild : TwineChildBase {
        template <typename Arg>
        explicit TwineChild(const Arg &arg) : TwineChildBase(arg) {}

        explicit TwineChild(const Twine &arg) : TwineChildBase(&arg) {}

        std::ostream &dump_impl(std::ostream &out) const;

        template <typename T>
        bool is_type() const {
            return std::holds_alternative<T>(*this);
        }

        size_t len() const {
            return std::visit(
                HomeworkCommon::overloaded{
                    [&](const char *val) -> size_t { return std::strlen(val); },
                    [&](std::string_view val) -> size_t { return val.size(); },
                    [&](const Twine *val) -> size_t { return val->len(); },
                    [&](EmptyTwineChild) -> size_t { return 0; }},
                *this);
        }

        void add_to_str(std::string &res) const {
            return std::visit(HomeworkCommon::overloaded{
                                  [&](const char *val) { res.append(val); },
                                  [&](std::string_view val) { res.append(val); },
                                  [&](const Twine *val) { val->add_to_str(res); },
                                  [&](EmptyTwineChild) {}},
                              *this);
        }
    };

    TwineChild lhs{EmptyTwineChild()};
    TwineChild rhs{EmptyTwineChild()};

    template <typename LT = EmptyTwineChild, typename RT = EmptyTwineChild>
    explicit Twine(const LT &lhs_ = EmptyTwineChild(), const RT &rhs_ = EmptyTwineChild())
        : lhs(lhs_), rhs(rhs_) {}

    std::ostream &dump_impl(std::ostream &out) const {
        lhs.dump_impl(out << "lhs: {") << '}';
        return rhs.dump_impl(out << "rhs: {") << '}';
    }

    Twine operator+(const Twine &rhs) const { return Twine(*this, rhs); }

    size_t len() const { return lhs.len() + rhs.len(); }

    std::string str() const {
        std::string res;
        res.reserve(len());
        add_to_str(res);
        return res;
    }

    void add_to_str(std::string &res) const {
        lhs.add_to_str(res);
        rhs.add_to_str(res);
    }
};

inline std::ostream &Twine::TwineChild::dump_impl(std::ostream &out) const {
    return std::visit(
        HomeworkCommon::overloaded{
            [&](const char *val) -> std::ostream & {
                return out << "char*: " << (void *)val << " [" << val << "] ";
            },
            [&](std::string_view val) -> std::ostream & {
                return out << "string_view: [" << val << "] ";
            },
            [&](const Twine *val) -> std::ostream & {
                return val->dump_impl(out << "twine: [" << &val << "\n") << "\n] ";
            },
            [&](EmptyTwineChild) -> std::ostream & { return out << "empty: []"; }},
        *this);
}

std::ostream &operator<<(std::ostream &out, const Twine::TwineChild &rhs);

inline std::ostream &operator<<(std::ostream &out, const Twine &rhs) {
    return out << rhs.lhs << rhs.rhs;
}

inline std::ostream &operator<<(std::ostream &out, const Twine::TwineChild &rhs) {
    auto functor = [&](auto &arg) -> std::ostream & {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Twine::EmptyTwineChild>)
            return out;
        else if constexpr (std::is_same_v<T, const Twine *>)
            return out << *arg;
        else
            return out << arg;
    };

    return std::visit(functor, rhs);
}

}  // namespace HomeworkTwine
