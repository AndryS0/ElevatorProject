#include "ElevatorsManager.h"

void ElevatorsManager::onStateUpdated(const ElevatorControllerUpdate& upd)
{
	// turn off highlight on arrival
	auto dir = upd.controller->getDirection();
	int floor = int(upd.controller->getEngine().getLevel());
	if (upd.state == ElevatorState::DOOR_OPENED) 
	{
		for (auto& i : this->outside_btns) 
		{
			if (i->floor == floor && i->dir == dir) 
			{
				i->disableHighlight();
			}
		}
	}
	for (auto& i : upd.cancelled_requests) 
	{
		for (auto& j : this->outside_btns) 
		{
			if (j->floor == i && dir == j->dir)
			{
				j->disableHighlight();
			}
		}
	}
	
	// on any state changed
	// try to reassign tasks to elevators
	std::vector<floor_queue> to_remove;
	for (const auto& i : queue) 
	{
		if (assignRequest(i)) 
		{
			to_remove.push_back(i);
			break;
		}
	}
	for (const auto& i : to_remove)
	{
		queue.erase(std::remove(queue.begin(), queue.end(), i), queue.end());
	}
}

bool ElevatorsManager::assignRequest(const floor_queue& queue)
{
	int floor = queue.floor;
	Direction callee_dir = queue.dir;

	bool direction_match = false;
	int min_distance = 2 << 15; // just large number
	bool elevator_agreed = false;
	// in this loop we are selecting best elevator based on not important parameters 
	ElevatorController* selected_elevator = nullptr;
	for (int i = 0; i < ctrls.size(); i++)
	{
		const auto& elevator = ctrls[i];
		// important parameters such as floor direction match are filtered in "can_take"
		if (elevator->canTakeRequest(queue.floor, queue.dir))
		{
			// at this point at least one elevator agreed
			elevator_agreed = true;
			bool approved = false;
			bool temp_dir_match = elevator->getDirection() == callee_dir;
			int distance = elevator->getDistance(floor);
			// if target guy is going in the same direction as elevator, we assign this elevator as best
			if (!direction_match && temp_dir_match)
			{
				approved = true;
			}
			// if target guy is nearer than selected elevator, we also assign it
			else if (distance < min_distance && temp_dir_match)
			{
				approved = true;
			}
			else if (distance < min_distance && !direction_match)
			{
				approved = true;
			}
			// if current elevator is chosen like best, make it candidate
			if (approved) {
				min_distance = distance;
				direction_match = temp_dir_match;
				selected_elevator = elevator;
			}
		}
	}
	// give to best candidate floor and remove it from queue
	if (elevator_agreed) {
		selected_elevator->takeRequest(queue.floor);
		return true;
	}
	return false;
}

void ElevatorsManager::onButtonPressed(int floor, Direction dir)
{
	floor_queue task(dir, floor);
	if (!this->assignRequest(task))
	{
		// if failed to assign for someone, enqueue
		// if it already exists, there is no need to enqueue
		if (std::find(queue.begin(), queue.end(), task) == queue.end())
			this->queue.push_back(task);
	}
}
