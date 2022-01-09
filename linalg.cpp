#ifndef LINALG2_CPP
#define LINALG2_CPP

#include "poe.cpp"

#define _ACCESSORS_ real &operator[](int index) { return data[index]; }
template <int T> union Vec {
    real data[T];
    _ACCESSORS_;
};
template <> union Vec<2> {
    struct { real x, y; };
    real data[2];
    _ACCESSORS_;
};
template <> union Vec<3> {
    struct { real x, y, z; };
    struct { real r, g, b; };
    struct { Vec<2> xy; real pad_; };
    real data[3];
    _ACCESSORS_;
};
template <> union Vec<4> {
    struct { real x, y, z, w; };
    struct { real r, g, b, a; };
    struct { Vec<3> xyz; real pad_; };
    real data[4];
    _ACCESSORS_;
};
template <int T> Vec<T> operator *(real scalar, Vec<T> v) {
    Vec<T> result;
    for (int i = 0; i < T; ++i) {
        result.data[i]  = scalar * v.data[i];
    }
    return result;
}
template <int T> Vec<T> operator*(Vec<T> v, real scalar) {
    Vec<T> result = scalar * v;
    return result;
}
template <int T> Vec<T> &operator*=(Vec<T> &v, real scalar) {
    v = scalar * v;
    return v;
}
template <int T> Vec<T> operator/(Vec<T> v, real scalar) {
    Vec<T> result;
    for (int i = 0; i < T; ++i) {
        result.data[i]  = v.data[i] / scalar;
    }
    return result;
}
template <int T> Vec<T> &operator/=(Vec<T> &v, real scalar) {
    v = v / scalar;
    return v;
}
template <int T> Vec<T> operator-(Vec<T> v) {
    Vec<T> result;
    for (int i = 0; i < T; ++i) {
        result.data[i] = -v.data[i];
    }
    return result;
}
template <int T> Vec<T> operator+(Vec<T> A, Vec<T> B) {
    Vec<T> result;
    for (int i = 0; i < T; ++i) {
        result.data[i] = A.data[i] + B.data[i];
    }
    return result;
}
template <int T> Vec<T> &operator+=(Vec<T> &A, Vec<T> B) {
    A = A + B;
    return A;
}
template <int T> Vec<T> operator-(Vec<T> A, Vec<T> B) {
    Vec<T> result;
    for (int i = 0; i < T; ++i) {
        result.data[i] = A.data[i] - B.data[i];
    }
    return result;
}
template <int T> Vec<T> &operator-=(Vec<T> &A, Vec<T> B) {
    A = A - B;
    return A;
}
template <int T> Vec<T> cwiseProduct(Vec<T> A, Vec<T> B) {
    Vec<T> result;
    for (int i = 0; i < T; ++i) {
        result.data[i] = A.data[i] * B.data[i];
    }
    return result;
}
template <int T> real dot(Vec<T> A, Vec<T> B) {
    real result = 0;
    for (int i = 0; i < T; ++i) {
        result += A.data[i] * B.data[i];
    }
    return result;
}
template <int T> Vec<T> cwiseAbs(Vec<T> A) {
    for (int i = 0; i < T; ++i) A[i] = ABS(A[i]);
    return A;
}
template <int T> real squaredNorm(Vec<T> v) {
    return dot(v, v);
}
template <int T> real norm(Vec<T> v) {
    return sqrt(squaredNorm(v));
}
template <int T> Vec<T> normalized(Vec<T> v) {
    return (1 / norm(v)) * v;
}
typedef Vec<2> vec2;
typedef Vec<3> vec3;
typedef Vec<4> vec4;
// function alternative to C++-style constructors (let's you create e.g. a vec2 anywhere)
// vec2 = { 1, 2 };     // don't need it here; free to use brace initialization
// vec2 = 3 * V2(1, 2); // do need it here
vec2 V2(real x, real y) { vec2 ret = { x, y }; return ret; }
vec3 V3(real x, real y, real z) { vec3 ret = { x, y, z }; return ret; }
real cross(vec2 A, vec2 B) {
    return A.x * B.y - A.y * B.x;
}
vec3 cross(vec3 A, vec3 B) {
    return { A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x };
}
template <int T> real inverseLerp(Vec<T> p, Vec<T> a, Vec<T> b) {
    Vec<T> abs_edge = cwiseAbs(a - b);
    int max_i = -1;
    real max_val = -INFINITY;
    for (int i = 0; i < T; ++i) {
        if (abs_edge[i] > max_val) {
            max_i = i;
            max_val = abs_edge[i];
        }
    }
    return INVERSE_LERP(p[max_i], a[max_i], b[max_i]);
}

