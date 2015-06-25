/*Ammar Alsibai Michael Monaghan VirtualMachine.h*/

#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <list>
#include "PCB.h"
#include <cstring>


using namespace std;

class format1
{
public:
    unsigned UNUSED:6;
    unsigned RS:2;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
};

class format2
{
public:
    unsigned ADDR:8;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
};

class format3
{
public:
    int CONST:8;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
};

union instruction
{
    int i;
    format1 f1;
    format2 f2;
    format3 f3;
};

class VirtualMachine
{
private:
    //typedefs
    typedef void(VirtualMachine::*FP)();

    // Variables
    static const int REG_FILE_SIZE = 4;
    static const int MEM_SIZE = 256;
    static const int FRAME_SIZE = 8;
    vector < int > reg;
    vector < int > mem;
    instruction finalcode;
    int pc, ir, sr, sp, base, limit, clock, nextMemoryLocation;
    map<int, FP> instr;
    string inputFileName, readFileName, writeFileName, originalFileName;
    bool execute;
    vector <int> frames;
    vector <int> valid;
    vector <int> modified;
    vector <int> accessRegisters;
    bool lru;
    vector <string> inverted;
    int pageNumberToGet;
    int masterClock;
    list<PCB *> * osJobs;


// Operations

    void setCarryBit();
    void setRightCarryBit();
    void setOverflow();
    int getCarryBit();
    void setGreaterThan();
    int getGreaterThan();
    void resetGreaterThan();
    void setLessThan();
    int getLessThan();
    void resetLessThan();
    void setEqualTo();
    int getEqualTo();
    void resetEqualTo();
    void returnCode(int i);
    void readOrWrite(int i);
    int pageNumber(int addr);
    int offsetNumber(int addr);
    bool isItInMemory(int ADDR);
    int physicalToLogical(int ADDR);

// Instruction Functions

    void load();
    void loadi();
    void store();
    void add();
    void addi();
    void addc();
    void addci();
    void sub();
    void subi();
    void subc();
    void subci();
    void _and();
    void andi();
    void _xor();
    void xori();
    void _compl();
    void shl();
    void shla();
    void shr();
    void shra();
    void compr();
    void compri();
    void getstat();
    void putstat();
    void jump();
    void jumpl();
    void jumpe();
    void jumpg();
    void call();
    void _return();
    void read();
    void write();
    void halt();
    void noop();

public:
    VirtualMachine();
    void run();
    int loadIntoMemory(string originalFileName, int pageNumber, int frameNumber, int * progBase, int * progLimit);
    void copyFromMemory(int frame, list<PCB *> *jobs);
    friend class OS;
};

#endif
