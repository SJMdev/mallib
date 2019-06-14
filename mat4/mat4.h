#ifndef INCLUDED_MAT4_
#define INCLUDED_MAT4_
#include "../vec4/vec4.h"
#include "../vector/vec3.h"
#include <string> //for toString
#include <cstring> //memcpy
#include <cmath> //tanh
#include <sstream> // for toString
// #include "mat3.h"

#define PI 3.14159265358979323846f //@Cleanup: move these to math
#define DEG2RAD  PI / 180.0f
#define RAD2DEG  180.f / PI

// internal column major, but behaves as row major from outside perspective.
// is a class for that exact reason.
// @Performance: this could be SIMD optimized, I think.
struct Mat4
{
    Vec4 d_matrix[4];

    public:
        // Mat4() = default;
        // // identity constructor?
        // //explicit Mat4(const Vec4f &x, const Vec4f &y, const Vec4f &z, const Vec4f &w);
        // explicit Mat4(const float xx, const float xy, const float xz, const float xw,
        //               const float yx, const float yy, const float yz, const float yw,
        //               const float zx, const float zy, const float zz, const float zw,
        //               const float wx, const float wy, const float wz, const float ww);
    
        // // explicit Mat4(const  Mat3 &rotation, const Vec3 &translation); we'll see how to fix this!
        // explicit Mat4(const float source[4][4]);

        const Vec4f &operator[](int index) const;
        Vec4f& operator[](int index);
        Mat4   operator*(const float rhs) const;
        Vec4f  operator*(const Vec4 &rhs) const;
        Vec3f  operator*(const Vec3 &rhs) const;
        Mat4   operator*(const Mat4 &rhs) const;
        Mat4   operator+(const Mat4 &rhs) const;
        Mat4   operator-(const Mat4 &rhs) const;

        Mat4  operator*=(const Mat4 &rhs);


        bool compare(const Mat4 &rhs) const;
        bool operator==(const Mat4 &rhs) const;
        bool operator!=(const Mat4 &rhs) const;

        //Hacky stuff here.
        const float *data() const;
        float *data();

};

// this behavior is not yet correct. Do we want to feed column vectors here too?

// inline Mat4::Mat4(const Vec4f &x, const Vec4f &y, const Vec4f &z, const Vec4f &w)
// :

// {
//     d_matrix[0] = x;
//     d_matrix[1] = y;
//     d_matrix[2] = z;
//     d_matrix[3] = w;
// }

// note: the internal data is thus "transposed".
// inline  Mat4::Mat4(   const float xx, const float xy, const float xz, const float xw,
//                       const float yx, const float yy, const float yz, const float yw,
//                       const float zx, const float zy, const float zz, const float zw,
//                       const float wx, const float wy, const float wz, const float ww)
// :
//     d_matrix(Vec4{xx, yx, zx, wx},
//              Vec4{xy, yy, zy, wy},
//              Vec4{xz, yz, zz, wz},
//              VEc4{xw, yw, zw, ww})
// {
//     // d_matrix[0][0]= xx; d_matrix[1][0] =xy; d_matrix[2][0] =xz; d_matrix[3][0] =xw;
//     // d_matrix[0][1]= yx; d_matrix[1][1] =yy; d_matrix[2][1] =yz; d_matrix[3][1] =yw; 
//     // d_matrix[0][2]= zx; d_matrix[1][2] =zy; d_matrix[2][2] =zz; d_matrix[3][2] =zw;
//     // d_matrix[0][3]= wx; d_matrix[1][3] =wy; d_matrix[2][3] =wz; d_matrix[3][3] =ww;
// }


inline bool Mat4::compare(const Mat4 &rhs) const
{
    const float *ptr1, *ptr2;
    ptr1 = reinterpret_cast<const float *>(d_matrix);
    ptr2 = reinterpret_cast<const float *>(rhs.d_matrix);
    for (int idx = 0; idx != 4 * 4; ++i)
    {
        if ( ptr1[i] != ptr2[i] )
        {
            return false;
        }
    }
    return true;
}


