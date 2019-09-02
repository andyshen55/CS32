#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <algorithm>

void Penelope::doSomething()
{
	if (!alive())
		return;
	
	StudentWorld* world = getWorld();
	if (infected())
	{
		if (getInfectedTimer() < 500)
			incInfectionTimer();
		else
		{
			world->playerDied();
			return;
		}
	}

	int ch; 
	if (world->getKey(ch))
	{
		double x = getX();
		double y = getY();

		switch (ch)
		{
		case KEY_PRESS_LEFT: 
			setDirection(left);
			if (world->checkMovementBlock(x - 4, y, this))
				moveTo(x - 4, y);
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (world->checkMovementBlock(x + 4, y, this))
				moveTo(x + 4, y);
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (world->checkMovementBlock(x, y + 4, this))
				moveTo(x, y + 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (world->checkMovementBlock(x, y - 4, this))
				moveTo(getX(), getY() - 4);
			break;
		case KEY_PRESS_SPACE:
			if (m_flameCharges)
			{
				decFlames();
				world->playSound(SOUND_PLAYER_FIRE);
				world->createFlame(this);
			}
			break;
		case KEY_PRESS_TAB:
			if (m_landMines)
			{
				world->createMine(this);
				decMine();
			}
			break;
		case KEY_PRESS_ENTER:
			if (m_vaccines)
			{
				infect(false);
				resetInfectionTimer();
				decVacc();
			}
		default: break;
		}
	}
	return;
}

void Exit::doSomething()
{
	StudentWorld* world = getWorld();
	int result = 0;
	Actor* temp = world->checkMortalOverlap(getX(), getY(), result, this);
	
	if (result == 2)
	{
		temp->setAlive(false);
		world->citizenSaved();
	}

	if (world->getCitizens() == 0 && result == 1)
	{
		world->levelFinish();
		world->playSound(SOUND_LEVEL_FINISHED);
	}

	return;
}

void Pit::doSomething()
{
	StudentWorld* world = getWorld();
	int result = 0;
	Actor* temp = world->checkMortalOverlap(getX(), getY(), result, this);
	switch (result)
	{
	case 0: break;
	case 1: world->playerDied(); break;
	case 2: world->citizenDied(temp->getX(), temp->getY()); temp->setAlive(false); break;
	case 3: world->zombieDied(temp->getX(), temp->getY(), temp); temp->setAlive(false); break;
	default: break;
	}
}

void GasCan::doSomething()
{
	if (!alive())
		return;

	StudentWorld* world = getWorld();
	int result = 0;
	Actor* temp = world->checkMortalOverlap(getX(), getY(), result, this);
	if (result == 1)
	{
		world->incGoodie(0);
		setAlive(false);
	}
}

void Vaccine::doSomething()
{
	if (!alive())
		return;

	StudentWorld* world = getWorld();
	int result = 0;
	Actor* temp = world->checkMortalOverlap(getX(), getY(), result, this);
	if (result == 1)
	{
		world->incGoodie(1);
		setAlive(false);
	}
}

void LandMine::doSomething()
{
	if (!alive())
		return;

	StudentWorld* world = getWorld();
	int result = 0;
	Actor* temp = world->checkMortalOverlap(getX(), getY(), result, this);
	if (result == 1)
	{
		world->incGoodie(2);
		setAlive(false);
	}
}

void Flame::doSomething()
{
	if (!alive())
		return;

	if (getTick() == 2)
	{
		setAlive(false);
		return;
	}
	incTick();

	getWorld()->checkFlameDamage(this);
}

void Vomit::doSomething()
{
	if (!alive())
		return;

	if (getTick() == 2)
	{
		setAlive(false);
		return;
	}
	incTick();

	getWorld()->checkVomitVictims(getX(), getY());
}

void Mine::doSomething()
{
	if (!alive())
		return;
	
	if (!active())
	{
		decTicks();
		if (getTicks() == 0)
			setActive();
		return;
	}

	StudentWorld* world = getWorld();
	if (world->checkOverlap(getX(), getY(), this))
	{
		setAlive(false);
		world->playSound(SOUND_LANDMINE_EXPLODE);
		world->createFlame(this);
	}
}

void DumbZombie::doSomething()
{
	if (!alive())
		return;

	if (paralysis())
	{
		setParalysis(false);
		return;
	}

	StudentWorld* world = getWorld();
	Direction dir = getDirection();
	double destX = getX(), destY = getY();

	switch (dir)
	{
	case GraphObject::left:
		destX -= SPRITE_WIDTH; break;
	case GraphObject::right:
		destX += SPRITE_WIDTH; break;
	case GraphObject::up:
		destY += SPRITE_HEIGHT; break;
	case GraphObject::down:
		destY -= SPRITE_HEIGHT; break;
	default: break;
	}

	if (world->checkVomitRange(destX, destY))
		if (randInt(1, 3) == 1)
		{
			world->createVomit(destX, destY, dir);
			return;
		}

	if (!getMovementPlan())
	{
		setMovementPlan(randInt(3, 10));
		int direction = randInt(1, 4);
		switch (direction)
		{
		case 1: setDirection(right); break;
		case 2: setDirection(left); break;
		case 3: setDirection(up); break;
		case 4: setDirection(down); break;
		default: break;
		}
	}

	dir = getDirection();
	destX = getX(), destY = getY();
	switch (dir)
	{
	case left:
		destX--; break;
	case right:
		destX++; break;
	case up:
		destY++; break;
	case down:
		destY--; break;
	default: break;
	}

	if (world->checkMovementBlock(destX, destY, this))
	{
		moveTo(destX, destY);
		decMovementPlan();
	}
	else
		setMovementPlan(0);
	setParalysis(true);
}

void SmartZombie::doSomething()
{
	if (!alive())
		return;

	if (paralysis())
	{
		setParalysis(false);
		return;
	}

	StudentWorld* world = getWorld();
	Direction dir = getDirection();
	double destX = getX(), destY = getY();

	switch (dir)
	{
	case GraphObject::left:
		destX -= SPRITE_WIDTH; break;
	case GraphObject::right:
		destX += SPRITE_WIDTH; break;
	case GraphObject::up:
		destY += SPRITE_HEIGHT; break;
	case GraphObject::down:
		destY -= SPRITE_HEIGHT; break;
	default: break;
	}

	if (world->checkVomitRange(destX, destY))
		if (randInt(1, 3) == 1)
		{
			world->createVomit(destX, destY, dir);
			setParalysis(true);
			return;
		}

	if (!getMovementPlan())
	{
		setMovementPlan(randInt(3, 10));
		double closestX = getX(), closestY = getY();
		double dist = getWorld()->closestPerson(closestX, closestY);
		if (dist > 80)
		{
			int direction = randInt(1, 4);
			switch (direction)
			{
			case 1: setDirection(right); break;
			case 2: setDirection(left); break;
			case 3: setDirection(up); break;
			case 4: setDirection(down); break;
			default: break;
			}
		}
		else
		{
			double zombieX = getX(), zombieY = getY();
			if (closestX == zombieX)					//same col
			{
				if (closestY - zombieY < 0)				//zombie to the top
					setDirection(down);
				else
					setDirection(up);					//zombie to the bot
			}
			else if (closestY == zombieY)
			{
				if (closestX - zombieX < 0)				//zombie to the right
					setDirection(right);
				else
					setDirection(left);					//zombie to the left
			}
			else
			{
				int dir = randInt(1, 2);
				if (zombieX < closestX)				//zombie to the left
				{
					if (zombieY < closestY)			//zombie to the botleft
					{
						switch (dir) 
						{
						case 1: setDirection(up); break;
						case 2: setDirection(right); break;
						default: break;
						}
					}
					else
					{								//zombie to the topleft
						switch (dir)
						{
						case 1: setDirection(down); break;
						case 2: setDirection(right); break;
						default: break;
						}
					}
				}
				else								//zombie to the right
				{
					if (zombieY < closestY)			//zombie to the botright
					{
						switch (dir)
						{
						case 1: setDirection(up); break;
						case 2: setDirection(left); break;
						default: break;
						}
					}
					else							//zombie to the topright
					{
						switch (dir)
						{
						case 1: setDirection(down); break;
						case 2: setDirection(left); break;
						default: break;
						}
					}
				}
			}
		}
	}

	dir = getDirection();
	destX = getX(), destY = getY();
	switch (dir)
	{
	case left:
		destX--; break;
	case right:
		destX++; break;
	case up:
		destY++; break;
	case down:
		destY--; break;
	default: break;
	}

	if (world->checkMovementBlock(destX, destY, this))
	{
		moveTo(destX, destY);
		decMovementPlan();
	}
	else
		setMovementPlan(0);

	setParalysis(true);
}

void Citizen::doSomething()
{
	if (!alive())
		return;

	StudentWorld* world = getWorld();
	if (infected())
	{
		if (getInfectedTimer() < 500)
			incInfectionTimer();
		else
		{
			setAlive(false);
			world->citizenDied(getX(), getY());
			world->createZombie(this);
			return;
		}
	}

	if (paralysis())
	{
		setParalysis(false);
		return;
	}

	double penX, penY;
	world->penelopeCoord(penX, penY);
	double citizenX = getX(), citizenY = getY();
	double distZ = world->closestZombie(citizenX, citizenY);
	double distP = distance(penX, penY, citizenX, citizenY);

	if ((distZ == 0 || distP < distZ) && distP <= 80)
	{
		if (citizenX == penX) //same col
		{
			if (citizenY - penY < 0) //penelope to the top
			{
				if (world->checkMovementBlock(citizenX, citizenY + 2, this))
				{
					setDirection(up);
					moveTo(citizenX, citizenY + 2);
					setParalysis(true);
					return;
				}
			}
			else					 //penelope to the bot
			{
				if (world->checkMovementBlock(citizenX, citizenY - 2, this))
				{
					setDirection(down);
					moveTo(citizenX, citizenY - 2);
					setParalysis(true);
					return;
				}
			}
		}
		else if (citizenY == penY) //same row
		{
			if (citizenX - penX < 0)	//penelope to the right
			{
				if (world->checkMovementBlock(citizenX + 2, citizenY, this))
				{
					setDirection(right);
					moveTo(citizenX + 2, citizenY);
					setParalysis(true);
					return;
				}
			}
			else 			//penelope to the left
			{
				if (world->checkMovementBlock(citizenX - 2, citizenY, this))
				{
					setDirection(left);
					moveTo(citizenX - 2, citizenY);
					setParalysis(true);
					return;
				}

			}
		}
		else
		{
			int dir = randInt(1, 2);
			if (citizenX < penX)				//citizen to the left
			{
				if (citizenY < penY)			//citizen to the botleft
				{
					switch (dir)
					{
					case 1:
						if (world->checkMovementBlock(citizenX, citizenY + 2, this))
						{
							setDirection(up);
							moveTo(citizenX, citizenY + 2);
							setParalysis(true);
							return;
						}
					case 2:
						if (world->checkMovementBlock(citizenX + 2, citizenY, this))
						{
							setDirection(right);
							moveTo(citizenX + 2, citizenY);
							setParalysis(true);
							return;
						}
					default: break;
					}
				}
				else
				{								//citizen to the topleft
					switch (dir)
					{
					case 1:
						if (world->checkMovementBlock(citizenX, citizenY - 2, this))
						{
							setDirection(down);
							moveTo(citizenX, citizenY - 2);
							setParalysis(true);
							return;
						}
					case 2:
						if (world->checkMovementBlock(citizenX + 2, citizenY, this))
						{
							setDirection(right);
							moveTo(citizenX + 2, citizenY);
							setParalysis(true);
							return;
						}
					default: break;
					}
				}
			}
			else								//citizen to the right
			{
				if (citizenY < penY)			//citizen to the botright
				{
					switch (dir)
					{
					case 1:
						if (world->checkMovementBlock(citizenX, citizenY + 2, this))
						{
							setDirection(up);
							moveTo(citizenX, citizenY + 2);
							setParalysis(true);
							return;
						}
					case 2:
						if (world->checkMovementBlock(citizenX - 2, citizenY, this))
						{
							setDirection(left);
							moveTo(citizenX - 2, citizenY);
							setParalysis(true);
							return;
						}
					default: break;
					}
				}
				else							//citizen to the topright
				{
					switch (dir)
					{
					case 1:
						if (world->checkMovementBlock(citizenX, citizenY - 2, this))
						{
							setDirection(down);
							moveTo(citizenX, citizenY - 2);
							setParalysis(true);
							return;
						}
					case 2:
						if (world->checkMovementBlock(citizenX - 2, citizenY, this))
						{
							setDirection(left);
							moveTo(citizenX - 2, citizenY);
							setParalysis(true);
							return;
						}
					default: break;
					}
				}
			}
		}
	}

	else if (distZ <= 80)
	{
		double distUp = 0, distDown = 0, distLeft = 0, distRight = 0;	//arbitrary choice to make max fucntion easier

		if (world->checkMovementBlock(getX() - 2, getY(), this)) //left
		{
			double distTemp = world->closestZombie(getX() - 2, getY());
			distLeft = distTemp;
		}
		
		if (world->checkMovementBlock(getX() + 2, getY(), this)) //right
		{
			double distTemp = world->closestZombie(getX() + 2, getY());
			distRight = distTemp;
		}
		
		if (world->checkMovementBlock(getX(), getY() + 2, this)) //up
		{
			double distTemp = world->closestZombie(getX(), getY() + 2);
			distUp = distTemp;
		}
			
		
		if (world->checkMovementBlock(getX(), getY() - 2, this)) //down
		{
			double distTemp = world->closestZombie(getX(), getY() - 2);
			distUp = distTemp;
		}
		
		if (distZ >= distUp && distZ >= distDown && distZ >= distLeft && distZ >= distRight)
		{
			setParalysis(true);
			return;
		}
		else
		{
			if (distUp >= distZ && distUp >= distDown && distUp >= distLeft && distUp >= distRight)//up is best
			{
				setDirection(up);
				moveTo(getX(), getY() + 2);
				setParalysis(true);
				return;
			}
			if (distDown >= distZ && distDown >= distUp && distDown >= distLeft && distDown >= distRight)//down is best
			{
				setDirection(down);
				moveTo(getX(), getY() - 2);
				setParalysis(true);
				return;
			}
			if (distRight >= distZ && distRight >= distUp && distRight >= distDown && distRight >= distLeft)//right is best
			{
				setDirection(right);
				moveTo(getX() + 2, getY());
				setParalysis(true);
				return;
			}
			if (distLeft >= distZ && distLeft >= distUp && distLeft >= distDown && distLeft >= distRight)//left is best
			{
				setDirection(180);
				moveTo(getX() - 2, getY());
				setParalysis(true);
				return;
			}
		}
	}

	setParalysis(true);
}
