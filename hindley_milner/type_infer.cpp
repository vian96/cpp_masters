#include "type_infer.hpp"

#include <stdexcept>
#include <string>

#include "expr_tree.hpp"
#include "type.hpp"

Type* TypeInferSolver::infer_type(const ExpressionTree& tree) {
    VarTypes var_types;
    Type* f_type = new_type();
    var_types[tree.get_root().id] = f_type;

    // define itself does nothing, need to get type of its definition
    Type* body_type = get_expression_type(tree.get_root().children[0], var_types);
    equations.push_back({f_type, body_type});

    for (const auto& eq : equations) solve_equation(eq.first, eq.second);

    return f_type;
}

std::string TypeInferSolver::infer_type_string(const ExpressionTree& tree) {
    return type_to_string(infer_type(tree));
}

Type* TypeInferSolver::new_type() {
    type_arena.emplace_back(TypeVariable{next_type_variable_id++});
    return &type_arena.back();
}

Type* TypeInferSolver::new_constructor(ComplexTypeKind type,
                                       std::vector<Type*> children) {
    type_arena.emplace_back(ComplexType{type, std::move(children)});
    return &type_arena.back();
}

Type* TypeInferSolver::new_list_type(Type* type) {
    return new_constructor(ComplexTypeKind::LIST, {type});
}

Type* TypeInferSolver::new_lambda_type(Type* from, Type* to) {
    return new_constructor(ComplexTypeKind::LAMBDA, {from, to});
}

Type* TypeInferSolver::get_expression_type(const Node& node, VarTypes& var_types) {
    switch (node.type) {
        case NodeType::IDENTIFIER: {
            if (var_types.count(node.id)) return var_types.at(node.id);
            // all ids are introduced either at case or at lambda
            throw std::runtime_error("unknown variable: " + node.id);
        }
        case NodeType::NIL: {
            return new_list_type(new_type());
        }
        case NodeType::LAMBDA: {
            Type* arg_type = new_type();
            // see docstring on get_expression_type to see why copy is needed
            VarTypes new_vars = var_types;
            new_vars[node.id] = arg_type;
            Type* body_type = get_expression_type(node.children[0], new_vars);
            return new_lambda_type(arg_type, body_type);
        }
        case NodeType::FUNCTION_CALL: {
            Type* func_type = get_expression_type(node.children[0], var_types);
            Type* arg_type = get_expression_type(node.children[1], var_types);
            Type* result_type = new_type();
            equations.push_back({func_type, new_lambda_type(arg_type, result_type)});
            return result_type;
        }
        case NodeType::CONS: {
            Type* head_type = get_expression_type(node.children[0], var_types);
            Type* tail_type = get_expression_type(node.children[1], var_types);
            equations.push_back({tail_type, new_list_type(head_type)});
            return tail_type;
        }
        case NodeType::CASE: {
            Type* matched_expr_type = var_types.at(node.id);
            Type* result_type = new_type();

            for (size_t i = 0; i < node.children.size(); i += 2) {
                const auto& pattern_node = node.children[i];
                const auto& result_node = node.children[i + 1];

                VarTypes case_types = var_types;
                Type* pattern_type = nullptr;

                if (pattern_node.type == NodeType::NIL) {
                    pattern_type = new_list_type(new_type());
                } else if (pattern_node.type == NodeType::CONS) {
                    Type* head_var = new_type();
                    Type* tail_var = new_type();
                    case_types[pattern_node.children[0].id] = head_var;
                    case_types[pattern_node.children[1].id] = tail_var;
                    pattern_type = new_list_type(head_var);
                    equations.push_back({tail_var, pattern_type});
                } else {
                    throw std::runtime_error("unsupported node type for case" +
                                             std::to_string(int(pattern_node.type)));
                }
                // technically case on lambda can also exist
                // but it is not well defined in task how it'd work

                equations.push_back({matched_expr_type, pattern_type});
                Type* current_result_type = get_expression_type(result_node, case_types);
                equations.push_back({result_type, current_result_type});
            }
            return result_type;
        }
        default:
            throw std::runtime_error("unknown node type" +
                                     std::to_string(int(node.type)));
    }
}

