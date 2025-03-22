#pragma once
#include <functional>
#include <vector>
template <typename T>
class StateNotifier
{
	// void onStateChange(bool closed)
	using handler_type = std::function<void(T)>;
	std::vector<handler_type> handlers;
public:
	void subscribeOnElevatorStateChanged(handler_type const& handler) 
	{
		this->handlers.push_back(handler);
	}
	void onStateChanged(T state)
	{
		for (const auto& i : handlers) {
			i(state);
		}
	}
};
