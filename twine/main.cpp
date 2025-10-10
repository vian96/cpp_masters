#include <iostream>
#include <ostream>

#include "twine.hpp"

int main() {
    using namespace HomeworkTwine;
    // std::cout << 0 << std::is_convertible_v<Twine, TwineChild> << '\n';
    // std::cout << "AB\n";
    // std::cout << HomeworkTwine::Twine("ab", "oba") << '\n';
    // HomeworkTwine::Twine("ab", "oba", "x");
    // Twine(Twine("hello", ", "), "world!").dump_impl(std::cout);
    std::cout << Twine("hello", ", ") + Twine("world", "!") << std::endl;
    // std::cout << Twine("hello", ", ", "world!") << std::endl;
    //

    std::cout << Twine("ABOBA!!", HomeworkTwine::Twine::EmptyTwineChild());
    std::cout << Twine("Aboba", "boba") << std::endl;
    //return 0;
    std::cout << Twine("hello", ", ") + Twine("world!") + Twine("ABIBAIBIAIB") +
                     Twine("\n\n\n\n") + Twine("abiba", "biboba\n")
              << std::endl;
    std::cout << Twine(Twine(Twine(Twine("hello", ", ") + Twine("world!")),
                             Twine(Twine("ABIBAIBIAIB"), Twine("\n\n\n\n"))),
                       Twine("abiba", "biboba\n"))
              << std::endl;
    std::cout << Twine(Twine("hello", ", "), Twine("world!")) << std::endl;

    // std::cout << Twine::from_several("hello", ", ", "world", "!") << std::endl;

    std::cout << "AB\n";
    // HomeworkTwine::Twine("hello", ", ", "world").dump_impl(std::cout);
    // std::cout << "AB\n";
    // std::cout << HomeworkTwine::Twine("hello", ", ", "world") << '\n';
}
