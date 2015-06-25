/* VirtualMachine.cpp Ammar Alsibai Michael Monaghan */

#include "VirtualMachine.h"

VirtualMachine::VirtualMachine()
{
    reg = vector <int> (REG_FILE_SIZE);
    mem = vector <int> (MEM_SIZE);
    execute = true;
    nextMemoryLocation = 0;

    instr[0] = &VirtualMachine::load;
    instr[1] = &VirtualMachine::store;
    instr[2] = &VirtualMachine::add;
    instr[3] = &VirtualMachine::addc;
    instr[4] = &VirtualMachine::sub;
    instr[5] = &VirtualMachine::subc;
    instr[6] = &VirtualMachine::_and;
    instr[7] = &VirtualMachine::_xor;
    instr[8] = &VirtualMachine::_compl;
    instr[9] = &VirtualMachine::shl;
    instr[10] = &VirtualMachine::shla;
    instr[11] = &VirtualMachine::shr;
    instr[12] = &VirtualMachine::shra;
    instr[13] = &VirtualMachine::compr;
    instr[14] = &VirtualMachine::getstat;
    instr[15] = &VirtualMachine::putstat;
    instr[16] = &VirtualMachine::jump;
    instr[17] = &VirtualMachine::jumpl;
    instr[18] = &VirtualMachine::jumpe;
    instr[19] = &VirtualMachine::jumpg;
    instr[20] = &VirtualMachine::call;
    instr[21] = &VirtualMachine::_return;  
    instr[22] = &VirtualMachine::read;
    instr[23] = &VirtualMachine::write;
    instr[24] = &VirtualMachine::halt;
    instr[25] = &VirtualMachine::noop;
}

void VirtualMachine::loadIntoMemory(string inputFileName, int * progBase, int * progLimit)
{
 // read into memory, ++
    fstream readIntoMemory;
    readIntoMemory.open(inputFileName.c_str(), ios::in);
	
    if (!readIntoMemory.is_open()) {
        cout << inputFileName << " failed to open.\n";
    }
 
    *progBase = nextMemoryLocation;

    string line;
    getline(readIntoMemory, line);
    
    while (!readIntoMemory.eof()) {
        mem[nextMemoryLocation++] = atoi(line.c_str());
        getline(readIntoMemory, line);
    }
    
    *progLimit = nextMemoryLocation - *progBase;
    
    readIntoMemory.close();
}

void VirtualMachine::run() {

    clock = 0;
    execute = true;
    sr &= 0xffffff1f;	    

    // execute loop
    while (execute) {  
     ir = mem[pc++];
        finalcode.i = ir;
        (this->*instr[finalcode.f1.OP])();
        if(clock >= 15)
            execute = false;
    }

} 

void VirtualMachine::setCarryBit() 
{
    int temp = reg[finalcode.f1.RD] & 0x10000; // grab only 17th bit
    temp = temp >> 17; // shift right 17
    
    if (sr & 0x1) // check if sr carry bit is 1
        sr -= 1;
    
    sr += temp; // add current carry value 
}

void VirtualMachine::setRightCarryBit()
{
    if (sr & 0x1) // check if sr carry bit is 1
        sr -= 1;
    
    sr += 1; // add current carry value 
}

int VirtualMachine::getCarryBit() 
{
    return (sr & 0x1);
}

void VirtualMachine::setOverflow() 
{
    if (!(sr & 0x10))
        sr += 16;
}

void VirtualMachine::setGreaterThan()
{
    if (!(sr & 0x2))
        sr += 2;
}

int VirtualMachine::getGreaterThan()
{
    int temp = sr & 0x2;
    temp = temp >> 1;
    return temp;
}

void VirtualMachine::resetGreaterThan()
{
    sr = sr & 0xfffffffd;
}

void VirtualMachine::setLessThan()
{
    if (!(sr & 0x8))
        sr += 8;
}

int VirtualMachine::getLessThan()
{
    int temp = sr & 0x8;
    temp = temp >> 3;
    return temp;
}

void VirtualMachine::resetLessThan()
{
    sr = sr & 0xfffffff7;
}

void VirtualMachine::setEqualTo()
{
    if (!(sr & 0x4))
        sr += 4;
}

int VirtualMachine::getEqualTo()
{
    int temp = sr & 0x4;
    temp = temp >> 2;
    return temp;
}

void VirtualMachine::resetEqualTo()
{
    sr = sr & 0xfffffffb;
}

void VirtualMachine::load() 
{
    if (finalcode.f2.I == 0) {

        if (finalcode.f2.ADDR + base >= base + limit || finalcode.f2.ADDR + base < base) {
         returnCode(2);
            execute = false;
         return;
        }

        clock += 4;
   
        reg[finalcode.f2.RD] = mem[finalcode.f2.ADDR + base];
    }
    else
        loadi();
}

void VirtualMachine::loadi() 
{
    clock += 1;
    
    reg[finalcode.f3.RD] = finalcode.f3.CONST;
    

    
}