template <int T> union Mat {
    real data[T * T];
    real &operator()(int i, int j) { return data[T * i + j]; }
    const real &operator()(int i, int j) const { return data[T * i + j]; }
};
template <int T> Mat<T> operator-(Mat<T> v) {
    Mat<T> result = {};
    for_(i, T * T) result.data[i] = -v.data[i];
    return result;
}
template <int T> Mat<T>  operator + (Mat<T> A, Mat<T> B) {
    Mat<T> ret = {};
    for_(i, T * T) ret.data[i] = A.data[i] + B.data[i];
    return ret;
}
template <int T> Mat<T> &operator +=(Mat<T> &A, Mat<T> B) {
    A = A + B;
    return A;
}
template <int T> Mat<T>  operator - (Mat<T> A, Mat<T> B) {
    Mat<T> ret = {};
    for_(i, T * T) ret.data[i] = A.data[i] - B.data[i];
    return ret;
}
template <int T> Mat<T>  operator * (Mat<T> A, Mat<T> B) {
    Mat<T> ret = {};
    for_(r, T) {
        for_(c, T) {
            for_(k, T) {
                ret(r, c) += A(r, k) * B(k, c);
            }
        }
    }
    return ret;
}
template <int T> Vec<T>  operator * (Mat<T> A, Vec<T> b) {
    Vec<T> ret = {};
    for_(r, T) {
        for_(c, T) {
            ret[r] += A(r, c) * b[c];
        }
    }
    return ret;
}
template <int T> Mat<T> operator *(real scalar, Mat<T> M) {
    Mat<T> result = {};
    for (int i = 0; i < T * T; ++i) {
        result.data[i]  = scalar * M.data[i];
    }
    return result;
}
template <int T> Mat<T> operator*(Mat<T> M, real scalar) {
    return scalar * M;
}
template <int T> Mat<T> operator/(Mat<T> M, real scalar) {
    return (1 / scalar) * M;
}
template <int T> Mat<T> transpose(Mat<T> M) {
    Mat<T> ret = {};
    for_(r, T) {
        for_(c, T) {
            ret(r, c) = M(c, r);
        }
    }
    return ret;
}
template <int T> Mat<T> Identity() {
    Mat<T> ret = {};
    for (int i = 0; i < T; ++i) {
        ret(i, i) = 1;
    }
    return ret;
}
template <int T> real squaredNorm(Mat<T> M) {
    real ret = 0;
    for (int i = 0; i < T * T; ++i) {
        ret += M.data[i] * M.data[i];
    }
    return ret;
}
template <int T> Mat<T> outer(Vec<T> u, Vec<T> v) {
    Mat<T> ret = {};
    for_(r, T) {
        for_(c, T) {
            ret(r, c) = u[r] * v[c];
        }
    }
    return ret;
}
template <int T> Mat<T> firstDerivativeofUnitVector(Vec<T> v) {
    Vec<T> tmp = normalized(v);
    return (1 / norm(v)) * (Identity<T>() - outer(tmp, tmp));
}
#define firstDerivativeOfNorm normalized
#define secondDerivativeOfNorm firstDerivativeofUnitVector
typedef Mat<2> mat2;
typedef Mat<3> mat3;
typedef Mat<4> mat4;
mat4 Translation(real x, real y, real z = 0) {
    mat4 ret = Identity<4>();
    ret(0, 3) = x;
    ret(1, 3) = y;
    ret(2, 3) = z;
    return ret;
}
mat4 Translation(vec2 xy) {
    return Translation(xy.x, xy.y);
}
mat4 Translation(vec3 xyz) {
    return Translation(xyz.x, xyz.y, xyz.z);
}
mat4 Scaling(real x, real y, real z = 1) {
    mat4 ret = {};
    ret(0, 0) = x;
    ret(1, 1) = y;
    ret(2, 2) = z;
    ret(3, 3) = 1;
    return ret;
}
mat4 Rotation(real theta) {
    mat4 ret = {};
    ret(0, 0) =  cos(theta); ret(0, 1) = -sin(theta);
    ret(1, 0) =  sin(theta); ret(1, 1) = cos(theta);
    ret(2, 2) = 1;
    ret(3, 3) = 1;
    return ret;
}
mat4 RotationX(real t) {
    mat4 ret = Identity<4>();
    ret(1, 1) = cos(t); ret(1, 2) = -sin(t);
    ret(2, 1) = sin(t); ret(2, 2) =  cos(t);
    return ret;
}
mat4 RotationY(real t) {
    mat4 ret = Identity<4>();
    ret(0, 0) =  cos(t); ret(0, 2) = sin(t);
    ret(2, 0) = -sin(t); ret(2, 2) = cos(t);
    return ret;
}
mat4 RotationZ(real t) {
    mat4 ret = Identity<4>();
    ret(0, 0) = cos(t); ret(0, 1) = -sin(t);
    ret(1, 0) = sin(t); ret(1, 1) =  cos(t);
    return ret;
}
template <int T> Vec<T> transformPoint(mat4 M, Vec<T> p) {
    Vec<4> p_hom = {};
    memcpy(p_hom.data, p.data, T * sizeof(real));
    p_hom.w = 1;
    Vec<4> ret_hom = M * p_hom;
    ret_hom /= ret_hom.w;
    Vec<T> ret = {};
    memcpy(ret.data, ret_hom.data, T * sizeof(real));
    return ret;
}
template <int T> Vec<T> transformVector(mat4 M, Vec<T> v) {
    Vec<4> v_hom = {};
    memcpy(v_hom.data, v.data, T * sizeof(real));
    Vec<4> ret_hom = M * v_hom;
    Vec<T> ret = {};
    memcpy(ret.data, ret_hom.data, T * sizeof(real));
    return ret;
}
vec2 transformPoint(mat3 M, vec2 p) { // fornow
    vec3 p_hom = {};
    memcpy(p_hom.data, p.data, 2 * sizeof(real));
    p_hom[2] = 1;
    vec3 ret_hom = M * p_hom;
    ret_hom /= ret_hom[2];
    vec2 ret = {};
    memcpy(ret.data, ret_hom.data, 2 * sizeof(real));
    return ret;
}
real determinant(mat2 M) {
    return M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
}
real determinant(mat3 M) {
    return M(0, 0) * (M(1, 1) * M(2, 2) - M(2, 1) * M(1, 2))
        - M(0, 1) * (M(1, 0) * M(2, 2) - M(1, 2) * M(2, 0))
        + M(0, 2) * (M(1, 0) * M(2, 1) - M(1, 1) * M(2, 0));
}
real determinant(mat4 M) {
    real A2323 = M(2, 2) * M(3, 3) - M(2, 3) * M(3, 2);
    real A1323 = M(2, 1) * M(3, 3) - M(2, 3) * M(3, 1);
    real A1223 = M(2, 1) * M(3, 2) - M(2, 2) * M(3, 1);
    real A0323 = M(2, 0) * M(3, 3) - M(2, 3) * M(3, 0);
    real A0223 = M(2, 0) * M(3, 2) - M(2, 2) * M(3, 0);
    real A0123 = M(2, 0) * M(3, 1) - M(2, 1) * M(3, 0);
    return M(0, 0) * ( M(1, 1) * A2323 - M(1, 2) * A1323 + M(1, 3) * A1223 ) 
        - M(0, 1) * ( M(1, 0) * A2323 - M(1, 2) * A0323 + M(1, 3) * A0223 ) 
        + M(0, 2) * ( M(1, 0) * A1323 - M(1, 1) * A0323 + M(1, 3) * A0123 ) 
        - M(0, 3) * ( M(1, 0) * A1223 - M(1, 1) * A0223 + M(1, 2) * A0123 ) ;
}
mat2 inverse(mat2 M) {
    // ASSERT(NON_ZERO(determinant(M)));
    real invdet = 1 / determinant(M);
    return { invdet * M(1, 1), 
        invdet * -M(0, 1), 
        invdet * -M(1, 0), 
        invdet * M(0, 0) };
}
mat3 inverse(mat3 M) {
    // if (IS_ZERO(determinant(M))) {
    //     printf("%E\n", determinant(M));
    //     ASSERT(0);
    // }
    real invdet = 1 / determinant(M);
    return { invdet * (M(1, 1) * M(2, 2) - M(2, 1) * M(1, 2)),
        invdet * (M(0, 2) * M(2, 1) - M(0, 1) * M(2, 2)),
        invdet * (M(0, 1) * M(1, 2) - M(0, 2) * M(1, 1)),
        invdet * (M(1, 2) * M(2, 0) - M(1, 0) * M(2, 2)),
        invdet * (M(0, 0) * M(2, 2) - M(0, 2) * M(2, 0)),
        invdet * (M(1, 0) * M(0, 2) - M(0, 0) * M(1, 2)),
        invdet * (M(1, 0) * M(2, 1) - M(2, 0) * M(1, 1)),
        invdet * (M(2, 0) * M(0, 1) - M(0, 0) * M(2, 1)),
        invdet * (M(0, 0) * M(1, 1) - M(1, 0) * M(0, 1)) };
}
mat4 inverse(mat4 M) {
    // ASSERT(NON_ZERO(determinant(M)));
    real invdet = 1 / determinant(M);
    real A2323 = M(2, 2) * M(3, 3) - M(2, 3) * M(3, 2) ;
    real A1323 = M(2, 1) * M(3, 3) - M(2, 3) * M(3, 1) ;
    real A1223 = M(2, 1) * M(3, 2) - M(2, 2) * M(3, 1) ;
    real A0323 = M(2, 0) * M(3, 3) - M(2, 3) * M(3, 0) ;
    real A0223 = M(2, 0) * M(3, 2) - M(2, 2) * M(3, 0) ;
    real A0123 = M(2, 0) * M(3, 1) - M(2, 1) * M(3, 0) ;
    real A2313 = M(1, 2) * M(3, 3) - M(1, 3) * M(3, 2) ;
    real A1313 = M(1, 1) * M(3, 3) - M(1, 3) * M(3, 1) ;
    real A1213 = M(1, 1) * M(3, 2) - M(1, 2) * M(3, 1) ;
    real A2312 = M(1, 2) * M(2, 3) - M(1, 3) * M(2, 2) ;
    real A1312 = M(1, 1) * M(2, 3) - M(1, 3) * M(2, 1) ;
    real A1212 = M(1, 1) * M(2, 2) - M(1, 2) * M(2, 1) ;
    real A0313 = M(1, 0) * M(3, 3) - M(1, 3) * M(3, 0) ;
    real A0213 = M(1, 0) * M(3, 2) - M(1, 2) * M(3, 0) ;
    real A0312 = M(1, 0) * M(2, 3) - M(1, 3) * M(2, 0) ;
    real A0212 = M(1, 0) * M(2, 2) - M(1, 2) * M(2, 0) ;
    real A0113 = M(1, 0) * M(3, 1) - M(1, 1) * M(3, 0) ;
    real A0112 = M(1, 0) * M(2, 1) - M(1, 1) * M(2, 0) ;
    return { invdet * ( M(1, 1) * A2323 - M(1, 2) * A1323 + M(1, 3) * A1223 ),
        invdet * - ( M(0, 1) * A2323 - M(0, 2) * A1323 + M(0, 3) * A1223 ),
        invdet *   ( M(0, 1) * A2313 - M(0, 2) * A1313 + M(0, 3) * A1213 ),
        invdet * - ( M(0, 1) * A2312 - M(0, 2) * A1312 + M(0, 3) * A1212 ),
        invdet * - ( M(1, 0) * A2323 - M(1, 2) * A0323 + M(1, 3) * A0223 ),
        invdet *   ( M(0, 0) * A2323 - M(0, 2) * A0323 + M(0, 3) * A0223 ),
        invdet * - ( M(0, 0) * A2313 - M(0, 2) * A0313 + M(0, 3) * A0213 ),
        invdet *   ( M(0, 0) * A2312 - M(0, 2) * A0312 + M(0, 3) * A0212 ),
        invdet *   ( M(1, 0) * A1323 - M(1, 1) * A0323 + M(1, 3) * A0123 ),
        invdet * - ( M(0, 0) * A1323 - M(0, 1) * A0323 + M(0, 3) * A0123 ),
        invdet *   ( M(0, 0) * A1313 - M(0, 1) * A0313 + M(0, 3) * A0113 ),
        invdet * - ( M(0, 0) * A1312 - M(0, 1) * A0312 + M(0, 3) * A0112 ),
        invdet * - ( M(1, 0) * A1223 - M(1, 1) * A0223 + M(1, 2) * A0123 ),
        invdet *   ( M(0, 0) * A1223 - M(0, 1) * A0223 + M(0, 2) * A0123 ),
        invdet * - ( M(0, 0) * A1213 - M(0, 1) * A0213 + M(0, 2) * A0113 ),
        invdet *   ( M(0, 0) * A1212 - M(0, 1) * A0212 + M(0, 2) * A0112 ) };
}
mat4 assembleCoordinateSystem(vec3 x, vec3 y, vec3 z, vec3 o) {
    mat4 F = {}; {
        for_(r, 3) {
            F(r, 0) = x[r]; F(r, 1) = y[r]; F(r, 2) = z[r]; F(r, 3) = o[r];
        }
        F(3, 3) = 1;
    }
    return F;
}
mat3 assembleCoordinateSystem(vec2 x, vec2 y, vec2 o) {
    mat3 F = {}; {
        for_(r, 2) {
            F(r, 0) = x[r]; F(r, 1) = y[r]; F(r, 2) = o[r];
        }
        F(2, 2) = 1;
    }
    return F;
}

