//#include <cassert>
#include <iostream>
#include <ostream>

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
    // assert((Twine("ABOBA!!", Twine::EmptyTwineChild())).str() == "ABOBA!!");
    // assert((Twine("ABOBA!!", Twine::EmptyTwineChild())).str() == "ABOBA!!");
}

int main() {
    using namespace HomeworkTwine;
    tests();
    std::cout << "tests are successfull!\n";
    // std::cout << Twine(Twine("hello", ", "), Twine("world!")) << std::endl;
    // std::cout << Twine(Twine("hello", ", "), Twine("world!")).str() << std::endl;
}
