#include "dataStructures.h"

bool compareButtonPress(ButtonPress a, ButtonPress b){
	return a.button == b.button && a.floor == b.floor;
}