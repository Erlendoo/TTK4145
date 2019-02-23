#include "dataStructures.h"
#include "elev.h"
#include "orders.h"
#include "timer.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "config.h"


static Order orders[N_FLOORS][N_BUTTONS];

bool orders_orderOnFloor(int floor){
	for (int button = 0; button < N_BUTTONS; button++){
		if((orders[floor][button]).state == LOCAL){
			return true;
		}
	}
	return false;
}

bool orders_addOrder(ButtonPress b, OrderState state){ //Returns wether  or not orders was changed
	switch(state){
	case LOCAL:
		switch((orders[b.floor][b.button]).state){
		case NONE:
			(orders[b.floor][b.button]).state = LOCAL;
			(orders[b.floor][b.button]).timeOut = timer_getTime() + ORDER_TIMEOUT;
			return true;
		case LOCAL:
			return false;
		case GLOBAL:
			(orders[b.floor][b.button]).state = LOCAL;
			(orders[b.floor][b.button]).timeOut = timer_getTime() + ORDER_TIMEOUT;
			return true;
		}
	case GLOBAL:
		switch((orders[b.floor][b.button]).state){
		case NONE:
			(orders[b.floor][b.button]).state = GLOBAL;
			(orders[b.floor][b.button]).timeOut = timer_getTime() + ORDER_TIMEOUT;
			return true;
		case LOCAL:
			return false;
		case GLOBAL:
			return false;
		}
	default: return false; //This shouldn't happen
	}
}

void orders_clearOrders(int floor, bool clearCommand){
	for (int button = 0; button < N_BUTTONS - (!clearCommand); button++){
		(orders[floor][button]).state = NONE;
		(orders[floor][button]).timeOut = 0;
	}
}

bool ordersInDirection(MotorDir dir, int floor){
	switch(dir){
	case DIRN_STOP:
		return orders_orderOnFloor(floor); //Not sure what to do about this case
	
	case DIRN_UP:
		for(int i = floor + 1; i < N_FLOORS; i++){
			for(int button = 0; button < N_BUTTONS; button ++){
				if((orders[i][button]).state == LOCAL){
					return true;
				}
			}
		}
		return false;
	
	case DIRN_DOWN:
		for(int i = 0; i < floor; i++){
			for(int button = 0; button < N_BUTTONS; button ++){
				if((orders[i][button]).state == LOCAL){
					return true;
				}
			}
		}
		return false;
	
	default: return false;
	}
}

bool orders_shouldStop(MotorDir dir, int floor){
	if((orders[floor][BUTTON_COMMAND]).state == LOCAL || !ordersInDirection(dir, floor)){
		return true;
	}
	switch(dir){
	case DIRN_STOP:
		return true; //Again not quite sure what to do here
	
	case DIRN_UP:
		return (orders[floor][BUTTON_CALL_UP]).state == LOCAL;
	
	case DIRN_DOWN:
		return (orders[floor][BUTTON_CALL_DOWN]).state == LOCAL;

	default: return true; //Always return true to be safe.
	}
}

MotorDir orders_getDirection(int floor, MotorDir dir){
	switch(dir){
	case DIRN_UP:
		if(ordersInDirection(DIRN_UP, floor)){
			return DIRN_UP;
		}
		if(ordersInDirection(DIRN_DOWN, floor)){
			return DIRN_DOWN;
		}
		return DIRN_STOP;
	
	case DIRN_STOP:
	
	case DIRN_DOWN:
		if(ordersInDirection(DIRN_DOWN, floor)){
			return DIRN_DOWN;
		}
		if(ordersInDirection(DIRN_UP, floor)){
			return DIRN_UP;
		}
		return DIRN_STOP;
	
	default: return DIRN_STOP; //Should never happen but just to be safe
	}
}

void *checkOrderTimeout(){
	while(true){
		double currentTime = timer_getTime(); //assume time remains constant through running of function, even though it might not :s
		for(int floor = 0; floor < N_FLOORS; floor++){
			for(int button = 0; button < N_BUTTONS; button++){
				if(((orders[floor][button]).timeOut != 0) && ((orders[floor][button]).timeOut < currentTime)){
					if((orders[floor][button]).state == LOCAL){
						//something's fucky -_-
						//printf("Local order timed out\n")
					}
					(orders[floor][button]).state = LOCAL;
					//printf("order timed out\n");
				}
			}
		}
		sleep(CHECK_FOR_ORDER_TIMEOUT_INTERVAL);	
	}
}


void orders_init(){
	for(int floor = 0; floor < N_FLOORS; floor++){
		for(int button = 0; button < N_BUTTONS; button++){
			(orders[floor][button]).state = NONE;
			(orders[floor][button]).timeOut = 0;
		}
	}
	pthread_t checkOrderTimeoutThread;
	pthread_create(&checkOrderTimeoutThread, NULL, checkOrderTimeout, NULL);
}

int orders_calculateCost(ButtonPress b){ //could be more optimal, but meh :)
	int cost = 0;
	for(int floor = 0; floor < N_FLOORS; floor++){
		for(int button = 0; button < N_BUTTONS; button++){
			if((orders[floor][button]).state == LOCAL){
				cost += 2;
			}
		}
	}
	for(int button = 0; button < N_BUTTONS; button++){
		if((orders[b.floor][button]).state == LOCAL){
			cost /= 2;
		}
	}
	return cost;
}
