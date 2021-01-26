#include "Hacker.h"
#include "Event.h"
#include "Desk.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <queue>
#include <iomanip> 

using namespace std;

// compare method for sticker queue
struct compare_s{
  public:
  bool operator()(const Hacker& a, const Hacker& b) {
     float diff = abs(a.sticker_attemp_time - b.sticker_attemp_time);
     if(diff < 0.00001){
     	return b.ID < a.ID;
     } else {
     	return b.sticker_attemp_time < a.sticker_attemp_time;
     }
   }
};

// compare method for hoodie queue
struct compare_h{
  public:
  bool operator()(const Hacker& a, const Hacker& b) {
    	if(a.valid_commits != b.valid_commits){
    		return b.valid_commits > a.valid_commits;
    	} else {
    		float diff = abs(a.hoodie_attemp_time-b.hoodie_attemp_time);
    		if(diff < 0.00001){
    			return b.ID < a.ID;
    		} else {
    			return a.hoodie_attemp_time > b.hoodie_attemp_time;
    		}
    	}
   }
};

// compare method for events
bool operator<(const Event& event1, const Event& event2) {
	float diff = abs(event1.time - event2.time);
	if(diff < 0.00001){
		return event1.id_of_hacker > event2.id_of_hacker;
	}
    return event2.time < event1.time;
}

// returns the index of first empty desk
int empty_desk(const vector<Desk>& arr){
	for(int i=0; i<arr.size(); i++){
		if(arr[i].empty){
			return i;
		}
	}
	return -1;
}

