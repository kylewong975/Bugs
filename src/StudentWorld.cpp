#include "StudentWorld.h"

#include "Field.h"
#include "GameConstants.h"
#include "GameWorld.h"
#include "Actor.h"
#include "Compiler.h"

#include <string>
#include <map>
#include <vector>
#include <list>
#include <cmath> //for sin, cos, etc

#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::
#include <iomanip>  // defines the manipulator setw

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

//Because a map automatically orders the left-hand value in order, we need a custom comparison operator, for left-hand Coord
bool operator<(const Coord& a, const Coord& b)
{
	return a.x < b.x || (a.x == b.x && a.y < b.y);
}

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
	m_ticks = 0;
	currentLead = -1;
	currentMax = 5;
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	Field f;
	string fieldFile = getFieldFilename();
	string error;
	if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
	{
		setError(fieldFile + " " + error);
		return GWSTATUS_LEVEL_ERROR; //something bad happened!
	}

	int count = 0; //handle cases of varying number of competitiors

	vector<string> fileNames = getFilenamesOfAntPrograms();
	if (fileNames.size() > 4) //invalid input
		return GWSTATUS_LEVEL_ERROR;

	//push the appropriate number of compilers needed for the simulation
	for (int x = 0; x < fileNames.size(); x++)
		m_compilers.push_back(new Compiler);

	//now allocate anthills and pass in compilers to how ever many competitors there are for this simulation
	for (int x = 0; x < fileNames.size(); x++)
	{
		// compile the source file… If the compile function returns false, there was a syntax error during compilation!
		if (!m_compilers[x]->compile(fileNames[x], error))
		{
			// entrant x’s source code had a syntax error! send this error to our framework to warn the user.
			setError(fileNames[x] + " " + error);
			return GWSTATUS_LEVEL_ERROR; // return an error to tell our simulation framework that something went wrong, and it’ll inform the user
		}
	}

	//get contents from the field
	for (int x = 0; x < VIEW_HEIGHT; x++)
	{
		for (int y = 0; y < VIEW_WIDTH; y++)
		{
			Coord loc(x, y);
			if (f.getContentsOf(x, y) == Field::FieldItem::rock)
				m_actors[loc].push_back(new Pebble(this, x, y, m_ticks));
			else if (f.getContentsOf(x, y) == Field::FieldItem::grasshopper)
				m_actors[loc].push_back(new BabyGrasshopper(this, IID_BABY_GRASSHOPPER, x, y, m_ticks, 500));
			else if (f.getContentsOf(x, y) == Field::FieldItem::food)
				m_actors[loc].push_back(new Food(this, x, y, m_ticks, 6000));
			else if (f.getContentsOf(x, y) == Field::FieldItem::water)
				m_actors[loc].push_back(new PoolOfWater(this, x, y, m_ticks));
			else if (f.getContentsOf(x, y) == Field::FieldItem::poison)
				m_actors[loc].push_back(new Poison(this, x, y, m_ticks));
			else if (f.getContentsOf(x, y) == Field::FieldItem::anthill0) //anthill for competitor 0
			{
				if (m_compilers.size() >= 1)
					m_actors[loc].push_back(new Anthill(this, x, y, m_ticks, 0, m_compilers[0]));
			}
			else if (f.getContentsOf(x, y) == Field::FieldItem::anthill1) //anthill for competitor 1
			{
				if (m_compilers.size() >= 2)
					m_actors[loc].push_back(new Anthill(this, x, y, m_ticks, 1, m_compilers[1]));
			}
			else if (f.getContentsOf(x, y) == Field::FieldItem::anthill2) //anthill for competitor 2
			{
				if (m_compilers.size() >= 3)
					m_actors[loc].push_back(new Anthill(this, x, y, m_ticks, 2, m_compilers[2]));
			}
			else if (f.getContentsOf(x, y) == Field::FieldItem::anthill3) //anthill for competitor 3
			{
				if (m_compilers.size() >= 4)
					m_actors[loc].push_back(new Anthill(this, x, y, m_ticks, 3, m_compilers[3]));
			}
		}
	}
	
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_NO_WINNER will cause our framework to end the simulation.

	m_ticks++; //increment tick count

	//give each actor a chance to do something, only if it is alive
	map<Coord, list<Actor*>>::iterator mapIter;
	for (mapIter = m_actors.begin(); mapIter != m_actors.end(); )
	{
		list<Actor*>::iterator actIter;
		Coord tmp = mapIter->first; //the key associated in the map is Coord (x, y coordinates)
		int oldX = tmp.x;
		int oldY = tmp.y;
		for (actIter = mapIter->second.begin(); actIter != mapIter->second.end(); ) //iterate through list<Actor*> within each map coordinate
		{
			Actor* obj = *actIter;
			//give each actor an opportunity to do something, if it is not dead, and make sure that actor has NOT made a move yet.
			if (!(obj->isDead()) && obj->getTicks() == m_ticks)
			{
				obj->doSomething();
			}

			//now, remove actor from the old position's list and put in new position's list if the actor has moved
			if (obj->getX() != oldX || obj->getY() != oldY) //actor has moved!
			{
				if (obj->becomesFoodUponDeath()) //aka insect
				{
					Insect* tmpObj = static_cast<Insect*>(obj);
					tmpObj->setAffected(false); //now the insect (ant and baby grasshopper) can be restunned if go back to the pool of water in previous location
				}
				Actor* moveObj = obj; //moveObj and obj points to same actor object
				Coord newLoc(obj->getX(), obj->getY());
				m_actors[newLoc].push_back(moveObj); //put the object in new position's list
				actIter = m_actors[tmp].erase(actIter); //unpoint obj, not delete, the dynamic actor
			}
			else
				actIter++;
		}

		if (mapIter->second.empty()) //to save space, remove any empty vectors
			mapIter = m_actors.erase(mapIter);
		else
			mapIter++;
	}

	//delete any dead objects
	for (mapIter = m_actors.begin(); mapIter != m_actors.end(); mapIter++)
	{
		list<Actor*>::iterator actIter;
		Coord tmp = mapIter->first;
		for (actIter = mapIter->second.begin(); actIter != mapIter->second.end(); )
		{
			if ((*actIter)->isDead())
			{
				delete (*actIter);
				actIter = mapIter->second.erase(actIter);
			}
			else actIter++;
		}
	}

	ostringstream oss;
	oss << "Ticks:" << setw(5) << (2000 - m_ticks);
	if (m_compilers.size() > 0) //then put a hyphen between the tick count and the ant colonies information
		oss << " - ";
	maxAnts(currentLead);
	for (int x = 0; x < m_compilers.size(); x++)
	{
		if(x != 0)
			oss << "  "; //2 spaces between colony data
		if (currentLead == x) //this colony is leading the scoreboard
		{
			oss.fill('0');
			oss << m_compilers[x]->getColonyName() << "*: " << setw(2) << antCount[x] << " ants";
			oss.fill(' ');
		}
		else
		{
			oss.fill('0');
			oss << m_compilers[x]->getColonyName() << ": " << setw(2) << antCount[x] << " ants";
			oss.fill(' ');
		}
	}

	setGameStatText(oss.str());

	if (m_ticks == 2000) 
	{
		if (currentLead == -1) //no colony has produced more than 5 ants
			return GWSTATUS_NO_WINNER;
		//otherwise, there is a winner!
		setWinner(m_compilers[currentLead]->getColonyName());
		return GWSTATUS_PLAYER_WON;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	//delete all actors in the field
	map<Coord, list<Actor*>>::iterator mapIter;
	for (mapIter = m_actors.begin(); mapIter != m_actors.end(); mapIter++)
	{
		//shortcut, just access the second component of the map.
		list<Actor*>::iterator actIter;
		for (actIter = mapIter->second.begin(); actIter != mapIter->second.end(); actIter++)
			delete (*actIter);
		//erasing list elements isn't necessary since simulation is over. The entire map is removed anyway
	}
	m_actors.clear(); //to not worry about deleting dangling pointers

	//also since Compilers are dynamically allocated objects, delete them
	for (int x = 0; x < m_compilers.size(); x++)
		delete m_compilers[x];
	m_compilers.clear(); //to not worry about deleting dangling pointers
}

