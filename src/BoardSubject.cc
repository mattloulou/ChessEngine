#include "BoardSubject.h"

void BoardSubject::attach(std::shared_ptr<DisplayObserver> o) 
{
    if (find(observers.begin(), observers.end(), o) == observers.end()) {
        observers.emplace_back(o);
    }
}

void BoardSubject::detach(std::shared_ptr<DisplayObserver> o) 
{
    for (int i = 0; i < static_cast<int>(observers.size()); ++i) {
        if (observers[i] == o) {
            observers.erase(observers.begin() + i);
            return;
        }
    }
}

void BoardSubject::notifyObservers() 
{
    for (std::shared_ptr<DisplayObserver> o : observers) {
        o->notify();
    }
}

void BoardSubject::notifyObserversQuit() 
{
    for (std::shared_ptr<DisplayObserver> o : observers) {
        o->notifyQuit();
    }
}

BoardSubject::~BoardSubject() {} 
