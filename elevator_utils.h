#pragma once
#include <type_traits>
#include <string>

enum class Direction
{
	UP, DOWN
};
enum class ElevatorState
{
	STOPPED,
	IDLE,
	EMERGENCY_STOPPED,
	START_MOVING,
	IN_MOVE,
	DIR_CHANGED, // means when we are started moving into another direction
	DOOR_OPENED // when door is closed it's IDLE or START_MOVING state, so we don't need separate one
};

std::string stateToStr(const ElevatorState& state);

#if (__cplusplus >= 202002 || _MSVC_LANG >= 202002)
template<typename It, typename Of>
concept IsIteratorOf = std::same_as<typename std::iterator_traits<It>::value_type, Of>;
#endif
// using in command parser
template <class ContainerT>
void split(const std::string& str, ContainerT& tokens,
	const std::string& delimiters = " ", bool trimEmpty = false)
{
	std::string::size_type pos, lastPos = 0, length = str.length();

	using value_type = typename ContainerT::value_type;
	using size_type = typename ContainerT::size_type;

	while (lastPos < length + 1)
	{
		pos = str.find_first_of(delimiters, lastPos);
		if (pos == std::string::npos)
			pos = length;

		if (pos != lastPos || !trimEmpty)
			tokens.emplace_back(value_type(str.data() + lastPos,
				(size_type)pos - lastPos));

		lastPos = pos + 1;
	}
}