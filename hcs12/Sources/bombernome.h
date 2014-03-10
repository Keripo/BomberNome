/*
 * Last updated: 2011/12/13
 * ~Philip Peng
 * 
 * This file is part of BomberNome.
 * 
 * BomberNome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BomberNome is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BomberNome.	If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BOMBERNOME_H
#define BOMBERNOME_H
 
#include <hidef.h>			// Common defines and macros
#include <MC9S12C128.h>		// Derivative information	
#include <stdio.h>			// Terminal stuff
#include <stdlib.h>			// Terminal stuff
#include <termio.h>			// Terminal stuff
#include <terminal.h>		// Terminal stuff

// Uncomment these to test stuff
//#define TEST_PRINT			1
//#define TEST_GRID				1
//#define TEST_LED				1
//#define TEST_INPUT			1

// Game Grid
#define GRID_WIDTH			7
#define GRID_HEIGHT			7
#define MONOME_PINS			8
#define LED_WIDTH			(MONOME_PINS * 2)
#define LED_HEIGHT			(MONOME_PINS * 2)
	
// Game objects
#define EMPTY				'-'
#define PLAYER_1			'1'
#define PLAYER_2			'2'
#define PLAYER_3			'3'
#define PLAYER_4			'4'
#define PLAYER_DYING		'@'
#define BOMB_A				'+'
#define BOMB_B				'*'
#define EXPLOSION_A			'>'
#define EXPLOSION_B			'<'
#define BLOCK				'#'
	
// Player actions
#define ACTION_DEAD			0
#define ACTION_DYING		1
#define ACTION_NONE			2
#define ACTION_BOMB			3
#define ACTION_UP			4
#define ACTION_DOWN			5
#define ACTION_LEFT			6
#define ACTION_RIGHT  7
	
// Bomb actions
#define BOMB_RANGE			2
#define BOMB_DEACTIVE		0
#define BOMB_FLASHING_A		1
#define BOMB_FLASHING_B		2
#define BOMB_EXPLODING_A	3
#define BOMB_EXPLODING_B	4

// Counters
#define COUNTER_BOMB_SET			10
#define COUNTER_BOMB_FLASH_SWITCH	1
#define COUNTER_BOMB_EXPLODE		3
#define COUNTER_BOMB_EXPLODE_SWITCH	1
#define COUNTER_PLAYER_DYING		12

// Bomb struct
typedef struct {
	short x, y;
	unsigned short action;
	unsigned short counter;
	short explosion_min_x, explosion_max_x;
	short explosion_min_y, explosion_max_y;
} bomb_t;

// Player struct
typedef struct {
	short x, y;
	unsigned short action;
	unsigned short counter;
	bomb_t *bomb_1, *bomb_2, *bomb_3;//, *bomb_4;
} player_t;

// Main
void setup_timer(void);

// Display
void setup_display(void);
void show_splash(void);
void update_led_grid(void);
void update_monome_led_lists(void);
void update_display(void);
void select_corners(void);
#ifdef TEST_PRINT
void print_led_grid(void);
#endif

// Input
void setup_input(void);
void update_actions(void);
#ifdef TEST_INPUT
void simulate_input(void);
#endif

// Game
void setup_game(void);
void reset_players(void);
void update_game(void);
void clear_grid(void);
void update_player(player_t *player);
void update_bomb(bomb_t *bomb);
void check_bomb_victims(player_t *player, bomb_t *bomb);
void draw_bomb(bomb_t *bomb);
void draw_player(player_t *player, char icon);
#ifdef TEST_PRINT
void print_game_grid(void);
#endif

// Global variables
extern unsigned int game_counter;
extern player_t *player_1, *player_2;//, *player_3, *player_4;
extern char game_grid[GRID_WIDTH][GRID_HEIGHT];
extern byte game_started;

#endif
