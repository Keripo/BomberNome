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
#include "splash.h"

// Private variables
static int led_grid[LED_WIDTH][LED_HEIGHT];
static int monome_leds_max[4];
static byte *monome_1x, *monome_2x, *monome_3x, *monome_4x;
static byte *monome_1y, *monome_2y, *monome_3y, *monome_4y;

// Called by main, init hardware
void setup_display() {
	
	// Output ports
	DDRA = 0x3F;		// PORTA[6:0] is for Monome #1
	DDRB = 0x3F;		// PORTB[6:0] is for Monome #2
	DDRAD |= 0x3F;	// PORTAD[6:0] is for Monome #3
		
	// Port T needs a few other thing set
	DDRT |= 0x3F;		// PTT[6:0] is for Monome #4
	MODRR = 0x00;		// Don't use Module Routing Register
	PWME = 0x00;		// Don't use PWM
		
	// Malloc lists (not enough static memory?)
	monome_1x = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_2x = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_3x = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_4x = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_1y = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_2y = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_3y = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
	monome_4y = (byte *)malloc((MONOME_PINS * MONOME_PINS) * sizeof(byte));
}

// Called by main and update_actions, displays the splash screen
void show_splash() {
	int x, y;
	game_started = 0;
	reset_players();
	for (y = 0; y < LED_HEIGHT; y++) {
		for (x = 0; x < LED_WIDTH; x++) {
			led_grid[x][y] = 0;
		}
	}
	for (x = 0; x < SPLASH_SIZE; x++) {
		led_grid[splash[x][0]][splash[x][1]] = 1;
	}
	update_monome_led_lists();	
}

// Called by update_game, update monome display
void update_led_grid() {

#ifdef TEST_LED
	// For testing
	int x, y;
	static int xtest = 0, ytest = 0;
	for (y = 0; y < LED_HEIGHT; y++) {
		for (x = 0; x < LED_WIDTH; x++) {
			led_grid[x][y] = 0;
		}
	}
	led_grid[xtest][ytest] = 1;
	xtest++;
	if (xtest >= LED_WIDTH) {
		xtest = 0;
		ytest++;
		if (ytest >= LED_HEIGHT) {
			ytest = 0;
		}
	}
#else
	
	int x, y;
	
	// Loop through entire grid and set led_grid
	// We assume led_grid is at least double the width and height of game_grid
	// Note: everything is offset by one since we want a nice border
	for (y = 0; y < GRID_HEIGHT; y++) {
		for (x = 0; x < GRID_WIDTH; x++) {	
			switch(game_grid[x][y]) {
				case EMPTY:			// --/--
					led_grid[2*x+1][2*y+1] = 0;
					led_grid[2*x+2][2*y+1] = 0;
					led_grid[2*x+1][2*y+2] = 0;
					led_grid[2*x+2][2*y+2] = 0;
					break;
				case PLAYER_1:		// xx/x-
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 0;
					break;
				case PLAYER_2:		// xx/-x
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 0;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case PLAYER_3:		// x-/xx
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 0;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case PLAYER_4:		// -x/xx
					led_grid[2*x+1][2*y+1] = 0;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case PLAYER_DYING:	// xx/xx
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case BOMB_A:		// x-/-x
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 0;
					led_grid[2*x+1][2*y+2] = 0;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case BOMB_B:		// -x/x-
					led_grid[2*x+1][2*y+1] = 0;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 0;
					break;
				case EXPLOSION_A:	// xx/xx
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case EXPLOSION_B:	// xx/xx
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				case BLOCK:			// xx/xx
					led_grid[2*x+1][2*y+1] = 1;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 1;
					led_grid[2*x+2][2*y+2] = 1;
					break;
				default: //error?	// -x/--
					led_grid[2*x+1][2*y+1] = 0;
					led_grid[2*x+2][2*y+1] = 1;
					led_grid[2*x+1][2*y+2] = 0;
					led_grid[2*x+2][2*y+2] = 0;
					break;
			}
		}
	}
	
	// Set borders points
	/*
	led_grid[0][1] = 1;
	led_grid[0][LED_HEIGHT - 1 - 2] = 1;
	led_grid[LED_WIDTH - 1][3] = 1;
	led_grid[LED_WIDTH - 1][LED_HEIGHT - 1 - 4] = 1;
	*/
	// Full border
	for (x = 0; x < LED_WIDTH; x++) {
		led_grid[x][0] = 1;
		led_grid[x][LED_HEIGHT - 1] = 1;
	}
	for (y = 0; y < LED_HEIGHT; y++) {
		led_grid[0][y] = 1;
		led_grid[LED_WIDTH - 1][y] = 1;
	}

#endif // TEST_LEDS
}

