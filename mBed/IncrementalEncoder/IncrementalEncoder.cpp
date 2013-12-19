#include "IncrementalEncoder.h"
#include "mbed.h"

IncrementalEncoder::IncrementalEncoder(PinName pin, int id):  _lastTicks(0),  _ticks(0), _totalTicks(0), _interrupt(pin), _Tlast(0), _Tnow(500), _ID(id)
{
    _interrupt.mode(PullNone); 
    _interrupt.rise(this, &IncrementalEncoder::_increment);
    timer.start();
}

unsigned long IncrementalEncoder::read()
{
    unsigned int ticks = _ticks - _lastTicks;
    _lastTicks = _ticks;
    return ticks;
}

unsigned long IncrementalEncoder::readTotal()
{
    return _ticks;
}

float IncrementalEncoder::getSpeed()
{
    // 4000 is an arbitrary number, it doesn't matter if you don't care the true speed.
    if ((4000.0/ (_Tnow - _Tlast))>900) {
        return 0;
    } else
        return (4000.0/ (_Tnow - _Tlast));
}

unsigned long IncrementalEncoder::getPeriod()
{
    return (_Tnow - _Tlast);
}

void IncrementalEncoder::reset()
{
    _ticks = _lastTicks = 0;
    _Tlast = 0;
    _Tnow = 0;
}


void IncrementalEncoder::_increment()
{
    unsigned int now = timer.read_ms();

    _ticks++;
    _totalTicks ++;
    _Tlast = _Tnow;
    _Tnow = timer.read_ms();
    printf("%d %f\r\n",_Tnow-_Tlast,4000.0/ (_Tnow - _Tlast));
}

unsigned long IncrementalEncoder::getPulse()
{
    return _totalTicks;
}
