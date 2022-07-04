#ifndef _IEVENTLISTENER_H_
#define _IEVENTLISTENER_H_

#include "IEvent.h"

class IEventListener
{
private:
    /* data */
public:
    IEventListener(/* args */) {}
    virtual ~IEventListener() {}

    virtual void operator()(IEvent &event) = 0;
};

#endif