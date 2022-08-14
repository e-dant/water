#include <numeric>

namespace number {
// generic
template <class T>
int digit_count(T number)
{
    int digits = 0;
    if (number < 0) digits = 1; // signs are not digits
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

// for 64-bit numbers
template <>
int digit_count(int64_t x) {
    if (x == INT64_MIN) return 19 + 1;
    if (x < 0) return digit_count(-x) + 1;

    if (x >= 10000000000) {
        if (x >= 100000000000000) {
            if (x >= 10000000000000000) {
                if (x >= 100000000000000000) {
                    if (x >= 1000000000000000000)
                        return 19;
                    return 18;
                }
                return 17;
            }
            if (x >= 1000000000000000)
                return 16;
            return 15;
        } 
        if (x >= 1000000000000) {
            if (x >= 10000000000000)
                return 14;
            return 13;
        }
        if (x >= 100000000000)
            return 12;
        return 11;
    }
    if (x >= 100000) {
        if (x >= 10000000) {
            if (x >= 100000000) {
                if (x >= 1000000000)
                    return 10;
                return 9;
            }
            return 8;
        }
        if (x >= 1000000)
            return 7;
        return 6;
    }
    if (x >= 100) {
        if (x >= 1000) {
            if (x >= 10000)
                return 5;
            return 4;
        }
        return 3;
    }
    if (x >= 10)
        return 2;
    return 1;
}

// for 32-bit numbers
template<>
int digit_count(int32_t x)
{
    if (x == INT32_MIN) return 10 + 1;
    if (x < 0) return digit_count(-x) + 1;

    if (x >= 10000) {
        if (x >= 10000000) {
            if (x >= 100000000) {
                if (x >= 1000000000)
                    return 10;
                return 9;
            }
            return 8;
        }
        if (x >= 100000) {
            if (x >= 1000000)
                return 7;
            return 6;
        }
        return 5;
    }
    if (x >= 100) {
        if (x >= 1000)
            return 4;
        return 3;
    }
    if (x >= 10)
        return 2;
    return 1;
}

// for 8-bit numbers
template <>
int digit_count(char n)
{
    // replace this with a static initialization to avoid
    // the initial overhead & unnecessary branch
    static char x[256] = {0};
    if (x[0] == 0) {
        for (char c = 1; c != 0; c++)
            x[c] = digit_count((int32_t)c);
        x[0] = 1;
    }
    return x[n];
}
}
