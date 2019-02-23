#include "dataStructures.h"
#include "elev.h"
#include "config.h"
#include "buttons.h"
#include "orders.h"
#include "fsm.h"
#include "costFunction.h"
#include "network.h"
#include <stdio.h>

const char *ips[N_ELEVATORS -1] = {"ip1", "ip2"};

void main_shareOrder(ButtonPress b){
	cost_newOrder(b);
	for(int i = 0; i < N_ELEVATORS - 1; i++){
		network_sendRequestCost(b, ips[i]);
	}
}

void main_handleOrder(ButtonPress b, OrderState state){
	if(b.button == BUTTON_COMMAND){
		state = LOCAL;
	}

	elev_set_button_lamp(b.button, b.floor, true);
	if(orders_addOrder(b, state) && state == GLOBAL){
		main_shareOrder(b);
	}
}

void main_clearOrders(int floor){
	orders_clearOrders(floor);
	for (int button = 0; button < N_BUTTONS; button++){
		elev_set_button_lamp(button, floor, false);
	}
}

int main(){
	elev_init();
	fsm_init(true);
	orders_init();
	cost_init();
	network_init();
	buttons_init(); //Order matters here: Make sure buttons is last
	printf("init over\n");
	while (true){
		//aint doin shiet;
		if(elev_get_stop_signal()){
			printf("Stop button pressed, stopping program\n");
			break;
		}
	}
}