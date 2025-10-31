#ifndef TYPE_H
#define TYPE_H

#include <variant>
#include <vector>

struct Type;

struct TypeVariable {
    int id;
};

enum class ComplexTypeKind {
    LAMBDA,
    LIST,
};

struct ComplexType {
    ComplexTypeKind type;
    std::vector<Type*> children;
};

struct Type {
    std::variant<TypeVariable, ComplexType> value;
};

#endif  // TYPE_H
