#include <string>
#include <vector>
#include <utility>
#include "Hacker.h"
using namespace std;

#ifndef EVENT_H
#define EVENT_H

class Event{
public:
	string type;
	int id_of_hacker;
	float time;
	int nof_line_changes;

	Event(string _type, int _id_of_hacker, float _time);
    Event(const Event& event);
    Event& operator=(const Event& event);

};

#endif //EVENT_H