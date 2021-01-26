#include <string>
#include <vector>
#include <utility>
using namespace std;

#ifndef HACKER_H
#define HACKER_H

class Hacker{
public:
	int ID;
	float arrival_time;
	int nof_code_commits;
	int valid_commits;
	int gifts;
	float sticker_attemp_time;
	float hoodie_attemp_time;
	int desk_id;
	float turnaround_first;
	
	Hacker(int ID, float arrival_time);
    Hacker(const Hacker& hacker);
    Hacker& operator=(const Hacker& hacker);

};

#endif //HACKER_H