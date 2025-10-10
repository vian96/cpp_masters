#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "cowstr.hpp"

void tests() {
    HomeworkCOW::CowStr x{"aboba"};
    assert(std::string("aboba") == x.c_str());
    x.append("very long word or sentence idk what");
    std::string longstr{"abobavery long word or sentence idk what"};
    assert(longstr == x.c_str());
    HomeworkCOW::CowStr y = x;
    assert(longstr == y.c_str());
    assert(longstr == x.c_str());
    HomeworkCOW::CowStr z = std::move(x);
    assert(longstr == z.c_str());
    z.append("another");
    std::string appended = longstr + "another";
    assert(appended == z.c_str());
    assert(longstr == y.c_str());

    HomeworkCOW::CowStr t1{"very very very very long test for write"};
    HomeworkCOW::CowStr t2 = t1;
    std::string old = t1.c_str();
    t1[5] = 'i';
    assert(t1[5] == 'i');
    assert(old == t2.c_str());

    HomeworkCOW::WCowStr w{L"wide str"};
    std::wstring wst{L"wide str"};
    assert(wst == w.c_str());
}

std::string generate_long_string(size_t length) {
    const std::string charset =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.reserve(length);
    std::mt19937 generator(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, charset.length() - 1);

    for (size_t i = 0; i < length; ++i) {
        result += charset[distribution(generator)];
    }
    return result;
}

template <typename StringType>
void run_benchmark(const std::string& test_name, const std::string& initial_long_string) {
    std::cout << "--- Running Benchmark for: " << test_name << " ---" << std::endl;

    const int num_copies = 10000;
    const int num_modifications = 100;

    auto start_time = std::chrono::high_resolution_clock::now();

    StringType original_string(initial_long_string.c_str());
    std::vector<StringType> string_copies;
    string_copies.reserve(num_copies);

    for (int i = 0; i < num_copies; ++i) {
        string_copies.push_back(original_string);
    }

    for (int i = 0; i < num_modifications; ++i) {
        string_copies[i][0] = 'X';
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;
    std::cout << "Total time taken: " << elapsed_time.count() << " ms" << std::endl;
    std::cout << "--------------------------------------\n" << std::endl;
}

int main() {
    tests();
    std::cout << "All initial tests passed successfully.\n" << std::endl;

    const size_t long_string_length = 8192;  // 8 KB string
    std::cout << "Generating a long string of length " << long_string_length
              << " for benchmarking...\n"
              << std::endl;
    std::string long_string_for_test = generate_long_string(long_string_length);
    run_benchmark<HomeworkCOW::CowStr>("HomeworkCOW::CowStr", long_string_for_test);
    run_benchmark<std::string>("std::string", long_string_for_test);
}
