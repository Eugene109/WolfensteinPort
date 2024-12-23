// matrixMath.h and vectorMath.h are inspired by glm syntax.
// written by Eugene Choi

#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

#include <cmath>

#include "vectorMath.h"

// first [] is x, second [] is y
// [0][0]  [1][0]
// [0][1]  [1][1]

class mat2 {
  public:
    mat2(double a = 1.0) {
        data[0][0] = a;
        data[1][0] = 0;
        data[0][1] = 0;
        data[1][1] = a;
    }
    template <size_t N> mat2(const double (&a)[N]) {
        data[0][0] = a[0];
        data[1][0] = a[1];
        data[0][1] = a[2];
        data[1][1] = a[3];
    }
    vec2 operator*(const vec2 &in) {
        return vec2(data[0][0] * in.x + data[1][0] * in.y, data[0][1] * in.x + data[1][1] * in.y);
    }
    mat2 operator*(const mat2 &in) {
        mat2 out = mat2();
        out.data[0][0] = in.data[0][0] * data[0][0] + in.data[0][1] * data[1][0];
        out.data[0][1] = in.data[0][0] * data[0][1] + in.data[0][1] * data[1][1];
        out.data[1][0] = in.data[1][0] * data[0][0] + in.data[1][1] * data[1][0];
        out.data[1][1] = in.data[1][0] * data[0][1] + in.data[1][1] * data[1][1];
        return out;
    }
    double data[2][2];
};

mat2 rotate(double theta) { return mat2((double[]){cos(theta), -sin(theta), sin(theta), cos(theta)}); }
#endif