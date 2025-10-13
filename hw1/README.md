## 1.1 volatile nullptr_t access:

in the assignmenet line (`int* a = p`), Lvalue-to-rvalue conversion is conducted. according to standard, there is no reading at that moment: (from Lvalue-to-rvalue conversion [conv.lval]):

> The result of the conversion is determined according to the following rules:
(3.1) — If T is cv std::nullptr_t, the result is a null pointer constant (7.3.12).
[Note 1 : Since the conversion does not access the object to which the glvalue refers, there is no side effect even
if T is volatile-qualified (6.9.1), and the glvalue can refer to an inactive member of a union (11.5). — end note]

after that conversion, only nullptr constant is left (without any volatile), which compiler may optimize.

## 1.2 legal nullptr deref

This statement includes dereferencing of nullptr but is not UB as, acording to expr.prim.req.general, requires expressions are not evaluated
`template<typename T> concept NotUB = requires (T t) { *(int*)nullptr = 0; };`

## 1.3 NotUB access:

This task shows how to write `func(int* a, int base, int off) -> a[base+off]` such that as most UB as possible is avoided. Ifs are written in a way that they won't get optimised. `res.s` shows what i got on my system with -O2 on clang. You can also check it out on [godbolt](https://godbolt.org/z/nobb3x8vG)
