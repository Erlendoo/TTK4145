#include "config.h"
#include "dataStructures.h"
#include "costFunction.h"
#include "network.h"
#include "main.h"
#include "timer.h"
#include "orders.h"
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

PendingCosts * first;
pthread_mutex_t costMutex = PTHREAD_MUTEX_INITIALIZER;

//Warning: might trigger c++ PTSD
//Here be linked lists. Ye be warned!

void cost_newOrder(ButtonPress b){
	PendingCosts *p = malloc(sizeof(PendingCosts));
	p->timeOut = timer_getTime() + COST_TIMEOUT;
	(p->costs[0]).cost = orders_calculateCost(b);
	for(int i = 1; i < N_ELEVATORS; i++){
		p->costs[i].cost = 255;
	}
	(p->costs[0]).ip = -1;
	p->index = 1;
	p->b = b;
	p->next = first;
	first = p;
}


void cost_handleCost(int cost, ButtonPress b, int ip){
	PendingCosts *p = first;
	while(p != NULL){
		if(compareButtonPress(p->b ,b)){
			pthread_mutex_lock(&costMutex);
			p->costs[p->index].cost = cost;
			p->costs[p->index].ip = ip;
			p->index++;
			pthread_mutex_unlock(&costMutex);
		}
		p = p->next;
	}
}

void clearPendingCosts(ButtonPress b){
	PendingCosts *p = first;
	PendingCosts *prev = NULL;
	while(p != NULL){
		if(compareButtonPress(p->b, b)){
			if(prev == NULL){
				first = NULL;
				free(p);
			}
			else{
				prev->next = p->next;
				free(p);
			}
			break;
		}
		prev = p;
		p = p->next;
	}
}

void *checkCostTimeout(){
	while(true){
		double currentTime = timer_getTime(); //assume time remains constant through running of function, even though it might not :s
		PendingCosts *p = first;
		while(p != NULL){
			if(p->timeOut < currentTime){
				int lowestCost = p->costs[0].cost;
				int returnIp = -1;
				for(int i = 1; i < N_ELEVATORS; i++){
					if(p->costs[i].cost < lowestCost){
						lowestCost = p->costs[i].cost;
						returnIp = p->costs[i].ip;
					}
				}
				if(returnIp != -1){
					network_sendDelegateOrder(p->b, returnIp);
					clearPendingCosts(p->b);
				}
				else{
					main_handleOrder(p->b, LOCAL);
					clearPendingCosts(p->b);
				}
			}
			p = p->next;
		}
		usleep(CHECK_FOR_COST_TIMEOUT_INTERVAL * 1000);
	}
}


void cost_init(){
	first = NULL;
	pthread_t checkCostTimeoutThread;
	pthread_create(&checkCostTimeoutThread, NULL, checkCostTimeout, NULL);
}
