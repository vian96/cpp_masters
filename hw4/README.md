## Task 4.1

The task is to find which overload will be called in such program:

```cpp
template <class T1, class T2> struct Pair {
  template<class U1 = T1, class U2 = T2> Pair(U1&&, U2&&) {}
};

struct S { S() = default; };
struct E { explicit E() = default; };

int f(Pair<E, E>) { return 1; }
int f(Pair<S, S>) { return 2; }

assert(f({{}, {}}) == 2); // Error or Assert or OK?
```

According to the standard, function has to be viable to be considered for compiler to call:

[over.match.viable]:
> 1 From the set of candidate functions constructed for a given context (12.2.2), a set of viable functions is chosen, from which the best function will be selected by comparing argument conversion sequences and associated constraints (13.5.3) for the best fit (12.2.4).

However, calling f(Pair<E, E>) is not viable because there is no implicit conversion sequence from `{{},{}}`:

[over.match.viable]:
>Third, for F to be a viable function, there shall exist for each argument an implicit conversion sequence

Which could also be seen by getting an error if you just comment overload with S in all compilers i tested.

There is no implicit conversion sequence because:
- only possible candidate constructor for calling `f(Pair<E, E>)` is `template<class U1 = T1, class U2 = T2> Pair(U1&&, U2&&) {}`
- Compiler can't deduce any other type of arguments (except T1, T2) here because due to [temp.deduct.type]:
> A function parameter for which the associated argument is an initializer list (9.4.5) but the parameter does not have a type for which deduction from an initializer list is specified (13.10.3.2).

Where 13.10.3.2 states some exceptions from this rule (which don't apply here). So we get non-deduced context here. And due to [temp.deduct.call], specified default template arguments T1, T2 are used:
> Note 1 : If a template-parameter is not used in any of the function parameters of a function template, or is used only in a non-deduced context, its corresponding template-argument cannot be deduced from a function call and the template-argument must be explicitly specified. — end note

So, compiler has nothing to do but to find an implicit conversion to call `Pair(E, E)` which is not possible because E requires explicit construction.

Therefore, viable set of candidates consists only of `f(Pair<S, S>)` which can be called, so it will be called and assert should pass fine.

## Task 4.2
The task is to find which overload will be called in such program:

```cpp
struct Foo {};
struct Bar : Foo {};
struct Baz : Foo {};

struct X {
    operator Bar() { std::cout << "Bar\n"; return Bar{}; }
    operator Baz() const { std::cout << "Baz\n"; return Baz{}; }
};

void foo(const Foo& f) {}

int main() { foo(X{}); } // Bar or Baz?
```

Here, we have only one function to call, but compiler has to choose which conversion function to call. Both conversion functions are viable. According to the standard, the best one has to be chosen [over.match.viable]:
> From the set of candidate functions constructed for a given context (12.2.2), a set of viable functions is chosen, from which the best function will be selected by comparing argument conversion sequences and associated constraints (13.5.3) for the best fit (12.2.4).

For both function, temporary value has to materialize first anyway, so lvalue/rvalue conversions don't matter here. And for calling Bar, you don't need any more conversions. For calling Baz, you have to do Qualification conversion. So, according to [over.ics.rank], Bar is a better candidate and `operator X::Bar()` will be called.

