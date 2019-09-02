#include "StudentWorld.h"
#include "GameConstants.h"
#include "GameWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <list>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	player = nullptr;
}

int StudentWorld::init()
{
	m_numCitizens = 0;
	m_levelFinished = false;
	m_playerDead = false;

	Level currentLev(assetPath());
	int lev = getLevel();
	
	ostringstream levelOSS;
	levelOSS.fill('0');
	levelOSS << "level" << setw(2) << lev << ".txt";
	string level = levelOSS.str();
	
	Level::LoadResult result = currentLev.loadLevel(level);
	if (result == Level::load_fail_file_not_found || lev > 99) 
	{
		cerr << "Cannot find " << level << "data file / Finished level 99" << endl;
		return GWSTATUS_PLAYER_WON;
	}
	else if (result == Level::load_fail_bad_format)
	{
		cerr << "Your level was improperly formatted" << endl;
		return GWSTATUS_LEVEL_ERROR;
	}
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level\n";
		for (int y = 0; y < LEVEL_HEIGHT; y++)
			for (int x = 0; x < LEVEL_WIDTH; x++)
			{
				Level::MazeEntry ge = currentLev.getContentsOf(x, y);
				switch (ge)
				{
				case Level::player:
					player = new Penelope(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, this);
					break;
				case Level::wall: 
					actors.push_back(new Wall(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, this)); 
					break;
				case Level::exit: 
					actors.push_back(new Exit(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 1, this)); 
					break;
				case Level::pit: 
					actors.push_back(new Pit(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, this)); 
					break;
				case Level::vaccine_goodie:
					actors.push_back(new Vaccine(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 1, this));
					break;
				case Level::landmine_goodie:
					actors.push_back(new LandMine(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 1, this));
					break;
				case Level::gas_can_goodie: 
					actors.push_back(new GasCan(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 1, this)); 
					break;
				case Level::citizen:
					actors.push_back(new Citizen(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, this));
					m_numCitizens++;
					break;
				case Level::dumb_zombie:
					actors.push_back(new DumbZombie(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, this));
					break;
				case Level::smart_zombie:
					actors.push_back(new SmartZombie(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, this));
					break;
				default: break;
				}
			}
	}

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	ostringstream scoreboard;
	scoreboard.fill('0');
	if (getScore() >= 0)
	{
		scoreboard << "Score: " << setw(6) << getScore();
		scoreboard.fill(' ');
		scoreboard << "  Level: " << setw(2) << getLevel() << "  Lives: " << getLives() << "  Vacc: " << player->getVacc() << "  Flames: " <<
			player->getFlames() << "  Mines: " << player->getMines() << "  Infected: " << player->getInfectedTimer();
		string score = scoreboard.str();
		setGameStatText(score);
	}
	else
	{
		scoreboard << "Score: -" << setw(5) << - getScore();
		scoreboard.fill(' ');
		scoreboard << "  Level: " << setw(2) << getLevel() << "  Lives: " << getLives() << "  Vacc: " << player->getVacc() << "  Flames: " <<
			player->getFlames() << "  Mines: " << player->getMines() << "  Infected: " << player->getInfectedTimer();
		string score = scoreboard.str();
		setGameStatText(score);
	}
	
	player->doSomething();

	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		temp->doSomething();
	}

	removeDead();

	if (m_levelFinished)
		return GWSTATUS_FINISHED_LEVEL;

	if (m_playerDead)
	{
		playSound(SOUND_PLAYER_DIE);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	if (player != nullptr)
	{
		list<Actor*>::iterator it;
		for (it = actors.begin(); it != actors.end(); it++)
		{
			Actor* temp = *it;
			delete temp;
		}
		actors.erase(actors.begin(), actors.end());
		
		delete player;
		player = nullptr;
	}
}

void StudentWorld::playerDied()
{
	player->setAlive(false);
	m_playerDead = true;
}

void StudentWorld:: citizenDied(double x, double y)
{
	playSound(SOUND_CITIZEN_DIE);
	increaseScore(-1000);
	m_numCitizens--;
}

void StudentWorld::zombieDied(double x, double y, Actor* zombie)
{
	playSound(SOUND_ZOMBIE_DIE);
	if (zombie->canHoldVaccine())
	{
		increaseScore(1000);
		if (randInt(1, 10) == 1)
		{
			int dir = randInt(1, 4);
			switch (dir)
			{
			case 1:
				x -= SPRITE_WIDTH; break;
			case 2:
				x += SPRITE_WIDTH; break;
			case 3:
				y += SPRITE_HEIGHT;  break;
			case 4:
				y -= SPRITE_HEIGHT; break;
			default: break;
			}
			if (checkOverlap(x, y, zombie))
				createVaccine(x, y);
		}
	}
	else
		increaseScore(2000);
}

