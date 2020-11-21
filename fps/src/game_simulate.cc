#include "game.h"
#include "camera.h"
#include "input.h"
#include "log.h"
#include "entity_system.h"
#include <glm/gtx/string_cast.hpp>
#include <algorithm> // for clamp


// these are the GLFW key presses.
constexpr const int KEY_SPACE = 32;
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;
constexpr const int KEY_P = 80;
constexpr const int KEY_O = 79;

constexpr const float FRAMETIME_120_HZ_IN_S = 0.00833333333f;
constexpr const float FRAMETIME_IN_S = FRAMETIME_120_HZ_IN_S;

namespace
{

	//@FIXME(Sjors): to be replaced by a type_player entity.
	struct Player
	{
		uint32_t ID;
		glm::vec3 position;
		glm::vec3 movement_vector;
	};

	// player entity fields.
   	Player g_player{0, glm::vec3{0.0f,0.0f,3.0f}, glm::vec3{0.0f,0.0f,0.0f}};

    // ---------------
	// cvars
	// ---------------
    float g_mouse_sensitivity = 0.5f;

    float g_camera_velocity = 0.2f;

  
  	float g_player_gravity = 0.667f;
    float g_player_friction = 0.2f;

    // player movement (120 hz baseline tickrate).
    float g_player_max_velocity = 15.0f;

    float g_player_ground_movespeed = 0.23f;
    float g_player_ground_acceleration = 0.46f;
    float g_player_ground_deceleration= 0.33f;

    float g_player_air_acceleration = 0.066f;
    float g_player_air_deceleration = 0.066f;
    float g_player_air_control = 0.01f;
    float g_player_jump_velocity = 0.266f;
    float g_player_side_strafe_acceleration = 1.6667f;
    float g_player_side_strafe_speed = 0.0033f;

