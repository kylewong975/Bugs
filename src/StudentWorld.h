#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"

#include "Field.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <map>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Compiler; //so it compiles

//create a struct holding coordinates x, y
struct Coord
{
	Coord(){ } //default constructor
	Coord(int xVal, int yVal) //constructor with specified x,y values
	{
		x = xVal;
		y = yVal;
	}
	int x;
	int y;
};
//operator function declaration
bool operator<(const Coord& a, const Coord& b);

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

	virtual ~StudentWorld();//destructor

	//mandatory functions
	virtual int init(); //initialize the StudentWorld object
	virtual int move(); //calls every tick, updates the interface, move all actors (if alive) and delete actors accordingly if dead
	virtual void cleanUp(); //calls after 2000 ticks, the game is over

	bool hasBlockableObject(int x, int y); //check objects in a given location with special properties, like if it's blockable, can bite, can poison, etc

	void stunAllActors(int x, int y); //for pool of water, stun all the actors that can be stunned in the given position
	void hurtAllActors(int x, int y); //for poison, harm all the actors that are in the given location

	void addFood(int x, int y, int amt); //add amt amount of food to the given location
	int getNumEnergy(int x, int y); //gets the amount of energy stored in food. -1 if not found

	void addActor(Actor* a); //add an actor in general, to the student world
	//note addFood still needed since you still need to check if food exists already, etc

	void selectRandomOpenPosition(int curX, int curY, int radius, int& randX, int& randY); //select a random open position (not blocked by pebbles) in a given circular radius. curX and curY should be changed at the very end if there is an open position

	bool isEnemyAt(int x, int y, int colony); //enemy ant, baby/adult grasshopper

	// Is something dangerous to an ant? any colony
	bool isDangerAt(int x, int y); //pool of water, poison

	// Is the anthill of the indicated colony at x,y?
	bool isAntHillAt(int x, int y, int colony);

	// Bite an enemy of an ant of the indicated colony at me's location
	// (other than me; insects don't bite themselves).  Return true if an
	// enemy was bitten.
	bool biteEnemyAt(Actor* me, int colony, int biteDamage);

	// Record another ant birth for the indicated colony.
	void increaseScore(int colony);

	//Get the colony number that has the most number of ants produced. If there are multiple colonies producing the same number of ants (>=5 count), then do not modify the leader
	void maxAnts(int& curColony);

	// If a pheromone of the indicated colony is at x,y, return a pointer to it; otherwise, return a null pointer.
	Actor* getPheromoneAt(int x, int y, int colony);

private:
	std::map<Coord, std::list<Actor*>> m_actors; //data structure holding Actor pointers
	std::vector<Compiler*> m_compilers; //vector of the compiler pointers to perform actions over for loops rather than copy-pasting

	int antCount[4]; //an array of number of ants produced in each colony
	int currentLead; //the colony number that is currently in the lead
	int currentMax; //the current maximum number of ants. if found new max change currentLead to appropriate colony number

	int m_ticks; //count the number of ticks occurred during the game
};

#endif // STUDENTWORLD_H_