bool StudentWorld::checkMovementBlock(double destX, double destY, Actor* caller) const
{
	if (caller != player)
		if ((abs(player->getX() - destX) <= SPRITE_WIDTH - 1) && (abs(player->getY() - destY) <= SPRITE_HEIGHT - 1))
			return false;

	list<Actor*>::const_iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->shouldBlock())
		{
			if (temp == caller)
				continue;
			else if ((abs(temp->getX() - destX) <= SPRITE_WIDTH - 1) && (abs(temp->getY() - destY) <= SPRITE_HEIGHT - 1))
				return false;
		}
	}
	return true;
}

Actor* StudentWorld::checkMortalOverlap(double x, double y, int& type, Actor* caller)	//returns 0 if not overlapping, 1 if overlapping with player, 2 if citizen, 3 if zombie
{
	if (pow(player->getX() - x, 2) + pow(player->getY() - y, 2) <= 100)	//check player
	{
		type = 1;
		return player;
	}

	list<Actor*>::const_iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->alive() && temp->shouldBlock() && (pow(temp->getX() - x, 2) + pow(temp->getY() - y, 2) <= 100))
		{
			if (temp->infectable())	//has to be citizen because player was already checked
			{
				type = 2;
				return temp;
			}
			else
			{
				type = 3;
				return temp;
			}
		}
	}
	return caller;
}

void StudentWorld::citizenSaved()
{
	playSound(SOUND_CITIZEN_SAVED);
	increaseScore(500);
	m_numCitizens--;
}

bool checkDead(Actor* p)
{
	return !(p->alive());
}

void StudentWorld::removeDead()
{
	list<Actor*>::iterator it = find_if(actors.begin(), actors.end(), checkDead);
	while (it != actors.end())
	{
		Actor* temp = *it;
		delete temp;
		it = actors.erase(it);
		it--;
		it = find_if(it, actors.end(), checkDead);
	}
}

void StudentWorld::incGoodie(int goodie) //0 = gascan, 1 = vaccine, 2 = landmine
{
	increaseScore(50);
	playSound(SOUND_GOT_GOODIE);
	switch (goodie)
	{
	case 0: player->incFlames(); break;
	case 1: player->incVacc(); break;
	case 2: player->incMine(); break;
	default: break;
	}
}

void StudentWorld::penelopeCoord(double& x, double& y)
{
	x = player->getX(); 
	y = player->getY();
}

bool StudentWorld::checkFlameBlock(double destX, double destY) const
{
	list<Actor*>::const_iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->alive() && temp->flameRetardant())
		{
			if (pow(temp->getX() - destX, 2) + pow(temp->getY() - destY, 2) <= 100)
				return false;
		}
	}
	return true;
}

void StudentWorld::createFlame(Actor* caller)
{
	double x = caller->getX(), y = caller->getY();

	if (caller == player)
	{
		Direction dir = player->getDirection();
		switch (dir)
		{
		case GraphObject::left:
			for (int i = 1; i <= 3; i++)
			{
				if (checkFlameBlock(x - (i * SPRITE_WIDTH), y))
					actors.push_back(new Flame(x - (i * SPRITE_WIDTH), y, 0, this, dir));
				else
					break;
			}
			break;
		case GraphObject::right:
			for (int i = 1; i <= 3; i++)
			{
				if (checkFlameBlock(x + (i * SPRITE_WIDTH), y))
					actors.push_back(new Flame(x + (i * SPRITE_WIDTH), y, 0, this, dir));
				else
					break;
			}
			break;
		case GraphObject::up:
			for (int i = 1; i <= 3; i++)
			{
				if (checkFlameBlock(x, y + (i * SPRITE_HEIGHT)))
					actors.push_back(new Flame(x, y + (i * SPRITE_HEIGHT), 0, this, dir));
				else
					break;
			}
			break;
		case GraphObject::down:
			for (int i = 1; i <= 3; i++)
			{
				if (checkFlameBlock(x, y - (i * SPRITE_HEIGHT)))
					actors.push_back(new Flame(x, y - (i * SPRITE_HEIGHT), 0, this, dir));
				else
					break;
			}
			break;
		}
		return;
	}
	else
	{
		actors.push_back(new Flame(x, y, 0, this));	//make flame at location of landmine (5)

		if (checkFlameBlock(x - SPRITE_WIDTH, y + SPRITE_HEIGHT))	//location (1)
			actors.push_back(new Flame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT, 0, this));

		if (checkFlameBlock(x, y + SPRITE_HEIGHT))					//location (2)
			actors.push_back(new Flame(x, y + SPRITE_HEIGHT, 0, this));

		if (checkFlameBlock(x + SPRITE_WIDTH, y + SPRITE_HEIGHT))	//location (3)
			actors.push_back(new Flame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT, 0, this));

		if (checkFlameBlock(x - SPRITE_WIDTH, y))					//location (4)
			actors.push_back(new Flame(x - SPRITE_WIDTH, y, 0, this));

		if (checkFlameBlock(x + SPRITE_WIDTH, y))					//location (6)
			actors.push_back(new Flame(x + SPRITE_WIDTH, y, 0, this));

		if (checkFlameBlock(x - SPRITE_WIDTH, y - SPRITE_HEIGHT))	//location (7)
			actors.push_back(new Flame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT, 0, this));

		if (checkFlameBlock(x, y - SPRITE_HEIGHT))					//location (8)
			actors.push_back(new Flame(x, y - SPRITE_HEIGHT, 0, this));

		if (checkFlameBlock(x + SPRITE_WIDTH, y - SPRITE_HEIGHT))	//location (9)
			actors.push_back(new Flame(x + SPRITE_WIDTH, y - SPRITE_HEIGHT, 0, this));

		actors.push_back(new Pit(x, y, 0, this));
	}
}