    // @dependencies:
    // g_camera_velocity
	glm::vec3 update_flying_camera_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 front,
		const glm::vec3 right,
		const glm::vec3 up,
		const float dt_factor)
	{
		glm::vec3 position = old_position;

		if (input.keyboard_state[KEY_W])
		{
	    	position = position + (front * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_S])
		{
            position = position - (front * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_A])
		{
	   		position = position - (right * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_D])
		{
            position = position + (right * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_SPACE])
		{
			position = position + (up * g_camera_velocity * dt_factor);
		}

		return position;
	}

    //@Dependencies:
    // g_player_movespeed
    // g_player_acceleration
    // world_up
	std::tuple<glm::vec3, glm::vec3> update_position_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 old_movement_vector,
		const glm::vec3 front,
		const glm::vec3 right,
		const float dt_factor)
	{
		bool grounded = (fabs(old_position.y) < 0.0001f);

		glm::vec3 input_ground_movement_vector = glm::vec3(0.0f);
		glm::vec3 front_without_height = glm::vec3(front.x, 0.0f, front.z);

		if (input.keyboard_state[KEY_W])
		{
			input_ground_movement_vector += front_without_height;
		}
		if (input.keyboard_state[KEY_S])
		{
			input_ground_movement_vector -= front_without_height;
		}
		if (input.keyboard_state[KEY_A])
		{
			input_ground_movement_vector -= right;
		}
		if (input.keyboard_state[KEY_D])
		{
			input_ground_movement_vector += right;
		}
		// if (input.keyboard_state[KEY_SPACE])
		// {
		// 	if (grounded)
		// 	{
		// 		input_movement_vector += world_up * g_player_jump_velocity; 
		// 	}
		// }

		glm::vec3 old_xz_movement_vector = glm::vec3(old_movement_vector.x, 0.0f, old_movement_vector.z);
		
		// ground control (to major tom)
    	float input_ground_vector_velocity = g_player_ground_movespeed * dt_factor;
    	input_ground_movement_vector = input_ground_movement_vector * input_ground_vector_velocity;
    	glm::vec3 movement_vector = old_xz_movement_vector + input_ground_movement_vector;


		float player_friction = 0.0f;
		// Do not apply friction if the player is queueing up the next jump
		if(!input.keyboard_state[KEY_SPACE])
		{
			float velocity = glm::length(movement_vector);
			float drop = 0.0f;

			/* Only if the player is on the ground then apply friction */
			if (grounded)
			{
				float control = velocity < g_player_ground_acceleration ? g_player_ground_deceleration : velocity;
				drop = control * g_player_friction * dt_factor;
			}

				float drop_adjusted_velocity = velocity - drop;
				
				player_friction = drop_adjusted_velocity;

				if(drop_adjusted_velocity < 0.0f) drop_adjusted_velocity = 0.0f;
				
				//@IC(Sjors): normally we would just call normalize here.
				if(velocity > 0.0f) drop_adjusted_velocity /= velocity;

				movement_vector = movement_vector * drop_adjusted_velocity;
		}

		glm::vec3 position = old_position + movement_vector;

		return std::make_tuple(position, movement_vector);

	}

	// processes input received from a mouse input system.
	// assumes world up direction is positive y.
	// @dependencies: 
	// g_mouse_sensitivity
	Camera update_camera_view_with_input(const Input& input, const Camera camera, const float dt_factor, const bool should_constrain_pitch = true)
	{
		Camera new_camera = camera;
	    glm::vec3 world_up(0.0f,1.0f, 0.0f);

	    float adjusted_x_offset = input.mouse_delta_x * g_mouse_sensitivity * dt_factor;
	    float adjusted_y_offset = input.mouse_delta_y * g_mouse_sensitivity * dt_factor;

	    new_camera.yaw   += adjusted_x_offset;
	    new_camera.pitch += adjusted_y_offset;

	    // make sure that when pitch is out of bounds, screen doesn't get flipped
	    if (should_constrain_pitch)
	    {
	        if (new_camera.pitch > 89.0f)  new_camera.pitch = 89.0f;
	        if (new_camera.pitch < -89.0f) new_camera.pitch = -89.0f;
	    }

	    // update front, right and up Vectors using the updated euler angles
	    glm::vec3 front;
	    front.x = cos(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
	    front.y = sin(glm::radians(new_camera.pitch));
	    front.z = sin(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
	    new_camera.front = glm::normalize(front);
	    // also re-calculate the right and up vector
	    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	    new_camera.right = glm::normalize(glm::cross(new_camera.front, world_up)); 
	    new_camera.up    = glm::normalize(glm::cross(new_camera.right, new_camera.front));

	    return new_camera;
	}

	void update_player_entity(const Input& input)
	{
		auto& player = g_player;
	}

	Camera update_camera_entity(const Input& input, const Camera old_camera, const float dt)
	{
		Camera camera = old_camera;
		auto [position, movement_vector] = update_position_with_input(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt); 
		camera.position = position;
		camera.movement_vector = movement_vector;


		if (input.mouse_delta_x || input.mouse_delta_x)
			return update_camera_view_with_input(input, camera, dt);

		return camera;
	}
}


// update and render world
void game_simulate(const double dt, Game_State& game_state,const Input& input, Particle_Cache& particle_cache)
{
	//@TODO(Sjors): set bounds for min / max frame time.
	float clamped_dt = dt;
	if (clamped_dt < 0.001f)
	{
			// logr::report("clamped dt.\n");
			clamped_dt = 0.001f;
	}	
	if (clamped_dt > 0.1f) clamped_dt = 0.1f;


	if (input.keyboard_state[KEY_P]) game_state.game_mode = GM_GAME;
	if (input.keyboard_state[KEY_O]) game_state.game_mode = GM_EDITOR;


	float dt_factor = clamped_dt / FRAMETIME_IN_S;



	// are we paused?
	if (game_state.paused)
	{

	}
	else
	{
		// update entities
		// for (entity_type : entity_types)
		// update camera position.
		// if game_mode = player_cam:
		{
			update_player_entity(input);
			game_state.camera = update_camera_entity(input, game_state.camera, dt_factor);
			g_player.position = game_state.camera.position;
		}
	}

	// BEGIN_SIMULATION
	// execute brains (update and render entities());


	// update and render particle systems();


}