inline Mat4::Mat4( const float source[ 4 ][ 4 ] )
{
  memcpy( d_matrix, source, 4 * 4 * sizeof( float ) );
}

inline const Vec4f &Mat4::operator[](int index) const
{
    if (index < 0 || index > 4) {}
    return d_matrix[index];
}

inline Vec4f &Mat4::operator[](int index)
{
    return d_matrix[index];
}

inline Mat4 Mat4::operator*(const float rhs) const
{
    return  {
                    d_matrix[0][0] * rhs, d_matrix[1][0] * rhs, d_matrix[2][0] * rhs, d_matrix[3][0] * rhs,
                    d_matrix[0][1] * rhs, d_matrix[1][1] * rhs, d_matrix[2][1] * rhs, d_matrix[3][1] * rhs,
                    d_matrix[0][2] * rhs, d_matrix[1][2] * rhs, d_matrix[2][2] * rhs, d_matrix[3][2] * rhs,
                    d_matrix[0][3] * rhs, d_matrix[1][3] * rhs, d_matrix[2][3] * rhs, d_matrix[3][3] * rhs
            };
}

inline Vec4f Mat4::operator*(const Vec4f &rhs) const
{
    return {
                d_matrix[0][0] * rhs.x + d_matrix[1][0] * rhs.y + d_matrix[2][0] * rhs.z + d_matrix[3][0] * rhs.w,
                d_matrix[0][1] * rhs.x + d_matrix[1][1] * rhs.y + d_matrix[2][1] * rhs.z + d_matrix[3][1] * rhs.w,
                d_matrix[0][2] * rhs.x + d_matrix[1][2] * rhs.y + d_matrix[2][2] * rhs.z + d_matrix[3][2] * rhs.w,
                d_matrix[0][3] * rhs.x + d_matrix[1][3] * rhs.y + d_matrix[2][3] * rhs.z + d_matrix[3][3] * rhs.w
            };
}

inline Mat4 Mat4::operator*(const Mat4 &rhs) const
{
    int i, j;
    const float *m1Ptr, *m2Ptr;
    float *dstPtr;
    Mat4 dst;

    m1Ptr = reinterpret_cast<const float *>(this);
    m2Ptr = reinterpret_cast<const float *>(&rhs);
    dstPtr = reinterpret_cast<float *>(&dst);

    for ( i = 0; i < 4; i++ ) {
        for ( j = 0; j < 4; j++ ) {
            *dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
            + m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
            + m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
            + m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
            dstPtr++;
        }
        m1Ptr += 4;
    }
    return dst;
}


inline Mat4 Mat4::operator+(const Mat4 &rhs) const
{
    return  {
                d_matrix[0][0] + rhs[0][0], d_matrix[1][0] + rhs[1][0], d_matrix[2][0] + rhs[2][0], d_matrix[3][0] + rhs[3][0],
                d_matrix[0][1] + rhs[0][1], d_matrix[1][1] + rhs[1][1], d_matrix[2][1] + rhs[2][1], d_matrix[3][1] + rhs[3][1],
                d_matrix[0][2] + rhs[0][2], d_matrix[1][2] + rhs[1][2], d_matrix[2][2] + rhs[2][2], d_matrix[3][2] + rhs[3][2],
                d_matrix[0][3] + rhs[0][3], d_matrix[1][3] + rhs[1][3], d_matrix[2][3] + rhs[2][3], d_matrix[3][3] + rhs[3][3]
            };
} 

inline Mat4 Mat4::operator-(const Mat4 &rhs) const
{
    return  {
                d_matrix[0][0] - rhs[0][0], d_matrix[1][0] - rhs[1][0], d_matrix[2][0] - rhs[2][0], d_matrix[3][0] - rhs[3][0],
                d_matrix[0][1] - rhs[0][1], d_matrix[1][1] - rhs[1][1], d_matrix[2][1] - rhs[2][1], d_matrix[3][1] - rhs[3][1],
                d_matrix[0][2] - rhs[0][2], d_matrix[1][2] - rhs[1][2], d_matrix[2][2] - rhs[2][2], d_matrix[3][2] - rhs[3][2],
                d_matrix[0][3] - rhs[0][3], d_matrix[1][3] - rhs[1][3], d_matrix[2][3] - rhs[2][3], d_matrix[3][3] - rhs[3][3]
            };
}

