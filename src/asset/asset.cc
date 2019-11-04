#include "asset.h"
#include "../file/file.h"
#include "fmt/core.h"
#include <sstream>

static void generate_vertices_from_raw_data(asset::Raw_Obj_Data& raw_data)
{
    //@Memory: preallocate space.   
    raw_data.vertices.reserve(raw_data.faces.size());
    for (auto &face : raw_data.faces)
    {
        //@Refactor: maybe rewrite face to use the index set?
            asset::Vertex v0 = {
                raw_data.positions[face.v0_indices.data[0]],
                raw_data.tex_coords[face.v0_indices.data[1]],
                raw_data.normals[face.v0_indices.data[2]]
            };

            asset::Vertex v1 = {
                raw_data.positions[face.v1_indices.data[0]],
                raw_data.tex_coords[face.v1_indices.data[1]],
                raw_data.normals[face.v1_indices.data[2]]
            };

            asset::Vertex v2 = {
                raw_data.positions[face.v2_indices.data[0]],
                raw_data.tex_coords[face.v2_indices.data[1]],
                raw_data.normals[face.v2_indices.data[2]]
            };

            raw_data.vertices.emplace_back(v0);
            raw_data.vertices.emplace_back(v1);
            raw_data.vertices.emplace_back(v2);
    }
}

//@Note: assume an empty scene here.
void asset::load_scene_from_file(scene::Scene& scene, const std::string& filename)
{
	auto file_content = std::string();
    file::file_to_string(filename, file_content);
    std::stringstream data_stream(file_content);

    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[max_string_length] = {0}; // used for the garbage at the start of each line. 


    // @Memory: how to predict how to preallocate?
    fmt::print("[asset] Warning: load_scene_from_file does not efficiently preallocate.\n");
    scene.set_pieces.reserve(10);
    // we use a pointer here so the logic for creating new set pieces 
    // is more concise.
    scene::Set_Piece* set_piece = nullptr;
    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
    	line_number++;
    	if (line[0] == '#') // comment
    	{
    		continue;
    	}    	
    	if (line[0] == ':' && line[1] == '/') // name of the object in the scene.
    	{    		
    		scene.set_pieces.emplace_back();
    		set_piece = &scene.set_pieces.back();
    		auto name = std::string{100};
            fmt::print("name.size(): {}", name.size());
    		sscanf(line.c_str(), "%2s %50s", garbage_buffer, &name[0]);
    		set_piece->name = name;
    	}

    	else if (line[0] == 'm' && line[1] == 'o') // model
    	{
    		auto model_name = std::string{100};
    		sscanf(line.c_str(), "%15s \"%50s\"", garbage_buffer, &model_name[0]);
    		set_piece->model_name = model_name;
    	}
    	else if (line[0] == 'm' && line[1] == 'a') // material
    	{
    		auto material_name = std::string{100};
    		sscanf(line.c_str(), "%15s \"%50s\"", garbage_buffer, &material_name[0]);
    		set_piece->material_name = material_name;
    	}
    	else if (line[0] == 't' && line[1] == 'e') // texture
    	{
    		auto texture_name = std::string{100};
    		sscanf(line.c_str(), "%15s \"%50s\"", garbage_buffer, &texture_name[0]);
    		set_piece->texture_name = texture_name;
    	}
    	else if (line[0] == 'p' && line[1] == '0') // position
    	{
    		Vec3 position = {};
    		sscanf(line.c_str(), "%15s %f %f %f", garbage_buffer, &position.x, &position.y, &position.z);
    		set_piece->xform_state.position = position;
    	}
    	else if (line[0] == 'q' && line[1] == '_') // q_orientation
    	{
    		Vec4 q_orientation = {};
    		sscanf(line.c_str(), "%15s %f %f %f %f", garbage_buffer, &q_orientation.x, &q_orientation.y, &q_orientation.z, &q_orientation.w);
    		set_piece->xform_state.q_orientation = q_orientation;
    	}
    	else if (line[0] == 's' && line[1] == 'c') // scale
    	{
    		float scale = 0.0f;
    		sscanf(line.c_str(), "%15s %f", garbage_buffer, &scale);
    		set_piece->xform_state.scale = scale;
    	}
    	else
    	{
    		fmt::print("[asset] Warning. file {}: No label recognized on line {}. content: {}\n",filename, line_number, line);
    	}
    }
}

