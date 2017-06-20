#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"

#include "Compiler.h"
#include "GameConstants.h"

#include <iostream> //cerr purposes

using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////////////////////////////////////////////////////////
//  Actor Class Implementation
///////////////////////////////////////////////////////////////////////////
Actor::Actor(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick) :GraphObject(imgID, startX, startY, dir, imgDepth)
{
	m_currTick = curTick + 1; //notice that in init() m_ticks is 0, but in move() you increment the tick first
	m_sw = sw;
}

bool Actor::isDead() const
{
	return false;
}

bool Actor::blocksOthers() const
{
	return false;
}

void Actor::increaseTick()
{
	m_currTick++;
}

int Actor::getTicks() const
{
	return m_currTick;
}

void Actor::getStunned() { }

void Actor::getPoisoned() { }

void Actor::getBitten(int amt) { }

StudentWorld* Actor::getStudentWorld() const
{
	return m_sw;
}

int Actor::getHP() const
{
	return 0;
}

void Actor::changeHP(int amt) { }

bool Actor::isFood() const
{
	return false;
}

bool Actor::isPheromone(int colony) const
{
	return false;
}

bool Actor::isEnemy(int colony) const
{
	return false;
}

bool Actor::isDangerous() const
{
	return false;
}

bool Actor::isAntHill() const
{
	return false;
}

bool Actor::becomesFoodUponDeath() const
{
	return false;
}

bool Actor::attemptMove(int x, int y, Direction dir)
{
	if (dir == GraphObject::Direction::up && y + 1 < VIEW_HEIGHT) //north
	{
		if (m_sw->hasBlockableObject(x, y + 1))
			return false;
		else
			moveTo(x, y + 1);
	}
	else if (dir == GraphObject::Direction::right && x + 1 < VIEW_WIDTH)
	{
		if (m_sw->hasBlockableObject(x + 1, y)) //east
			return false;
		else
			moveTo(x + 1, y);
	}
	else if (dir == GraphObject::Direction::down && y - 1 >= 0) //south
	{
		if (m_sw->hasBlockableObject(x, y - 1))
			return false;
		else
			moveTo(x, y - 1);
	}
	else if (dir == GraphObject::Direction::left && x - 1 >= 0) //west
	{
		if (m_sw->hasBlockableObject(x - 1, y))
			return false;
		else
			moveTo(x - 1, y);
	}
	return true;
}

Actor::~Actor() { }

///////////////////////////////////////////////////////////////////////////
//  Energy Holder Class Implementation
///////////////////////////////////////////////////////////////////////////
EnergyHolder::EnergyHolder(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick, int hitpoints) :Actor(sw, imgID, startX, startY, dir, imgDepth, curTick)
{
	m_hp = hitpoints;
}

int EnergyHolder::getHP() const
{
	return m_hp;
}

void EnergyHolder::changeHP(int amt)
{
	m_hp += amt;
}

bool EnergyHolder::isDead() const
{
	if (becomesFoodUponDeath() && m_hp <= 0)
	{
		getStudentWorld()->addFood(getX(), getY(), 100);
	}
	return m_hp <= 0;
}

bool EnergyHolder::attemptEat(int amt)
{
	int foodLeft = getStudentWorld()->getNumEnergy(getX(), getY());
	if (foodLeft != -1) //it has food
	{
		int eatAmt = foodLeft < amt ? foodLeft : amt; //get the minimum of how much food is left and 200, to ensure the proper amount of food is eaten
		getStudentWorld()->addFood(getX(), getY(), -eatAmt);
		changeHP(eatAmt);
		return true;
	}
	return false;
}

EnergyHolder::~EnergyHolder() {}

///////////////////////////////////////////////////////////////////////////
//  Activating Object Class Implementation
///////////////////////////////////////////////////////////////////////////
ActivatingObject::ActivatingObject(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick):Actor(sw, imgID, startX, startY, dir, imgDepth, curTick)
{

}

bool ActivatingObject::isDangerous() const
{
	return true;
}

ActivatingObject::~ActivatingObject()
{

}

///////////////////////////////////////////////////////////////////////////
//  Insect Class Declaration
///////////////////////////////////////////////////////////////////////////
Insect::Insect(StudentWorld* sw, int imgID, int startX, int startY, Direction dir, int imgDepth, int curTick, int hitpoints) :EnergyHolder(sw, imgID, startX, startY, dir, imgDepth, curTick, hitpoints)
{
	m_affected = false;
	//m_bitten = false;
	m_state = 0;
}

