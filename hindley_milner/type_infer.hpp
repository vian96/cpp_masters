#ifndef TYPE_INFER_H
#define TYPE_INFER_H

#include <deque>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "expr_tree.hpp"
#include "type.hpp"

class TypeInferSolver {
   public:
    // INFO: pointed object lives exactly as long as Solver
    Type* infer_type(const ExpressionTree& tree);

    // WARNING: UB if there are more than 26 type template params
    std::string infer_type_string(const ExpressionTree& tree);

    std::string type_to_string(Type* t);

    using Equation = std::pair<Type*, Type*>;

   private:
    Type* get_solution(Type* t);

    int next_type_variable_id = 0;

    std::unordered_map<std::string, Type*> var_types;

    // i want all Type objects to live as long as Solver lives
    // so i don't want smart ptrs and use arena method instead.
    // deque is preferred since it's easirer to work with pointers
    // than indexes, so i need their validity.
    std::deque<Type> type_arena;

    std::vector<Equation> equations;
    std::unordered_map<int, Type*> solutions;

    Type* new_type();
    Type* new_constructor(TypeKind type, std::vector<Type*> children);
    Type* new_list_type(Type* type);
    Type* new_lambda_type(Type* from, Type* to);

    Type* get_expression_type(const Node& node);

    void solve_equation(Type* t1, Type* t2);
    void handle_simple_equation(Type* var, Type* other);

    std::string type_to_string_impl(Type* t, std::map<int, std::string>& names,
                                    bool inside_several_args_func = false);

    void assert_recursive_eq(Type* var, Type* t);
};

#endif  // TYPE_INFER_H
