#include "config.h"
#include "dataStructures.h"
#include "orders.h"
#include "network.h"
#include "costFunction.h"
#include "sverresnetwork.h"
#include "main.h"


void udpListener(const char * ip, char * data, int dataLength){
	int keyword = (int)data[0];
	if(keyword == CLEAR_FLOOR_KEYWORD){
		main_clearOrders(data[1]);
	}
	if(keyword == REQUEST_COST_KEYWORD){
		ButtonPress b;
		b.floor = data[1];
		b.button = data[2];
		network_sendCost(orders_calculateCost(b), b ,ip);
	}
	if(keyword == DELEGATE_ORDER_KEYWORD){
		ButtonPress b;
		b.floor = data[1];
		b.button = data[2];
		main_handleOrder(b, LOCAL);
	}
	if(keyword == RECIEVE_COST_KEYWORD){
		ButtonPress b;
		b.floor = data[2];
		b.button = data[3];
		cost_handleCost(data[1], b ,ip);
	}
}

void network_sendCost(int cost, ButtonPress b ,char * ip){
	char data[4];
	data[0] = RECIEVE_COST_KEYWORD;
	data[1] = cost;
	data[2] = b.floor;
	data[3] = b.button;
	udp_send(ip, PORT, data, 4);
}
void network_sendDelegateOrder(ButtonPress b, char * ip){
	char data[3];
	data[0] = DELEGATE_ORDER_KEYWORD;
	data[1] = b.floor;
	data[2] = b.button;
	udp_send(ip, PORT, data, 3);
}
void network_sendClearFloor(int floor){
	char data[2];
	data[0] = CLEAR_FLOOR_KEYWORD;
	data[1] = floor;
	udp_broadcast(PORT, data, 2);
}
void network_sendRequestCost(ButtonPress b, char * ip){
	char data[3];
	data[0] = REQUEST_COST_KEYWORD;
	data[1] = b.floor;
	data[2] = b.button;
	udp_send(ip, PORT, data, 3);
}

void network_init(){
	udp_startReceiving(PORT, udpListener);
}// kaka