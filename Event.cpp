#include "Event.h"
#include <iostream>

Event::Event(string _type, int _id_of_hacker, float _time){
	this->type = _type;
	this->id_of_hacker = _id_of_hacker;
	this->time = _time;
	this->nof_line_changes = -1;
}
Event::Event(const Event& event){
	this->type = event.type;
	this->id_of_hacker = event.id_of_hacker;
	this->time = event.time;
	this->nof_line_changes = event.nof_line_changes;
}
Event& Event::operator=(const Event& event){
	this->type = event.type;
	this->id_of_hacker = event.id_of_hacker;
	this->time = event.time;
	this->nof_line_changes = event.nof_line_changes;
	return *this;
}