// Called by update_game, update LED lists for each monome
void update_monome_led_lists() {
	// Create a list of led sequences for each monome individually
	// We do this because cycling through all the LEDs leads to very dim displays	
	int p;
	byte x, y; // byte so can be used directly for pins
	
	// Monome #1
	p = 0;
	for (y = 0; y < MONOME_PINS; y++) {
		for (x = 0; x < MONOME_PINS; x++) {
			if (led_grid[x][y]) {
				monome_1x[p] = x;
				monome_1y[p] = y;
				p++;
			}
		}
	}
	monome_leds_max[0] = p - 1;
	
	// Monome #2
	p = 0;
	for (y = MONOME_PINS; y < MONOME_PINS * 2; y++) {
		for (x = 0; x < MONOME_PINS; x++) {
			if (led_grid[x][y]) {
				monome_2x[p] = x;
				monome_2y[p] = y - MONOME_PINS;
				p++;
			}
		}
	}
	monome_leds_max[1] = p - 1;
	
	// Monome #3
	p = 0;
	for (y = 0; y < MONOME_PINS; y++) {
		for (x = MONOME_PINS; x < MONOME_PINS * 2; x++) {
			if (led_grid[x][y]) {
				monome_3x[p] = x - MONOME_PINS;
				monome_3y[p] = y;
				p++;
			}
		}
	}
	monome_leds_max[2] = p - 1;
	
	// Monome #4
	p = 0;
	for (y = MONOME_PINS; y < MONOME_PINS * 2; y++) {
		for (x = MONOME_PINS; x < MONOME_PINS * 2; x++) {
			if (led_grid[x][y]) {
				monome_4x[p] = x - MONOME_PINS;
				monome_4y[p] = y - MONOME_PINS;
				p++;
			}
		}
	}
	monome_leds_max[3] = p - 1;
}

// Experimentally determined LED delay
// 1000 is too choppy, 10 is too dim
#define LED_DELAY			100

// Called by main, update decoder pins
void update_display() {
	/*
	Monome boards layout
	Each monome has 8 pins, so we use a 3-to-8 decoder
	(0,0)								(15,0)
			------				------
			| #1 | A[3-5]		| #2 | B[3-5]
			------				------
			A[0-2]				 B[0-2]
	
			------				------
			| #3 | AD[3-5]		| #4 | T[3-5]
			------				------
			AD[0-2]				 T[0-2]
	(0,15)								(15,15)
	*/
	
	int p1, p2, p3, p4;
	int delay;
	
	// For each cycle, update ports accordingly
	for(;;) { // Infinite loop
		p1 = (p1 < monome_leds_max[0]) ? p1 + 1 : 0;
		p2 = (p2 < monome_leds_max[1]) ? p2 + 1 : 0;
		p3 = (p3 < monome_leds_max[2]) ? p3 + 1 : 0;
		p4 = (p4 < monome_leds_max[3]) ? p4 + 1 : 0;
		PORTA = monome_1x[p1] | (monome_1y[p1] << 3);
		PORTB = monome_2x[p2] | (monome_2y[p2] << 3);
		PTAD	= monome_3x[p3] | (monome_3y[p3] << 3);
		PTT	 = monome_4x[p4] | (monome_4y[p4] << 3);
		for (delay = LED_DELAY; delay > 0; delay--);
	}
}

// Called by timer_overflow, set lit LEDs to corners before game update lag
void select_corners() {
  PORTA = 0x00; // 0bxx000000
  PORTB = 0x38; // 0bxx111000
  PTAD = 0x07;  // 0bxx000111
  PTT = 0x3F;   // 0bxx111111
}

// For debugging, print out the grid
#ifdef TEST_PRINT
void print_led_grid() {
	byte x, y;
	printf("=========================\r\n");
	printf("led_grid at game_counter = %d\r\n", game_counter);
	for (y = 0; y < LED_HEIGHT; y++) {
		for (x = 0; x < LED_WIDTH; x++) {
			printf("%d ", led_grid[x][y]);
		}
		printf("\r\n");
	}
	printf("=========================\r\n");
}
#endif
