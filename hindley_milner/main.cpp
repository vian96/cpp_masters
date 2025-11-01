#include <iostream>

#include "expr_parser.hpp"
#include "expr_tree.hpp"
#include "type_infer.hpp"

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

    ExpressionTree tree(create_define(
        "f",
        create_lambda(
            "g",
            create_lambda(
                "xs",
                create_case(
                    "xs", {{create_nil(), create_nil()},
                           {create_cons(create_identifier("x"), create_identifier("xs1")),
                            create_cons(create_function_call(create_identifier("g"),
                                                             create_identifier("x")),
                                        create_function_call(
                                            create_function_call(create_identifier("f"),
                                                                 create_identifier("g")),
                                            create_identifier("xs1")))}})))));

    std::cout << "Expression Tree Structure:" << std::endl;
    print_tree(tree.get_root());

    std::cout << "parsing\n";

    std::string to_parse =
        "(define f (lambda (g) (lambda (xs)"
        "(case xs (nil nil)"
        "((cons x xs1) (cons (g x) ((f g) xs1)))))))";
    SExpressionParser parser;
    ExpressionTree parsed = parser.parse(to_parse);
    print_tree(parsed.get_root());
    tree = std::move(parsed);

    TypeInferSolver inferer;
    try {
        std::string f_type = inferer.infer_type_string(tree);
        std::cout << "Type of function 'f' is: " << f_type << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Type inference failed: " << e.what() << std::endl;
    }

    std::cout << TypeInferSolver().infer_type_string(
        SExpressionParser().parse("(define f (lambda (xs) (lambda (g)"
                                  "(case xs (nil nil)"
                                  "((cons x xs1) (cons (g x) ((f xs1) g)))))))")) << '\n';

    std::cout << TypeInferSolver().infer_type_string(
        SExpressionParser().parse("(define f (lambda xs (case xs (nil nil))))")) << '\n';

    std::cout << TypeInferSolver().infer_type_string(
        SExpressionParser().parse("(define f (lambda g (lambda x (g x))))")) << '\n';

    std::cout << TypeInferSolver().infer_type_string(
        SExpressionParser().parse("(define f (lambda x (lambda g (g x))))")) << '\n';

    std::cout << TypeInferSolver().infer_type_string(
        SExpressionParser().parse("(define f (lambda g (lambda x (case x (nil (g x))))))")) << '\n';

    return 0;
}
