#pragma once

namespace HomeworkCommon {

/* for easy overload of lambdas */
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace HomeworkCommon
