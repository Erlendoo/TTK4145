#pragma once
typedef struct{
	int cost;
	int ip;
} CostIp;

typedef struct pending{
	double timeOut;
	CostIp costs[N_ELEVATORS];
	int index;
	ButtonPress b;
	struct pending * next;
} PendingCosts;

void cost_handleCost(int cost, ButtonPress b,int ip);
void cost_newOrder(ButtonPress b);
void cost_init();