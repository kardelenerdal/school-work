#include "Character.h"
#include <iostream>
#include <fstream>

using namespace std;

// returns the index of the given Character.
int find(string name, Character* arr[]){
    int i = 0;
    for(i=0; i<5; i++){
        if(arr[i]->name == name){

            if(arr[i]->isAlive){
                return i;
            }

            while(i<=4 && !arr[i]->isAlive){ 
                i++;
            }

            if(i>4){  
                i--;
                while(!arr[i]->isAlive && i>=0){
                    i--;
                }
            }
            return i;
        }
    }
    return 0;
}

// sorts the Character array alphabetically.
void sortAlphabetically(Character** arr){
    bool swapped = false;
    for(int i=0; i<5; i++){
        swapped = false;
        for(int j=0; j<4-i; j++){
            if(!(*arr[j]<*arr[j+1])){
                Character* temp = arr[j];              
                arr[j] = arr[j+1];
                arr[j+1] = temp;
                swapped = true;
            }
        }
        if(!swapped){
            break;
        }
    }
}

int main(int argc, char* argv[]){

    ifstream infile;                            
    infile.open(argv[1]);
    ofstream outfile;
    outfile.open(argv[2]);

    int max_nof_rounds;
    infile >> max_nof_rounds;

    // two pointer arrays to store characters from 2 communities.
    Character* first_community[5];
    Character* second_community[5];

    // two string arrays to store the input order.
    string first_com_names[5];
    string second_com_names[5];

    string name, type;
    int attack_point, defense_point, initial_health;
    for(int i=0; i<5; i++){
        infile >> name >> type >> attack_point >> defense_point >> initial_health;
        first_community[i] = new Character(name,type, attack_point, defense_point, initial_health, max_nof_rounds);
        first_com_names[i] = name;
    }
    for(int i=0; i<5; i++){
        infile >> name >> type >> attack_point >> defense_point >> initial_health;
        second_community[i] = new Character(name,type, attack_point, defense_point, initial_health, max_nof_rounds);
        second_com_names[i] = name;
    }

    sortAlphabetically(first_community);
    sortAlphabetically(second_community);

    // int array to store number of dead characters in 2 communities.
    int nof_dead [2];
    nof_dead[0] = 0;   
    nof_dead[1] = 0;   

    string name_of_last_killed[2];
    name_of_last_killed[0] = "n"; 
    name_of_last_killed[1] = "n"; 
    
    bool double_dwarf = false;
    
    string winner ="";
    
    // turn points the community that attacks.
    // other points the community that defends.
    Character** turn = first_community;
    Character** other = second_community;
    
    int round = 1;

    for(round = 1; round<=max_nof_rounds; round++) {

        if(winner != ""){
            break;
        }
        if(round%2==1){ 
            turn = first_community;
            other = second_community;
        } else { 
            turn = second_community;
            other = first_community;
        }

        string attacker_name, defender_name, special;
        infile >> attacker_name >> defender_name >> special;

        int index_of_attacker = find(attacker_name, turn);
        int index_of_defender = find(defender_name, other);


        Character* attacker = turn[index_of_attacker];
        Character* defender = other[index_of_defender];

        if(special == "SPECIAL") {
            if(attacker->type == "Elves"){
                if(attacker->nRoundsSinceSpecial >= 10){
                    for(int j=0; j<5; j++){
                        if(turn[j]->type == "Hobbit"){
                            turn[j]->remainingHealth += (attacker->remainingHealth)/2;
                            attacker->remainingHealth -= (attacker->remainingHealth)/2;
                            break;
                        }
                    }
                    attacker->nRoundsSinceSpecial = -1;
                }
            } else if(attacker->type =="Dwarfs"){
                if(attacker->nRoundsSinceSpecial >= 20){
                    double_dwarf = true;
                    attacker->nRoundsSinceSpecial = -1;
                }
            } else if(attacker->type == "Wizards"){
                if(attacker->nRoundsSinceSpecial >= 50 && name_of_last_killed[(round+1)%2] != "n" && nof_dead[(round+1)%2] != 0){
                    int index_of_last_killed = 0;
                    for(int k=0; k<5; k++){
                        if(turn[k]->name == name_of_last_killed[(round+1)%2]){
                            index_of_last_killed = k;
                            break;
                        }
                    }

                    if(turn[index_of_last_killed]->isAlive){
                         turn[index_of_last_killed]->remainingHealth = turn[index_of_last_killed]->healthHistory[0];
                    } else {
                        turn[index_of_last_killed]->remainingHealth = turn[index_of_last_killed]->healthHistory[0];
                        turn[index_of_last_killed]->isAlive = true;
                        turn[index_of_last_killed]->nRoundsSinceSpecial = 0;
                        nof_dead[(round+1)%2]--;
                    }
                    attacker->nRoundsSinceSpecial = -1;
                }
            }        
        }
      
        int damage = attacker->attack - defender->defense;
       
        if(double_dwarf){
            damage = damage*2;
            double_dwarf = false;
        }

        if(damage > 0){
            defender->remainingHealth-=damage;

            if(defender->remainingHealth <= 0) {
                // the case when someone died.
                defender->remainingHealth = 0;
                defender->isAlive = false;                
                nof_dead[round%2]++;
                name_of_last_killed[round%2] = defender->name;
                if(defender->type == "Hobbit"){    
                    if(round%2==1){
                        winner = "Community-1";
                    } else {
                        winner = "Community-2";
                    }
                }
                
                if(nof_dead[0]==4){     
                    winner = "Community-2";
                } else if(nof_dead[1]==4){    
                    winner = "Community-1";
                }
            }
        }

         // updates health histories.
        for(int j=0; j<5; j++){
            (*turn[j]).healthHistory[round] = (*turn[j]).remainingHealth;
            if( (*turn[j]).type == "Wizards" || (*turn[j]).type == "Dwarfs" || (*turn[j]).type == "Elves" ){
                (*turn[j]).nRoundsSinceSpecial++;
            }
        }
        for(int j=0; j<5; j++){
            (*other[j]).healthHistory[round] = (*other[j]).remainingHealth;
            if( (*other[j]).type == "Wizards" || (*other[j]).type == "Dwarfs" || (*other[j]).type == "Elves" ){
                (*other[j]).nRoundsSinceSpecial++;
            }
        }

    }

    if(winner.length() == 0){
        winner = "Draw";
    }
    
    // output part.
    outfile << winner << endl;
    outfile << round-1 << endl;
    outfile << nof_dead[0]+nof_dead[1] << endl;
    for(int i=0; i<5; i++){
        string next_name = first_com_names[i];
        outfile << next_name << " ";
        int index = 0;
        for(int j=0; j<5; j++){
            if(first_community[j]->name == next_name){
                index = j;
                break;
            }
        }
        for(int j=0; j<round; j++){
            outfile << first_community[index]->healthHistory[j]<< " ";
        }
        outfile << endl;
    }
    for(int i=0; i<5; i++){
        string next_name = second_com_names[i];
        outfile << next_name << " ";
        int index = 0;
        for(int j=0; j<5; j++){
            if(second_community[j]->name == next_name){
                index = j;
                break;
            }
        }
        for(int j=0; j<round; j++){
            outfile << second_community[index]->healthHistory[j]<< " ";
        }
        
        outfile << endl;
        
    }

    infile.close();
    outfile.close();

    return 0;

}
