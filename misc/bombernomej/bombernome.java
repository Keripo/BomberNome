import java.util.Scanner;

/*
 * Last updated: 2011/11/21
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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BomberNome.	If not, see <http://www.gnu.org/licenses/>.
 */

public class bombernome {
	
// ============================================
// bombernome.h
// ============================================
	
	// Game Grid
	static int GRID_WIDTH		= 7;
	static int GRID_HEIGHT		= 5;
	static int BOMB_RANGE		= 2;
	
	// Game objects
	static char EMPTY			= '-';
	static char PLAYER_1		= '1';
	static char PLAYER_2		= '2';
	static char PLAYER_3		= '3';
	static char PLAYER_4		= '4';
	static char BOMB_A			= '+';
	static char BOMB_B			= '*';
	static char EXPLOSION_A		= '>';
	static char EXPLOSION_B		= '<';
	static char BLOCK			= '#';
	static char PLAYER_DYING	= '@';
	
	// Player actions
	static int ACTION_DEAD		= 0;
	static int ACTION_DYING		= 1;
	static int ACTION_NONE		= 2;
	static int ACTION_BOMB		= 3;
	static int ACTION_UP		= 4;
	static int ACTION_DOWN		= 5;
	static int ACTION_LEFT		= 6;
	static int ACTION_RIGHT		= 7;
	
	// Bomb actions
	static int BOMB_DEACTIVE		= 0;
	static int BOMB_FLASHING_A		= 1;
	static int BOMB_FLASHING_B		= 2;
	static int BOMB_EXPLODING_A		= 3;
	static int BOMB_EXPLODING_B		= 4;
	
	// Counters
	static int COUNTER_BOMB_SET				= 10;//50;
	static int COUNTER_BOMB_FLASH_SWITCH	= 2;//4;
	static int COUNTER_BOMB_EXPLODE			= 3;//20;
	static int COUNTER_BOMB_EXPLODE_SWITCH	= 1;//2;
	static int COUNTER_PLAYER_DYING			= 4;//20;
	
	
	// C's printf
	private static void printf(String format, Object args) {
		System.out.printf(format, args);
	}
	private static void printf(String s) {
		System.out.printf(s);
	}

// ============================================
// bombernome.c
// ============================================
	
	// Private vars
	static int game_counter = 0;
	static char[][] game_grid = new char[GRID_WIDTH][GRID_HEIGHT];
	static player_t player_1 = new player_t();
	static player_t player_2 = new player_t();
	static player_t player_3 = new player_t();
	static player_t player_4 = new player_t(); // player_t *player_1, *player_2, *player_3, *player_4;
	
	// Main logic
	public static void main(String[] args) {
		
		// Terminal
		//TERMIO_Init();				// Initialize terminal IO
		printf("==========================\r\n");
		printf("  Welcome to BomberNomeJ\r\n");
		printf("==========================\r\n");
		
		// Setup everything
		setup();
		
		// Keep looping
		while(true) {// Infinite loop
			timer_overflow_interrupt(); // TODO
		}
	}
	
	// Initialize hardware and game state
	static void setup() {
		
		/*
		// TODO
		// Initialize hardware
		DDRB = 0xFF;				// PortA configured for input
		DDRA = 0xFF;				// PortB configured for input
		TSCR1 = 0x80;				// Enable timer 1 for keypad
		RTICTL = 0x31;				// Set RTI period to 4ms
		CRGINT = 0x80;				// Enable RTI interrupt
		
		// Setup clock
		TSCR2 = 0x80;				// Enable timer 2 for overflow
		EnableInterrupts;			// Enable interrupts
		*/
		
		// Setup players
		setup_player(player_1, 0, 0);
		setup_player(player_2, GRID_WIDTH - 1, 0);
		setup_player(player_3, 0, GRID_HEIGHT - 1);
		setup_player(player_4, GRID_WIDTH - 1, GRID_HEIGHT - 1);
		
	}
	
	static void setup_player(player_t player, int x, int y) {
		//player_1 = (player_t *) malloc(sizeof(player_t));
		player.action = ACTION_NONE;
		player.x = x;
		player.y = y;
		player.bomb_1 = new bomb_t();
		player.bomb_2 = new bomb_t();
		player.bomb_3 = new bomb_t();
		player.bomb_4 = new bomb_t();
	}
	
	// void interrupt 16 TOFISR()
	static void timer_overflow_interrupt() {
		// TODO
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {} // Ignore
		input_interrupt();
		update_game();
	}
	
