#include <climits>
#include <stdlib.h>

int foo(
    int *a,
    int base,
    int off
    //, size_t size // if possible, pass it too
) {
    // all ifs are not optimized!

    // can't check pointer validity in any other way
    if (a == nullptr)
        return 42;

    // can't cast to bigger type as all integer types may have the same size
    // so just check mathematically
    if (off > 0 && base > INT_MAX - off)
        return 42;
    else if (off < 0 && base < INT_MIN - off)
        return 42;

    int res = base + off;
    if (res < 0)  // probably you don't want to get behind
        return 42;

    /*
    if (res >= size)  // ideally also check for right boundary if possible
        return 42;
    //*/

    return a[res];
}

