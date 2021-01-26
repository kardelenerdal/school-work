using namespace std;

#ifndef DESK_H
#define DESK_H

class Desk{
public:
	float service_time;
	bool empty;
	
	Desk(float service_time);
    Desk(const Desk& desk);
    Desk& operator=(const Desk& desk);

};

#endif //DESK_H