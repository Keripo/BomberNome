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

// Variables
// Game variables
unsigned int game_counter;
player_t *player_1, *player_2;//, *player_3, *player_4;
char game_grid[GRID_WIDTH][GRID_HEIGHT];
byte game_started;

// Main
void main() {

	// Terminal
	TERMIO_Init();
#ifdef TEST_PRINT
	printf("=========================\r\n");
	printf("	Welcome to BomberNome\r\n");
	printf("=========================\r\n");
#endif
	
	// Setup everything
	setup_display();
	setup_input();
	setup_game();
	setup_timer();
	
	// Start things up
	game_counter = 0;
	show_splash();
	EnableInterrupts;
	
	// This infinite loops
	update_display();
}

// Called by main, set up game update timer
void setup_timer() {
	
	// TSCR1 = [TEN][TSWAI][TSFRZ][TFFCA][---][---][---][---]
	// TEN = Timer Enable (want 1 to turn on main timer)
	// TSWAI = Timer Module Stops While in Wait (want 0 since continual timer)
	// TSFRZ = Timer Stops While in Freeze Mode (want 0?)
	// TFFCA = Timer Fast Flag Clear All (want 0?)
	TSCR1 = 0x80;				// Enable main timer
	
	// TSCR2 = [TOI][---][---][---][TCRE][PR2][PR1][PR0]
	// TIO = Timer Overflow Interrupt Enable (want 1 for interrupt)
	// TCRE = Timer Counter Reset Enable (want 0 since not comparing)
	TSCR2 = 0x80;				// Enable overflow interrupt
	
	// MC9S12C128 has a 2MHz bus clock
	// 16-bit = 2^16 = 65536 cycles
	// When PR2 = 0, PR1 = 1, PR0 = 1, timer = bus / 8
	// [1 / (2MHz / 8)] * 65536 cycles = 0.2621s
	// 0b00000111 = 0x03
	// When PR2 = 0, PR1 = 0, PR0 = 0, timer = bus / 1
	// [1 / (2MHz / 1)] * 65536 cycles = 0.03277s
	// 0b00000000 = 0x00
	//TSCR2 |= TIMER_PRESCALER;	// Prescale timer
	
	// TIE = [C7I][C6I][C5I][C4I][C3I][C2I][C1I][C0I]
	// Clear to prevent Input Capture/Output Compare interrupts
	TIE = 0x00;					// Disable input capture/output compare interrupts

}

// Standard Timer Overflow (clock)
void interrupt 16 timer_overflow() {
	// TFLG2 = [TOF][---][---][---][---][---][---][---]
	// TOF = Timer Overflow Flag (set to 1 to clear)
	TFLG2 &= 0x80;			// Clear TOF flag
	
	// Next stage
	game_counter++;
	
#ifdef TEST_INPUT
	if (game_counter % 60 == 0) { // Every ~2s
		simulate_input();	// Move players
	}
#endif	
	
	if (game_counter % 10 == 0) { // Every ~0.33s
		if (game_started > 0) {
			// To prevent random flickery lights
			select_corners();
			// Update game state
			update_game();
			// Update display
			update_led_grid();
			update_monome_led_lists();
		} else if (player_1->action == ACTION_BOMB && player_2->action == ACTION_BOMB) {
			player_1->action = ACTION_NONE;
			player_2->action = ACTION_NONE;
			game_started = 60; // Actually start game ~2s later
		}
#ifdef TEST_PRINT
		print_game_grid();	// Print out game state
		//print_led_grid();	// Print out display state
#endif
	} else {
		update_actions();	// Read actions input
	}
	
	_FEED_COP(); // Needed?
}
