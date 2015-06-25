/* OS.h Ammar Alsibai Michael Monaghan */

#ifndef OS_H
#define OS_H

#include "Assembler.h"
#include "VirtualMachine.h"
#include "PCB.h"
#include <list>
#include <queue>
#include <string>
#include <cstring>

using namespace std;

class OS {
private:
    //variables
    Assembler as;
    VirtualMachine vm;
    list<PCB *> jobs;
    queue<PCB *> readyQ, waitQ;
    PCB * running;
    int masterClock, contextSwitchClock, idleClock;

    // internal functions
    void getPCBFromVM();
    void givePCBToVM();
    void outputErrorCode(string s);
    bool done();
    
public:
    void start();
    OS();
};

#endif
