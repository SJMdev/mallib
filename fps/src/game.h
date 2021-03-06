#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <cstdint>
#include "camera.h"
#include "entity_manager.h"

// forward declaration
struct Input;
struct Particle_Cache;


enum Dev_Mode: uint8_t
{
	DM_NONE,
	DM_CAMERA,
};

enum Game_Mode: uint8_t
{
	GM_TITLESCREEN,
	GM_GAME,
	GM_EDITOR,
};


struct Game_State
{
	Dev_Mode dev_mode = DM_NONE;
	Game_Mode game_mode = GM_GAME;

	Camera camera = default_camera();
	bool paused = false;
};


void game_init(Entity_Manager& entity_manager);

void game_simulate(Game_State& game_state, const double dt,  const Input& input, Particle_Cache& particle_cache, Entity_Manager& manager);
void game_render(Game_State& game_state, Particle_Cache& particle_cache);
void game_end_frame(Game_State& game_state, Entity_Manager& entity_manager);



// game_combat.cc
void damage(Entity& target, Entity& inflictor, Entity& attacker,const glm::vec3 dmg_direction_in, int damage);



// game_player_movement.cc 
std::tuple<glm::vec3, glm::vec3> walk_move(
	const Input& input,
    const glm::vec3 old_position,
    const glm::vec3 old_movement_vector,
	const glm::vec3 front,
    const glm::vec3 right,
    const float dt);

std::tuple<glm::vec3, glm::vec3> air_move(
    const Input& input,
    const glm::vec3 old_position,
    const glm::vec3 old_movement_vector,
    const glm::vec3 front,
    const glm::vec3 right,
    const float dt);

#endif