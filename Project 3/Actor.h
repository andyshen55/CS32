#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <iostream>

class StudentWorld; 

class Actor : public GraphObject
{
public:
	Actor(double startX, double startY, int depth, StudentWorld* world, int imageID, Direction dir = right)
		: GraphObject(imageID, startX, startY, dir, depth), m_alive(true), m_world(world)
	{}
	virtual ~Actor() {}
	virtual void doSomething() = 0;
	virtual bool shouldBlock() const = 0;
	virtual bool mortal() const { return false; }
	virtual bool canHoldVaccine() const { return false; }
	virtual bool infectable() const { return false; }
	virtual void infect(bool status) { return; }
	virtual bool infected() const { return true; }
	virtual void setInfectionStatus() const { return; }
	virtual bool flameRetardant() const { return false; }
	bool alive() const { return m_alive; }
	void setAlive(bool status) { m_alive = status; }
	inline StudentWorld* getWorld() const { return m_world; }
private:
	bool m_alive;
	StudentWorld* m_world;
};

class Mortal : public Actor
{
public:
	Mortal(double startX, double startY, int depth, StudentWorld* world, int imageID)
		: Actor(startX, startY, depth, world, imageID), m_paralysis(false)
	{}
	virtual bool shouldBlock() const { return true; }
	virtual bool mortal() const { return true; }
	virtual bool paralysis() const { return m_paralysis; }
	void setParalysis(bool status) { m_paralysis = status; }
private:
	bool m_paralysis;
};

class Infectable : public Mortal
{
public: 
	Infectable(double startX, double startY, int depth, StudentWorld* world, int imageID)
		: Mortal(startX, startY, depth, world, imageID), m_infectedTimer(0), m_infected(false)
	{}
	virtual bool infectable() const { return true; }
	virtual void infect(bool status) { m_infected = status; }
	inline int getInfectedTimer() const { return m_infectedTimer; }
	virtual bool infected() const { return m_infected; }
	inline void incInfectionTimer() { m_infectedTimer++; }
	inline void resetInfectionTimer() { m_infectedTimer = 0; }
private: 
	int m_infectedTimer;
	bool m_infected;
};

class Penelope : public Infectable
{
public:
	Penelope(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_PLAYER)
		: Infectable(startX, startY, depth, world, imageID), m_flameCharges(0), m_vaccines(0), m_landMines(0)
	{}
	~Penelope() {};
	virtual void doSomething();
	inline int getVacc() const { return m_vaccines;  }
	inline int getFlames() const { return m_flameCharges; }
	inline int getMines() const { return m_landMines; }
	void incFlames() { m_flameCharges += 5; }
	void decFlames() { m_flameCharges--; }
	void incVacc() { m_vaccines++; }
	void decVacc() { m_vaccines--; }
	void incMine() { m_landMines += 2; }
	void decMine() { m_landMines--; }
private:
	int m_flameCharges;
	int m_vaccines;
	int m_landMines;
};

class Wall : public Actor
{
public:
	Wall(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_WALL)
		: Actor(startX, startY, depth, world, imageID)
	{}
	inline virtual void doSomething() { return; }
	virtual bool shouldBlock() const { return true; }
	virtual bool flameRetardant() const { return true; }
};

class Overlap : public Actor
{
public:
	Overlap(double startX, double startY, int depth, StudentWorld* world, int imageID, Direction dir = right)
		: Actor(startX, startY, depth, world, imageID, dir)
	{}
	virtual ~Overlap() {}
	virtual bool shouldBlock() const { return false; }
	virtual void doSomething() = 0;
};

class Exit : public Overlap
{
public:
	Exit(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_EXIT)
		: Overlap(startX, startY, depth, world, imageID)
	{}
	virtual bool flameRetardant() const { return true; }
	virtual void doSomething();
};

class Pit : public Overlap
{
public:
	Pit(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_PIT)
		: Overlap(startX, startY, depth, world, imageID)
	{}
	virtual void doSomething();
};

class Goodie : public Overlap
{
public:
	Goodie(double startX, double startY, int depth, StudentWorld* world, int imageID)
		: Overlap(startX, startY, depth, world, imageID)
	{}
	virtual bool mortal() const { return true; }
};

class GasCan : public Goodie
{
public:
	GasCan(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_GAS_CAN_GOODIE)
		: Goodie(startX, startY, depth, world, imageID)
	{}
	virtual void doSomething();
};

class Vaccine : public Goodie
{
public:
	Vaccine(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_VACCINE_GOODIE)
		: Goodie(startX, startY, depth, world, imageID)
	{}
	virtual void doSomething();
};

class LandMine : public Goodie
{
public:
	LandMine(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_LANDMINE_GOODIE)
		: Goodie(startX, startY, depth, world, imageID)
	{}
	virtual void doSomething();
};

class Mine : public Overlap
{
public:
	Mine(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_LANDMINE)
		: Overlap(startX, startY, depth, world, imageID), m_active(false), m_ticks(30)
	{}
	virtual void doSomething();
	inline bool active() const { return m_active; }
	inline int getTicks() const { return m_ticks; }
	void decTicks() { m_ticks--; }
	void setActive() { m_active = true; }
private:
	bool m_active;
	int m_ticks;
};

class Projectile : public Overlap
{
public:
	Projectile(double startX, double startY, int depth, StudentWorld* world, Direction dir, int imageID)
		: Overlap(startX, startY, depth, world, imageID, dir), m_tick(0)
	{}
	virtual bool flameRetardant() const { return true; }
	void incTick() { m_tick++; }
	inline int getTick() const { return m_tick; }
private:
	int m_tick;
};

class Flame : public Projectile
{
public:
	Flame(double startX, double startY, int depth, StudentWorld* world, Direction dir = up, int imageID = IID_FLAME)
		: Projectile(startX, startY, depth, world, dir, imageID)
	{}
	virtual void doSomething();
};

class Vomit : public Projectile
{
public:
	Vomit(double startX, double startY, int depth, StudentWorld* world, Direction dir, int imageID = IID_VOMIT)
		: Projectile(startX, startY, depth, world, dir, imageID)
	{}
	virtual void doSomething();
};

class Citizen : public Infectable
{
public:
	Citizen(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_CITIZEN)
		: Infectable(startX, startY, depth, world, imageID)
	{}
	virtual void doSomething();
};

class Zombie : public Mortal
{
public:
	Zombie(double startX, double startY, int depth, StudentWorld* world, int imageID = IID_ZOMBIE)
		: Mortal(startX, startY, depth, world, imageID), m_movementPlan(0)
	{}
	inline int getMovementPlan() const { return m_movementPlan; }
	void setMovementPlan(int newPlan) { m_movementPlan = newPlan; }
	void decMovementPlan() { m_movementPlan--; }
private:
	int m_movementPlan;
};

class DumbZombie : public Zombie
{
public:
	DumbZombie(double startX, double startY, int depth, StudentWorld* world)
		: Zombie(startX, startY, depth, world)
	{}
	virtual bool canHoldVaccine() const { return true; }
	virtual void doSomething();
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(double startX, double startY, int depth, StudentWorld* world)
		: Zombie(startX, startY, depth, world)
	{}
	virtual void doSomething();
};

#endif // ACTOR_H_
