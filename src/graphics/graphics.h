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
#include <map> // for VAO/VBO stuff. will move.


/*
 *  init_graphics()->init_opengl()
 *  setup_shaders()->load_compile_attach_shader();
 *  set_shader()
 *  render_nd_whatever();
 *  actually render objects.
 */


namespace graphics
{
	struct Light
	{
		Vec3 position;
		Vec3 light_color;
		// what kind of light is this?
	};

	enum class Shader_Stage
	{
		VERTEX,
		COMPUTE,
		TESSELLATION_CONTROL,
		TESSELLATION_EVALUATION,
		FRAGMENT
	};

	enum class Shader_Type
	{
		SHADER_TEXT,
		SHADER_DEFAULT,
		SHADER_NORMALS,
		SHADER_GOURAUD,
		SHADER_ISOPHOTES,
		SHADER_CEL
	};

	struct Shaders //@Note: shader_programs?
	{
		uint32_t text;
		uint32_t default;
		uint32_t normals;
		uint32_t gouraud;
		uint32_t isophotes;
		uint32_t cel;
	};

	void init_graphics(); //@Note: does that also mean to load all the shaders etc?
	void init_opengl();

	
	void setup_shaders();
	void render_frame();
	void draw_game_3d();
	void clear_buffers();
	void swap_buffers();
	

	// openGL helpers.
	void get_shader_info(uint32_t shader_program);
	bool link_success(uint32_t shader_program);

	Shaders& shaders();
	
	//@Refactor what does this return? shader_id, program, id?
	// using program_id, shader_id = uint32_t;
	uint32_t load_compile_attach_shader(uint32_t program, std::string file_name);
	uint32_t shader_type_from_extension(const std::string& filename);

	// font/text mode.
	void gl_text_mode();
	void draw_text();
	void reload_shaders(uint32_t& program);
	void set_shader(Shader_Type shader);
	

	//@temporary
	scene::Scene& active_scene();
	void init_texture_settings(std::map<std::string, asset::Texture>& textures);

	//@Incomplete: these draw modes / VBO&VAO management should move to the manager.
	struct Buffers
	{
		uint32_t VAO;
		uint32_t VBO;
		uint32_t IBO;
	};

	std::map<std::string, graphics::Buffers>& buffers();

	// draw modes.
	void render_2d_left_handed_dc();
	void draw_3d_left_handed_perspective();
	// void draw_2d_left_handed_orthographic();
	// void draw_2d_left_handed_normalized();

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

#endif