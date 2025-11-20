#include <variant>
#include <vector>

#include "compile_sort.hpp"

namespace CompileSortTests {

using namespace CompileSort;

// WARNING: on other platform with other sizes, tests may file

template <typename T>
using IsBigger3 = IsBiggerEq<T, 3>;

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

template <typename... Args>
using TestTuple = std::tuple<Args...>;

using Empty = TestTuple<>;
static_assert(std::is_same_v<Sort<Empty>::T, Empty>, "Test Failed: Empty list");

using Single = TestTuple<long long>;
static_assert(std::is_same_v<Sort<Single>::T, Single>,
              "Test Failed: Single element list");

using PreSorted = TestTuple<char, short, int, double>;
static_assert(std::is_same_v<Sort<PreSorted>::T, PreSorted>,
              "Test Failed: Already sorted list");

using ReverseSorted = TestTuple<double, long long, int, short, char>;
static_assert(std::is_same_v<Sort<ReverseSorted>::T,
                             TestTuple<char, short, int, double, long long>>,
              "Test Failed: Reverse sorted list");

// sizeof(int) == sizeof(float) on most common platforms
// sizeof(SomeStruct) == sizeof(int)
// sizeof(void*) == sizeof(long long) on 64-bit platforms
struct SomeStruct {
    int x;
};

using StabilityCheck =
    TestTuple<double, int, SomeStruct, float, char, void*, short, long long>;
using ExpectedStable =
    TestTuple<char, short, int, SomeStruct, float, double, void*, long long>;
static_assert(std::is_same_v<Sort<StabilityCheck>::T, ExpectedStable>,
              "Test Failed: Stability check with same-sized types");

struct Struct1 {
    char a;
};
struct Struct2 {
    short a;
};
struct Struct4A {
    int a;
};
struct Struct4B {
    float a;
};
struct Struct8A {
    double a;
};
struct Struct8B {
    long long b;
};

using StabilityCheckCustom =
    TestTuple<Struct8A, Struct4A, Struct1, Struct8B, Struct4B, Struct2>;
using ExpectedStableCustom =
    TestTuple<Struct1, Struct2, Struct4A, Struct4B, Struct8A, Struct8B>;
static_assert(std::is_same_v<Sort<StabilityCheckCustom>::T, ExpectedStableCustom>,
              "Test Failed: Stability check with custom structs");

template <typename T>
using SizeIs4 = std::bool_constant<sizeof(T) == 4>;

using GetIfSource = TestTuple<char, int, double, float, short, SomeStruct>;

using FilteredTo4 = TestTuple<int, float, SomeStruct>;
static_assert(std::is_same_v<GetIf<GetIfSource, SizeIs4>::T, FilteredTo4>,
              "Test Failed: GetIf multiple matches");

template <typename T>
using SizeIs3 = std::bool_constant<sizeof(T) == 3>;
static_assert(std::is_same_v<GetIf<GetIfSource, SizeIs3>::T, TestTuple<>>,
              "Test Failed: GetIf no matches");

static_assert(std::is_same_v<GetIf<TestTuple<>, SizeIs4>::T, TestTuple<>>,
              "Test Failed: GetIf on empty list");

using PtrTypes = TestTuple<double*, int*, char**, char*, int**>;
static_assert(std::is_same_v<Sort<PtrTypes>::T, PtrTypes>,
              "Test Failed: Pointer types were not handled correctly or stability failed.");

using ArrayTypes = TestTuple<int[10], char[5], int[2], double[100]>; // sizes: 40, 5, 8, 800
using ArrayExpected = TestTuple<char[5], int[2], int[10], double[100]>;
static_assert(std::is_same_v<Sort<ArrayTypes>::T, ArrayExpected>,
              "Test Failed: C-style array types were not sorted correctly.");

using MixedBag = TestTuple<long long, int[10], const char*, char[5]>; // 8, 40, 8, 5
using MixedBagExpected = TestTuple<char[5], long long, const char*, int[10]>; // 5, 8, 8, 40
static_assert(std::is_same_v<Sort<MixedBag>::T, MixedBagExpected>,
              "Test Failed: Mixed complex types failed sorting or stability.");

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