int main(int argc, char* argv[]) {

	ifstream infile;                            
	infile.open(argv[1]);

	int nof_hackers;
	int nof_code_commits;
	int nof_queue_attemps;
	int nof_sticker_desks;
	int nof_hoodie_desks;
	int invalid_attemps = 0;
	int gifts = 0;
	int more_than_three = 0;
	int total_lines = 0;
	float second = 0;

	infile >> nof_hackers;

	vector<Hacker> hackers;
	priority_queue<Event> des;
	vector<Desk> sticker_desks;
	vector<Desk> hoodie_desks;
	priority_queue<Hacker, vector<Hacker>, compare_s> sticker_queue;
	priority_queue<Hacker, vector<Hacker>, compare_h> hoodie_queue;
	float wait_sticker = 0;
	float wait_hoodie = 0;
	float total_turnaround = 0;
	bool threeGift = false;
	int threeGift_id;
	float threeGift_time;
	vector<float> waiting_times(nof_hackers, 0);
	
	float arrival_time;
	for(int i=1; i<=nof_hackers; i++){
		infile >> arrival_time;
		Hacker h(i, arrival_time);
		hackers.push_back(h);
	}

	infile >> nof_code_commits;

	int id_of_hacker;
	int line_changes;
	float commit_time;
	string type = "code commit";
	for(int i=0; i<nof_code_commits; i++){
		infile >> id_of_hacker >> line_changes >> commit_time;
		Event e(type, id_of_hacker, commit_time);
		e.nof_line_changes = line_changes;
		des.push(e);
	}
	
	infile >> nof_queue_attemps;

	float attemp_time;
	type = "queue attemp";
	for(int i=0; i<nof_queue_attemps; i++){
		infile >> id_of_hacker >> attemp_time;
		Event e(type, id_of_hacker, attemp_time);
		des.push(e);
	}

	infile >> nof_sticker_desks;

	float service_time;
	for(int i=0; i<nof_sticker_desks; i++){
		infile >> service_time;
		Desk d(service_time);
		sticker_desks.push_back(d);
	}

	infile >> nof_hoodie_desks;

	for(int i=0; i<nof_hoodie_desks; i++){
		infile >> service_time;
		Desk d(service_time);
		hoodie_desks.push_back(d);
	}

	int max_length_sticker = 0;
	int max_length_hoodie = 0;

	while(!des.empty()){

		Event current = des.top();
		second = current.time;
		des.pop();

		if(current.type == "code commit") {
			
			int hacker_id = current.id_of_hacker;
			int line_c = current.nof_line_changes;
			hackers[hacker_id-1].nof_code_commits++;
			if(line_c >= 20){
				hackers[hacker_id-1].valid_commits++;
			}
			total_lines += line_c;
		}

		else if(current.type == "queue attemp"){

			int hacker_id = current.id_of_hacker;
			int valid_c = hackers[hacker_id-1].valid_commits;
			
			//invalid attemps
			if(valid_c < 3){
				invalid_attemps++;
			} 
			else if(hackers[hacker_id-1].gifts==3){
				more_than_three++;
			}

			//valid attemps
			if(valid_c >=3 && hackers[hacker_id-1].gifts<3){
				hackers[hacker_id-1].turnaround_first = current.time;
				int desk_idx = empty_desk(sticker_desks);

				// if there is an empty desk
				if(desk_idx != -1){                   
					
					sticker_desks[desk_idx].empty = false;
					hackers[hacker_id-1].desk_id = desk_idx;
					float time_taken = sticker_desks[desk_idx].service_time;
					Event e("sticker exit", hacker_id, current.time + time_taken);
					des.push(e);

				} else {                                               

					hackers[hacker_id-1].sticker_attemp_time = current.time;
					sticker_queue.push(hackers[hacker_id-1]);

					if(sticker_queue.size() > max_length_sticker){
						max_length_sticker = sticker_queue.size();
					}

				}
			}

		}

		else if(current.type == "sticker exit"){
			
			int hacker_id = current.id_of_hacker;
			int desk_idx = hackers[hacker_id-1].desk_id;
			sticker_desks[desk_idx].empty = true;

			int empty_hoodie_desk_idx = empty_desk(hoodie_desks);

			// if there is an empty desk
			if(empty_hoodie_desk_idx != -1){                     

				hoodie_desks[empty_hoodie_desk_idx].empty = false;
				hackers[hacker_id-1].desk_id = empty_hoodie_desk_idx;
				float time_taken = hoodie_desks[empty_hoodie_desk_idx].service_time;
				Event e("hoodie exit", hacker_id, current.time + time_taken);
				des.push(e);

			} else {                                                
				
				hackers[hacker_id-1].hoodie_attemp_time = current.time;
				hoodie_queue.push(hackers[hacker_id-1]);
				if(hoodie_queue.size() > max_length_hoodie){
					max_length_hoodie = hoodie_queue.size();
				}

			}

			if(!sticker_queue.empty()){                             
				
				int empty_sticker_desk_idx = desk_idx;
				int next_hacker = sticker_queue.top().ID;
				wait_sticker += current.time - hackers[next_hacker-1].sticker_attemp_time;
				waiting_times[next_hacker-1] += current.time - hackers[next_hacker-1].sticker_attemp_time;
				sticker_desks[empty_sticker_desk_idx].empty = false;
				hackers[next_hacker-1].desk_id = empty_sticker_desk_idx;
				float time_taken = sticker_desks[empty_sticker_desk_idx].service_time;
				Event e("sticker exit", next_hacker, current.time + time_taken);
				des.push(e);
				sticker_queue.pop();

			}

		}

		else if(current.type == "hoodie exit"){

			int hacker_id = current.id_of_hacker;
			int desk_tobe_empty = hackers[hacker_id-1].desk_id;
			hoodie_desks[desk_tobe_empty].empty = true;
			total_turnaround += current.time - hackers[hacker_id-1].turnaround_first;

			hackers[hacker_id-1].gifts++;
			gifts++;
			
			if(hackers[hacker_id-1].gifts == 3){
				if(!threeGift){
					threeGift = true;
					threeGift_id = hacker_id;
					threeGift_time = waiting_times[hacker_id-1];
				} else {
					if((threeGift_time - waiting_times[hacker_id-1]) > 0.00001){

						threeGift_id = hacker_id;
						threeGift_time = waiting_times[hacker_id-1];

					} else if(abs(threeGift_time - waiting_times[hacker_id-1]) < 0.00001){

						if(hacker_id < threeGift_id){
							threeGift_id = hacker_id;
							threeGift_time = waiting_times[hacker_id-1];
						}

					}
				}
			}

			if(!hoodie_queue.empty()){                                            

				int empty_hoodie_desk_idx = desk_tobe_empty;
				int next_hacker = hoodie_queue.top().ID;
				wait_hoodie += current.time - hackers[next_hacker-1].hoodie_attemp_time;
				waiting_times[next_hacker-1] += current.time - hackers[next_hacker-1].hoodie_attemp_time;
				hoodie_desks[empty_hoodie_desk_idx].empty = false;
				hackers[next_hacker-1].desk_id = empty_hoodie_desk_idx;
				float time_taken = hoodie_desks[empty_hoodie_desk_idx].service_time;
				Event e("hoodie exit", next_hacker, current.time + time_taken);
				des.push(e);
				hoodie_queue.pop();

			} 

		}

	}

	infile.close();


	ofstream outfile;
	outfile.open(argv[2]);

	outfile << max_length_sticker << endl;
	outfile << max_length_hoodie << endl;
	outfile << fixed << setprecision(3) << ((float)gifts/nof_hackers) << endl;
	
	if(gifts != 0){
		outfile << wait_sticker/gifts << endl;
		outfile << wait_hoodie/gifts << endl;
	} else {
		outfile << wait_sticker << endl;
		outfile << wait_hoodie << endl;
	}
	outfile << ((float)nof_code_commits) / nof_hackers << endl;
	outfile << total_lines/((float)nof_code_commits) << endl;

	if(gifts != 0){
		outfile << total_turnaround/gifts << endl;
	} else {
		outfile << total_turnaround << endl;
	}
	outfile << invalid_attemps << endl;
	outfile << more_than_three << endl;

	float max_wait = 0;
	int max_wait_hacker = 0;
	for(int i=0; i<waiting_times.size(); i++) {
		if((waiting_times[i] - max_wait) > 0.00001){
			max_wait = waiting_times[i];
			max_wait_hacker = i;
		}

	}

	outfile << max_wait_hacker+1 << " " << max_wait << endl; 

	if(!threeGift){
		outfile << -1 << " " << "-1.000" << endl;
	} else {
		outfile << threeGift_id << " " << threeGift_time << endl;
	}

	outfile << second;

	outfile.close();

    return 0;
}