bool Insect::preliminaryCheck()
{
	changeHP(-1); //loses 1 hit point as it gets hungrier
	increaseTick();

	if (getHP() <= 0)
	{
		//drop 100 food initiated at the end of the tick, in isDead() true condition
		//setDead();
		return true;
	}
	if (getState() != 0) //that is, it is sleeping or stunned
	{
		addState(-1);
		return true;
	}
	return false;
}


int Insect::getState() const
{
	return m_state;
}

void Insect::addState(int amt)
{
	m_state += amt;
}

void Insect::setAffected(bool cond)
{
	m_affected = cond;
}

bool Insect::hasBeenAffected() const
{
	return m_affected;
}

void Insect::getBitten(int amt)
{
	changeHP(-amt);
}

bool Insect::becomesFoodUponDeath() const
{
	return true;
}

bool Insect::isEnemy(int colony) const
{
	return false;
}

void Insect::getXYInFrontOfMe(int& x, int& y) const
{
	if (getDirection() == GraphObject::Direction::up && y + 1 < VIEW_HEIGHT)
		y++;
	else if (getDirection() == GraphObject::Direction::right && x + 1 < VIEW_WIDTH)
		x++;
	else if (getDirection() == GraphObject::Direction::down && y - 1 >= 0)
		y--;
	else if (getDirection() == GraphObject::Direction::left && x - 1 >= 0)
		x--;
}

Insect::~Insect() { }

///////////////////////////////////////////////////////////////////////////
//  Pebble Class Implementation
///////////////////////////////////////////////////////////////////////////
Pebble::Pebble(StudentWorld* sw, int startX, int startY, int curTick) :Actor(sw, IID_ROCK, startX, startY, right, 1, curTick)
{

}

bool Pebble::blocksOthers() const
{
	return true;
}

void Pebble::doSomething()
{
	//technically does nothing, just increase the number of ticks to be in sync with student world.
	increaseTick();
}

Pebble::~Pebble() { }

///////////////////////////////////////////////////////////////////////////
//  Food Class Declaration
///////////////////////////////////////////////////////////////////////////
Food::Food(StudentWorld* sw, int startX, int startY, int curTick, int hitpoints) :EnergyHolder(sw, IID_FOOD, startX, startY, right, 2, curTick, hitpoints)
{

}

void Food::doSomething()
{
	//technically does nothing, just increase the number of ticks to be in sync with student world.
	increaseTick();
}

bool Food::isFood() const
{
	return true;
}

Food::~Food() {}

///////////////////////////////////////////////////////////////////////////
//  Pheromone Class Declaration
///////////////////////////////////////////////////////////////////////////
Pheromone::Pheromone(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int colony) :EnergyHolder(sw, imgID, startX, startY, right, 2, curTick, 256)
{
	m_colony = colony;
}

void Pheromone::doSomething()
{
	changeHP(-1);
	increaseTick();
}

bool Pheromone::isPheromone(int colony) const
{
	return m_colony == colony;
}

Pheromone::~Pheromone() { }

///////////////////////////////////////////////////////////////////////////
//  Pool of Water Class Declaration
///////////////////////////////////////////////////////////////////////////
PoolOfWater::PoolOfWater(StudentWorld* sw, int startX, int startY, int curTick):ActivatingObject(sw, IID_WATER_POOL, startX, startY, right, 2, curTick)
{

}

void PoolOfWater::doSomething()
{
	getStudentWorld()->stunAllActors(getX(), getY());
	increaseTick();
}

PoolOfWater::~PoolOfWater() { }

///////////////////////////////////////////////////////////////////////////
//  Poison Class Declaration
///////////////////////////////////////////////////////////////////////////
Poison::Poison(StudentWorld* sw, int startX, int startY, int curTick):ActivatingObject(sw, IID_POISON, startX, startY, right, 2, curTick)
{

}

void Poison::doSomething()
{
	increaseTick();
	getStudentWorld()->hurtAllActors(getX(), getY());
}

Poison::~Poison()
{

}

///////////////////////////////////////////////////////////////////////////
//  Anthill Class Implementation
///////////////////////////////////////////////////////////////////////////
Anthill::Anthill(StudentWorld* sw, int startX, int startY, int curTick, int colony, Compiler* program) :EnergyHolder(sw, IID_ANT_HILL, startX, startY, right, 2, curTick, 8999)
{
	m_colony = colony;
	m_program = program;
}

