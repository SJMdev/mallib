#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <cstdint>
#include "camera.h"

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


void game_simulate(const double dt, Game_State& game_state,const Input& input, Particle_Cache& particle_cache);
void game_render(Game_State& game_state, Particle_Cache& particle_cache);


#endif