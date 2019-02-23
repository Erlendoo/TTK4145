#pragma once

typedef enum{
	IDLE,
	MOVING,
	DOOR_OPEN
}fsmState;

typedef struct{
	fsmState state;
	int floor;
	MotorDir dir;
}elevator;

void fsm_init(bool skipStartup);