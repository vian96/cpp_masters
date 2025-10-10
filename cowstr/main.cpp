#include <cassert>
#include <iostream>
#include <string>

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
    //std::cout << w.c_str();
    assert(wst == w.c_str());
}

int main() { tests(); }
