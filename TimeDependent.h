#pragma once
class TimeDependent
{
public:
	virtual void update(float deltaSeconds) = 0;
};