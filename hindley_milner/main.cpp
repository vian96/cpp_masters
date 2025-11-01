#include <cassert>
#include <iostream>
#include <string>

#include "expr_parser.hpp"
#include "expr_tree.hpp"
#include "type_infer.hpp"

int main() {
    std::string to_parse;
    std::getline(std::cin, to_parse);
    SExpressionParser parser;
    ExpressionTree tree = parser.parse(to_parse);
    std::cout << "Expression Tree Structure:" << std::endl;
    tree.get_root().print_tree();

    TypeInferSolver inferer;
    try {
        std::string f_type = inferer.infer_type_string(tree);
        std::cout << "Type of function 'f' is: " << f_type << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Type inference failed: " << e.what() << std::endl;
    }

    return 0;
}