inline Mat4 &Mat4::operator*=(const float rhs) 
{
    d_matrix[0][0] *= rhs; d_matrix[1][0] *= rhs; d_matrix[2][0] *= rhs; d_matrix[3][0] *= rhs;
    d_matrix[0][1] *= rhs; d_matrix[1][1] *= rhs; d_matrix[2][1] *= rhs; d_matrix[3][1] *= rhs;
    d_matrix[0][2] *= rhs; d_matrix[1][2] *= rhs; d_matrix[2][2] *= rhs; d_matrix[3][2] *= rhs;
    d_matrix[0][3] *= rhs; d_matrix[1][3] *= rhs; d_matrix[2][3] *= rhs; d_matrix[3][3] *= rhs;
    
    return *this;
}

inline Mat4 &Mat4::operator*=(const Mat4 &rhs)
{
     *this = (*this) * rhs;
     return *this;
}

// inline Mat4 &Mat4::operator+=(const Mat4 &rhs)
// {
//     d_matrix[0][0] += rhs[0][0]; d_matrix[1][0] += rhs[1][0]; d_matrix[2][0] += rhs[2][0]; d_matrix[3][0] += rhs[3][0];
//     d_matrix[0][1] += rhs[0][1]; d_matrix[1][1] += rhs[1][1]; d_matrix[2][1] += rhs[2][1]; d_matrix[3][1] += rhs[3][1];
//     d_matrix[0][2] += rhs[0][2]; d_matrix[1][2] += rhs[1][2]; d_matrix[2][2] += rhs[2][2]; d_matrix[3][2] += rhs[3][2];
//     d_matrix[0][3] += rhs[0][3]; d_matrix[1][3] += rhs[1][3]; d_matrix[2][3] += rhs[2][3]; d_matrix[3][3] += rhs[3][3];
    
//     return *this;
// }

// inline Mat4 &Mat4::operator-=(const Mat4 &rhs)
// {
//     d_matrix[0][0] -= rhs[0][0]; d_matrix[1][0] -= rhs[1][0]; d_matrix[2][0] -= rhs[2][0]; d_matrix[3][0] -= rhs[3][0];
//     d_matrix[0][1] -= rhs[0][1]; d_matrix[1][1] -= rhs[1][1]; d_matrix[2][1] -= rhs[2][1]; d_matrix[3][1] -= rhs[3][1];
//     d_matrix[0][2] -= rhs[0][2]; d_matrix[1][2] -= rhs[1][2]; d_matrix[2][2] -= rhs[2][2]; d_matrix[3][2] -= rhs[3][2];
//     d_matrix[0][3] -= rhs[0][3]; d_matrix[1][3] -= rhs[1][3]; d_matrix[2][3] -= rhs[2][3]; d_matrix[3][3] -= rhs[3][3];

//     return *this;
// }

inline Mat4 operator*(const float lhs, const Mat4 &rhs)
{
    return rhs * lhs; //matrix * float
}

inline Vec4f operator*(const Vec4f &lhs, const Mat4 &rhs)
{
    return rhs * lhs; //matrix * vector4;
}

inline Vec3f operator*(const Vec3f &lhs, const Mat4 &rhs)
{
    return rhs * lhs; //matrix * vector3;
}

// inline Vec4f &operator*=(Vec4f &lhs, const Mat4 &rhs)
// {
//     lhs = rhs * lhs;
//     return lhs;
// }

inline bool Mat4::operator==(const Mat4 &rhs) const 
{
  return compare(rhs);
}

inline bool Mat4::operator!=(const Mat4 &rhs) const 
{
  return !compare(rhs);
}


inline float *Mat4::data()
{
    return d_matrix[0].data();
}

inline const float *Mat4::data() const
{
    return d_matrix[0].data();
}





          
#endif