	// void interrupt 7 RTIISR(){
	static void input_interrupt() {		
		// TODO This is to emulate sending input
		
		String input = " ";
		while (input.length() != 4 && input.length() != 0) {
			printf("Enter actions: ");
			Scanner sc = new Scanner(System.in);
			input = sc.nextLine();
		}
		
		if (input.length() == 0) {
			input = "    "; // Me being lazy
		}
		set_action(player_1, input.charAt(0));
		set_action(player_2, input.charAt(1));
		set_action(player_3, input.charAt(2));
		set_action(player_4, input.charAt(3));
		
	}
	static void set_action(player_t player, char input) {
		if (player.action != ACTION_DYING && player.action != ACTION_DEAD) {
			switch(input) {
				case ' ': player.action = ACTION_NONE; break;
				case 'w': player.action = ACTION_UP; break;
				case 'a': player.action = ACTION_LEFT; break;
				case 's': player.action = ACTION_DOWN; break;
				case 'd': player.action = ACTION_RIGHT; break;
				case 'c': player.action = ACTION_BOMB; break;
			}
		}
	}

	// Update function called every timer overflow
	static void update_game() {
		
		// Increase game counter
		game_counter++;
		
		// Clear grid
		clear_grid();
		
		// Update players
		update_player(player_1);
		update_player(player_2);
		update_player(player_3);
		update_player(player_4);
		
		// Update and draw bombs
		update_bomb(player_1.bomb_1);
		update_bomb(player_1.bomb_2);
		update_bomb(player_1.bomb_3);
		update_bomb(player_1.bomb_4);
		update_bomb(player_2.bomb_1);
		update_bomb(player_2.bomb_2);
		update_bomb(player_2.bomb_3);
		update_bomb(player_2.bomb_4);
		update_bomb(player_3.bomb_1);
		update_bomb(player_3.bomb_2);
		update_bomb(player_3.bomb_3);
		update_bomb(player_3.bomb_4);
		update_bomb(player_4.bomb_1);
		update_bomb(player_4.bomb_2);
		update_bomb(player_4.bomb_3);
		update_bomb(player_4.bomb_4);
		
		// Draw players
		draw_player(player_1, PLAYER_1);
		draw_player(player_2, PLAYER_2);
		draw_player(player_3, PLAYER_3);
		draw_player(player_4, PLAYER_4);
		
		// Display grid
		print_game();
	}
	
