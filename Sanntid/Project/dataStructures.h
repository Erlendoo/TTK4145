#pragma once
typedef enum { false, true } bool;

typedef enum tag_elev_lamp_type { 
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} ButtonType;

typedef struct{
	int floor;
	ButtonType button;
} ButtonPress;

typedef enum{
	NONE,
	LOCAL,
	GLOBAL
} OrderState;

typedef struct{
	OrderState state;
	double timeOut;
} Order;

typedef enum tag_elev_motor_direction { 
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
} MotorDir;

bool compareButtonPress(ButtonPress a, ButtonPress b);