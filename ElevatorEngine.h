#pragma once
#include <iostream>
#include "StateNotifier.h"
#include "TimeDependent.h"
#include "elevator_utils.h"

// sends state update on every new floor and stop/start moving action
class ElevatorEngine : public StateNotifier<int>, public TimeDependent
{
	float level;
	bool stopped = true;
	const float elevator_speed = 1 / 2.f; // constant speed of elevator engine, floors per second
	float target_level = 0.f;
	int max_floor;
	Direction dir = Direction::UP;
public:
	ElevatorEngine(float starting_floor, int max_floor);
	virtual void stop();
	virtual void liftToFloor(int floor);
	virtual void lowerToFloor(int floor);
	virtual bool isStopped() const { return this->stopped; };
	// in real world controller must count floors, 
	// but to make some "simulation" we need to know when to stop,
	virtual float getLevel() const { return level; };
	int getMaxFloor() const { return max_floor; }
	virtual void update(float deltaSeconds) override;
	friend std::ostream& operator<<(std::ostream& os, const ElevatorEngine& e)
	{
		os << "\nEngine:\n" << " - level="
			<< e.level << "\n - stopped=" 
			<< e.stopped << "\n - target_level=" 
			<< e.target_level << "\n - dir=" 
			<< (e.dir == Direction::UP ? "UP\n" : "DOWN\n");
		return os;
	}
};

