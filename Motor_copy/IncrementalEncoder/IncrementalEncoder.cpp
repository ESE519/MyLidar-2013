#include "IncrementalEncoder.h"
#include "mbed.h"

IncrementalEncoder::IncrementalEncoder(PinName pin, int id):  _lastTicks(0),  _ticks(0), _interrupt(pin), _Tlast(0), _Tnow(500), _ID(id)
{
    _interrupt.mode(PullDown); // default is pulldown but my encoder board uses a pull-up and that just don't work
    _interrupt.rise(this, &IncrementalEncoder::_increment);
    //_interrupt.fall(this, &IncrementalEncoder::_increment_f);
    timer.start();
}

unsigned long IncrementalEncoder::read()
{
// disable interrupts?
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
    if ((2000.0/ (_Tnow - _Tlast))>40){
    return 0;
    }
    else
    return (2000.0/ (_Tnow - _Tlast));
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

        _Tlast = _Tnow;
        _Tnow = timer.read_ms();
        //printf("%d %f\r\n",_Tnow-_Tlast,2000.0/ (_Tnow - _Tlast));
       // printf("id :%d  %d\r\n",_ID ,_ticks);
}