bool StudentWorld::hasBlockableObject(int x, int y)
{
	//we can technically just check if it's a rock but for OOP best practices, I just added a blocksOthers() in case later on, we add boulders that can block
	Coord loc(x, y);
	list<Actor*>::iterator actIter;
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		if ((*actIter)->blocksOthers())
			return true;
	}
	return false;
}

void StudentWorld::stunAllActors(int x, int y)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		Actor* obj = *actIter;
		obj->getStunned(); //sleeps 2 more moves in addition to whatever the state is
	}
}

void StudentWorld::hurtAllActors(int x, int y)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		Actor* obj = *actIter;
		obj->getPoisoned(); //poison, no matter what, decreases actor's hit points by 150 for those that can be affected
	}
}

void StudentWorld::addFood(int x, int y, int amt)
{
	if (amt == 0) return; //no need to do all the work if you are not adding any food. Note negative numbers are fine: it means taking out food from the food stack
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	bool hasStackable = false; //check if the current location has a food/pheromone object existing
	Actor* stackableObj = nullptr;
	Actor* obj = nullptr;
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		obj = *actIter;
		if (obj->isFood())
		{
			hasStackable = true;
			stackableObj = obj;
			break; //since there can be at most 1 stackable object in the given location
		}
	}
	if (!hasStackable)
		m_actors[loc].push_back(new Food(this, x, y, m_ticks, amt));
	else
		stackableObj->changeHP(amt);
}