//@Refactor:    
// we need a null-terminated bytestring in order to use sscanf.
// we can point to an address in memory, but then it doesn't null terminate.
// read until either linebreak or EOF. replace linebreak by \0.
// hand ptr over to user.
// in essence, we want to make our own getline for a char array.
// read line by line 
// size_t file_size = file::get_file_size(filename);
// if (file_size == 0)
//     fmt::print("load_obj: could not read file {}", filename);
// hand me a pointer, and I will alloc for you.
// we can mitigate the unsafety of sscanf by limiting the number of characters that are 
// read by %s by affixing a number, i.e. %1s for 1 character only.  
void asset::load_obj_from_file(asset::Raw_Obj_Data& raw_data, const std::string& filename)
{
    std::string data ={};
    file::file_to_string(filename, data);
    std::stringstream data_stream(data);
    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[20] = {}; // used for the garbage in each line. 

    // @Memory: how to predict how to preallocate?
    fmt::print("[asset] Warning: load_obj does not efficiently preallocate.\n");
    raw_data.positions.reserve(4000);
    raw_data.normals.reserve(4000);
    raw_data.tex_coords.reserve(4000);
    raw_data.faces.reserve(4000);

    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        ++line_number;
        if (line[0] == '#') // comment 
        {
            continue;
        }
        else if (line[0] == 's')
        {
            // what does the 's' mean?
            continue;
        }
        else if (line[0] ==  'v' && line[1] == ' ') // vertex
        {
            Vec3 pos = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &pos.x, &pos.y, &pos.z);
            raw_data.positions.emplace_back(pos);
        } 
        else if (line[0] == 'v' && line[1] == 't') // texture coordinates
        {
            Vec2 tex_coords = {};
            sscanf(line.c_str(), "%9s %f %f", garbage_buffer, &tex_coords.x, &tex_coords.y);
            raw_data.tex_coords.emplace_back(tex_coords);
        }
        else if (line[0] == 'v' && line[1] == 'n') // vertex normals
        {
            Vec3 normal = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &normal.x, &normal.y, &normal.z);
            raw_data.normals.emplace_back(normal);
        }
        else if (line[0] == 'f') // face indices
        {
            Face face = {};
            //@incomplete: This will barf on unstructured obj files. for now, we assume everything's present.
            sscanf(line.c_str(), "%9s %u / %u / %u %u /%u /%u %u /%u /%u",
                   garbage_buffer,
                   &face.v0_indices.data[0], &face.v0_indices.data[1], &face.v0_indices.data[2],
                   &face.v1_indices.data[0], &face.v1_indices.data[1], &face.v1_indices.data[2],
                   &face.v2_indices.data[0], &face.v2_indices.data[1], &face.v2_indices.data[2]
                   );

            // The indices in the wavefront obj start at 1. we offset them to use them correctly
            // with the arrays in raw_data.
            //@Note: be careful with this subtraction. made a dumb mistake.
            face.v0_indices -= 1;
            face.v1_indices -= 1;
            face.v2_indices -= 1;
            
            raw_data.faces.emplace_back(face);
        }
        else
            fmt::print("[asset] ERROR: load_obj: no matching indicator. line number: {}. content: {}\n", line_number, line);
    }
    fmt::print("[asset] succesfully loaded {}. num_faces: {}\n", filename, raw_data.faces.size());
    generate_vertices_from_raw_data(raw_data);
}


void asset::load_mtl_from_file(Material& mat, const std::string& filename)
{
    std::string data ={};
    file::file_to_string(filename, data);
    std::stringstream data_stream(data);
    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[20] = {}; // used for the garbage at the start of    each line. 

//     //@Hack: we assume that if a line starts with "new"
//     // that the line declares a new mtl.
//     // the same is true for 'illum'.

    auto mat_vector = std::vector<Material>(1);
    Material& active_material = mat_vector[0];
    fmt::print("FIXME!! we have a spare material at index 0 here.\n");
    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        ++line_number;
        if (line[0] == '#') // comment
        {
            continue;
        }
        //@TODO: substring lookup instead of this?
        else if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w')
        {
            // the current mtl is done. create a new one and rebind the reference.
            //@FIXME FIXME FIXME: this only works if the first one does not start with "new".
            mat_vector.emplace_back();
            active_material = mat_vector.back();
        }
        else if (line[0] == 'K' && line[1] == 'a') // ambient color
        {
            Vec3 color = {};
            sscanf("%9s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
        }
        else if (line[0] == 'K' && line[1] == 'd') // diffuse color
        {
            Vec3 color = {};
            sscanf("%9s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
        }
        else if (line[0] == 'K' && line[1] == 's') // specular color
        {
            Vec3 color = {};
            sscanf("%9s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
        }
        else if (line[0] == 'd') // non_transparency
        {
            float alpha = 0;
            sscanf("%9s %f", garbage_buffer, &alpha);
        }
        else if (line[0] == 'T' && line[1] == 'r') // transparency
        {
            float inv_alpha = 0;
            sscanf("%9s %f", garbage_buffer, &inv_alpha);

            
        }
        else if (line[0] == 'i' && line[1] == 'l') // illumination
        {
            int illum = 0;
            sscanf("%9s %d",garbage_buffer, &illum);
            // if illum == 1: we can skip Ks.
            // if illum == 2: requires ks to be defined.
        }
        else if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p') // map_Ka
        {
            //@Incomplete:
        }
    }
}