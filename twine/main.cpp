#include <iostream>
#include <ostream>

#include "twine.hpp"

int main() {
    using namespace HomeworkTwine;
    std::cout << Twine("hello", ", ") + Twine("world", "!") << std::endl;

    std::cout << Twine("ABOBA!!", HomeworkTwine::Twine::EmptyTwineChild());
    std::cout << Twine("Aboba", "boba") << std::endl;
    std::cout << Twine("hello", ", ") + Twine("world!") + Twine("ABIBAIBIAIB") +
                     Twine("\n\n\n\n") + Twine("abiba", "biboba\n")
              << std::endl;
    std::cout << Twine(Twine(Twine(Twine("hello", ", ") + Twine("world!")),
                             Twine(Twine("ABIBAIBIAIB"), Twine("\n\n\n\n"))),
                       Twine("abiba", "biboba\n"))
              << std::endl;
    std::cout << Twine(Twine("hello", ", "), Twine("world!")) << std::endl;
    std::cout << Twine(Twine("hello", ", "), Twine("world!")).str() << std::endl;
}
