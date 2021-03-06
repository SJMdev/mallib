#ifndef INCLUDED_vec4_
#define INCLUDED_vec4_

//@Note: is this a problem with matrices?
namespace mgl
{
    union vec4
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        struct 
        {
            float r;
            float g;
            float b;
            float a;
        };
        float data[4];

        float operator[](size_t idx)
        {
            return data[idx];
        }
        float const operator[](size_t idx) const
        {
            return data[idx];
        }
    }; 
};



// //@Refactor: restructure the vec3 /vec4 inconsistencies. now vec3 lives in the vec namespace.
// // do we want all to live in the vec namespace?
// inline void normalize(mgl::vec4& lhs)
// {
//     float one_over_sum =  1.0f / sqrt(powf(lhs.x,2.0f) + powf(lhs.y, 2.0f) + powf(lhs.z, 2.0f) + powf(lhs.w, 2.0f));
//     lhs.x *= one_over_sum;
//     lhs.y *= one_over_sum;
//     lhs.z *= one_over_sum;
//     lhs.w *= one_over_sum;
// }


#endif