void Anthill::doSomething()
{
	changeHP(-1);
	increaseTick();
	if (getHP() <= 0)
		return;

	if (attemptEat(10000)) //check to see if there exists food on the square
		return; //if so, eat and then return

	if (getHP() >= 2000)
	{
		int antImageID = 0;
		switch (m_colony)
		{
			case 0: antImageID = IID_ANT_TYPE0; break;
			case 1: antImageID = IID_ANT_TYPE1; break;
			case 2: antImageID = IID_ANT_TYPE2; break;
			case 3: antImageID = IID_ANT_TYPE3; break;
			default: antImageID = 0; //will not go to this case, since specified colony number MUST be 0 to 3 inclusive
		}
		getStudentWorld()->addActor(new Ant(getStudentWorld(), antImageID, getX(), getY(), getTicks() - 1, m_colony, m_program));
		changeHP(-1500);
		getStudentWorld()->increaseScore(m_colony);
	}
}

int Anthill::getColonyNum() const
{
	return m_colony;
}

bool Anthill::isMyHill(int colony) const
{
	return m_colony == colony;
}

bool Anthill::isAntHill() const
{
	return true;
}

Anthill::~Anthill() { }

///////////////////////////////////////////////////////////////////////////
//  Grasshopper Class Implementation
///////////////////////////////////////////////////////////////////////////
Grasshopper::Grasshopper(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int hitpoints) :Insect(sw, imgID, startX, startY, static_cast<Direction>(randInt(1, 4)), 1, curTick, hitpoints)
{
	m_dist = randInt(2, 10);
}

void Grasshopper::doSomething()
{
	if (preliminaryCheck())
		return;

	if (doSpecializedGrasshopperMethod())
		return;

	bool willSleep = false;
	//50 % chance it will want to immediately rest, in which case it proceeds to step 12 (set state to 2 again).
	if (attemptEat(200)) //attempts to eat any food on the current square
	{
		int randNum = randInt(1, 2);
		if (randNum == 1)
			willSleep = true;
	}

	if (!willSleep)
	{
		//check if baby grasshopper has finished walking the desired distance in the current direction
		if (getDistance() == 0)
		{
			//pick random direction
			setDirection(static_cast<Direction>(randInt(1, 4)));
			//pick random distance
			setDistance(randInt(2, 10));
		}

		//attempt to move
		if (attemptMove(getX(), getY(), getDirection())) //if it can move, decrease desired distance to walk by 1
			setDistance(getDistance() - 1);
		else //otherwise set to 0
			setDistance(0);
	}

	addState(2); //now asleep
}

bool Grasshopper::doSpecializedGrasshopperMethod()
{
	return false;
}

bool Grasshopper::isEnemy(int colony) const
{
	return true;
}

int Grasshopper::getDistance() const
{
	return m_dist;
}

void Grasshopper::setDistance(int amt)
{
	m_dist = amt;
}

Grasshopper::~Grasshopper()
{

}

///////////////////////////////////////////////////////////////////////////
//  Baby Grasshopper Class Implementation
///////////////////////////////////////////////////////////////////////////
BabyGrasshopper::BabyGrasshopper(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int hitpoints) :Grasshopper(sw, imgID, startX, startY, curTick, hitpoints)
{
	
}

void BabyGrasshopper::getStunned()
{
	if(!hasBeenAffected())
		addState(2);
	setAffected(true);
}

void BabyGrasshopper::getPoisoned()
{
	changeHP(-150);
}

bool BabyGrasshopper::doSpecializedGrasshopperMethod()
{
	// If its hit points are greater than or equal to 1600, then it will moult and turn into an adult grasshopper
	if (getHP() >= 1600)
	{
		getStudentWorld()->addActor(new AdultGrasshopper(getStudentWorld(), getX(), getY(), getTicks() - 1));
		changeHP(-getHP()); //set dead, in context of the energy holder class
		return true;
	}
	return false;
}


BabyGrasshopper::~BabyGrasshopper() { }

///////////////////////////////////////////////////////////////////////////
//  Adult Grasshopper Class Implementation
///////////////////////////////////////////////////////////////////////////
AdultGrasshopper::AdultGrasshopper(StudentWorld* sw, int startX, int startY, int curTick) :Grasshopper(sw, IID_ADULT_GRASSHOPPER, startX, startY, curTick, 1600)
{

}

