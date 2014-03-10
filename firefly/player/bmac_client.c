/******************************************************************************
 *  Nano-RK, a real-time operating system for sensor networks.
 *  Copyright (C) 2007, Real-Time and Multimedia Lab, Carnegie Mellon University
 *  All rights reserved.
 *
 *  This is the Open Source Version of Nano-RK included as part of a Dual
 *  Licensing Model. If you are unsure which license to use please refer to:
 *  http://www.nanork.org/nano-RK/wiki/Licensing
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2.0 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/


#include <nrk.h>
#include <include.h>
#include <ulib.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <hal.h>
#include <bmac.h>
#include <nrk_error.h>

#include <nrk_driver_list.h>
#include <nrk_driver.h>
#include <ff_basic_sensor.h>

nrk_task_type TX_TASK;
NRK_STK tx_task_stack[NRK_APP_STACKSIZE];
void tx_task (void);

void nrk_create_taskset ();
void nrk_register_drivers();

uint8_t tx_buf[RF_MAX_PAYLOAD_SIZE];

int main ()
{
	uint16_t div;
	nrk_setup_ports ();
	nrk_setup_uart (UART_BAUDRATE_115K2);

	nrk_init ();

	nrk_led_clr (0);
	nrk_led_clr (1);
	nrk_led_clr (2);
	nrk_led_clr (3);

	nrk_time_set (0, 0);

	bmac_task_config ();

	nrk_register_drivers();
	nrk_create_taskset ();
	nrk_start ();

	return 0;
}

#define THRESHOLD	50
#define L_TRIGGER	990 // Bomb

#define PLAYER_NUM	'A'
#define ACTION_NONE	'-'
#define ACTION_UP	'U'	
#define ACTION_DOWN	'D'
#define ACTION_LEFT	'L'
#define ACTION_RIGHT	'R'
#define ACTION_BOMB	'B'

void tx_task ()
{
	uint8_t j, i, val, len, cnt,fd;
	nrk_sig_t tx_done_signal;
	uint16_t x_val, y_val, light;
	uint16_t x_base, y_base;
	uint16_t x_min, x_max, y_min, y_max;
	char action;
	nrk_sig_mask_t ret;

	// init bmac on channel 25 
	bmac_init (25);

	if(val==NRK_ERROR) nrk_kprintf( PSTR("ERROR setting bmac rate\r\n" ));	

	printf ("tx_task PID=%d\r\n", nrk_get_pid ());

	// Wait until the tx_task starts up bmac
	// This should be called by all tasks using bmac that
	// do not call bmac_init()...
	while (!bmac_started ())
		nrk_wait_until_next_period ();

	// Get and register the tx_done_signal if you want to
	// do non-blocking transmits
	tx_done_signal = bmac_get_tx_done_signal ();
	nrk_signal_register (tx_done_signal);


	nrk_gpio_direction(NRK_BUTTON,NRK_PIN_INPUT); //set button as input
	nrk_gpio_direction(NRK_DEBUG_0,NRK_PIN_OUTPUT);//PA.3

	// Open ADC device as read 
	fd=nrk_open(FIREFLY_SENSOR_BASIC,READ);
	if(fd==NRK_ERROR) nrk_kprintf(PSTR("Failed to open sensor driver\r\n"));
	
	bmac_auto_ack_disable();
	for (cnt = 0; cnt < 20; cnt++) {
		val=nrk_set_status(fd,SENSOR_SELECT,ACC_X);
		val=nrk_read(fd,&x_val,2);
		
		val=nrk_set_status(fd,SENSOR_SELECT,ACC_Y);
		val=nrk_read(fd,&y_val,2);		

		if (cnt == 0) {
			x_base = x_val;
			y_base = y_val;
		} else {
			x_base = (x_base + x_val) / 2;
			y_base = (y_base + y_val) / 2;
		}
		printf("calibrate: x=%d, y=%d\r\n", x_base, y_base);
	}
	x_min = x_base - THRESHOLD;
	x_max = x_base + THRESHOLD;
	y_min = y_base - THRESHOLD;
	y_max = y_base + THRESHOLD;
		
	nrk_led_set (BLUE_LED);	
	while (1) {
		
		// Read new values before waiting
		//val=nrk_set_status(fd,SENSOR_SELECT,LIGHT);
		//val=nrk_read(fd,&light,2);
		
		val=nrk_set_status(fd,SENSOR_SELECT,ACC_X);
		val=nrk_read(fd,&x_val,2);
		
		val=nrk_set_status(fd,SENSOR_SELECT,ACC_Y);
		val=nrk_read(fd,&y_val,2);

		//read button value
		val=nrk_gpio_get(NRK_BUTTON);
		if(val==0){ //button pressed
			nrk_gpio_set(NRK_DEBUG_0);
			nrk_led_set(RED_LED);
			light = L_TRIGGER;
		}
		else {
			nrk_gpio_clr(NRK_DEBUG_0);
			nrk_led_clr(RED_LED);
			light = 0;
		}
		
		// Set output 
		action = PLAYER_NUM;
		if (light >= L_TRIGGER) {
			// Bomb 0bx101
			action = ACTION_BOMB;
		} else {
			if (y_val < y_max && y_val > y_min && x_val < x_max && x_val > x_min) {
				action = ACTION_NONE;
			} else if (y_val > y_max && x_val < x_max && x_val > x_min) {
				action = ACTION_UP;
			} else if (y_val < y_min && x_val < x_max && x_val > x_min) {
				action = ACTION_DOWN;
			} else if (x_val > x_max && y_val < y_max && y_val > y_min) {
				action = ACTION_RIGHT;
			} else if (x_val < x_min && y_val < y_max && y_val > y_min) {
				action = ACTION_LEFT;
			} else {
				action = ACTION_NONE;
			}
		}
		sprintf(tx_buf, "%c %c\r\n", PLAYER_NUM, action);
		printf( "tx_buf=%s",tx_buf );
		

		// Build a sensor packet
		/*
		sprintf (tx_buf,
			"> Sensor packet #%d: ACC_X=%d ACC_Y=%d ACC_Z=%d AUDIO=%d LIGHT=%d TEMP=%d BATT=%d\r\n",
			cnt, adxl_x, adxl_y, adxl_z, mic, light, temp, bat);
		*/		
		//sprintf (tx_buf, "X=%d Y=%d L=%d\r\n", x_val, y_val, light);
		//printf( "tx_buf=%s",tx_buf );


		//cnt++;
		//nrk_led_toggle (BLUE_LED);

		// For blocking transmits, use the following function call.
		// For this there is no need to register  
		//val=bmac_tx_pkt(tx_buf, strlen(tx_buf));

		// This function shows how to transmit packets in a
		// non-blocking manner  
		val = bmac_tx_pkt_nonblocking(tx_buf, strlen (tx_buf));
		//nrk_kprintf (PSTR ("Tx packet enqueued\r\n"));

		// This functions waits on the tx_done_signal
		ret = nrk_event_wait (SIG(tx_done_signal));

		// Just check to be sure signal is okay
		//if(ret & SIG(tx_done_signal) == 0 ) 
		//	nrk_kprintf (PSTR ("TX done signal error\r\n"));

		// Task gets control again after TX complete
		//nrk_kprintf (PSTR ("Tx task sent data!\r\n"));
		nrk_led_toggle (BLUE_LED);
		//nrk_wait_until_next_period ();
	}

}

