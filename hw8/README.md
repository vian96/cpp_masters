## Task 8.1

The task is to decide if this program compiles:

```cpp
struct S {
  int : *new int{0};
};

enum E {
  V = *new int{1},
};
```

According to standard ([class.bit]), width has to be constant expression. Same is true for enum value ([dcl.enum]).

New expression is disallowed in constant expression under some conditions: ([expr.const]):
> a new-expression (7.6.2.8), unless the selected allocation function is a replaceable global allocation
function (17.6.3.2, 17.6.3.3) and the allocated storage **is deallocated within the evaluation of E**;

Therefore, these statements are invalid. So this should not compile and gcc is right

## Task 8.2

The task is to decide if this program compiles:

```cpp
struct Type {
  int a; const int& b;
  constexpr Type() : a(1), b(a) {}
};

constexpr auto get() { return Type(); }

constexpr Type t2 = get(); // gcc error, clang ok
constexpr int c2 = t2.a;
```

According to standard ([expr.const]):
> Expressions that satisfy these requirements, **assuming that copy elision (11.9.6) is not performed**, are called constant expressions.

So, copy ellision should not be taken into mind. If no copy ellision is performed, `t2.b` would be a dangling reference due to pointing out to a temprorary object that is dead after assignment. As it is a field of a constexpr object, it cannot be returned from a function due to (p.13):
> A constant expression is either a glvalue core constant expression that refers to an entity that is a permitted result of a constant expression (as defined below), or a prvalue core constant expression whose value satisfies the following constraints:
 - if the value is an object of class type, each non-static data member of reference type refers to an entity that is a permitted result of a constant expression,

Where permitted result is defined as:
> An entity is a permitted result of a constant expression if it is an object with static storage duration that either is not a temporary object or is a temporary object whose value satisfies the above constraints, or if it is a non-immediate function.

Therefore, returning an object with a reference field pointing to a temporary object is not a constant expression. So, this should not compile and gcc is right
