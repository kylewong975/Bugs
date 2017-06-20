#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;
class Compiler;

///////////////////////////////////////////////////////////////////////////
//  Actor Class Declaration
///////////////////////////////////////////////////////////////////////////
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick); //actor constructor
	virtual void doSomething() = 0; //generic do something for each type of actor
	virtual bool blocksOthers() const; //check if this object can block other actor objects (prevent them from moving to this object)
	
	virtual bool isDead() const; //check to see if the Actor is dead or not (like running out of hit points)
	
	void increaseTick(); //increase the number of ticks in the local Actor, to keep track of whether it has moved or not
	int getTicks() const; //get the current tick in student world, but defined locally in actor to check if it HAS MOVED in the current tick
	
	virtual void getStunned(); //get stunned, only occurs for ants and baby grasshoppers
	virtual void getPoisoned(); //get poisoned, lose 150 hp, only occurs for ants and baby grasshoppers
	virtual void getBitten(int amt); // Cause this actor to be be bitten, suffering an amount of damage (specialized for insects).
	
	StudentWorld* getStudentWorld() const; //get the pointer to the student world object

	virtual int getHP() const; //useless for actor, to call the appropriate getHP in energy holder classes
	virtual void changeHP(int amt); //useless for actor, to call the appropriate method in energy holder classes
	//treat changeHP as analogous to getBitten(int amt); the behavior is the same

	virtual bool isFood() const; //check if it is food
	virtual bool isPheromone(int colony) const; //check if it is pheromone
	virtual bool becomesFoodUponDeath() const; //applicable for insects in general

	bool attemptMove(int x, int y, Direction dir); //attempt to move in the given direction dir. If move is successful, return true; otherwise return false
	
	// Is this actor an enemy of an ant of the indicated colony?
	virtual bool isEnemy(int colony) const; //semantically, enemy means enemy ants, baby grasshoppers, and adult grasshoppers

	// Is this actor detected as dangerous by an ant of the indicated colony?
	virtual bool isDangerous() const; //semantically, dangerous means exclusively pool of waters and poison

	// Is this actor the anthill of the indicated colony?
	virtual bool isAntHill() const;

	virtual ~Actor(); //virtual destructor for the base class
private:
	int m_currTick; //current tick, to check if the actor has done something in the current tick or not
	StudentWorld* m_sw; //student world pointer
};

//contains HP
///////////////////////////////////////////////////////////////////////////
//  Energy Holder Class Declaration
///////////////////////////////////////////////////////////////////////////
class EnergyHolder : public Actor
{
public:
	EnergyHolder(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick, int hitpoints); //energy holder constructor
	virtual int getHP() const; //gets the hit points for this baby grasshopper
	virtual void changeHP(int amt); //change the amount of hit points, positive = increase, negative = decrease

	virtual bool isDead() const; //check to see if the energy holder is dead or not (like running out of hit points)

	bool attemptEat(int amt); //attempt to eat specified amt or whatever is left; true = success

	virtual ~EnergyHolder(); //virtual destructor for energy holder
private:
	int m_hp; //number of health
};

//poison, pool of water
///////////////////////////////////////////////////////////////////////////
//  Activating Object Class Declaration
///////////////////////////////////////////////////////////////////////////
class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick); //activating object constructor
	virtual bool isDangerous() const; //pool of water and poison are dangerous to an ant of ANY colony
	virtual ~ActivatingObject(); //virtual destructor for activating objects
};

//ant, baby grasshopper, adult grasshopper
///////////////////////////////////////////////////////////////////////////
//  Insect Class Declaration
///////////////////////////////////////////////////////////////////////////
class Insect : public EnergyHolder
{
public:
	Insect(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick, int hitpoints); //insect constructor
	bool preliminaryCheck(); //the first few steps of doSomething() for insects is really similar: lose 1 hp, check dead, check if stunned/sleeping

	int getState() const; //gets the current state of this baby grasshopper (if it is sleeping/stunned or not)
	void addState(int amt); //adds the number of moves, say if it stuns or for grasshopper purposes

	bool hasBeenAffected() const; //check if the actor has been affected or not
	void setAffected(bool cond); //change the affect state, handle infinite stun

	virtual void getBitten(int amt); //get bitten, losing hp

	virtual bool becomesFoodUponDeath() const; //true since insects can die and drop 100 food
	virtual bool isEnemy(int colony) const; //base class to actually check if actors are enemies; enemy ant and grasshoppers are enemies
	void getXYInFrontOfMe(int& x, int& y) const; // Set x,y to the coordinates of the spot one step in front of this insect.
	virtual ~Insect(); //virtual destructor for insect
private:
	int m_state; //the number of ticks left to sleep
	bool m_affected; //if it has been affected by a pool of water or not
};

///////////////////////////////////////////////////////////////////////////
//  Pebble Class Declaration
///////////////////////////////////////////////////////////////////////////
class Pebble : public Actor
{
public:
	Pebble(StudentWorld* sw, int startX, int startY, int curTick); //pebble constructor
	virtual void doSomething();
	virtual bool blocksOthers() const; //is allowed to block others
	virtual ~Pebble(); //virtual destructor for pebble
};