int StudentWorld::getNumEnergy(int x, int y)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	bool hasStackable = false; //check if the current location has a food/pheromone object existing
	Actor* stackableObj = nullptr;
	Actor* obj = nullptr;
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		obj = *actIter;
		if (obj->isFood())
		{
			hasStackable = true;
			stackableObj = obj;
			break; //since there can be at most 1 stackable object in the given location
		}
	}
	if (hasStackable)
	{
		if (stackableObj->getHP() <= 0) return -1; //in case if at the same tick someone took all the food then say there is no food left
		return stackableObj->getHP();
	}
	return -1;
}

void StudentWorld::addActor(Actor* a)
{
	Coord c(a->getX(), a->getY());
	m_actors[c].push_back(a);
}

void StudentWorld::selectRandomOpenPosition(int curX, int curY, int radius, int& randX, int& randY)
{
	int lowerX, lowerY, upperX, upperY;
	if (radius % 2 == 0)
	{
		lowerX = curX - radius / 2;
		lowerY = curY - radius / 2;
		upperX = curX + radius / 2;
		upperY = curY + radius / 2;
	}
	else
	{
		lowerX = curX - radius / 2 - 1;
		lowerY = curY - radius / 2 - 1;
		upperX = curX + radius / 2 + 1;
		upperY = curY + radius / 2 + 1;
	}
	vector<Coord> possibleLocs; //possible open locations
	for (int x = lowerX; x <= upperX; x++)
	{
		for (int y = lowerY; y <= upperY; y++)
		{
			if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT)
				continue;
			if (!hasBlockableObject(x, y) && static_cast<int>(sqrt(pow(x - curX, 2) + pow(y - curY, 2))) <= radius)
			{
				Coord tmp(x, y);
				possibleLocs.push_back(tmp);
			}
		}
	}
	//now choose a random possible location
	if (possibleLocs.empty()) return;
	int randNum = randInt(0, possibleLocs.size() - 1);
	Coord ans = possibleLocs[randNum];
	randX = ans.x;
	randY = ans.y;
}

bool StudentWorld::isEnemyAt(int x, int y, int colony)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		Actor* obj = *actIter;
		if (obj->isEnemy(colony))
			return true;
	}
	return false;
}

bool StudentWorld::isDangerAt(int x, int y)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		Actor* obj = *actIter;
		if (obj->isDangerous())
			return true;
	}
	return false;
}

bool StudentWorld::isAntHillAt(int x, int y, int colony)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		Actor* obj = *actIter;
		if (obj->isAntHill())
		{
			Anthill* ah = static_cast<Anthill*>(obj);
			return ah->isMyHill(colony);
		}
	}
	return false;
}

bool StudentWorld::biteEnemyAt(Actor* me, int colony, int biteDamage)
{
	vector<Actor*> enemies;
	list<Actor*>::iterator actIter; //vector easy random access
	Coord loc(me->getX(), me->getY());
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
	{
		if ((*actIter) != me && (*actIter)->isEnemy(colony))
			enemies.push_back(*actIter);
	}
	if (enemies.empty()) return false;
	Actor* victim = enemies[randInt(0, enemies.size() - 1)]; //choose a random enemy to bite, if there exists multiple
	victim->getBitten(biteDamage);
	return true;
}

void StudentWorld::increaseScore(int colony)
{
	if (colony >= m_compilers.size()) //you can only access the score of however many colonies there exists in the simulation
		return;
	antCount[colony]++;
}

void StudentWorld::maxAnts(int& curColony)
{
	int max = 0;
	int indexOfMax = 0;
	for (int x = 0; x < m_compilers.size(); x++)
	{
		if (max < antCount[x])
		{
			max = antCount[x];
			indexOfMax = x;
		}
	}
	if (max <= currentMax) //no colony has surpassed the current maximum yet
		return;

	curColony = indexOfMax;
	currentMax = max;
}

Actor* StudentWorld::getPheromoneAt(int x, int y, int colony)
{
	list<Actor*>::iterator actIter;
	Coord loc(x, y);
	for (actIter = m_actors[loc].begin(); actIter != m_actors[loc].end(); actIter++)
		if ((*actIter)->isPheromone(colony))
			return *actIter;
	return nullptr;
}