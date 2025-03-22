#pragma once
#include <vector>
#include <unordered_set>
#include "ElevatorController.h"

class ElevatorsManager
{
	struct floor_queue
	{
		Direction dir;
		int floor;
		floor_queue(Direction dir, int floor) : dir(dir), floor(floor) {}
		bool operator==(const floor_queue& other)
		{
			return other.dir == this->dir && other.floor == this->floor;
		}
	};
	// this queue is requested and not assigned outside buttons actions
	std::vector<floor_queue> queue;
	std::vector<ElevatorController*> ctrls;
	std::vector<OutsideFloorButton*> outside_btns;
public:
	template<typename ElIt, typename BtnIt>
	ElevatorsManager(ElIt ctrlBegin, ElIt ctrlEnd, 
		BtnIt outsideBtnBegin = nullptr, BtnIt outsideBtnEnd = nullptr) 
#if (__cplusplus >= 202002 || _MSVC_LANG >= 202002) 
		requires IsIteratorOf<ElIt, ElevatorController> and IsIteratorOf<BtnIt, OutsideFloorButton>
#endif
	{
		ctrls.reserve(std::distance(ctrlBegin, ctrlEnd));
		outside_btns.reserve(std::distance(outsideBtnBegin, outsideBtnEnd));
		while (ctrlBegin != ctrlEnd)
		{
			ctrlBegin->subscribeOnElevatorStateChanged([&](const ElevatorControllerUpdate& upd)
				{
					this->onStateUpdated(upd);
				});
			ctrls.push_back(&(*ctrlBegin));
			ctrlBegin++;
		}
		while (outsideBtnBegin != outsideBtnEnd)
		{
			outsideBtnBegin->subscribeOnElevatorStateChanged([&](ElevatorButton* btn)
				{
					auto outsideBtn = static_cast<OutsideFloorButton*>(btn);
					this->onButtonPressed(outsideBtn->floor, outsideBtn->dir);
				});
			outside_btns.push_back(&(*outsideBtnBegin));
			outsideBtnBegin++;
		}
	}
	


protected:
	virtual void onStateUpdated(const ElevatorControllerUpdate&);
	// returns false if assign failed (no candidates found)
	virtual bool assignRequest(const floor_queue& queue);
	virtual void onButtonPressed(int floor, Direction dir);
};

