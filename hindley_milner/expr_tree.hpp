#ifndef EXPRESSION_TREE_H
#define EXPRESSION_TREE_H

#include <string>
#include <utility>
#include <vector>

enum class NodeType { DEFINE, LAMBDA, CASE, CONS, NIL, IDENTIFIER, FUNCTION_CALL };

// probably it is not "the best" design decision
// in terms of efficiency and memory footprint
// but it does not really matter since half of nodes contain id and
// it wouldn't save much memory. also acces to children is indirect
// with both vector and hardcoded pointers so it does not matter either.
// so i chose simplicity over little performance win
struct Node {
    // TODO: is const needed? it may break assignment but it doesn't really matter
    const NodeType type;

    std::string id;  // for identifiers, functions and lambda args
    std::vector<Node> children;

    explicit Node(NodeType nodeType, std::string identifier = "",
                  std::vector<Node> child_nodes = {})
        : type(nodeType), id(std::move(identifier)), children(std::move(child_nodes)) {}
};

class ExpressionTree {
   private:
    Node root;

   public:
    explicit ExpressionTree(Node root_node) : root(std::move(root_node)) {}

    const Node& get_root() const { return root; }
};

Node create_define(std::string name, Node expression);
Node create_lambda(std::string argument_name, Node body);
Node create_case(std::string argument_to_match, std::vector<std::pair<Node, Node>> cases);
Node create_cons(Node left, Node right);
Node create_nil();
Node create_identifier(std::string name);
Node create_function_call(Node function, Node argument);

#endif  // EXPRESSION_TREE_H
