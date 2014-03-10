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
 
 #include "bombernome.h"
 
// Called by main, init game
void setup_game() {

	player_1 = (player_t *)malloc(sizeof(player_t));
	player_2 = (player_t *)malloc(sizeof(player_t));

	player_1->bomb_1 = (bomb_t *)malloc(sizeof(bomb_t));
	player_1->bomb_2 = (bomb_t *)malloc(sizeof(bomb_t));
	player_1->bomb_3 = (bomb_t *)malloc(sizeof(bomb_t));
	
	player_2->bomb_1 = (bomb_t *)malloc(sizeof(bomb_t));
	player_2->bomb_2 = (bomb_t *)malloc(sizeof(bomb_t));
	player_2->bomb_3 = (bomb_t *)malloc(sizeof(bomb_t));
	
	reset_players();
	
}

// Called by setup_game and show_splash()
void reset_players() {
  player_1->action = ACTION_NONE;
	player_1->x = 0;
	player_1->y = 0;
	player_1->bomb_1->counter = 0;
	player_1->bomb_2->counter = 0;
	player_1->bomb_3->counter = 0;
	player_2->action = ACTION_NONE;
	player_2->x = GRID_WIDTH - 1;
	player_2->y = GRID_HEIGHT - 1;
	player_2->bomb_1->counter = 0;
	player_2->bomb_2->counter = 0;
	player_2->bomb_3->counter = 0;
}

// Called by timer_overflow, update game state
void update_game() {

#ifdef TEST_GRID
	// For testing
	static int xtest = 0, ytest = 0;
	int x, y;
	for (y = 0; y < GRID_HEIGHT; y++) {
		for (x = 0; x < GRID_WIDTH; x++) {
			game_grid[x][y] = EMPTY;
		}
	}
	game_grid[xtest][ytest] = PLAYER_1;
	xtest++;
	if (xtest >= GRID_WIDTH) {
		xtest = 0;
		ytest++;
		if (ytest >= GRID_HEIGHT) {
			ytest = 0;
		}
	}
#else

	// Update players
	clear_grid();
	update_player(player_1);
	update_player(player_2);
	
	// Update and draw bombs
	if (player_1->bomb_1->action != BOMB_DEACTIVE) update_bomb(player_1->bomb_1);
	if (player_1->bomb_2->action != BOMB_DEACTIVE) update_bomb(player_1->bomb_2);
	if (player_1->bomb_3->action != BOMB_DEACTIVE) update_bomb(player_1->bomb_3);
	if (player_2->bomb_1->action != BOMB_DEACTIVE) update_bomb(player_2->bomb_1);
	if (player_2->bomb_2->action != BOMB_DEACTIVE) update_bomb(player_2->bomb_2);
	if (player_2->bomb_3->action != BOMB_DEACTIVE) update_bomb(player_2->bomb_3);
	
	// Draw players
	draw_player(player_1, PLAYER_1);
	draw_player(player_2, PLAYER_4);

#endif // TEST_GRID
}

// Called by update_game, clears entire grid to either empty or blocks
void clear_grid() {
	byte x, y;
	for (y = 0; y < GRID_HEIGHT; y++) {
		for (x = 0; x < GRID_WIDTH; x++) {
			if (y % 2 == 1 && x % 2 == 1) { // Odd
				game_grid[x][y] = BLOCK;
			} else {
				game_grid[x][y] = EMPTY;
			}
		}
	}
}

// Called by update_game, updates player's state and location
void update_player(player_t *player) {
	switch(player->action) {
		case ACTION_DYING:
			player->counter--;
			if (player->counter <= 0) {
				player->action = ACTION_DEAD;
			}
			break;
		case ACTION_BOMB:
			if (player->bomb_1->counter <= 0) {
				player->bomb_1->x = player->x;
				player->bomb_1->y = player->y;
				player->bomb_1->action = BOMB_FLASHING_A;
				player->bomb_1->counter = COUNTER_BOMB_SET;
			} else if (player->bomb_2->counter <= 0) {
				player->bomb_2->x = player->x;
				player->bomb_2->y = player->y;
				player->bomb_2->action = BOMB_FLASHING_A;
				player->bomb_2->counter = COUNTER_BOMB_SET;
			} else if (player->bomb_3->counter <= 0) {
				player->bomb_3->x = player->x;
				player->bomb_3->y = player->y;
				player->bomb_3->action = BOMB_FLASHING_A;
				player->bomb_3->counter = COUNTER_BOMB_SET;
			}
			player->action = ACTION_NONE;
			break;
		case ACTION_RIGHT:
			if (player->y > 0 && game_grid[player->x][player->y - 1] != BLOCK) {
				player->y--;
			}
			player->action = ACTION_NONE;
			break;
		case ACTION_LEFT:
			if (player->y < (GRID_HEIGHT - 1) && game_grid[player->x][player->y + 1] != BLOCK) {
				player->y++;
			}
			player->action = ACTION_NONE;
			break;
		case ACTION_DOWN:
			if (player->x > 0 && game_grid[player->x - 1][player->y] != BLOCK) {
				player->x--;
			}
			player->action = ACTION_NONE;
			break;
		case ACTION_UP:
			if (player->x < (GRID_WIDTH - 1) && game_grid[player->x + 1][player->y] != BLOCK) {
				player->x++;
			}
			player->action = ACTION_NONE;
			break;
		case ACTION_NONE:
		default:
			break;
	}
}

