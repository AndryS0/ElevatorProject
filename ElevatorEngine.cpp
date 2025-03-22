#include "ElevatorEngine.h"
#include <cassert>

ElevatorEngine::ElevatorEngine(float starting_floor, int max_floor) : level(starting_floor), max_floor(max_floor)
{
	assert(starting_floor <= max_floor);
	assert(elevator_speed > 0.f);
}

void ElevatorEngine::stop()
{
	this->stopped = true;
	this->onStateChanged(int(this->level));
}

void ElevatorEngine::liftToFloor(int target_floor)
{
	assert(level < target_floor);
	target_level = float(target_floor);
	this->dir = Direction::UP;
	this->stopped = false;
}

void ElevatorEngine::lowerToFloor(int target_floor)
{
	assert(level > target_floor);
	target_level = float(target_floor);
	this->dir = Direction::DOWN;
	this->stopped = false;
}
#include <iostream>
void ElevatorEngine::update(float deltaSeconds)
{
	if (this->stopped) return;
	int old_level = int(this->level);
	if (dir == Direction::UP)
	{
		this->level += deltaSeconds * this->elevator_speed;
		// reached target floor
		if (this->target_level < this->level)
		{
			this->level = this->target_level;
			this->stop();
			return;
		}
	}
	else
	{
		this->level -= deltaSeconds * this->elevator_speed;
		// reached target floor
		if (this->target_level > this->level)
		{
			this->level = this->target_level;
			this->stop();
			return;
		}
	}
	// just a new floor, keep moving
	if (old_level != int(level))
	{
		this->onStateChanged(int(this->level));
		return;
	}
}
