#ifndef EXPR_PARSER_HPP
#define EXPR_PARSER_HPP

#include <string_view>

#include "expr_tree.hpp"

class SExpressionParser {
   public:
    ExpressionTree parse(std::string_view expression);

   private:
    Node parse_expression(std::string_view& s);
    Node handle_brackets(std::string_view& s);
    std::string_view next_token(std::string_view& s);
};

#endif  // EXPR_PARSER_HPP
