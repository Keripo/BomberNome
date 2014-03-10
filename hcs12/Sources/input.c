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

// Called by main, init hardware
void setup_input() {
	// Input ports
	DDRA = 0x3F;	// PORTA[7:6] is for the input[1:0] from FireFly
	DDRB = 0x3F;	// PORTB[7:6] is for the input[3:2] from FireFly
	DDRM = 0xFF;	// PORTM[2] for game over buzzer sound
	PTM = 0x00;		// Off at beginning
}

// Called by input_interrupt, update player actions
void update_actions() {
	byte input;

	// (~PORTB & 0x80) = 3 - Red
	// (~PORTB & 0x40) = 2 - Green
	// (~PORTA & 0x80) = 1 - Blue
	// (~PORTA & 0x40) = 0 - Orange
	input = ((~PORTA & 0xC0) >> 6) | ((~PORTB & 0xC0) >> 4);
	
	if (game_started == 1) {
		if (player_1->action == ACTION_DYING || player_2->action == ACTION_DYING) {
			PTM = 0xFF;
			return;
		} else if (player_1->action == ACTION_DEAD || player_2->action == ACTION_DEAD) {
			PTM = 0x00;
			show_splash();
			return;
		}
	} else if (game_started > 1) {
			// This is done to give players some prep time
			// i.e. prevent players from immediately bombing and killing themselves 
			game_started--;
  		return;
	}
	switch (input) {
		case 0x00: // 0000
			player_1->action = ACTION_NONE;
			break;
		case 0x01: // 0001
			player_1->action = ACTION_UP;
			break;
		case 0x02: // 0010
			player_1->action = ACTION_DOWN;
			break;
		case 0x03: // 0011
			player_1->action = ACTION_LEFT;
			break;
		case 0x04: // 0100
			player_1->action = ACTION_RIGHT;
			break;
		case 0x05: // 0101
			player_1->action = ACTION_BOMB;
			break;
		case 0x08: // 1000
			player_2->action = ACTION_NONE;
			break;
		case 0x09: // 1001
			player_2->action = ACTION_UP;
			break;
		case 0x0A: // 1010
			player_2->action = ACTION_DOWN;
			break;
		case 0x0B: // 1011
			player_2->action = ACTION_LEFT;
			break;
		case 0x0C: // 1100
			player_2->action = ACTION_RIGHT;
			break;
		case 0x0D: // 1101
			player_2->action = ACTION_BOMB;
			break;
		default:
			break;
	}
	
}

#ifdef TEST_INPUT
// For testing without real input
void simulate_input() {
	static int itest = 0;
	if (itest < 3) {
		player_1->action = ACTION_DOWN;
		player_2->action = ACTION_DOWN;
	} else if (itest < 6) {
		player_1->action = ACTION_NONE;
		player_2->action = ACTION_NONE;
	} else if (itest < 9) {
		player_1->action = ACTION_UP;
		player_2->action = ACTION_UP;
	} else if (itest < 12) {
		player_1->action = ACTION_NONE;
		player_2->action = ACTION_NONE;
	}
	itest++;									
	if (itest == 15) itest = 0;	
}
#endif
