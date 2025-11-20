#include <variant>
#include <vector>

#include "compile_sort.hpp"

namespace CompileSortTests {

using namespace CompileSort;

template <typename T>
using IsBigger3 = IsBigger<T, 3>;

using x = std::variant<int, char, long long, float, double, short>;
static_assert(
    std::is_same_v<Sort<x>::T, std::variant<char, short, int, float, long long, double>>,
    "");

static_assert(
    std::is_same_v<GetIf<x, IsBigger3>::T, std::variant<int, long long, float, double>>,
    "");

template <typename T>
concept HasT = requires() { T::T; };

static_assert(!HasT<MyTuple<short, void, int>>, "");

}  // namespace CompileSortTests

using X = std::variant<float, float*, int, char, short>;

template <typename... Args>
struct IncompleteTuple;

int main() {
    [[maybe_unused]] CompileSort::Sort<X>::T y;
    // CompileSort::Sort<CompileSort::MyTuple<void>>::T hh; // static assertion failed:
    // non-sizeofable objects are not allowed

    // CompileSort::Sort<int>::T ii;  // static assertion failed: Non-tuple-like types are
    // not allowed

    // typename CompileSort::Sort<std::vector<int>>::T::allocator_type vv;
    [[maybe_unused]] CompileSort::Sort<typename CompileSort::ToMyTuple<
        CompileSort::Sort<IncompleteTuple<int, float, short>>::T>::T> g;
}
