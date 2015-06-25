/*Ammar Alsibai Michael Monaghan OS.h*/

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

class OS
{
private:
    //variables
    Assembler as;
    VirtualMachine vm;
    list<PCB *> jobs;
    queue<PCB *> readyQ, waitQ;
    PCB * running;
    int contextSwitchClock, idleClock;
    int MULTIPROGCAP;
    list<int> unusedFrames;
    fstream readProgs;
    bool lessThanMultiProgCap;
    int pageReplaced;

    // internal functions
    void getPCBFromVM();
    void givePCBToVM();
    void outputErrorCode(string s);
    bool done();
    void readIn(string line);
    void replaceAlgorithm();


public:
    void start(int lruValue);
    OS();
    ~OS();
};

#endif
