#include <cassert>
#include <iostream>
#include <string>
#include "cowstr.hpp"

int main() {
    HomeworkCOW::CowStr x{"aboba"};
    std::cout << x.is_sso() <<"inited\n";
    assert(std::string("aboba") == x.c_str());
    std::cout << x.c_str() << '\n';
    x.append("very long word or sentence idk what");
    std::string longstr{"abobavery long word or sentence idk what"};
    assert(longstr == x.c_str());
    std::cout << x.c_str() << '\n';
    HomeworkCOW::CowStr y = x;
    std::cout << "copied" << '\n';
    assert(longstr == y.c_str());
    assert(longstr == x.c_str());
}
