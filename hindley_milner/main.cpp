#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "expr_parser.hpp"
#include "type_infer.hpp"

int main() {
    TypeInferSolver solver;
    SExpressionParser parser;

    std::string input_line;
    while (std::getline(std::cin, input_line)) {
        if (input_line.empty()) continue;

        try {
            std::string result = solver.infer_type_string(parser.parse(input_line));
            std::cout << result << std::endl;
        } catch (const std::runtime_error& e) {
            std::cerr << "Error processing input: " << input_line << " - " << e.what()
                      << std::endl;
        }
    }
}
