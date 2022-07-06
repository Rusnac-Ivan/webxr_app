#ifndef _IEVENTSOURCE_H_
#define _IEVENTSOURCE_H_

#include "IEventListener.h"
#include <list>

class IEventSource
{
protected:
    std::list<IEventListener *> mEventListeners;

public:
    IEventSource(/* args */) {}
    virtual ~IEventSource() {}

    virtual void AddListener(IEventListener *listener) = 0;
    virtual void RemoveListener(IEventListener *listener) = 0;
    virtual void Notify(IEvent &event) = 0;
};

#endif