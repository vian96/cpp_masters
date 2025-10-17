#include <cmath>
#include <complex>
#include <concepts>
#include <type_traits>

#include "gtest/gtest.h"

template <typename T>
concept MultiplyableBySelf = requires(T t) { t *= t; };

template <MultiplyableBySelf T>
T nth_power(T x, T acc, unsigned n) {
    while (n > 0) {
        if ((n & 0x1) == 0x1) {
            acc *= x;
            n -= 1;
        }
        x *= x;
        n /= 2;
    }
    return acc;
}

template <typename T>
// remove ambiguity with general overload
    requires std::floating_point<T> && std::convertible_to<int, T> &&
             MultiplyableBySelf<T> && std::copyable<T>
T nth_power(T x, unsigned n) {
    // old cpp standard had special overload for calls like pow(float, int)
    // but it was removed due to overload collisions
    // hopefully, pow still handles it more efficient than our own implementation
    // but rewriting it for your own implementation is quite easy
    return std::pow(x, n);
}

template <typename T>
    requires std::is_unsigned_v<T> && std::integral<T>
T nth_power(T x, unsigned n) {
    if (x < T(2) || n == 1u) return x;
    return nth_power<T>(x, T(1), n);
}

// code is the same for signed integers and int-like, so i just merge 2 overloads together
// non-copyable types are weird and the code inside is not for them
template <typename T>
    requires(std::is_signed_v<T> && std::integral<T>) ||
            (std::convertible_to<int, T> && MultiplyableBySelf<T> && std::copyable<T>)
T nth_power(T x, unsigned n) {
    T acc = T(1);
    if (x == acc || n == 1u) return x;
    return nth_power<T>(x, acc, n);
}

// NOT sure if it is a good idea to use default template argument:
// probably may lead to unexpected behaviour when calling function
// if such call would be ambigous (eye of an animal)
// which would make it a BAD overload set
// so probably it's better to remove it, but removing is easier than writing so i left it
// non-copyable types are weird and the code inside is not for them
template <typename T, auto NeutralElementFactory = T::eye>
    requires(MultiplyableBySelf<T> && std::copyable<T>)
T nth_power(T x, unsigned n) {
    T acc = NeutralElementFactory();
    if (x == acc || n == 1u) return x;
    return nth_power<T>(x, acc, n);
}

struct Matrix2x2 {
    int n[2][2];
    bool operator==(const Matrix2x2& m) const {
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                if (n[i][j] != m.n[i][j]) return false;
        return true;
    }
    Matrix2x2& operator*=(const Matrix2x2& m) {
        Matrix2x2 r;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                int acc = 0;
                for (int k = 0; k < 2; k++) acc += n[i][k] * m.n[k][j];
                r.n[i][j] = acc;
            }
        }
        *this = r;
        return *this;
    }
};

struct EyebleMatrix2x2 : Matrix2x2 {
    static EyebleMatrix2x2 eye() { return {1, 0, 0, 1}; }
};

// example of explicit specialization for other types
Matrix2x2 nth_power(Matrix2x2 x, unsigned n) {
    Matrix2x2 eye{1, 0, 0, 1};
    if (x == eye || n == 1u) return x;
    return nth_power<Matrix2x2>(x, eye, n);
}

TEST(nthpower_test_case, testWillPass) {
    EXPECT_EQ(nth_power(2, 11), (1 << 11));
    EXPECT_EQ(nth_power(2.0, 11), (1 << 11));
    EXPECT_EQ(nth_power(2.0, 11), (1 << 11));
    Matrix2x2 m{1, 0, 0, 1};
    EXPECT_EQ(nth_power(m, 5), m);
    auto res = nth_power<Matrix2x2, []() { return Matrix2x2{1, 0, 0, 1}; }>(m, 5);
    EXPECT_EQ(res, m);
    std::complex<float> c(1, 1);
    EXPECT_EQ(nth_power(c, 4), std::complex<float>(-4));
    EyebleMatrix2x2 m2{1, 0, 0, 1};
    EXPECT_EQ(nth_power(m2, 5), m2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