struct BarycentricSolveResult {
    bool inside;
    real weights[4]; // note empty slot when used for triangle lookup (performBarycentricSolve<2>)
};
template <int T> BarycentricSolveResult performBarycentricSolve(Vec<T> p, Vec<T> *simplexVertices) {
    Mat<T + 1> X = {}; {
        for_(c, T + 1) {
            Vec<T> col = simplexVertices[c];
            for_(r, T) {
                X(r, c) = col[r];
            }
            X(T, c) = 1;
        }
    }
    // ASSERT(NON_ZERO(X.determinant())); // fornow
    Vec<T + 1> P = {}; {
        memcpy(&P, &p, sizeof(p));
        P[T] = 1;
    }
    Vec<T + 1> W = inverse(X) * P;
    bool inside = true; {
        for_(d, T + 1) inside &= (W[d] >= -TINY);
    }

    BarycentricSolveResult ret = {};
    ret.inside = inside;
    memcpy(ret.weights, W.data, sizeof(W));
    return ret;
}

struct Matrix { // minimal dense matrix struct note no allocations
    int num_rows, num_cols;
    real *data;
    // real &operator[] (int i) {
    //     ASSERT(data);
    //     ASSERT(num_rows == 1 || num_cols == 1);
    //     ASSERT(i < MAX(num_rows, num_cols));
    //     return data[i];
    // }
    real &operator() (int r, int c) {
        ASSERT(data);
        ASSERT(r < num_rows && c < num_cols);
        return data[r * num_cols + c];
    }
};

void Matrix_Vector_product(Matrix A, real *x, real *b) {
    memset(b, 0, A.num_rows);
    for_(r, A.num_rows) {
        for_(c, A.num_cols) {
            b[r] += A(r, c) * x[c];
        }
    }
}
void Vector_Matrix_product(real *x, Matrix A, real *b) {
    memset(b, 0, A.num_cols);
    for_(c, A.num_cols) {
        for_(r, A.num_rows) {
            b[c] += x[r] * A(r, c);
        }
    }
}

real Vector_dot(real *x, real *y, int N) {
    real ret = 0;
    for_(i, N) {
        ret += x[i] * y[i];
    }
    return ret;
}
real Vector_squaredNorm(real *x, int N) {
    return Vector_dot(x, x, N);
}
real Vector_norm(real *x, int N) {
    return sqrt(Vector_squaredNorm(x, N));
}

vec2 e_theta(real theta) {
    return { cos(theta), sin(theta) };
}

mat2 R_theta(real theta) {
    return { cos(theta), -sin(theta), sin(theta), cos(theta) };
}

#endif