void VirtualMachine::store() 
{
    if (finalcode.f2.ADDR + base >= base + limit || finalcode.f2.ADDR + base < base) {
        returnCode(2);
        execute = false;
        return;
    }
 
    clock += 4;

    mem[finalcode.f2.ADDR + base] = reg[finalcode.f2.RD];
}

void VirtualMachine::add() 
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {
        
        int temp = reg[finalcode.f1.RD] + reg[finalcode.f1.RS]; 

        
        if (reg[finalcode.f1.RD] >= 0 && reg[finalcode.f1.RS] >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && reg[finalcode.f1.RS] < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
        

    }
    else
        addi();
}

void VirtualMachine::addi() 
{
    clock += 1;
     
    int temp = reg[finalcode.f3.RD] + finalcode.f3.CONST;
        
    if (reg[finalcode.f3.RD] >= 0 && finalcode.f3.CONST >= 0 && temp < 0)
        setOverflow();
    else if (reg[finalcode.f3.RD] < 0 && finalcode.f3.CONST < 0 && temp >= 0)
        setOverflow();
         
    reg[finalcode.f3.RD] = temp;
    setCarryBit(); 
}


void VirtualMachine::addc() 
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {
    
        int carry = getCarryBit();      
        int temp = reg[finalcode.f1.RD] + reg[finalcode.f1.RS] + carry; 
        
        if (reg[finalcode.f1.RD] >= 0 && reg[finalcode.f1.RD] >= 0 &&  carry >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && reg[finalcode.f1.RD] < 0 && carry < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
    }
    else
        addci();
}

void VirtualMachine::addci() 
{
    clock += 1;
    
    int carry = getCarryBit();     
    int temp = reg[finalcode.f3.RD] + finalcode.f3.CONST + carry;
        
    if (reg[finalcode.f3.RD] >= 0 && finalcode.f3.CONST >= 0 &&  carry >= 0 && temp < 0)
        setOverflow();
    else if (reg[finalcode.f3.RD] < 0 && finalcode.f3.CONST < 0 && carry < 0 && temp >= 0)
        setOverflow();
            
    reg[finalcode.f3.RD] = temp;
    setCarryBit(); 
}

void VirtualMachine::sub() 
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {
        
        int temp = reg[finalcode.f1.RD] - reg[finalcode.f1.RS];
        int complRS = ~reg[finalcode.f1.RS] + 1;
        
        if (reg[finalcode.f1.RD] >= 0 && complRS >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && complRS < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
    }
    else
        subi();
}

void VirtualMachine::subi() 
{
    clock += 1;

        int temp = reg[finalcode.f1.RD] - finalcode.f3.CONST;
        int complCONST = ~finalcode.f3.CONST + 1;
        
        if (reg[finalcode.f1.RD] >= 0 && complCONST >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && complCONST < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit();
}

void VirtualMachine::subc() 
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {

        int temp = reg[finalcode.f1.RD] - reg[finalcode.f1.RS] - getCarryBit();
        int complRS = ~reg[finalcode.f1.RS] + 1;
        
        if (reg[finalcode.f1.RD] >= 0 && complRS >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && complRS < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
    }
    else
        subci();
}

void VirtualMachine::subci() 
{
    clock += 1;

    int temp = reg[finalcode.f1.RD] - finalcode.f3.CONST - getCarryBit();
    int complRS = ~finalcode.f3.CONST + 1;
        
    if (reg[finalcode.f1.RD] >= 0 && complRS >= 0 && temp < 0)
        setOverflow();
    else if (reg[finalcode.f1.RD] < 0 && complRS < 0 && temp >= 0)
        setOverflow();
            
    reg[finalcode.f1.RD] = temp;
    setCarryBit(); 
}

void VirtualMachine::_and()
{
    clock += 1;
    
    if (finalcode.f1.I == 0) 
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] & reg[finalcode.f1.RS];  
    else
        andi();
}

void VirtualMachine::andi()
{
    clock += 1;
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] & finalcode.f3.CONST;
}


void VirtualMachine::_xor() 
{
    clock += 1;
    
    if (finalcode.f1.I == 0)
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] ^ reg[finalcode.f1.RS]; 
    else
        xori();
}

void VirtualMachine::xori() 
{
    clock += 1; 
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] ^ finalcode.f3.CONST;
}


void VirtualMachine::_compl() 
{
    clock += 1;
    reg[finalcode.f1.RD] = ~reg[finalcode.f1.RD];
}

void VirtualMachine::shl() 
{
    clock += 1;
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] << 1;
    setCarryBit();
}

void VirtualMachine::shla() 
{
    clock += 1;
    if (reg[finalcode.f1.RD] < 0) {
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] << 1; 
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] | 0xffff8000;
    }
    else
       reg[finalcode.f1.RD] = reg[finalcode.f1.RD] << 1;
    
    setCarryBit();
}


