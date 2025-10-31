#include "expr_tree.hpp"
#include "type_infer.hpp"

#include <iostream>

void print_tree(const Node& node, int indent = 0) {
    std::cout << std::string(indent, ' ');
    switch (node.type) {
        case NodeType::DEFINE:
            std::cout << "DEFINE: " << node.id << std::endl;
            break;
        case NodeType::LAMBDA:
            std::cout << "LAMBDA, arg: " << node.id << std::endl;
            break;
        case NodeType::CASE:
            std::cout << "CASE, matching on: " << node.id << std::endl;
            break;
        case NodeType::CONS:
            std::cout << "CONS" << std::endl;
            break;
        case NodeType::NIL:
            std::cout << "NIL" << std::endl;
            break;
        case NodeType::IDENTIFIER:
            std::cout << "IDENTIFIER: " << node.id << std::endl;
            break;
        case NodeType::FUNCTION_CALL:
            std::cout << "FUNCTION_CALL" << std::endl;
            break;
    }

    for (const auto& child : node.children) {
        print_tree(child, indent + 2);
    }
}

int main() {
    // (define f (lambda (g) (lambda (xs)
    //   (case xs (nil nil)
    //     ((cons x xs1) (cons (g x) ((f g) xs1)))))))

    ExpressionTree tree(
        create_define("f",
            create_lambda("g",
                create_lambda("xs",
                    create_case("xs",
                        {
                            {create_nil(), create_nil()},
                            {create_cons(create_identifier("x"), create_identifier("xs1")),
                             create_cons(
                                 create_function_call(create_identifier("g"), create_identifier("x")),
                                 create_function_call(
                                     create_function_call(create_identifier("f"), create_identifier("g")),
                                     create_identifier("xs1")
                                 )
                             )}
                        }
                    )
                )
            )
        )
    );

    std::cout << "Expression Tree Structure:" << std::endl;
    print_tree(tree.get_root());

    TypeInferSolver inferer;
    try {
        std::string f_type = inferer.infer_type_string(tree);
        std::cout << "Type of function 'f' is: " << f_type << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Type inference failed: " << e.what() << std::endl;
    }


    return 0;
}

