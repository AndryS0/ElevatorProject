#pragma once
#include <vector>
#include "ElevatorController.h"
#include "ElevatorsManager.h"

class CLIElevatorManager
{
	std::vector<ElevatorController*> ctrls;
	std::vector<InsideFloorButton*> inside_btns;
	std::vector<OutsideFloorButton*> outside_btns;
	// list of elevators controllers
	ElevatorsManager& manager;
public:
	CLIElevatorManager(
		std::vector<ElevatorController*>& ctrls,
		ElevatorsManager& mgr,
		std::vector<InsideFloorButton> inside_btns,
		std::vector<OutsideFloorButton> outside_btns
		) : ctrls(ctrls), manager(mgr)
	{

	}
private:
};

