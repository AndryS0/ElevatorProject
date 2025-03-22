#include "ElevatorController.h"
#include <cassert>

// utility method just for smaller codesize in ::canTakeRequest
// returns -1 if there is no destinations
int ElevatorController::getNearestDestinationFloor() const
{
	if (target_floors.empty())
	{
		return -1;
	}
	int current_floor = int(this->engine.getLevel());
	int nearest_floor = -1;
	for (size_t i = 0; i < target_floors.size(); i++) 
	{
		// if target floor is by direction
		// and there's no nearest target floor at this moment
		// nearest floor equals to this one
		if (dir == Direction::UP)
		{
			if (target_floors[i] > current_floor &&
				(nearest_floor == -1 || nearest_floor > target_floors[i]))
			{
				nearest_floor = target_floors[i];
			}
		}
		else
		{
			if (target_floors[i] < current_floor &&
				(nearest_floor == -1 || nearest_floor < target_floors[i]))
			{
				nearest_floor = target_floors[i];
			}
		}
	}
	return nearest_floor;
}

void ElevatorController::pushUpdate(
	bool emergency,
	bool dir_changed,
	bool started_moving,
	std::vector<int> cancelled_requests)
{
	ElevatorState state;
	if (emergency)
	{
		state = ElevatorState::EMERGENCY_STOPPED;
	}
	else if (this->getDoor().isOpen())
	{
		state = ElevatorState::DOOR_OPENED;
	}
	else if (dir_changed)
	{
		state = ElevatorState::DIR_CHANGED;
	}
	else if (this->isIdle())
	{
		state = ElevatorState::IDLE;
	}
	else if (this->getEngine().isStopped())
	{
		state = ElevatorState::STOPPED;
	}
	else if (started_moving)
	{
		state = ElevatorState::START_MOVING;
	}
	else
	{
		state = ElevatorState::IN_MOVE;
	}
	auto update = ElevatorControllerUpdate(this, state, cancelled_requests);
	this->onStateChanged(update);
}

// used by manager to check if it's okay to assign some controller the floor
// for efficiency we "can take" request only if: 
// 1. the direction is match or we stopped
// 2. there's no closer destination to us 
// (for example if we are checking floor 5 and going to floor 3, then "can take" returns false)
// it is also possible to add support of elevator overloading, which will ignore external requests until it empties
bool ElevatorController::canTakeRequest(int floor, Direction dir) const
{
	// float-to-int conversation is trunc implementation defined
	// exactly what we needed, so let's ignore some cobersome std::trunc() calls
	int current_floor = int(this->engine.getLevel());
	// if we are idle, then we are surely can take request
	if (this->isIdle())
	{
		return true;
	}
	// let's ignore this request if dir is no match
	// we don't want to force people travelling up-down whithout any point
	if (this->dir != dir) return false;

	int nearest_dest = this->getNearestDestinationFloor();
	
	if ((this->dir == Direction::UP && nearest_dest < floor) ||
		(this->dir == Direction::DOWN && nearest_dest > floor))
	{
		// we have a closer destination already
		return false;
	}
	// likely we can take this request
	return true;
}

// plans stop for the elevator
// this method does not check anything, just forcing to add stop destination
// if it's okay to assign this destination check ::canTake method
void ElevatorController::takeRequest(int floor)
{
	assert(floor > 0 && floor <= this->floors);
	if (this->isIdle())
	{
		int current_floor = int(this->engine.getLevel());
		// we are already on the target floor, so just open the door
		if (current_floor == floor) 
		{
			this->onOpenButtonPressed();
			return;
		}
	}
	this->target_floors.push_back(floor);
	this->goNextTarget();
}

void ElevatorController::onDoorStateChanged(bool open)
{
	assert(engine.isStopped() || (open && engine.isStopped()));
	// if door closed
	if (!open) {
		if (!this->isIdle())
			this->goNextTarget();
		else
			this->pushUpdate();
	}
	else 
	{
		this->pushUpdate();
	}
}

bool ElevatorController::isIdle() const
{
	return this->engine.isStopped() && this->target_floors.empty();
}

void ElevatorController::onInsideFloorButtonPressed(int floor)
{
	this->takeRequest(floor);
}

void ElevatorController::onStopButtonPressed()
{
	this->engine.stop();
	std::vector<int> cancelled_requests = target_floors;
	this->target_floors.clear();
	this->pushUpdate(true, false, false, cancelled_requests);
}

void ElevatorController::onCloseButtonPressed()
{
	this->door.close();
}

void ElevatorController::onOpenButtonPressed()
{
	this->door.open();
}

void ElevatorController::goNextTarget()
{
	// we cant do it while doors are open
	if (this->door.isOpen()) return;

	if (!this->target_floors.empty())
	{
		int newTarget = this->getNearestDestinationFloor();
		// if newTarget is -1 and we still have target_floors it's likely our destination does not match
		bool dir_changed = false;
		if (newTarget == -1)
		{
			this->dir = this->dir == Direction::UP ? Direction::DOWN : Direction::UP;
			newTarget = this->getNearestDestinationFloor();
			assert(newTarget != -1);
			dir_changed = true;
		}
		assert(newTarget <= engine.getMaxFloor());

		engine.getLevel() > newTarget ? engine.lowerToFloor(newTarget) : engine.liftToFloor(newTarget);
		this->pushUpdate(false, dir_changed, true, {});
	}
}

void ElevatorController::onElevatorStateChanged(int floor)
{
	if (this->engine.isStopped())
	{
		if (this->inside_buttons.count(floor) > 0)
			this->inside_buttons.at(floor)->disableHighlight();
		if (!target_floors.empty())
			target_floors.erase(std::remove(target_floors.begin(), target_floors.end(), floor), target_floors.end());
		this->door.open();
	}
	else
	{
		// we are just passing by another floor
		this->pushUpdate();
	}

}