void VirtualMachine::shr() 
{
    clock += 1;
    int temp = reg[finalcode.f1.RD];
    temp = temp & 0x1;
    
    if (temp)
        setRightCarryBit();
    
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] >> 1;   
}

void VirtualMachine::shra() 
{
    clock += 1;
    int temp = reg[finalcode.f1.RD];
    temp = temp & 0x1;
    
    if (temp)
        setRightCarryBit();
     
    if (reg[finalcode.f1.RD] < 0) {
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] >> 1; 
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] | 0xffff8000;
    }
    else
       reg[finalcode.f1.RD] = reg[finalcode.f1.RD] >> 1;   
}

void VirtualMachine::compr() 
{
    clock += 1;
    if (finalcode.f1.I == 0) {
        
        if (reg[finalcode.f1.RD] < reg[finalcode.f1.RS]) {
            setLessThan();
            resetEqualTo();
            resetGreaterThan();
        }
        else if (reg[finalcode.f1.RD] > reg[finalcode.f1.RS]) {
            setGreaterThan();
            resetEqualTo();
            resetLessThan();
        }
        else {
            setEqualTo();
            resetLessThan();
            resetGreaterThan();
        }
    }
    else 
        compri();
}

void VirtualMachine::compri()
{
    clock += 1;
    if (reg[finalcode.f1.RD] < finalcode.f3.CONST) {
        setLessThan();
        resetEqualTo();
        resetGreaterThan();
    }
    else if (reg[finalcode.f1.RD] > finalcode.f3.CONST) {
        setGreaterThan();
        resetEqualTo();
        resetLessThan();
    }
    else if (reg[finalcode.f1.RD] == finalcode.f3.CONST) {
        setEqualTo();
        resetLessThan();
        resetGreaterThan();
    }
}

void VirtualMachine::getstat() 	
{
    clock += 1;
    reg[finalcode.f1.RD] = sr;
}

void VirtualMachine::putstat() 
{
    clock += 1;
    sr = reg[finalcode.f1.RD];
}

void VirtualMachine::jump() 
{
    if (finalcode.f2.ADDR + base >= base + limit || finalcode.f2.ADDR + base < base) {
        returnCode(2);
        execute = false;
        return;
    }
    
    clock += 1;

    pc = finalcode.f2.ADDR + base;
}

void VirtualMachine::jumpl() // if LESS == 1 than pc = ADDR
{
    if (finalcode.f2.ADDR + base >= base + limit || finalcode.f2.ADDR + base < base) {
        returnCode(2);
        execute = false;
        return;
    }
    
    clock += 1;    
    
    if(getLessThan())
        pc = finalcode.f2.ADDR + base;
}

void VirtualMachine::jumpe() // if EQUAL == 1 than  pc = ADDR
{
    if (finalcode.f2.ADDR + base >= base + limit || finalcode.f2.ADDR + base < base) {
        returnCode(2);
        execute = false;
        return;
    }
   
    if(getEqualTo()) 
        pc = finalcode.f2.ADDR + base;
}

void VirtualMachine::jumpg() // if GREATER == 1thanpc = ADDR
{
    if (finalcode.f2.ADDR + base >= base + limit || finalcode.f2.ADDR + base < base) {
        returnCode(2);
        execute = false;
        return;
    }

    clock += 1;
    
    if(getGreaterThan())
        pc = finalcode.f2.ADDR + base;
}

void VirtualMachine::call() 
{
    clock += 4;
    
    if (sp < limit + 6) {
        returnCode(3);
        execute = false;
        return;
    }
        
    mem[--sp] = sr;    
    mem[--sp] = reg[0];
    mem[--sp] = reg[1];
    mem[--sp] = reg[2];
    mem[--sp] = reg[3];
    mem[--sp] = pc;
   
    pc = finalcode.f2.ADDR;  
}

void VirtualMachine::_return() // pop and restore status for vm
{
    clock += 4;

    if (sp==256){
        returnCode(4);
        execute=false;
        return;
    }
    
    pc = mem[sp++];
    reg[3] = mem[sp++];
    reg[2] = mem[sp++];
    reg[1] = mem[sp++];
    reg[0] = mem[sp++];
    sr = mem[sp++];  
}

void VirtualMachine::read() // read new content of r[RD] from .in
{
    returnCode(6);
    readOrWrite(finalcode.f1.RD);
    execute = false;
}

void VirtualMachine::write() // write r[RD] into .out 
{
    returnCode(7);
    readOrWrite(finalcode.f1.RD);
    execute = false;
}

void VirtualMachine::halt() // stop
{
    clock += 1;
    returnCode(1);
    execute = false;
}

void VirtualMachine::noop() // noop
{
    clock += 1;     
}

void VirtualMachine::returnCode(int i)
{
    sr &= 0xffffff1f; 

    int temp = i << 5;
    
    sr |= temp;
}

void VirtualMachine::readOrWrite(int i)
{
    sr &= 0xfffffcff;

    int temp = i << 8;
    
    sr |= temp;
}
