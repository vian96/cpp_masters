#pragma once

#include <concepts>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>

namespace HomeworkTwine {

class EmptyTwineChild {};

struct Twine;
inline std::ostream &operator<<(std::ostream &out, const Twine &rhs);

typedef std::variant<const char *, const std::string_view, const Twine *, EmptyTwineChild>
    TwineChildBase;

struct TwineChild : TwineChildBase {
    template <std::convertible_to<TwineChildBase> Arg>
    // template<typename Arg>
    TwineChild(Arg arg) : TwineChildBase(arg) {
        std::cout << "templated child: " << typeid(arg).name() << ' ' << arg << "\n";
    }

    TwineChild(const Twine &arg) : TwineChildBase(&arg) {
        std::cout << "const twine & child\n";
    }

    std::ostream &dump_impl(std::ostream &out) const;
};

inline std::ostream &operator<<(std::ostream &out, const TwineChild &rhs) {
    auto functor = [&](auto &arg) -> std::ostream & {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, TwineChild>)
            return out;
        else if constexpr (std::is_same_v<T, const Twine *>)
            return out << *arg;
        else
            return out << arg;
    };

    return std::visit(functor, rhs);
}

struct Twine {
    class EmptyChild {};

    TwineChild lhs;
    TwineChild rhs;

    // template <std::convertible_to<TwineChild> LT = EmptyChild,
    //           std::convertible_to<TwineChild> RT = EmptyChild>
    template <typename LT = EmptyChild, typename RT = EmptyChild>
    Twine(const LT &lhs_ = EmptyChild(), const RT &rhs_ = EmptyChild())
        : lhs(lhs_), rhs(rhs_) {
        std::cout << "!constr of " << 2 << '\n';
        std::cout << "lhs_: " << typeid(lhs_).name() << ' ' << lhs_ << '\n';
        std::cout << "rhs_: " << typeid(rhs_).name() << ' ' << rhs_ << '\n';
        dump_impl(std::cout) << '\n';
    }

    template <typename First, typename... Args>
        requires(sizeof...(Args) > 1)
    Twine(First first, Args... args) : Twine(Twine(first), Twine(args...)) {
        std::cout << "constr of " << sizeof...(Args) << '\n';
    }

    std::ostream &dump_impl(std::ostream &out) const {
        lhs.dump_impl(out << "lhs: {") << '}';
        return rhs.dump_impl(out << "rhs: {") << '}';
    }
};

inline std::ostream &TwineChild::dump_impl(std::ostream &out) const {
    if (std::holds_alternative<const char *>(*this))
        return out << "char*: " << (void *)std::get<const char *>(*this) << " ["
                   << std::get<const char *>(*this) << "] ";
    if (std::holds_alternative<const std::string_view>(*this))
        return out << "string_view: [" << std::get<const std::string_view>(*this) << "] ";
    if (std::holds_alternative<const Twine *>(*this))
        return std::get<const Twine *>(*this)->dump_impl(out << "twine: [\n") << "\n] ";
    if (std::holds_alternative<EmptyTwineChild>(*this)) return out << "empty: []";
    throw std::logic_error("unknown class");
}

inline std::ostream &operator<<(std::ostream &out, const Twine &rhs) {
    return out << rhs.lhs << rhs.rhs;
}

}  // namespace HomeworkTwine
