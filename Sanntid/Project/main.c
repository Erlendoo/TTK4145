#include "dataStructures.h"
#include "elev.h"
#include "config.h"
#include "buttons.h"
#include "orders.h"
#include "fsm.h"
#include "costFunction.h"
#include "network.h"
#include <stdio.h>

void main_handleOrder(ButtonPress b, OrderState state){
	if(b.button == BUTTON_COMMAND){
		elev_set_button_lamp(b.button, b.floor, true);
		orders_addOrder(b, LOCAL);
		return;
	}
	if(state == NONE){
		elev_set_button_lamp(b.button, b.floor, true);
		if(orders_addOrder(b, GLOBAL)){
			cost_newOrder(b);
			network_sendRequestCost(b);
		}
		return;
	}
	orders_addOrder(b, state);
	elev_set_button_lamp(b.button, b.floor, true);
}

void main_clearOrders(int floor, bool fromNetwork){
	if(!fromNetwork){
		orders_clearOrders(floor, true);
		network_sendClearFloor(floor);
		for (int button = 0; button < N_BUTTONS; button++){
			elev_set_button_lamp(button, floor, false);
		}
	}
	else{
		orders_clearOrders(floor, false);
		for (int button = 0; button < N_BUTTONS - 1; button++){
			elev_set_button_lamp(button, floor, false);
		}
	}
	
}

int main(){
	elev_type e = ET_Comedi;
	elev_init(e);
	fsm_init(false);
	orders_init();
	cost_init();
	network_init();
	buttons_init(); //Order matters here: Make sure buttons is last
	printf("init over\n");
	while (true){
		//noffin
	}
}
