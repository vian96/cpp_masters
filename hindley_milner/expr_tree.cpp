#include "expr_tree.hpp"

Node create_define(std::string name, Node expression) {
    return Node(NodeType::DEFINE, std::move(name), {std::move(expression)});
}

Node create_lambda(std::string argument_name, Node body) {
    return Node(NodeType::LAMBDA, std::move(argument_name), {std::move(body)});
}

Node create_case(std::string argument_to_match,
                 std::vector<std::pair<Node, Node>> cases) {
    std::vector<Node> children;
    children.reserve(cases.size() * 2);
    for (auto& p : cases) {
        children.push_back(std::move(p.first));
        children.push_back(std::move(p.second));
    }
    return Node(NodeType::CASE, std::move(argument_to_match), std::move(children));
}

Node create_cons(Node left, Node right) {
    return Node(NodeType::CONS, "", {std::move(left), std::move(right)});
}

Node create_nil() { return Node(NodeType::NIL); }

Node create_identifier(std::string name) {
    return Node(NodeType::IDENTIFIER, std::move(name));
}

Node create_function_call(Node function, Node argument) {
    return Node(NodeType::FUNCTION_CALL, "", {std::move(function), std::move(argument)});
}
