#ifndef TYPE_H
#define TYPE_H

#include <vector>

enum class TypeKind {
    VARIABLE,
    LAMBDA,
    LIST,
};

// same as exprTree: prefer simplicity over prematurely optimized memory footprint
struct Type {
    TypeKind kind;
    int id;
    std::vector<Type*> children;
};

#endif  // TYPE_H
