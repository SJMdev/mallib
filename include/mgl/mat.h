#ifndef INCLUDED_MAT_
#define INCLUDED_MAT_

#include <fmt/format.h>
#include <fmt/core.h> // for printing mat3, mat4.

#include <mgl/mat4.h>
#include <mgl/mat3.h>
#include <mgl/vec3.h>
#include <mgl/vec4.h>
#include <mgl/xform_state.h>

#include <limits> // for double det.

//idea: instead of creating a translation / scale / rotation matrix everytime,
// keep a static one in the namespace?
namespace mgl
{
    //@Refactor: move this to math?
    static inline const float PI = 3.1415926535;
    static inline const float DEG2RAD = mgl::PI / 180.0f;
    static inline const float RAD2DEG = 180.0f / mgl::PI;

    inline float radians(float angle_in_degrees)
    {
      return angle_in_degrees * DEG2RAD;
    }

    inline float degrees(float angle_in_radians)
    {
      return angle_in_radians * RAD2DEG;
    }

    // mat4
    float* value_ptr(mgl::mat4& lhs);
    mgl::mat4 model_from_xform_state(const Xform_State& state);
    // mgl::mat4 mat4_from_quat(const mgl::vec4& quaternion);
    // mgl::mat4 mat4_from_mat3(const mgl::mat4& lhs);
    // mgl::mat4 mat4_from_row_vec3(const mgl::vec3& v0, const mgl::vec3& v1, const mgl::vec3& v2);

    mgl::mat4 mat4_identity();
    void to_transpose(mgl::mat4& lhs);
    mgl::mat4 transpose(const mgl::mat4& lhs);
    void to_identity(mgl::mat4 &lhs);

    mgl::mat4 translate(const mgl::mat4& matrix, const mgl::vec3& vector);
    mgl::mat4 scale(const mgl::mat4& matrix, const float scale_factor);
    mgl::mat4 scale(const float s);
    mgl::mat4 rotate(mgl::mat4& lhs, const float angle, const mgl::vec3& axis); //expects identity
    mgl::mat4 perspective(const float fov_y,
                     const float aspect_ratio,
                     const float near_plane,
                     const float far_plane);

    mgl::mat4 translation(const mgl::vec3& position);
    mgl::mat4 look_at(const mgl::vec3& eye, const mgl::vec3& center, const mgl::vec3& up);
    mgl::mat4 ortho(const float left,
               const float right,
               const float top,
               const float bottom,
               const float z_near,
               const float z_far
              );

    // mat3
    float* value_ptr(mgl::mat3& lhs);
    mgl::mat3 mat3_identity();
    mgl::mat3 normal_transform(const mgl::mat4& model_view_matrix);
    mgl::mat3 mat3_from_mat4(const mgl::mat4& matrix);

    // @Note: return reference for chaining?
    // try_inverse, since it can fail.
    bool try_inverse(mgl::mat3& lhs);
    void transpose(mgl::mat3& lhs); 
    inline float determinant(mgl::mat3& lhs);


};

inline float* mgl::value_ptr(mgl::mat4& lhs)
{
  return &lhs.data[0][0];
}

inline float* mgl::value_ptr(mgl::mat3& lhs)
{
  return &lhs.data[0][0];
}

// @Refactor: either write templated swap, use an intrinsic or include it from utility.
static void swap(float & lhs, float& rhs)
{
    float t = lhs;
    lhs = rhs;
    rhs = t;
}


// inline mgl::mat4 mgl::mat4_from_row_vec3(const mgl::vec3& v0, const mgl::vec3& v1, const mgl::vec3& v2)
// {
//     return {v0.x, v0.y, v0.z, 0,
//             v1.x, v1.y, v1.z, 0,
//             v2.x, v2.y, v2.z, 0,
//             0,      0,     0, 1};
// }

// //@Refactor: rotation matrix is generated as transpose. hm..
// inline mgl::mat4 mgl::mat4_from_quat(const mgl::vec4& quaternion)
// {
//     mgl::vec3 v0 = rotate_by_quat(mgl::vec3{1,0,0}, quaternion);  
//     mgl::vec3 v1 = rotate_by_quat(mgl::vec3{0,1,0}, quaternion);
//     mgl::vec3 v2 = rotate_by_quat(mgl::vec3{0,0,1}, quaternion);

//     mgl::mat4 result =  mgl::mat4_from_row_vec3(v0, v1, v2); // in mat4.
//     mgl::to_transpose(result);

//     return result;
// }

