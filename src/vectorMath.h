// matrixMath.h and vectorMath.h are inspired by glm syntax.
// written by Eugene Choi
// sorry about the questionable code
// it works tho

#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

// #include <iostream>
// #include <vector>
#include <cmath>

using namespace std;

class vec2 {
  public:
    vec2() : x(0), y(0) {}
    vec2(double x_in, double y_in) : x(x_in), y(y_in) {}
    vec2(double in) : x(in), y(in) {}

    double operator[](unsigned i) {
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        default:
            return 6969696969.69;
        }
    }

    vec2 operator*(const double &a) { // idk if this
        return vec2(x * a, y * a);
    }
    void operator*=(double a) { // or this
        x *= a;
        y *= a;
    }
    vec2 operator/(const double &a) { // idk if this
        return vec2(x / a, y / a);
    }
    const vec2 operator/(const double &a) const { // idk if this
        return vec2(x / a, y / a);
    }
    void operator/=(double a) { // or this
        x /= a;
        y /= a;
    }

    const vec2 operator+(const vec2 &a) const { return vec2(x + a.x, y + a.y); }

    vec2 operator+(const vec2 &a) { return vec2(x + a.x, y + a.y); }

    vec2 operator+(double a) { return vec2(x + a, y + a); }
    void operator+=(vec2 a) {
        x += a.x;
        y += a.y;
    }

    const vec2 operator-(const vec2 &a) const { return vec2(x - a.x, y - a.y); }

    vec2 operator-(const vec2 &a) { return vec2(x - a.x, y - a.y); }
    vec2 operator-(double a) { return vec2(x - a, y - a); }
    void operator-=(vec2 a) {
        x -= a.x;
        y -= a.y;
    }

    double x;
    double y;
};

vec2 operator*(double a, vec2 b) { return b * a; }

class vec3 {
  public:
    vec3() : x(0), y(0), z(0) {}
    vec3(double x_in, double y_in, double z_in) : x(x_in), y(y_in), z(z_in) {}
    vec3(double in) : x(in), y(in), z(in) {}

    vec2 xy() { return vec2(x, y); }

    double operator[](unsigned i) {
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            return 6969696969.69;
        }
    }

    vec3 operator*(const double &a) { // idk if this
        return vec3(x * a, y * a, z * a);
    }
    const vec3 operator*(const double &a) const { // idk if this
        return vec3(x * a, y * a, z * a);
    }
    vec3 operator*(const vec3 &a) { // idk if this
        return vec3(x * a.x, y * a.y, z * a.z);
    }
    const vec3 operator*(const vec3 &a) const { // idk if this
        return vec3(x * a.x, y * a.y, z * a.z);
    }
    void operator*=(double a) { // or this
        x *= a;
        y *= a;
        z *= a;
    }
    vec3 operator/(const double &a) { // idk if this
        return vec3(x / a, y / a, z / a);
    }
    const vec3 operator/(const double &a) const { // idk if this
        return vec3(x / a, y / a, z / a);
    }
    void operator/=(double a) { // or this
        x /= a;
        y /= a;
        z /= a;
    }

    vec3 operator+(const vec3 &a) { return vec3(x + a.x, y + a.y, z + a.z); }
    const vec3 operator+(const vec3 &a) const { return vec3(x + a.x, y + a.y, z + a.z); }
    void operator+=(vec3 a) {
        x += a.x;
        y += a.y;
        z += a.z;
    }

    vec3 operator-() { return vec3(-x, -y, -z); }

    vec3 operator-(const vec3 &a) { return vec3(x - a.x, y - a.y, z - a.z); }
    void operator-=(vec3 a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
    }

    double x;
    double y;
    double z;
};

vec3 operator*(double a, const vec3 &b) { return b * a; }
vec3 operator-(double a, const vec3 &b) { return vec3(a) - b; }
vec3 operator+(double a, const vec3 &b) { return b + a; }

vec3 pow(const vec3 &base, double e) { return vec3(pow(base.x, e), pow(base.y, e), pow(base.z, e)); }

class vec4 {
  public:
    vec4() : x(0), y(0), z(0), w(0) {}
    vec4(vec3 in, double w_in) : x(in.x), y(in.y), z(in.z), w(w_in) {}
    vec4(double x_in, double y_in, double z_in, double w_in) : x(x_in), y(y_in), z(z_in), w(w_in) {}
    vec4(double in) : x(in), y(in), z(in), w(in) {}

    vec2 xy() { return vec2(x, y); }
    vec3 xyz() { return vec3(x, y, z); }

    double operator[](unsigned i) {
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            return 6969696969.69;
        }
    }

    vec4 operator*(const double &a) { // idk if this
        return vec4(x * a, y * a, z * a, w * a);
    }
    const vec4 operator*(const double &a) const { // idk if this
        return vec4(x * a, y * a, z * a, w * a);
    }
    void operator*=(double a) { // or this
        x *= a;
        y *= a;
        z *= a;
        w *= a;
    }
    vec4 operator/(const double &a) { // idk if this
        return vec4(x / a, y / a, z / a, w / a);
    }
    void operator/=(double a) { // or this
        x /= a;
        y /= a;
        z /= a;
        w /= a;
    }

    vec4 operator+(const vec4 &a) { return vec4(x + a.x, y + a.y, z + a.z, w + a.w); }
    void operator+=(const vec4 &a) {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.z;
    }

    vec4 operator-(const vec4 &a) { return vec4(x - a.x, y - a.y, z - a.z, w - a.w); }
    void operator-=(const vec4 &a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
    }

    double x;
    double y;
    double z;
    double w;
};

vec3 cross(const vec3 &a, const vec3 &b) {
    return vec3(a.y * b.z - a.z * b.y, -1 * (a.x * b.z - a.z * b.x), a.x * b.y - a.y * b.x);
}

vec4 operator*(double a, const vec4 &b) { return b * a; }

double dot(const vec2 &a, const vec2 &b) { return a.x * b.x + a.y * b.y; }
double dot(const vec3 &a, const vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

double min(double a, double b) {
    if (a > b) {
        return b;
    }
    return a;
}
double max(double a, double b) {
    if (a < b) {
        return b;
    }
    return a;
}

vec3 normalize(const vec3 &in) {
    double length = sqrt(in.x * in.x + in.y * in.y + in.z * in.z);
    return in / length;
}

vec2 normalize(const vec2 &in) {
    double length = sqrt(in.x * in.x + in.y * in.y);
    return in / length;
}

double lengthSQ(const vec3 &in) { return in.x * in.x + in.y * in.y + in.z * in.z; }
double lengthSQ(const vec2 &in) { return in.x * in.x + in.y * in.y; }

float clamp(double in, double min = 0, double max = 1) {
    if (in < min) {
        return min;
    }
    if (in > max) {
        return max;
    }
    return in;
}

#endif