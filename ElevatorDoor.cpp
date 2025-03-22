#include "ElevatorDoor.h"

bool ElevatorDoor::close()
{
    if (_open) {
        _open = false;
        this->onStateChanged(_open);
    }
    return true;
}

bool ElevatorDoor::open()
{
    if (!_open) {
        _open = true;
        this->onStateChanged(_open);
    }
    return true;
}

void ElevatorDoor::resetOpenTimer()
{
    if (this->_open)
        this->time_opened = 0.f;
}

void ElevatorDoor::update(float seconds)
{
    if (_open) 
    {
        this->time_opened += seconds;
        if (this->auto_close_time < this->time_opened)
        {
            this->close();
            this->time_opened = 0.f;
        }
    }
}
