//#include <cassert>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

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

    assert(WTwine(L"Wide", L" wstr").str() == L"Wide wstr");

    std::string_view a = "hi!", b = "hello!";
    assert(Twine(a, b).str() == "hi!hello!");
    assert((Twine(a, b) + "STR").str() == "hi!hello!STR");
}

int main() {
    using namespace HomeworkTwine;
    tests();
    std::cout << "tests are successfull!\n";
}
