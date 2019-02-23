// Wrapper for libComedi Elevator control.
// These functions provide an interface to the elevators in the real time lab
#pragma once

typedef enum {
    ET_Comedi,
    ET_Simulation
} elev_type;

void elev_init(elev_type e);

void elev_set_motor_direction(MotorDir dirn);
void elev_set_button_lamp(ButtonType button, int floor, int value);
void elev_set_floor_indicator(int floor);
void elev_set_door_open_lamp(int value);
void elev_set_stop_lamp(int value);

int elev_get_button_signal(ButtonType button, int floor);
int elev_get_floor_sensor_signal(void);
int elev_get_stop_signal(void);
int elev_get_obstruction_signal(void);



