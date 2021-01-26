#include "Hacker.h"
#include <iostream>
using namespace std;

Hacker::Hacker(int ID, float arrival_time){  
	this->ID = ID;
	this->arrival_time = arrival_time;
	this->nof_code_commits = 0;
	this->gifts = 0;
	this->valid_commits = 0;
	this->sticker_attemp_time = 0;
	this->hoodie_attemp_time = 0;
	this->desk_id = 0;
	this->turnaround_first = 0;
}
Hacker::Hacker(const Hacker& hacker){
	this->ID = hacker.ID;
	this->arrival_time = hacker.arrival_time;
	this->nof_code_commits = hacker.nof_code_commits;
	this->gifts = hacker.gifts;
	this->valid_commits = hacker.valid_commits;
	this->sticker_attemp_time = hacker.sticker_attemp_time;
	this->hoodie_attemp_time = hacker.hoodie_attemp_time;
	this->desk_id = hacker.desk_id;
	this->turnaround_first = hacker.turnaround_first;
}
Hacker& Hacker::operator=(const Hacker& hacker){
	if(this == &hacker){
		return *this;
	}
	this->ID = hacker.ID;
	this->arrival_time = hacker.arrival_time;
	this->nof_code_commits = hacker.nof_code_commits;
	this->gifts = hacker.gifts;
	this->valid_commits = hacker.valid_commits;
	this->sticker_attemp_time = hacker.sticker_attemp_time;
	this->hoodie_attemp_time = hacker.hoodie_attemp_time;
	this->desk_id = hacker.desk_id;
	this->turnaround_first = hacker.turnaround_first;
	return *this;
}
