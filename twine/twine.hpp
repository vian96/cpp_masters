#pragma once

#include <concepts>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>

namespace HomeworkTwine {

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct Dumper {
    Dumper(const char *str) { std::cout << str << '\n'; }
    Dumper(int x) { std::cout << x << '\n'; }
};

class EmptyTwineChild {};

struct Twine;
inline std::ostream &operator<<(std::ostream &out, const Twine &rhs);

typedef std::variant<const char *, std::string_view, const Twine *, EmptyTwineChild>
    TwineChildBase;

struct TwineChild : TwineChildBase {
    template <typename Arg>
    explicit TwineChild(const Arg &arg) : TwineChildBase(arg) {
        std::cout << "templated child: " << typeid(arg).name() << ' ' << arg << "\n";
    }

    explicit TwineChild(const Twine &arg) : TwineChildBase(&arg) {
        std::cout << "const twine & child\n";
    }

    std::ostream &dump_impl(std::ostream &out) const;

    template <typename T>
    bool is_type() const {
        return std::holds_alternative<T>(*this);
    }
};

inline std::ostream &operator<<(std::ostream &out, const TwineChild &rhs) {
    auto functor = [&](auto &arg) -> std::ostream & {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, EmptyTwineChild>)
            return out;
        else if constexpr (std::is_same_v<T, const Twine *>)
            return out << *arg;
        else
            return out << arg;
    };

    return std::visit(functor, rhs);
}

struct Twine {
    Dumper dumper;

    TwineChild lhs{EmptyTwineChild()};
    TwineChild rhs{EmptyTwineChild()};

    // template <std::convertible_to<TwineChild> LT = EmptyChild,
    //           std::convertible_to<TwineChild> RT = EmptyChild>
    template <typename LT = EmptyTwineChild, typename RT = EmptyTwineChild>
    explicit Twine(const LT &lhs_ = EmptyTwineChild(), const RT &rhs_ = EmptyTwineChild())
        : dumper("constr of 2"), lhs(lhs_), rhs(rhs_) {
        // std::cout << "!constr of " << 2 << '\n';
        std::cout << "lhs_: " << typeid(lhs_).name() << ' ' << lhs_ << '\n';
        std::cout << "rhs_: " << typeid(rhs_).name() << ' ' << rhs_ << '\n';
        dump_impl(std::cout) << '\n';
    }

    template <typename First, typename... Args>
        requires(sizeof...(Args) > 1)
    explicit Twine(const First &first, const Args &...args)
        : Twine(first, Twine(args...)) {
        std::cout << "constr of many: " << sizeof...(Args) + 1 << '\n';
        dump_impl(std::cout) << '\n';
    }

    std::ostream &dump_impl(std::ostream &out) const {
        lhs.dump_impl(out << "lhs: {") << '}';
        return rhs.dump_impl(out << "rhs: {") << '}';
    }

    Twine operator+(const Twine &rhs) { return Twine(*this, rhs); }
};

inline std::ostream &TwineChild::dump_impl(std::ostream &out) const {
    return std::visit(
        overloaded{[&](const char *val) -> std::ostream & {
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

inline std::ostream &operator<<(std::ostream &out, const Twine &rhs) {
    #ifdef DEBUG
    std::cout << "oper<< twine\n";
    rhs.dump_impl(std::cout) << '\n';
    #endif
    return out << rhs.lhs << rhs.rhs;
}

}  // namespace HomeworkTwine
