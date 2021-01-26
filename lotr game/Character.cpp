#include "Character.h"

Character::Character(string _name, string _type, int _attack, int _defense, int _remainingHealth, int _nMaxRounds) {
	this->isAlive = true;
    this->name = _name;
    this->type = _type;
    this->attack = _attack;
    this->defense = _defense;
    this->nMaxRounds = _nMaxRounds;
    this->remainingHealth = _remainingHealth;
    this->nRoundsSinceSpecial = 0;
    this->healthHistory = new int[_nMaxRounds + 1];
    this->healthHistory[0] = _remainingHealth;
}

Character::Character(const Character& character) {
    this->isAlive = character.isAlive;
    this->type = character.type;
    this->name = character.name;
    this->attack = character.attack;
    this->defense = character.defense;
    this->remainingHealth = character.remainingHealth;
    this->nMaxRounds = character.nMaxRounds; 
    this->nRoundsSinceSpecial = character.nRoundsSinceSpecial;
    this->healthHistory = new int[nMaxRounds + 1];
    for(int i=0; i<nMaxRounds+1; i++){
        this->healthHistory[i] = character.healthHistory[i];
    }
}

Character& Character::operator=(const Character& character) {
    if(this == &character){
        return *this;
    }
    // If this object was used before, free the memory it has used.
    if(this->healthHistory != NULL){
        delete[] this->healthHistory;
    }
    this->isAlive = character.isAlive;
    this->type = character.type;
    this->name = character.name;
    this->attack = character.attack;
    this->defense = character.defense;
    this->remainingHealth = character.remainingHealth;
    this->nMaxRounds = character.nMaxRounds;
    this->nRoundsSinceSpecial = character.nRoundsSinceSpecial;
    this->healthHistory = new int[nMaxRounds + 1];
    for(int i=0; i<nMaxRounds+1; i++){
        this->healthHistory[i] = character.healthHistory[i];
    }
    return *this;
}

bool Character::operator<(const Character& other) {
    if(this->name < other.name){
        return true;
    }
    return false;
}

Character::~Character() {
    if(this->healthHistory != NULL){
        delete[] this->healthHistory;
    }
}