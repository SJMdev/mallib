#ifndef INCLUDED_ASSET_
#define INCLUDED_ASSET_
#include "../scene/scene.h"
#include "../vec3/vec3.h"
#include "../vec2/vec2.h"
#include <vector>

namespace asset
{
    enum Asset_Type
    {
        MODEL,
        MUSIC,
        SOUND_FX
    };

    struct Face
    {
        Vec3u v0_indices;
        Vec3u v1_indices;
        Vec3u v2_indices;
    };

    //@Refactor: put this in a shared header.
    struct Vertex
    {
        Vec3 position;
        Vec2 tex_coords;
        Vec3 normals;
    };

    struct Raw_Obj_Data
    {
        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<Vec2> tex_coords;
        std::vector<Face> faces; // these indices are offset by 1.
        std::vector<Vertex> vertices;
    };

    struct Material
    {
        Vec3 Ka; // ambient color
        Vec3 Kd; // diffuse color
        Vec3 Ks; // specular color
        float Ns; // specular exponent?
        uint16_t illum; // if illum == 1, skip Ks.
        float alpha; // either d or inverse transparency.
 
    };

    //@Incomplete: load into array, return index?
    struct Texture
    {
        std::vector<char> buffer;
        Vec2 dimensions;
    };

    void load_scene_from_file(scene::Scene& scene,            const std::string& filename);
    void load_obj_from_file(asset::Raw_Obj_Data& raw_data,    const std::string& filename);
    void load_mtl_from_file(asset::Material& material,        const std::string& filename);

}

#endif