///////////////////////////////////////////////////////////////////////////
//  Food Class Declaration
///////////////////////////////////////////////////////////////////////////
class Food : public EnergyHolder
{
public:
	Food(StudentWorld* sw, int startX, int startY, int curTick, int hitpoints); //food constructor
	virtual void doSomething();
	virtual bool isFood() const; //it is food
	virtual ~Food(); //virtual destructor for food
};

///////////////////////////////////////////////////////////////////////////
//  Pheromone Class Declaration
///////////////////////////////////////////////////////////////////////////
class Pheromone :public EnergyHolder
{
public:
	Pheromone(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int colony); //pheromone constructor
	virtual void doSomething();
	virtual bool isPheromone(int colony) const; //it is a pheromone, only if it is in this same colony number
	virtual ~Pheromone(); //virtual destructor for pheromone
private:
	int m_colony; //colony number of the pheromone
};

///////////////////////////////////////////////////////////////////////////
//  Pool of Water Class Declaration
///////////////////////////////////////////////////////////////////////////
class PoolOfWater : public ActivatingObject
{
public:
	PoolOfWater(StudentWorld* sw, int startX, int startY, int curTick); //pool of water constructor
	virtual void doSomething();
	virtual ~PoolOfWater(); //virtual destructor for pool of water
};

///////////////////////////////////////////////////////////////////////////
//  Poison Class Declaration
///////////////////////////////////////////////////////////////////////////
class Poison : public ActivatingObject
{
public:
	Poison(StudentWorld* sw, int startX, int startY, int curTick); //poison constructor
	virtual void doSomething();
	virtual ~Poison(); //virtual destructor for poison
};

///////////////////////////////////////////////////////////////////////////
//  Anthill Class Declaration
///////////////////////////////////////////////////////////////////////////
class Anthill :public EnergyHolder
{
public:
	Anthill(StudentWorld* sw, int startX, int startY, int curTick, int colony, Compiler* program); //anthill constructor
	virtual void doSomething();
	int getColonyNum() const; //get the colony number of this anthill
	bool isMyHill(int colony) const; //check if the anthill colony number passed matches that of this colony
	virtual bool isAntHill() const; //it is anthill
	virtual ~Anthill(); //virtual destuctor for anthill
private:
	int m_colony; //the colony number of this anthill
	Compiler* m_program; //pointer to a compiler to give instructions to the ants
};

///////////////////////////////////////////////////////////////////////////
//  Grasshopper Class Declaration
///////////////////////////////////////////////////////////////////////////
class Grasshopper : public Insect
{
public:
	Grasshopper(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int hitpoints); //grasshopper constructor
	virtual void doSomething();

	//doSpecializedGrasshopperMethod() returns true if you return out of doSomething(), false if you want to keep going
	virtual bool doSpecializedGrasshopperMethod(); //do the special methods like morping to adult grasshopper (baby grasshopper) or bite/jump (adult grasshopper)
	
	virtual bool isEnemy(int colony) const; //all grasshoppers are enemies to ants of any colony
	int getDistance() const; //gets the current distance left to move to the given random direction
	void setDistance(int amt); //sets the distance to specified amt
	virtual ~Grasshopper(); //virtual destructor for grasshopper
private:
	int m_dist; //randomly generated distance the grasshopper will move
};

///////////////////////////////////////////////////////////////////////////
//  Baby Grasshopper Class Declaration
///////////////////////////////////////////////////////////////////////////
class BabyGrasshopper : public Grasshopper
{
public:
	BabyGrasshopper(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int hitpoints); //baby grasshopper constructor
	virtual bool doSpecializedGrasshopperMethod(); //do the special methods only defined for this class
	virtual void getStunned(); //gets stunned, sleep two moves
	virtual void getPoisoned(); //get poisoned, lose 150 hp
	virtual ~BabyGrasshopper(); //virtual destructor for baby grasshopper
};

///////////////////////////////////////////////////////////////////////////
//  Adult Grasshopper Class Declaration
///////////////////////////////////////////////////////////////////////////
class AdultGrasshopper : public Grasshopper
{
public:
	AdultGrasshopper(StudentWorld* sw, int startX, int startY, int curTick); //adult grasshopper constructor
	virtual bool doSpecializedGrasshopperMethod(); //do the special methods only defined for this class
	virtual void getBitten(int amt); //get bitten by another enemy, and do the specialized follow up
	virtual ~AdultGrasshopper(); //virtual destructor for adult grasshopper
};

///////////////////////////////////////////////////////////////////////////
//  Ant Class Declaration
///////////////////////////////////////////////////////////////////////////
class Ant : public Insect
{
public:
	Ant(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int colonyNum, Compiler* program); //ant constructor
	virtual void doSomething();
	virtual bool canBeAffected() const; //ants can be susceptible to poison and pool of waters
	virtual bool isEnemy(int colony) const; //enemy only if the ant is not of this colony
	virtual void getStunned(); //gets stunned, sleep two moves
	virtual void getPoisoned(); //gets poisoned, lose 150 hp
	virtual void getBitten(int amt); //get bitten, and remember that it is bitten
	virtual ~Ant(); //virtual destructor for ant
private:
	int m_food; //number of food carried
	int m_colonyNum; //the colony number of the ant
	bool m_bitten; //if the ant is bitten or not
	bool m_blocked; //if the ant is blocked or not
	int m_lastRandNum; //last random number
	int m_counter; //last instruction counter
	Compiler* m_program; //the program submitted by competitor
};

#endif // ACTOR_H_
