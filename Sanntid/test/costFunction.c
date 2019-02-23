#include "config.h"
#include "dataStructures.h"
#include "costFunction.h"
#include "network.h"
#include "main.h"
#include "timer.h"
#include "orders.h"
#include <unistd.h>
#include <pthread.h>





static PendingCosts pending[N_FLOORS][N_BUTTONS - 1];

void cost_newOrder(ButtonPress b){ //add safety here?
	PendingCosts p;
	p.timeOut = timer_getTime() + COST_TIMEOUT;
	(p.costs[0]).cost = orders_calculateCost(b);
	(p.costs[0]).ip = 0;
	p.index = 1;
	pending[b.floor][b.button] = p;
}

void cost_handleCost(int cost, ButtonPress b, const char * ip){
	if((pending[b.floor][b.button]).timeOut != 0){
		(pending[b.floor][b.button]).costs[(pending[b.floor][b.button]).index].cost = cost;
		(pending[b.floor][b.button]).costs[(pending[b.floor][b.button]).index].ip = ip;
		(pending[b.floor][b.button]).index++;
	}
}

void clearPendingCosts(ButtonPress b){
	pending[b.floor][b.button].timeOut = 0;
	pending[b.floor][b.button].index = 0;
	for(int i = 0; i < N_ELEVATORS; i++){
		pending[b.floor][b.button].costs[i].cost = 0;
		pending[b.floor][b.button].costs[i].ip = 0;
	}
}

void *checkCostTimeout(){
	while(true){
		double currentTime = timer_getTime(); //assume time remains constant through running of function, even though it might not :s
		for(int floor = 0; floor < N_FLOORS; floor++){
			for(int button = 0; button < N_BUTTONS - 1; button++){
				PendingCosts p = (pending[floor][button]);
				if(p.timeOut != 0 && p.timeOut < currentTime){
					int lowestCost = p.costs[0].cost;
					char * ip = 0;
					for(int i = 1; i < N_ELEVATORS; i++){
						if(p.costs[i].cost < lowestCost){
							lowestCost = p.costs[i].cost;
							ip = p.costs[i].ip;
						}
					}
					ButtonPress b;
					b.floor = floor;
					b.button = button;
					if(ip != 0){
						network_sendDelegateOrder(b, ip);
					}
					else{
						main_handleOrder(b, LOCAL);
					}

				}
			}
		}
		sleep(CHECK_FOR_COST_TIMEOUT_INTERVAL);
	}
}

void cost_init(){
	for(int floor = 0; floor < N_FLOORS; floor++){
		for(int button = 0; button < N_BUTTONS - 1; button++){
			ButtonPress b;
			b.floor = floor;
			b.button = button;
			clearPendingCosts(b);
		}
	}
	pthread_t checkCostTimeoutThread;
	pthread_create(&checkCostTimeoutThread, NULL, checkCostTimeout, NULL);
}