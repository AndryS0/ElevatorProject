#pragma once
#include <functional>
#include <vector>
#include "elevator_utils.h"
#include "StateNotifier.h"

class ElevatorButton : public StateNotifier<ElevatorButton*>
{
public:
	virtual void press() 
	{
		this->onStateChanged(this);
	}

};

class HighlightableButton : public ElevatorButton
{
protected:
	bool highlighted;

public:
	HighlightableButton() : highlighted(false) {}
	virtual void press() override
	{
		ElevatorButton::press();
		highlighted = true;
	}
	void disableHighlight() { highlighted = false; }
	bool isHighlighted() { return highlighted; }
};

class InsideFloorButton : public HighlightableButton
{
public:
	int floor;
	InsideFloorButton(int floor = -1) : HighlightableButton(), floor(floor) {}
};


class OutsideFloorButton : public InsideFloorButton
{
public:
	Direction dir;
	OutsideFloorButton(int floor = -1, Direction dir = Direction::DOWN) : InsideFloorButton(floor), dir(dir) {}
};