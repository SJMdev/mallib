#ifndef INCLUDE_GRAPHICS
#define INCLUDE_GRAPHICS_
#include <string>
#include <vector> // load_obj_to_vertex
#include <stdint.h>
// #include "gl_lite.h"

#ifdef _WIN32
	#include <windows.h>
	// #include <GL/GL.h>
#endif

#include "../asset/asset.h"
#include "../scene/scene.h"
#include "../vec3/vec3.h" // Light
//@todo: service locator pattern for the shaders?

namespace graphics
{
	struct Light
	{
		Vec3 position;
		Vec3 light_color;
		// what kind of light is this?
	};

	enum class Shader_Type
	{
		SHADER_TEXT,
		SHADER_DEFAULT,
		SHADER_NORMALS,
		SHADER_GOURAUD,
		SHADER_BOMB
	};

	struct Shaders //@Note: shader_programs?
	{
		uint32_t bomb;
		uint32_t text;
		uint32_t normals;
		uint32_t gouraud;
		uint32_t default;
	};

	struct Window_Settings
	{
		float width;
		float height;
		bool v_sync;
	};

	void init_graphics(); //@Note: does that also mean to load all the shaders etc?
	void render_frame();
	void draw_game_3d();
	void clear_buffers();
	void swap_buffers();
	void get_shader_info(uint32_t program);

	Shaders& shaders();
	Window_Settings& window_settings();
	
	void set_shader(Shader_Type shader);
	//@Refactor what does this return?
	uint32_t load_compile_attach_shader(uint32_t program, std::string file_name);
	uint32_t shader_type_from_extension(const std::string& filename);

	// font/text mode.
	void gl_text_mode();
	void draw_text();
	void reload_shaders(uint32_t& program);

	//@temporary
	scene::Scene& active_scene();

	void init_texture_settings(std::map<std::string, asset::Texture>& textures);

	// draw modes.
	// void draw_2d_right_handed_normalized()
	// void draw_3d_right_hand_perspective();

	// platform graphics?
	#ifdef _WIN32
	struct Win32_Context
	{
		HGLRC gl_context;
		HDC	device_context;
	};
	Win32_Context& global_Win32_context(); // this is abysmal.
	#endif
	
};


// struct Renderer
// {
//     // list of shaders?
//     // map of shaders?
// };



// class ShaderProgram
// {
//     std::vector<GLuint> shader_IDs;
//     int ID;
// }
// struct Platform_Renderer_Limits
// {
// 	uint32_t max_quads_per_frame;
// };


#endif