// Called by update_game, updates bomb's state and kills players
void update_bomb(bomb_t *bomb) {
	
	// Next state
	bomb->counter--;
	// Disappear
	if (bomb->counter <= 0) {
		bomb->action = BOMB_DEACTIVE;
	  return;
	// Exploding
	} else if (bomb->counter < COUNTER_BOMB_EXPLODE) {
		// Update state
		if ((bomb->counter % (COUNTER_BOMB_EXPLODE_SWITCH * 2)) < COUNTER_BOMB_EXPLODE_SWITCH) {
			bomb->action = BOMB_EXPLODING_B;
		} else {
			bomb->action = BOMB_EXPLODING_A;
		}
		// Check for victims
		bomb->explosion_min_x = (bomb->x - BOMB_RANGE > 0) ?
				bomb->x - BOMB_RANGE : 0;
		bomb->explosion_max_x = (bomb->x + BOMB_RANGE < GRID_WIDTH - 1) ?
				bomb->x + BOMB_RANGE : GRID_WIDTH - 1;
		bomb->explosion_min_y = (bomb->y - BOMB_RANGE > 0) ?
				bomb->y - BOMB_RANGE : 0;
		bomb->explosion_max_y = (bomb->y + BOMB_RANGE < GRID_HEIGHT - 1) ?
				bomb->y + BOMB_RANGE : GRID_HEIGHT - 1;
		check_bomb_victims(player_1, bomb);
		check_bomb_victims(player_2, bomb);
	// Flashing
	} else if (bomb->counter < COUNTER_BOMB_SET) {
		// Update state
		if ((bomb->counter % (COUNTER_BOMB_FLASH_SWITCH * 2)) < COUNTER_BOMB_FLASH_SWITCH) {
			bomb->action = BOMB_FLASHING_B;
		} else {
			bomb->action = BOMB_FLASHING_A;
		}
	}
	
	// Draw bomb
	draw_bomb(bomb);
}

// Called by update_bomb, kill players
void check_bomb_victims(player_t *player, bomb_t *bomb) {
	if (player->action != ACTION_DEAD &&
		player->action != ACTION_DYING) {
		if ((player->x >= bomb->explosion_min_x && player->x <= bomb->explosion_max_x && player->y == bomb->y) ||
			(player->y >= bomb->explosion_min_y &&	player->y <= bomb->explosion_max_y && player->x == bomb->x)) {
			player->counter = COUNTER_PLAYER_DYING;
			player->action = ACTION_DYING;
		}
	}
}

// Called by update_bomb, sets grid objects for bombs
void draw_bomb(bomb_t *bomb) {
	unsigned short i;
	
	if (bomb->action == BOMB_DEACTIVE) {
		return; // Ignore if not set 
	} else if (bomb->action == BOMB_FLASHING_A) {
		game_grid[bomb->x][bomb->y] = BOMB_A;
	} else if (bomb->action == BOMB_FLASHING_B) {
		game_grid[bomb->x][bomb->y] = BOMB_B;
	} else if (bomb->action == BOMB_EXPLODING_A) {
		for (i = bomb->explosion_min_x; i <= bomb->explosion_max_x; i++) {
			game_grid[i][bomb->y] = EXPLOSION_A;
		}
		for (i = bomb->explosion_min_y; i <= bomb->explosion_max_y; i++) {
			game_grid[bomb->x][i] = EXPLOSION_A;
		}
	} else if (bomb->action == BOMB_EXPLODING_B) {
		for (i = bomb->explosion_min_x; i <= bomb->explosion_max_x; i++) {
			game_grid[i][bomb->y] = EXPLOSION_B;
		}
		for (i = bomb->explosion_min_y; i <= bomb->explosion_max_y; i++) {
			game_grid[bomb->x][i] = EXPLOSION_B;
		}
	}
}

// Called by update_game, call this after bombs have been checked and on top of bombs
void draw_player(player_t *player, char icon) {
	if (player->action == ACTION_DYING) {
		game_grid[player->x][player->y] = PLAYER_DYING;
	} else if (player->action != ACTION_DEAD) {
		game_grid[player->x][player->y] = icon;
	}
}

// For debugging, print out the grid
#ifdef TEST_PRINT
void print_game_grid() {
	byte x, y;
	printf("=========================\r\n");
	printf("game_grid at game_counter = %d\r\n", game_counter);
	for (y = 0; y < GRID_HEIGHT; y++) {
		for (x = 0; x < GRID_WIDTH; x++) {
			printf("%c ", game_grid[x][y]);
		}
		printf("\r\n");
	}
	printf("=========================\r\n");
}
#endif
