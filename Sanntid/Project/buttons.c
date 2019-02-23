#include <pthread.h>
#include "dataStructures.h"
#include "elev.h"
#include "main.h"
#include "buttons.h"
#include <unistd.h>
#include "config.h"
#include <stdio.h>


static bool previousButtonStates[N_FLOORS][N_BUTTONS];

bool readButton(int floor, ButtonType button){ //Only returns true when button goes from unpressed to pressed
	if(elev_get_button_signal(button, floor) != previousButtonStates[floor][button]){  
		previousButtonStates[floor][button] = elev_get_button_signal(button, floor);
		return previousButtonStates[floor][button];
	}
	return false;
}

void *buttonListener(){
	while(true){
		for(int floor = 0; floor < N_FLOORS; floor++){
			for(int button = 0; button < N_BUTTONS; button++){
				if(readButton(floor, button)){
					ButtonPress b;
					b.floor = floor;
					b.button = button;
					main_handleOrder(b, NONE);
				}
			}
		}
		usleep(BUTTONS_LISTENER_INTERVAL * 1000);
	}
}

void buttons_init(){
	for(int floor = 0; floor < N_FLOORS; floor++){
		for(int button = 0; button < N_BUTTONS; button++){
			previousButtonStates[floor][button] = false;
		}
	}
	pthread_t buttonListenerThread;
	pthread_create(&buttonListenerThread, NULL, buttonListener, NULL);
}