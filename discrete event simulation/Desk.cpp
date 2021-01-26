#include "Desk.h"

Desk::Desk(float service_time){
	this->service_time = service_time;
	this->empty = true;
}
Desk::Desk(const Desk& desk){
	this->service_time = desk.service_time;
	this->empty = desk.empty;
}
Desk& Desk::operator=(const Desk& desk){
	this->service_time = desk.service_time;
	this->empty = desk.empty;
	return *this;
}