#pragma once

bool orders_orderOnFloor(int floor);
bool orders_addOrder(ButtonPress b, OrderState state);
void orders_clearOrders(int floor);
void orders_init();
bool orders_shouldStop(MotorDir dir, int floor);
MotorDir orders_getDirection();
int orders_calculateCost(ButtonPress b);