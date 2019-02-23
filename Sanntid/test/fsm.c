#include <unistd.h>
#include <pthread.h>
#include "dataStructures.h"
#include "elev.h"
#include "timer.h"
#include "orders.h"
#include "main.h"
#include "fsm.h"
#define OPEN_DOOR_DURATION 3
#define FSM_EVENT_LISTENER_INTERVAL 50

static int previousFloorIndicator;
static elevator e;

void fsm_timeOut(){
	switch(e.state){
	case DOOR_OPEN:
		elev_set_door_open_lamp(false);
		e.dir = orders_getDirection(e.floor, e.dir);
		if(e.dir == DIRN_STOP){
			e.state = IDLE;
			break;
		}
		e.state = MOVING;
		elev_set_motor_direction(e.dir);
		break;
	
	default:
		break;
	}
}

void fsm_checkForNewOrders(){
	switch(e.state){
	case IDLE:
		e.dir = orders_getDirection(e.floor, e.dir);
		if(e.dir == DIRN_STOP){
			if(orders_orderOnFloor(e.floor)){
				e.dir = DIRN_STOP;
				elev_set_motor_direction(e.dir);
				elev_set_door_open_lamp(true);
				timer_start(OPEN_DOOR_DURATION);
				main_clearOrders(e.floor);
				e.state = DOOR_OPEN;
			}
			break;
		}
		e.state = MOVING;
		elev_set_motor_direction(e.dir);
		break;
	
	default:
		break;
	}
}

void fsm_arrivalAtFloor(int floor){
	e.floor = floor;
	elev_set_floor_indicator(floor);
	switch(e.state){
	case MOVING:
		if(orders_shouldStop(e.dir, e.floor)){
			e.dir = DIRN_STOP;
			elev_set_motor_direction(e.dir);
			elev_set_door_open_lamp(true);
			timer_start(OPEN_DOOR_DURATION);
			main_clearOrders(floor);
			e.state = DOOR_OPEN;
			break;
		}
		break;
	
	default:
		break;
	}
}

void *eventListener(){
	while(true){
		fsm_checkForNewOrders();
		if(elev_get_floor_sensor_signal() != previousFloorIndicator){
			previousFloorIndicator = elev_get_floor_sensor_signal();
			if(previousFloorIndicator != -1){
				fsm_arrivalAtFloor(previousFloorIndicator);
			}
		}
		if(timer_timedOut()){
			fsm_timeOut();
			timer_stop();
		}
		usleep(FSM_EVENT_LISTENER_INTERVAL * 1000);
	}
}

void fsm_init(bool skipStartup){
	if(!skipStartup){ //Startup takes time and we skip it while testing. It is also quite redundant, but a lot of our security relies on redundancy
		if(elev_get_floor_sensor_signal() != N_FLOORS - 1){
			elev_set_motor_direction(DIRN_UP);
			while(elev_get_floor_sensor_signal() != N_FLOORS - 1){
				usleep(100); //Just so we're not constantly polling;
			}
			elev_set_motor_direction(DIRN_STOP);
		}
		elev_set_door_open_lamp(true);
		sleep(3);
		elev_set_door_open_lamp(false);
		for(int i = N_FLOORS - 2; i >= 0; i--){
			elev_set_motor_direction(DIRN_DOWN);
			while(elev_get_floor_sensor_signal() != i){
				usleep(100); //Just so we're not constantly polling;
			}
			elev_set_motor_direction(DIRN_STOP);
			elev_set_door_open_lamp(true);
			sleep(3);
			elev_set_door_open_lamp(false);
		}
	}
	
	previousFloorIndicator = elev_get_floor_sensor_signal();
	
	if(previousFloorIndicator == -1){ //shouldn't happen after having run startup, but we don't always do that so...
		elev_set_motor_direction(DIRN_UP);
		while(elev_get_floor_sensor_signal() ==  -1){
			usleep(100); //Just so we're not constantly polling;
		}
		elev_set_motor_direction(DIRN_STOP);
		previousFloorIndicator = elev_get_floor_sensor_signal();
	}
	
	e.floor = previousFloorIndicator;
	e.state = IDLE;
	e.dir = DIRN_STOP;
	
	pthread_t eventListenerThread;
	pthread_create(&eventListenerThread, NULL, eventListener, NULL);
}