inline mgl::mat4 mgl::model_from_xform_state(const Xform_State& state)
{
    mgl::mat4 model_matrix       = mgl::mat4_identity();
    mgl::mat4 rotation_matrix    = mgl::mat4_identity();//mat4_from_quat(state.q_orientation); // in mat4.
    mgl::mat4 translation_matrix = mgl::translation(state.position);
    mgl::mat4 scale_matrix = mgl::scale(state.scale);

    //@Note: since we are in a row-matrix world, we apply the transformations in the  reverse order.""
    // translation_matrix = mgl::mat4_identity();
    // model_matrix *= translation_matrix;
    // model_matrix *= rotation_matrix;
    // model_matrix[0][0] *= state.scale;
    // model_matrix[1][1] *= state.scale;
    // model_matrix[2][2] *= state.scale;

    model_matrix = translation_matrix * rotation_matrix * scale_matrix * model_matrix;

    return model_matrix;
}


// doesn't really matter.
inline void mgl::to_transpose(mgl::mat4& lhs)
{
   swap(lhs[0][1], lhs[1][0]);
   swap(lhs[0][2], lhs[2][0]);
   swap(lhs[0][3], lhs[3][0]);
   swap(lhs[1][2], lhs[2][1]);
   swap(lhs[1][3], lhs[3][1]);
   swap(lhs[2][3], lhs[3][2]);
}


// RH COLUMN MAJOR!
inline mgl::mat4 mgl::translation(const mgl::vec3& position)
{
  // row major
  // return {
  //          1, 0, 0, position.x,
  //          0, 1, 0, position.y,
  //          0, 0, 1, position.z,
  //          0, 0, 0, 1          
  //        };
    // column major
    mgl::mat4 result = mat4_identity();
    result[3][0] = position.x;
    result[3][1] = position.y;
    result[3][2] = position.z;

    return result;

}


inline mgl::mat4 mgl::scale(const float s)
{
    return {
              s, 0, 0, 0,
              0, s, 0, 0,
              0, 0, s, 0,
              0, 0, 0, 1
           };
}

inline mgl::mat4 mgl::mat4_identity()
{
  return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
         };
}

// RH COLUMN MAJOR
inline mgl::mat4 mgl::ortho(const float left,
                           const float right,
                           const float top,
                           const float bot,
                           const float z_near,
                           const float z_far
                           )
{
    // row major:
    // return mgl::mat4{ 2.0f / right - left,             0.0f,                        0.0f,    - (right + left / right - left),
    //                  0.0f, 2.0f / top - bot,                       0.0f,                      -(top + bot / top - bot),
    //                  0.0f,             0.0f, (-2.0f / (z_far - z_near)),            -(z_far + z_near / z_far - z_near),
    //                  0.0f,             0.0f,                       0.0f,                                  1};  

    mgl::mat4 result{};

    // column major.
    result[0][0] =   2.0f / (right - left);
    result[1][1] =   2.0f / (top - bot);
    result[2][2] = - 2.0f / (z_far - z_near);
    result[3][0] = - (right + left) / (right - left);
    result[3][1] = - (top + bot) / (top - bot);
    result[3][2] = - (z_far + z_near) / (z_far - z_near);
    result[3][3] = 1.0f;

    return result;
}   



