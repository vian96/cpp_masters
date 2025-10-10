//#include <cassert>
#include <iostream>
#include <ostream>
#include <string>

#include "twine.hpp"

#define assert(cond)       \
    do {                   \
        if (!(cond)) abort(); \
    } while (0)

void tests() {
    using namespace HomeworkTwine;
    assert((Twine("hello", ", ") + Twine("world", "!")).str() == "hello, world!");

    assert((Twine("ABOBA!!", Twine::EmptyTwineChild())).str() == "ABOBA!!");

    assert((Twine("Aboba", "boba")).str() == "Abobaboba");

    assert((Twine("hello", ", ") + Twine("world!") + Twine("ABIBAIBIAIB") +
            Twine("\n\n\n\n") + Twine("abiba", "biboba\n"))
               .str() == "hello, world!ABIBAIBIAIB\n\n\n\nabibabiboba\n");

    assert((Twine(Twine("hello", ", "), Twine("world!"))).str() == "hello, world!");

    assert(((Twine("xa", "bob") + " " + "bib" + "ob").str() == "xabob bibob"));

    assert(WTwine(L"Wide", L" wstr").str() == std::wstring(L"Wide wstr"));
}

int main() {
    using namespace HomeworkTwine;
    tests();
    std::cout << "tests are successfull!\n";
}
