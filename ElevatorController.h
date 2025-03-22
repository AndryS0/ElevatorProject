#pragma once
#include <initializer_list>
#include <type_traits>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include "elevator_utils.h"
#include "ElevatorButton.h"
#include "ElevatorDoor.h"
#include "ElevatorEngine.h"
class ElevatorController;
// TODO: implement update
struct ElevatorControllerUpdate;


// TODO: exatract interface

class ElevatorController : public StateNotifier<const ElevatorControllerUpdate&>
{
	const int floors;
	ElevatorDoor& door;
	ElevatorEngine& engine;
	// map from floor to button pointer
	// used to disable highlighting of buttons on elevator stop
	std::unordered_map<int, InsideFloorButton*> inside_buttons;
	// this vector contains list of target floors locations
	std::vector<int> target_floors;
	// by default we are stopped, so direction does not really affect anything
	Direction dir = Direction::UP; 
public:
	
	// insideFloorButtonBegin/end is button array for floors
	template<typename It>
	ElevatorController(
		int floors,
		ElevatorDoor& door,
		ElevatorEngine& engine,
		It insideFloorButtonBegin = nullptr,
		It insideFloorButtonEnd = nullptr,
		ElevatorButton* stop = nullptr,
		ElevatorButton* close = nullptr,
		ElevatorButton* open = nullptr
	) 
#if (__cplusplus >= 202002 || _MSVC_LANG >= 202002)
		requires IsIteratorOf<It, InsideFloorButton>
#endif
	: floors(engine.getMaxFloor()), door(door), engine(engine)
	{
		// setting up button bindings
		while (insideFloorButtonBegin != insideFloorButtonEnd && floors--) 
		{
			insideFloorButtonBegin->subscribeOnElevatorStateChanged([&](ElevatorButton* btn)
				{
					this->onInsideFloorButtonPressed(static_cast<InsideFloorButton*>(btn)->floor);
				}
			);
			inside_buttons[insideFloorButtonBegin->floor] = &(*insideFloorButtonBegin);
			insideFloorButtonBegin++;
		}

		this->engine.subscribeOnElevatorStateChanged([&](int floor) 
			{
				this->onElevatorStateChanged(floor);
			});
		this->door.subscribeOnElevatorStateChanged([&](bool open) 
			{
				this->onDoorStateChanged(open);
			});

		if (floors) 
			std::cerr << "WARNING: not all buttons assigned to floors\n";
		if (stop)
			stop->subscribeOnElevatorStateChanged([&](ElevatorButton* btn)
				{
					this->onStopButtonPressed();
				});
		if (close)
			close->subscribeOnElevatorStateChanged([&](ElevatorButton* btn)
				{
					this->onCloseButtonPressed();
				});
		if (open)
			open->subscribeOnElevatorStateChanged([&](ElevatorButton* btn)
				{
					this->onOpenButtonPressed();
				});
		
	}
	
	// returns true if current elevator can take request (i.e. no other requests nearer and direction is match)
	virtual bool canTakeRequest(int floor, Direction dir) const;
	virtual bool isIdle() const;
	const ElevatorEngine getEngine() const {return this->engine;}
	const ElevatorDoor getDoor() const { return this->door; }
	const Direction getDirection() const { return this->dir; }
	const int getDistance(int floor) const { return abs(floor - int(this->engine.getLevel())); }
	// plans this elevator to be stopped at the following floor
	virtual void takeRequest(int floor);
protected:
	virtual void onDoorStateChanged(bool open);
	// pressed inside floor button, we must take such request
	virtual void onInsideFloorButtonPressed(int floor); // think about how button should be unpressed
	// pressed "stop" button inside elevator
	virtual void onStopButtonPressed();
	// pressed "close" button inside elevator
	virtual void onCloseButtonPressed();
	// pressed "open" button inside elevator
	virtual void onOpenButtonPressed();
	// returns false if no targets
	virtual void goNextTarget();

	virtual void onElevatorStateChanged(int floor);
private:
	int getNearestDestinationFloor() const;
	void pushUpdate(bool emergency = false,
		bool dir_changed = false,
		bool started_moving = false,
		std::vector<int> cancelled_requests = {});
};
struct ElevatorControllerUpdate
{
	const ElevatorController* controller;

	// list of requests cancelled due to emergency stop
	std::vector<int> cancelled_requests;
	ElevatorState state;
	ElevatorControllerUpdate(
		const ElevatorController* ctrl,
		ElevatorState state,
		std::vector<int> cancelled_requests = {})
		: controller(ctrl), cancelled_requests(cancelled_requests), state(state)
	{}
};