Type* TypeInferSolver::get_solution(Type* t) {
    // if not simple type -> no solution. if simple, recursive definition may occur
    if (auto* var = std::get_if<TypeVariable>(&t->value))
        if (solutions.count(var->id))
            return solutions[var->id] = get_solution(solutions[var->id]);
    return t;
}

void TypeInferSolver::solve_equation(Type* t1, Type* t2) {
    // avoid more equations on already solved types
    t1 = get_solution(t1);
    t2 = get_solution(t2);

    // need to check both parts since left can be non-elementary type, e.g. for nil
    auto* var1 = std::get_if<TypeVariable>(&t1->value);
    auto* var2 = std::get_if<TypeVariable>(&t2->value);
    auto* ctor1 = std::get_if<ComplexType>(&t1->value);
    auto* ctor2 = std::get_if<ComplexType>(&t2->value);

    if (var1) {
        handle_simple_equation(*var1, t2);
    } else if (var2) {
        handle_simple_equation(*var2, t1);
    } else if (ctor1 && ctor2) {
        if (ctor1->type != ctor2->type ||
            ctor1->children.size() != ctor2->children.size())
            throw std::runtime_error(
                "unexpected situation: types that dhould be equal have different types "
                "or number of children, types: " +
                std::to_string(int(ctor1->type)) + " and " +
                std::to_string(int(ctor2->type)) +
                ", children: " + std::to_string(ctor1->children.size()) + " and " +
                std::to_string(ctor2->children.size()));
        // if type is complex, we need exact match of its params:
        // type of elems of lists and args/result of funcs
        for (size_t i = 0; i < ctor1->children.size(); ++i)
            solve_equation(ctor1->children[i], ctor2->children[i]);
    }
}

void TypeInferSolver::assert_recursive_eq(TypeVariable var, Type* t) {
    // webpages say it's necessary do such "occurs check".
    // otherwise, it seems infinite recursion would occur and
    // we don't want such behaviour of our program so i put it
    // even though i am not sure it's needed for this task
    t = get_solution(t);
    if (auto* t_var = std::get_if<TypeVariable>(&t->value)) {
        if (t_var->id == var.id) throw std::runtime_error("recursive type detected");
    } else if (auto* t_ctor = std::get_if<ComplexType>(&t->value)) {
        for (Type* child : t_ctor->children) assert_recursive_eq(var, child);
    }
}

void TypeInferSolver::handle_simple_equation(TypeVariable var, Type* other) {
    // trivial equation
    if (auto* t2_var = std::get_if<TypeVariable>(&other->value))
        if (var.id == t2_var->id) return;

    assert_recursive_eq(var, other);  // unsolvable equation
    solutions[var.id] = other;
}

std::string TypeInferSolver::type_to_string(Type* t) {
    std::map<int, std::string> names;
    std::string inner_str = type_to_string_impl(t, names);
    std::string args_def = "(forall (";
    for (size_t i = 0; i < names.size(); i++) {
        args_def += 'a' + i;
        if (i + 1 != names.size()) args_def += ' ';
    }
    return args_def + ") " + inner_str + ')';
}

std::string TypeInferSolver::type_to_string_impl(Type* t,
                                                 std::map<int, std::string>& names) {
    t = get_solution(t);
    if (auto* var = std::get_if<TypeVariable>(&t->value)) {
        if (!names.count(var->id)) names[var->id] = 'a' + names.size();
        return names[var->id];
    } else if (auto* ctor = std::get_if<ComplexType>(&t->value)) {
        if (ctor->type == ComplexTypeKind::LAMBDA)
            return "(func " + type_to_string_impl(ctor->children[0], names) + " " +
                   type_to_string_impl(ctor->children[1], names) + ")";

        return "(list " + type_to_string_impl(ctor->children[0], names) + ')';
    }
    return "";
}