// Projection Matrix:
// perspective near_z and far_z define the clipping, not the actual bounds.
// OpenGL assumes that the points in the scene are projected on the near clipping planes,
// rather than on a plane that lies one unit away from the camera position.
// note that near_z and far_z should be positive: they indicate DISTANCE from the camera. 
// RH COLUMN MAJOR
inline mgl::mat4 mgl::perspective(const float fov_y,
                     const float aspect_ratio,
                     const float near_plane,
                     const float far_plane)
{
    mgl::mat4 matrix = {}; // identity?

    const float tan_half_fov = tanf(fov_y * 0.5f);

    // row major
    // matrix[0][0] = 1.0f /(aspect_ratio * tan_half_fov);
    // matrix[1][1] = 1.0f / tan_half_fov;
    // matrix[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    // matrix[2][3] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    // matrix[3][2] = - 1.0f;
    // matrix[3][3] = 0.0f;

    // column major
    matrix[0][0] = 1.0f /(aspect_ratio * tan_half_fov);
    matrix[1][1] = 1.0f / tan_half_fov;
    matrix[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    matrix[3][2] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    matrix[2][3] = - 1.0f;
    matrix[3][3] = 0.0f;



    return matrix;
}

// RH COLUMN MAJOR
inline mgl::mat4 mgl::look_at(const vec3& eye, const vec3& target, const vec3& up)
{
   // modeled after gluLookAt. 

   vec3 forward = target - eye; // f = coord_system
   forward      = mgl::normalize(forward);
   vec3 up_norm = mgl::normalize(up);
   vec3 right   = mgl::cross(forward, up_norm);
   vec3 new_up  = mgl::cross(right, forward);

   // The up vector must not be parallel to the line of sight from the
   //           eye point to the reference point.  
   // assert(!(up_norm == forward));

   // row major.
   // return {right.x, right.y,  right.z,          -mgl::dot(right, eye),
   //         new_up.x, new_up.y, new_up.z,         -mgl::dot(new_up, eye), 
   //         -forward.x, -forward.y, -forward.z,   mgl::dot(forward, eye),
   //          0.0f, 0.0f, 0.0f,                     1.0f};

   // column major.
   mgl::mat4 result{};
   result[0][0] = right.x;
   result[1][0] = right.y;
   result[2][0] = right.z;
   result[3][0] = -mgl::dot(right,eye);
   result[0][1] = new_up.x;
   result[1][1] = new_up.y;
   result[2][1] = new_up.z;
   result[3][1] = -mgl::dot(new_up, eye);
   result[0][2] = -forward.x;
   result[1][2] = -forward.y;
   result[2][2] = -forward.z;
   result[3][2] = mgl::dot(forward,eye);
   result[3][3] = 1.0f;


   return result;
}
   

inline mgl::vec4 operator*(const float lhs, mgl::vec4& rhs)
{
    return mgl::vec4{rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs};
}


// mat4 with vec4. Mat4 is column major.
inline mgl::vec4 operator*(mgl::mat4& lhs, const mgl::vec4& rhs)
{
    mgl::vec4 result{};

    // row major:
    // result.x = mgl::dot(rhs, mgl::vec4{lhs[0][0], lhs[0][1], lhs[0][2], lhs[0][3]});
    // result.y = mgl::dot(rhs, mgl::vec4{lhs[1][0], lhs[1][1], lhs[1][2], lhs[1][3]});
    // result.z = mgl::dot(rhs, mgl::vec4{lhs[2][0], lhs[2][1], lhs[2][2], lhs[2][3]});
    // result.w = mgl::dot(rhs, mgl::vec4{lhs[3][0], lhs[3][1], lhs[3][2], lhs[3][3]});

    // column major
    result.x = mgl::dot(mgl::vec4{lhs[0][0], lhs[1][0], lhs[2][0], lhs[3][0]}, rhs);
    result.y = mgl::dot(mgl::vec4{lhs[0][1], lhs[1][1], lhs[2][1], lhs[3][1]}, rhs);
    result.z = mgl::dot(mgl::vec4{lhs[0][2], lhs[1][2], lhs[2][2], lhs[3][2]}, rhs);
    result.w = mgl::dot(mgl::vec4{lhs[0][3], lhs[1][3], lhs[2][3], lhs[3][3]}, rhs);

    return result;
}





//@VOLATILE: at the time of writing, mat4 is column major, while mat3 is row major.
inline mgl::mat3 mgl::mat3_from_mat4(const mat4& origin)
{
    // mat3 row, mat4 row:
    // return {matrix[0][0], matrix[0][1], matrix[0][2],
    //         matrix[1][0], matrix[1][1], matrix[1][2],
    //         matrix[2][0], matrix[2][1], matrix[2][2]};

    //mat3 row, mat4 col:

    // select top left of the matrix. first three rows, first three columns.
    mat3 result{};
    result[0][0] = origin[0][0];
    result[0][1] = origin[1][0];
    result[0][2] = origin[2][0];
    result[1][0] = origin[0][1];
    result[1][1] = origin[1][1];
    result[1][2] = origin[2][1];
    result[2][0] = origin[0][2];
    result[2][1] = origin[1][2];
    result[2][2] = origin[2][2];
    
    return result;
}

inline bool mgl::try_inverse(mat3& lhs)
{
    mgl::mat3 inverse = {};

    inverse[0][0] = lhs[1][1] * lhs[2][2] - lhs[2][1] * lhs[1][2];
    inverse[0][1] = lhs[2][1] * lhs[0][2] - lhs[0][1] * lhs[2][2];
    inverse[0][2] = lhs[0][1] * lhs[1][2] - lhs[1][1] * lhs[0][2];

    double det = lhs[0][0] * inverse[0][0] + lhs[1][0] * inverse[0][1] + lhs[2][0] * inverse[0][2];
    //@FIXME:::FIXME::
    // if det is too small, the inverse does not exist.
    if (det <= std::numeric_limits<double>::lowest()) {
        return false;
    }
    double inv_det = 1.0f / det;
    inverse[1][0] = lhs[2][0] * lhs[1][2] - lhs[1][0] * lhs[2][2];
    inverse[2][0] = lhs[1][0] * lhs[2][1] - lhs[2][0] * lhs[1][1];
    inverse[1][1] = lhs[0][0] * lhs[2][2] - lhs[2][0] * lhs[0][2];
    inverse[2][1] = lhs[2][0] * lhs[0][1] - lhs[0][0] * lhs[2][1];
    inverse[1][2] = lhs[1][0] * lhs[0][2] - lhs[0][0] * lhs[1][2];
    inverse[2][2] = lhs[0][0] * lhs[1][1] - lhs[1][0] * lhs[0][1];

    lhs[0][0] = inverse[0][0] * inv_det;

    lhs[1][0] = inverse[1][0] * inv_det;
    lhs[2][0] = inverse[2][0] * inv_det;

    lhs[0][1] = inverse[0][1] * inv_det;
    lhs[1][1] = inverse[1][1] * inv_det;
    lhs[2][1] = inverse[2][1] * inv_det;

    lhs[0][2] = inverse[0][2] * inv_det;
    lhs[1][2] = inverse[1][2] * inv_det;
    lhs[2][2] = inverse[2][2] * inv_det;

    return true;
}


inline float mgl::determinant(mat3& lhs)
{
    //det(A) = a11a22a33 + a12a23a31 + a13a21a32 
    //         -a13a22a31 - a12a21a33 - a11a23a32.

    return {   lhs[0][0] * lhs[1][1] * lhs[2][2]
             + lhs[0][1] * lhs[1][2] * lhs[2][0]
             + lhs[0][2] * lhs[1][0] * lhs[2][1]
             - lhs[0][2] * lhs[1][1] * lhs[2][0]
             - lhs[0][1] * lhs[1][0] * lhs[2][2]
             - lhs[0][0] * lhs[1][2] * lhs[2][1] }; 
}


inline void mgl::transpose(mat3& lhs)
{
    swap(lhs[0][1], lhs[1][0]);
    swap(lhs[0][2], lhs[2][0]);
    swap(lhs[1][2], lhs[2][1]);
}

inline mgl::mat3 mgl::normal_transform(const mat4& model_view_matrix)
{
    // normal matrix is calculated from the modelview
    mat3 normal_matrix = mat3_from_mat4(model_view_matrix);
    if (!try_inverse(normal_matrix))
    { 
        fmt::print("[mat] normal_transform: try_inverse failed.\n");
    }
    transpose(normal_matrix);

    return normal_matrix;
}

inline mgl::mat3 mgl::mat3_identity()
{
  return mgl::mat3{1.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 1.0f};
}



// print definitions for mat3 and mat4.
//@Refactor: we need to move these to somewhere.
namespace fmt {
    template <>
    struct formatter<mgl::mat3> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const mgl::mat3 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "\n|{:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f}|\n|{:.3f} {:.3f} {:.3f}|\n",
            lhs[0][0], lhs[0][1], lhs[0][2],
            lhs[1][0], lhs[1][1], lhs[1][2],
            lhs[2][0], lhs[2][1], lhs[2][2]);
      }
    };

    template <>
    struct formatter<mgl::mat4> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const mgl::mat4 &lhs, FormatContext &ctx) {
        return format_to(ctx.out(),
            "\n c0: |{:.3f} {:.3f} {:.3f} {:.3f}|\n c1: |{:.3f} {:.3f} {:.3f} {:.3f}|\n c2: |{:.3f} {:.3f} {:.3f} {:.3f}|\n c3: |{:.3f} {:.3f} {:.3f} {:.3f}|\n",
            lhs[0][0], lhs[0][1], lhs[0][2], lhs[0][3],
            lhs[1][0], lhs[1][1], lhs[1][2], lhs[1][3],
            lhs[2][0], lhs[2][1], lhs[2][2], lhs[2][3],
            lhs[3][0], lhs[3][1], lhs[3][2], lhs[3][3]);
      }
    };

    template <>
    struct formatter<Xform_State> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const Xform_State &state, FormatContext &ctx) {
        return format_to(ctx.out(),
            "\nx: {:.3f}, y: {:.3f}, z: {:.3f}\nq_x: {:.3f}, q_y: {:.3f}, q_z: {:.3f}, q_w: {:.3f}\nscale:{:.3f}\n",
            state.position.x, state.position.y, state.position.z,
            state.q_orientation.x, state.q_orientation.y, state.q_orientation.z, state.q_orientation.w,
            state.scale
            );
      }
    };

};



#endif