bool AdultGrasshopper::doSpecializedGrasshopperMethod()
{
	int randNum = randInt(1, 3);
	if (randNum == 1) //1 in 3 chance it tries to bite another insect on the same square (50 damage). Then add state of 2 (sleep move)
	{
		if (getStudentWorld()->biteEnemyAt(this, -1, 50)) //essentially, the colony number passed does not matter, as long as it is not [0,3]. That way, ants of ALL colonies are enemies
		{
			addState(2);
			return true;
		}
	}
	randNum = randInt(1, 10);
	if (randNum == 1)
	{
		int tmpX = 0;
		int tmpY = 0;
		getStudentWorld()->selectRandomOpenPosition(getX(), getY(), 10, tmpX, tmpY);
		if (tmpX != getX() || tmpY != getY()) //there is an open square!
		{
			moveTo(tmpX, tmpY);
			addState(2);
			return true;
		}
	}
	return false;
}

void AdultGrasshopper::getBitten(int amt)
{
	changeHP(-amt);
	if (isDead())
		return;
	//if it is still alive, there is a 1 in 2 chance it will retaliate
	int randNum = randInt(1, 2);
	if (randNum == 1)
	{
		getStudentWorld()->biteEnemyAt(this, -1, 50);
	}
}

AdultGrasshopper::~AdultGrasshopper()
{

}

///////////////////////////////////////////////////////////////////////////
//  Ant Class Implementation
///////////////////////////////////////////////////////////////////////////
Ant::Ant(StudentWorld* sw, int imgID, int startX, int startY, int curTick, int colonyNum, Compiler* program) :Insect(sw, imgID, startX, startY, static_cast<Direction>(randInt(1, 4)), 1, curTick, 1500)
{
	m_food = 0;
	m_colonyNum = colonyNum;
	m_bitten = false;
	m_blocked = false;
	m_lastRandNum = 0;
	m_counter = 0;
	m_program = program;
}