	// Called by update_game, clears entire grid to either empty or blocks
	static void clear_grid() {
		int x, y;
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
	static void update_player(player_t player) {
		switch(player.action) {
			case 1: //ACTION_DYING:
				player.counter--;
				if (player.counter <= 0) {
					player.action = ACTION_DEAD;
				}
				break;
			case 3: //ACTION_BOMB:
				if (player.bomb_1.counter == 0) {
					player.bomb_1.x = player.x;
					player.bomb_1.y = player.y;
					player.bomb_1.action = BOMB_FLASHING_A;
					player.bomb_1.counter = COUNTER_BOMB_SET;
				} else if (player.bomb_2.counter == 0) {
					player.bomb_2.x = player.x;
					player.bomb_2.y = player.y;
					player.bomb_2.action = BOMB_FLASHING_A;
					player.bomb_2.counter = COUNTER_BOMB_SET;
				} else if (player.bomb_3.counter == 0) {
					player.bomb_3.x = player.x;
					player.bomb_3.y = player.y;
					player.bomb_3.action = BOMB_FLASHING_A;
					player.bomb_3.counter = COUNTER_BOMB_SET;
				} else if (player.bomb_4.counter == 0) {
					player.bomb_4.x = player.x;
					player.bomb_4.y = player.y;
					player.bomb_4.action = BOMB_FLASHING_A;
					player.bomb_4.counter = COUNTER_BOMB_SET;
				}
				break;
			case 4: //ACTION_UP:
				if (player.y > 0 && game_grid[player.x][player.y - 1] != BLOCK) {
					player.y--;
				}
				break;
			case 5: //ACTION_DOWN:
				if (player.y + 1 < (GRID_HEIGHT - 1) && game_grid[player.x][player.y + 1] != BLOCK) {
					player.y++;
				}
				break;
			case 6: //ACTION_LEFT:
				if (player.x > 0 && game_grid[player.x - 1][player.y] != BLOCK) {
					player.x--;
				}
				break;
			case 7: //ACTION_RIGHT:
				if (player.x + 1 < (GRID_WIDTH - 1) && game_grid[player.x + 1][player.y] != BLOCK) {
					player.x++;
				}
				break;
		}
	}
	
	// Called by update_game, updates bomb's state and kills players
	static void update_bomb(bomb_t bomb) {
		
		// Ignore if not set
		if (bomb.action == BOMB_DEACTIVE) {
			return;
		}
		
		// Next state
		bomb.counter--;
		// Disappear
		if (bomb.counter <= 0) {
			bomb.action = BOMB_DEACTIVE;
		// Exploding
		} else if (bomb.counter < COUNTER_BOMB_EXPLODE) {
			// Update state
			if ((bomb.counter % (COUNTER_BOMB_EXPLODE_SWITCH * 2)) < COUNTER_BOMB_EXPLODE_SWITCH) {
				bomb.action = BOMB_EXPLODING_B;
			} else {
				bomb.action = BOMB_EXPLODING_A;
			}
			// Check for victims
			bomb.explosion_min_x = (bomb.x - BOMB_RANGE > 0) ?
					bomb.x - BOMB_RANGE : 0;
			bomb.explosion_max_x = (bomb.x + BOMB_RANGE < GRID_WIDTH - 1) ?
					bomb.x + BOMB_RANGE : GRID_WIDTH - 1;
			bomb.explosion_min_y = (bomb.y - BOMB_RANGE > 0) ?
					bomb.y - BOMB_RANGE : 0;
			bomb.explosion_max_y = (bomb.y + BOMB_RANGE < GRID_HEIGHT - 1) ?
					bomb.y + BOMB_RANGE : GRID_HEIGHT - 1;
			check_bomb_victims(player_1, bomb);
			check_bomb_victims(player_2, bomb);
			check_bomb_victims(player_3, bomb);
			check_bomb_victims(player_4, bomb);
		// Flashing
		} else if (bomb.counter < COUNTER_BOMB_SET) {
			// Update state
			if ((bomb.counter % (COUNTER_BOMB_FLASH_SWITCH * 2)) < COUNTER_BOMB_FLASH_SWITCH) {
				bomb.action = BOMB_FLASHING_B;
			} else {
				bomb.action = BOMB_FLASHING_A;
			}
		}
		
		// Draw bomb
		draw_bomb(bomb);
	}
	
	// Kill players, called by update_bomb
	static void check_bomb_victims(player_t player, bomb_t bomb) {
		if (player.action != ACTION_DEAD &&
			player.action != ACTION_DYING) {
			if ((player.x > bomb.explosion_min_x && player.x < bomb.explosion_max_x && player.y == bomb.y) ||
				(player.y > bomb.explosion_min_y &&	player.y < bomb.explosion_max_y && player.x == bomb.x)) {
				player.counter = COUNTER_PLAYER_DYING;
				player.action = ACTION_DYING;
			}
		}
	}
	
	// Called by update_bomb, sets grid objects for bombs
	static void draw_bomb(bomb_t bomb) {
		int i;
		
		if (bomb.action == BOMB_DEACTIVE) {
			return; // Ignore if not set 
		} else if (bomb.action == BOMB_FLASHING_A) {
			game_grid[bomb.x][bomb.y] = BOMB_A;
		} else if (bomb.action == BOMB_FLASHING_B) {
			game_grid[bomb.x][bomb.y] = BOMB_B;
		} else if (bomb.action == BOMB_EXPLODING_A) {
			for (i = bomb.explosion_min_x; i <= bomb.explosion_max_x; i++) {
				game_grid[i][bomb.y] = EXPLOSION_A;
			}
			for (i = bomb.explosion_min_y; i <= bomb.explosion_max_y; i++) {
				game_grid[bomb.x][i] = EXPLOSION_A;
			}
		} else if (bomb.action == BOMB_EXPLODING_B) {
			for (i = bomb.explosion_min_x; i <= bomb.explosion_max_x; i++) {
				game_grid[i][bomb.y] = EXPLOSION_B;
			}
			for (i = bomb.explosion_min_y; i <= bomb.explosion_max_y; i++) {
				game_grid[bomb.x][i] = EXPLOSION_B;
			}
		}
	}
	
	// Called by update_game, call this after bombs have been checked and on top of bombs
	static void draw_player(player_t player, char icon) {
		if (player.action == ACTION_DYING) {
			game_grid[player.x][player.y] = PLAYER_DYING;
		} else if (player.action != ACTION_DEAD) {
			game_grid[player.x][player.y] = icon;
		}
	}
	
	// For debugging, print out the grid
	static void print_game() {
		int x, y;
		printf("game_counter = %d\r\n", game_counter);
		for (y = 0; y < GRID_HEIGHT; y++) {
			for (x = 0; x < GRID_WIDTH; x++) {
				printf("%c ", game_grid[x][y]);
			}
			printf("\r\n");
		}
		printf("=========================\r\n");
	}

}
