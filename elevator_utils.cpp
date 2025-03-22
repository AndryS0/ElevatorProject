#include "elevator_utils.h"

std::string stateToStr(const ElevatorState& state)
{
	switch (state)
	{
	case ElevatorState::STOPPED:
		return "STOPPED";
	case ElevatorState::IDLE:
		return "IDLE";
	case ElevatorState::EMERGENCY_STOPPED:
		return "EMERGENCY_STOPPED";
	case ElevatorState::START_MOVING:
		return "START_MOVING";
	case ElevatorState::IN_MOVE:
		return "IN_MOVE";
	case ElevatorState::DIR_CHANGED:
		return "DIR_CHANGED";
	case ElevatorState::DOOR_OPENED:
		return "DOOR_OPENED";
	default:
		return "UNKNOWN";
	}
}