void Ant::doSomething()
{
	if (preliminaryCheck())
		return;
	Compiler::Command cmd; //fetch the next command, in the following line
	int numCommandsExecuted = 0;
	while (numCommandsExecuted != 10) //max run 10 commands in a tick
	{
		if (!m_program->getCommand(m_counter, cmd)) //there is an error fetching the command
		{
			changeHP(-getHP()); //logically equivalent to set dead, draining all the health
			return;
		}
		//cerr << cmd.lineNum << " " << cmd.text << " ";
		//cerr << "instruction counter" << m_counter << endl;
		switch (cmd.opcode)
		{
			case Compiler::Opcode::moveForward:
			{
				if (attemptMove(getX(), getY(), getDirection()))
				{
					m_blocked = false;
					m_bitten = false;
				}
				else m_blocked = true; //failed to move, blocked by a pebble
				m_counter++;
				return;
			}
			case Compiler::Opcode::eatFood:
			{
				if (m_food < 100) //less than 100 units of food
				{
					changeHP(m_food);
					m_food = 0;
				}
				else //eat 100 food, general case
				{
					changeHP(100);
					m_food -= 100;
				}
				m_counter++;
				return;
			}
			case Compiler::Opcode::dropFood:
			{
				getStudentWorld()->addFood(getX(), getY(), m_food);
				m_food = 0;
				m_counter++;
				return;
			}
			case Compiler::Opcode::bite: //causes 15 damange to enemies
			{
				getStudentWorld()->biteEnemyAt(this, m_colonyNum, 15);
				m_counter++;
				return;
			}
			case Compiler::Opcode::pickupFood:
			{
				int foodInPosition = getStudentWorld()->getNumEnergy(getX(), getY());
				if (foodInPosition == -1)
				{
					m_counter++;
					return;
				}
				int willPickUp = foodInPosition < 400 ? foodInPosition : 400; //determine how much food to pickup: 400 or whatever is left if less than 400
				if (m_food + willPickUp > 1800) //can only hold up to 1800 units of food
				{
					getStudentWorld()->addFood(getX(), getY(), m_food - 1800); //pick up however much it can until it reaches 1800 capacity
					m_food = 1800;
				}
				else //general case, just pick up and update the food stack accordingly
				{
					getStudentWorld()->addFood(getX(), getY(), -willPickUp);
					m_food += willPickUp;
				}
				m_counter++;
				return;
			}
			case Compiler::Opcode::emitPheromone:
			{ 
				Actor* obj = getStudentWorld()->getPheromoneAt(getX(), getY(), m_colonyNum);
				if (obj == nullptr)
				{
					int pheromoneImgID = 0; //will never be this value since colony number passed must be between 0 and 3 inclusive
					if (m_colonyNum == 0)
						pheromoneImgID = IID_PHEROMONE_TYPE0;
					else if (m_colonyNum == 1)
						pheromoneImgID = IID_PHEROMONE_TYPE1;
					else if (m_colonyNum == 2)
						pheromoneImgID = IID_PHEROMONE_TYPE2;
					else if (m_colonyNum == 3)
						pheromoneImgID = IID_PHEROMONE_TYPE3;
					getStudentWorld()->addActor(new Pheromone(getStudentWorld(), pheromoneImgID, getX(), getY(), getTicks() - 1, m_colonyNum));
				}
				else
				{
					//if adding 256 goes over 768
					if (obj->getHP() + 256 > 768)
						obj->changeHP(768 - obj->getHP());
					else //just add 256
						obj->changeHP(256);
				}
				m_counter++;
				return;
			}
			case Compiler::Opcode::faceRandomDirection:
			{
				setDirection(static_cast<Direction>(randInt(1, 4))); //north, east, south, or west, at random
				m_counter++;
				return;
			}
			case Compiler::Opcode::rotateClockwise:
			{
				if (getDirection() == left)
					setDirection(up); //since left (west) is at position 4 of Direction enum, wrap around back to up (north)
				else
					setDirection(static_cast<Direction>(getDirection() + 1)); //move to next position of Direction enum, which accurately sets direction to be 90 degrees clockwise from current direction
				m_counter++;
				return;
			}
			case Compiler::Opcode::rotateCounterClockwise:
			{
				if (getDirection() == up)
					setDirection(left); //since up (north) is at position 0 of Direction enum, wrap around back to left (west)
				else
					setDirection(static_cast<Direction>(getDirection() - 1)); //move to 90 degrees counterclockwise direction from current direction, which is available by going 1 position left in Direction enum
				m_counter++;
				return;
			}
			case Compiler::Opcode::generateRandomNumber:
			{
				if (stoi(cmd.operand1) == 0)
					m_lastRandNum = 0;
				else
					m_lastRandNum = randInt(0, stoi(cmd.operand1) - 1); //random number from 0 to N - 1
				m_counter++;
			}
				break;
			case Compiler::Opcode::goto_command:
			{
				m_counter = stoi(cmd.operand1); //set instruction counter to the position specified in operand1
			}
				break;
			case Compiler::Opcode::if_command: //various if statements, check
			{
				if (stoi(cmd.operand1) == 9) //last_random_number_was_zero
				{
					if (m_lastRandNum == 0)
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 3) //i_am_carrying_food
				{
					if (m_food > 0)
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 4) //i_am_hungry
				{
					if (getHP() <= 25)
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 7) //i_am_standing_with_an_enemy
				{
					if (getStudentWorld()->isEnemyAt(getX(), getY(), m_colonyNum))
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 6) //i_am_standing_on_food
				{
					if (getStudentWorld()->getNumEnergy(getX(), getY()) != -1) //there is food
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 5) //i_am_standing_on_my_anthill
				{
					if (getStudentWorld()->isAntHillAt(getX(), getY(), m_colonyNum))
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 1) //i_smell_pheromone_in_front_of_me
				{
					int x = getX();
					int y = getY();
					getXYInFrontOfMe(x, y);
					if (getStudentWorld()->getPheromoneAt(x, y, m_colonyNum) != nullptr)
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 0) //i_smell_danger_in_front_of_me
				{
					int x = getX();
					int y = getY();
					getXYInFrontOfMe(x, y);
					if (getStudentWorld()->isDangerAt(x, y) || getStudentWorld()->isEnemyAt(x, y, m_colonyNum))
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 2) //i_was_bit
				{
					if (m_bitten)
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
				else if (stoi(cmd.operand1) == 8) //i_was_blocked_from_moving
				{
					if (m_blocked)
						m_counter = stoi(cmd.operand2);
					else
						m_counter++;
				}
			}
				break;
			default: break;
		}
		numCommandsExecuted++;
	}
}

bool Ant::canBeAffected() const
{
	return true;
}

bool Ant::isEnemy(int colony) const
{
	return m_colonyNum != colony;
}

void Ant::getStunned()
{
	if (!hasBeenAffected())
		addState(2);
	setAffected(true);
}

void Ant::getPoisoned()
{
	changeHP(-150);
}

void Ant::getBitten(int amt)
{
	changeHP(-amt);
	m_bitten = true;
}

Ant::~Ant()
{

}