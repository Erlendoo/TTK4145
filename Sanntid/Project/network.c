#include "config.h"
#include "dataStructures.h"
#include "orders.h"
#include "network.h"
#include "costFunction.h"
#include "sverresnetwork.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

const char *ips[N_ELEVATORS - 1] = {"129.241.187.161","129.241.187.159"}; //Put ip of elevators other than this here NB: remember to update N_ELEVATORS!
const char *myIp = "129.241.187.156"; //Put ip address of THIS elevator here

const char * getIp(int index){
	if(index == -2){
	    return myIp;
	}
	if(index >= 0 && index <= N_ELEVATORS - 2){
		return ips[index];
	}
	return -1;
}

int getIpIndex(const char * ip){
	for(int index = 0; index < N_ELEVATORS - 1; index++){
		if(!strcmp(ip, ips[index])){
			return index;
		}
	}
	if(!strcmp(ip, myIp)){
	    return -2;
	}
	return -1;
}

void udpListener(const char * ip, char * data, int dataLength){
	int keyword = (int)data[0];
	int ipIndex = getIpIndex(ip);
	ButtonPress b;
	
	switch(keyword){
	case CLEAR_FLOOR_KEYWORD:
		if(ipIndex == -1){
			printf("Ip error: CLEAR_FLOOR\n");
			return;
		}
		main_clearOrders(data[1], true);
		break;
	
	case REQUEST_COST_KEYWORD:
		if(ipIndex == -1){
			printf("Ip error: REQUEST_COST\n");
			return;
		}
		b.floor = data[1];
		b.button = data[2];
		main_handleOrder(b, GLOBAL);
		network_sendCost(orders_calculateCost(b), b ,ipIndex);
		break;
	
	case DELEGATE_ORDER_KEYWORD:
		if(ipIndex == -1){
			printf("Ip error: DELEGATE_ORDER\n");
			return;
		}
		b.floor = data[1];
		b.button = data[2];
		main_handleOrder(b, LOCAL);
		break;
	
	case RECEIVE_COST_KEYWORD:
		if(ipIndex == -1){
			printf("Ip error: RECEIVE_COST\n");
			return;
		}
		b.floor = data[2];
		b.button = data[3];
		cost_handleCost(data[1], b, ipIndex);
		break;
	}	
}

void network_sendCost(int cost, ButtonPress b ,int ip){
	char data[4];
	data[0] = RECEIVE_COST_KEYWORD;
	data[1] = cost;
	data[2] = b.floor;
	data[3] = b.button;
	udp_send(getIp(ip), PORT, data, 4);
}

void network_sendDelegateOrder(ButtonPress b, int ip){
	char data[3];
	data[0] = DELEGATE_ORDER_KEYWORD;
	data[1] = b.floor;
	data[2] = b.button;
	udp_send(getIp(ip), PORT, data, 3);
}

void network_sendClearFloor(int floor){
	char data[2];
	data[0] = CLEAR_FLOOR_KEYWORD;
	data[1] = floor;
	udp_broadcast(PORT, data, 2);
}

void network_sendRequestCost(ButtonPress b){
	char data[3];
	data[0] = REQUEST_COST_KEYWORD;
	data[1] = b.floor;
	data[2] = b.button;
	for(int i = 0; i < N_ELEVATORS - 1; i++){
		udp_send(getIp(i), PORT, data, 3);
	}
}

void network_init(){
	udp_startReceiving(PORT, udpListener);
}