void nrk_create_taskset ()
{
	TX_TASK.task = tx_task;
	TX_TASK.Ptos = (void *) &tx_task_stack[NRK_APP_STACKSIZE - 1];
	TX_TASK.Pbos = (void *) &tx_task_stack[0];
	TX_TASK.prio = 2;
	TX_TASK.FirstActivation = TRUE;
	TX_TASK.Type = BASIC_TASK;
	TX_TASK.SchType = PREEMPTIVE;
	TX_TASK.period.secs = 0;
	TX_TASK.period.nano_secs = 10* NANOS_PER_MS;
	TX_TASK.cpu_reserve.secs = 0;
	TX_TASK.cpu_reserve.nano_secs = 0;//30* NANOS_PER_MS;
	TX_TASK.offset.secs = 0;
	TX_TASK.offset.nano_secs = 0;
	nrk_activate_task (&TX_TASK);



	printf ("Create done\r\n");
}


void nrk_register_drivers()
{
	int8_t val;

	// Register the Basic FireFly Sensor device driver
	// Make sure to add: 
	//     #define NRK_MAX_DRIVER_CNT  
	//     in nrk_cfg.h
	// Make sure to add: 
	//     SRC += $(ROOT_DIR)/src/drivers/platform/$(PLATFORM_TYPE)/source/ff_basic_sensor.c
	//     in makefile
	val=nrk_register_driver( &dev_manager_ff_sensors,FIREFLY_SENSOR_BASIC);
	if(val==NRK_ERROR) nrk_kprintf( PSTR("Failed to load my ADC driver\r\n") );

}