void StudentWorld::checkFlameDamage(Actor* caller)
{
	if (pow(player->getX() - caller->getX(), 2) + pow(player->getY() - caller->getY(), 2) <= 100)
		playerDied();

	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->alive() && temp->mortal() && !temp->flameRetardant() && pow(temp->getX() - caller->getX(), 2) + pow(temp->getY() - caller->getY(), 2) <= 100)
		{
			temp->setAlive(false);
			if (temp->shouldBlock())
			{
				if (temp->infectable())
				{
					temp->setAlive(false);
					citizenDied(temp->getX(), temp->getY());
				}
				else
				{
					temp->setAlive(false);
					zombieDied(temp->getX(), temp->getY(), temp);
				}
			}
		}
	}
}

void StudentWorld::createMine(Actor* caller)
{
	actors.push_back(new Mine(caller->getX(), caller->getY(), 1, this));
}

bool StudentWorld::checkOverlap(double x, double y, Actor* caller) const
{
	if (pow(player->getX() - x, 2) + pow(player->getY() - y, 2) <= 100)
		return true;

	list<Actor*>::const_iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp != caller && pow(temp->getX() - x, 2) + pow(temp->getY() - y, 2) <= 100)
			return true;
	}
	return false;
}

bool StudentWorld::checkVomitRange(double x, double y) const
{
	if (pow(player->getX() - x, 2) + pow(player->getY() - y, 2) <= 100)
		return true;

	list<Actor*>::const_iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->infectable() && pow(temp->getX() - x, 2) + pow(temp->getY() - y, 2) <= 100)
			return true;
	}
	return false;
}

void StudentWorld::checkVomitVictims(double x, double y)
{
	if (pow(player->getX() - x, 2) + pow(player->getY() - y, 2) <= 100)
		player->infect(true);

	list<Actor*>::const_iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->infectable() && pow(temp->getX() - x, 2) + pow(temp->getY() - y, 2) <= 100)
		{
			if (!temp->infected())
			{
				temp->infect(true);
				playSound(SOUND_CITIZEN_INFECTED);
			}
		}
	}
}

void StudentWorld::createVomit(double x, double y, Direction dir)
{
	actors.push_back(new Vomit(x, y, 0, this, dir));
	playSound(SOUND_ZOMBIE_VOMIT);
}

void StudentWorld::createVaccine(double x, double y)
{
	actors.push_back(new Vaccine(x, y, 0, this));
}

void StudentWorld::createZombie(Actor* caller)
{
	playSound(SOUND_ZOMBIE_BORN);
	increaseScore(-1000);
	if (randInt(1, 10) <= 3)
		actors.push_back(new SmartZombie(caller->getX(), caller->getY(), 0, this));
	else
		actors.push_back(new DumbZombie(caller->getX(), caller->getY(), 0, this));
}

double StudentWorld::closestPerson(double& x, double& y)
{
	double distP = distance(x, y, player->getX(), player->getY());
	Actor* closest = player;

	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->infectable())
		{
			double tempDist = distance(x, y, temp->getX(), temp->getY());
			if (tempDist < distP)
			{
				distP = tempDist;
				closest = temp;
			}
		}
	}

	x = closest->getX();
	y = closest->getY();
	return distP;
}

double StudentWorld::closestZombie(double x, double y)
{
	double distZ = 0;

	list<Actor*>::iterator it;
	for (it = actors.begin(); it != actors.end(); it++)
	{
		Actor* temp = *it;
		if (temp->mortal() && !temp->infectable())
		{
			double tempDist = distance(x, y, temp->getX(), temp->getY());
			if (distZ == 0 || tempDist < distZ)
				distZ = tempDist;
		}
	}

	return distZ;
}

double distance(double x1, double y1, double x2, double y2)
{
	return pow((pow(x1 - x2, 2) + pow((y1 - y2), 2)), 0.5);
}
