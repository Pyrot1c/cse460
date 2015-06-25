/* os.h Ammar Alsibai, Michael Monaghan*/

#ifndef OS_H
#define OS_H

#include "Assembler.h"
#include "VirtualMachine.h"
#include "PCB.h"//dat pcb
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
    list<PCB *> jobs; //pcb jobs
    queue<PCB *> readyQ, waitQ;
    PCB * running;
    int masterClock, contextSwitchClock, idleClock;
    
    
    // internal functions
    void getPCBFromVM(); //get pcb from the vm
    void givePCBToVM(); //give pcb to the vm
    void outputErrorCode(string s);
    bool done();
    
public:
    void start();
    OS();
};

#endif
