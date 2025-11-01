#include "expr_parser.hpp"

#include <stdexcept>
#include <string>
#include <vector>

static void skip_whitespace(std::string_view& s) {
    size_t pos = s.find_first_not_of(" \t\n\r");
    if (pos == std::string_view::npos)
        s.remove_prefix(s.size());
    else
        s.remove_prefix(pos);
}

std::string_view SExpressionParser::next_token(std::string_view& s) {
    skip_whitespace(s);
    if (s.empty()) return {};

    if (s.front() == '(' || s.front() == ')') {
        std::string_view token = s.substr(0, 1);
        s.remove_prefix(1);
        return token;
    }

    size_t token_end = s.find_first_of(" \t\n\r()");
    if (token_end == std::string_view::npos) token_end = s.length();

    std::string_view token = s.substr(0, token_end);
    s.remove_prefix(token_end);
    return token;
}

Node SExpressionParser::parse_expression(std::string_view& s) {
    std::string_view token = next_token(s);

    if (token.empty()) throw std::runtime_error("Unexpected end of expression");

    if (token == "(")
        return handle_brackets(s);
    else if (token == "nil")
        return create_nil();
    else
        return create_identifier(std::string(token));
}

Node SExpressionParser::handle_brackets(std::string_view& s) {
    // FIXME: everything that does not match define/lambda/case/cons is considered a
    // function call so case branches are "function calls". need proper node type for that
    // or make explicit handling?
    std::vector<Node> elements;
    while (true) {
        skip_whitespace(s);
        if (s.empty() || s.front() == ')') break;

        elements.push_back(parse_expression(s));
    }

    if (s.empty() || s.front() != ')')
        throw std::runtime_error("Expected ')' at the end of a list");

    s.remove_prefix(1);  // consume ')'

    if (elements.empty())
        throw std::runtime_error("Empty expression list is not allowed");

    if (elements[0].type == NodeType::IDENTIFIER) {
        const std::string& keyword = elements[0].id;
        if (keyword == "define") {
            if (elements.size() != 3 || elements[1].type != NodeType::IDENTIFIER)
                throw std::runtime_error("Invalid define syntax");

            return create_define(std::move(elements[1].id), std::move(elements[2]));
        } else if (keyword == "lambda") {
            if (elements.size() != 3 || elements[1].type != NodeType::IDENTIFIER)
                throw std::runtime_error("Invalid lambda syntax");

            return create_lambda(std::move(elements[1].id), std::move(elements[2]));
        } else if (keyword == "case") {
            // elem[1] is name of cased var
            if (elements.size() < 3 || elements[1].type != NodeType::IDENTIFIER)
                throw std::runtime_error("Invalid case syntax");

            std::vector<std::pair<Node, Node>> cases;
            for (size_t i = 2; i < elements.size(); ++i) {
                // as said in FIXME, case branches are considered function calls
                if (elements[i].type != NodeType::FUNCTION_CALL ||
                    elements[i].children.size() != 2)
                    throw std::runtime_error("Invalid case pair");

                cases.emplace_back(std::move(elements[i].children[0]),
                                   std::move(elements[i].children[1]));
            }
            return create_case(std::move(elements[1].id), std::move(cases));
        } else if (keyword == "cons") {
            if (elements.size() != 3) throw std::runtime_error("Invalid cons syntax");

            return create_cons(std::move(elements[1]), std::move(elements[2]));
        }
    }

    Node result = std::move(elements[0]);
    for (size_t i = 1; i < elements.size(); ++i)
        result = create_function_call(std::move(result), std::move(elements[i]));

    return result;
}

ExpressionTree SExpressionParser::parse(std::string_view expression) {
    return ExpressionTree(parse_expression(expression));
}
