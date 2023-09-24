#ifndef BOARDSUBJECT_H
#define BOARDSUBJECT_H

#include <vector>
#include <memory>
#include "DisplayObserver.h"

class BoardSubject {
    std::vector<std::shared_ptr<DisplayObserver>> observers;

public:
    void attach(std::shared_ptr<DisplayObserver> o);
    void detach(std::shared_ptr<DisplayObserver> o);
    void notifyObservers();
    void notifyObserversQuit();
    virtual ~BoardSubject() = 0;
};

#endif
