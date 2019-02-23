#pragma once

/**** ELEVATOR CONFIGURATION ****/

//Elevator constants
#define N_FLOORS 4
#define N_BUTTONS 3
#define N_ELEVATORS 3

//Network constants, keywords must be int between -128 and 127
#define CLEAR_FLOOR_KEYWORD 100
#define REQUEST_COST_KEYWORD 101
#define RECEIVE_COST_KEYWORD 102
#define DELEGATE_ORDER_KEYWORD 103
#define PORT 2456

//Buttons config
#define BUTTONS_LISTENER_INTERVAL 50 //in ms

//CostFunction config
#define COST_TIMEOUT 0.1 //in s
#define CHECK_FOR_COST_TIMEOUT_INTERVAL 20 //in ms

//Orders config
#define ORDER_TIMEOUT 20 //in s
#define CHECK_FOR_ORDER_TIMEOUT_INTERVAL 2 //in s

//fsm config
#define OPEN_DOOR_DURATION 3 //in s
#define FSM_EVENT_LISTENER_INTERVAL 50 //in ms
