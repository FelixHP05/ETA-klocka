#pragma once

#include <stdint.h>
#include <math.h>

#define PI 3.1415926535F

inline int min(int a, int b) { return a < b ? a : b; }
inline int max(int a, int b) { return a > b ? a : b; }
inline unsigned int min(unsigned int a, unsigned int b) { return a < b ? a : b; }
inline unsigned int max(unsigned int a, unsigned int b) { return a > b ? a : b; }
inline long long min(long long a, long long b) { return a < b ? a : b; }
inline long long max(long long a, long long b) { return a > b ? a : b; }
inline unsigned long long min(unsigned long long a, unsigned long long b) { return a < b ? a : b; }
inline unsigned long long max(unsigned long long a, unsigned long long b) { return a > b ? a : b; }

// positive floating point modulo
inline float modulo(float dividend, float divisor) {
    float res = fmod(dividend, divisor);
    if (res < 0) res += fabs(divisor);
    return res;
}
// positive integer modulo
inline int modulo(int dividend, int divisor) {
    int res = dividend % divisor;
    if (res < 0) res += abs(divisor);
    return res;
}
// floor division
inline int floorDiv(int dividend, int divisor) {
    int q = dividend / divisor;
    int r = dividend % divisor;
    if ((r != 0) && ((r < 0) != (divisor < 0))) q--;
    return q;
}
// ceiling division
inline int ceilDiv(int dividend, int divisor) {
    int q = dividend / divisor;
    int r = dividend % divisor;
    if ((r != 0) && ((r > 0) != (divisor > 0))) q++;
    return q;
}

//inline float abs(float x) { return x >= 0 ? x : -x; }
//inline int abs(int x) { return x >= 0 ? x : -x; }