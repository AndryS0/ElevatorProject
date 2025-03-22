#pragma once
#include <iostream>
#include "StateNotifier.h"
#include "TimeDependent.h"

class ElevatorDoor : public StateNotifier<bool>, public TimeDependent
{
	bool _open = false;
	float time_opened = 0.f;
	const float auto_close_time = 10.f;
public:
	ElevatorDoor() {}
	// plans to close the door
	virtual bool close();
	// plans to open the door
	virtual bool open();
	virtual void resetOpenTimer();
	// returns if door is currently opened
	bool isOpen() const { return _open; }
	void update(float) override;
	friend std::ostream& operator<<(std::ostream& os, const ElevatorDoor& d)
	{
		os << "\nDoor:\n" << " - open="
			<< d._open << "\n - time_opened="
			<< d.time_opened << "\n";
		return os;
	}
};

