/* OS.h Ammar Alsibai Michael Monaghan */

#include "OS.h"


OS::OS() 
{
    masterClock = contextSwitchClock = idleClock = 0;
}

void OS::start() 
{
 // Get the s files
 
    system("ls *.s > progs");
     
    fstream readProgs;
    readProgs.open("progs", ios::in);
	
    if (!readProgs.is_open()) {
        cout << "cant open progs\n";
        exit(1);
    }

    string line;
    getline(readProgs, line);
    
    while (!readProgs.eof()) {
        
        as.assemble(line); // assemble the files
    
        //make the PCB for each file, put the jobs list, and then the ready queue
    
        PCB * p = new PCB;
        jobs.push_back(p);
        readyQ.push(p);
        p->state = "ready";
        
        string fileName = line.erase(line.length() -2, 2);
        
        // assign file names, input and outputs
        p->originalfilename = fileName + ".o";          
        p->readfilename = fileName + ".in";           
        p->writefilename = fileName + ".out";       
        p->stfilename =  fileName + ".st";        

        // Read in files
        p->openReadInFileStream();
        p->openWriteOutFileStream();
        
        //read into mem
    
        int base, limit;
    
        vm.loadIntoMemory(p->originalfilename, &base, &limit);
    
        p->base = base;
        p->limit = limit;
        p->pc = base;
        
        getline(readProgs, line);      
    } 
    
    readProgs.close();
    system("rm progs");

    // copy item 1 in readyQueue to running , load pcb data to vm  

    running = readyQ.front();
    readyQ.pop();
    givePCBToVM();
    
    int code = 0; 
    
    while (!done()) // main loops
    {
        code = 0;
   
        vm.run(); 
        
        code = vm.sr;    
        code = code & 0xe0;  
        code >>= 5;          
        
        masterClock += 5;
        contextSwitchClock += 5;
        
        for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
            if(!strcmp((*it)->state.c_str(), "terminated"))
                (*it)->contextSwitchTime += 5;

    
        // if interrupt move.
        if(!waitQ.empty()) {
            while (waitQ.front()->interruptTime <= masterClock) {
                PCB * temp = waitQ.front();
                waitQ.pop();
                readyQ.push(temp); 
                temp->state = "ready";      
                if(waitQ.empty())
                    break;     
            }
        }
        

        for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
            if(strcmp((*it)->state.c_str(), "ready"))
                (*it)->waitingTime += vm.clock; 
            
        if(!(running == NULL)) {
    
            //update clocks
            masterClock += vm.clock;
            running->cpuTime += vm.clock;

			//them errors cases
            switch (code) {
                case 0:
                    getPCBFromVM();
                    readyQ.push(running);          
                    break;           
                case 1:
                    running->state = "terminated";
                    break;           
                case 2:
                    running->state = "terminated";
                    outputErrorCode("Out-of-Bound Reference");
                    break;           
                case 3:
                    running->state = "terminated";           
                    outputErrorCode("Stack Overflow");
                    break;      
                case 4:
                    running->state = "terminated";
                    outputErrorCode("Stack Underflow");
                    break;
                case 5:
                    running->state = "terminated";
                    outputErrorCode("Invalid OPCode");
                    break;
                case 6:
                {          
                    int readIn;
               
                    running->readIntoRegister >> readIn;

                    int readReg = vm.sr;
 
                    readReg &= 0x300;         
                    readReg >>= 8; 
                     
                    if(readReg < 0 || readReg > 3) {
                        outputErrorCode("Invalid IO Register");
                        break;
                    }
                
                    vm.reg[readReg] = readIn; 
      
                    masterClock++;
                    running->ioTime += 27;
                    running->cpuTime++;  
                    waitQ.push(running);
                    running->state = "waiting";
                    getPCBFromVM(); 
                    running->interruptTime = masterClock + 28;
               
                    break;
                }
            
                case 7:
                {        
                    int writeToReg = vm.sr;
           
                    writeToReg &= 0x300;   
                
                    writeToReg >>= 8;   
                
                    int temp = vm.reg[writeToReg] & 0x8000;
      
                    if (temp) 
                        temp = vm.reg[writeToReg] | 0xffff0000;
                    else
                        temp = vm.reg[writeToReg] & 0xffff;
    
                    running->writeToRegister << temp << endl;
                
                    masterClock++;
                    running->ioTime += 27;
                    running->cpuTime++;                   
               
                    waitQ.push(running);
                    running->state = "waiting";
                    getPCBFromVM();
                    running->interruptTime = masterClock + 28;

                    break;
                }
                
                default:
                    outputErrorCode("Invalid Return Code");
                    running->state = "terminated";
                    break;
            }
        
            running = NULL;
 
        } 
        

   
		//update clock after item 1
        if(readyQ.empty() && !waitQ.empty()) {

            int temp = waitQ.front()->interruptTime - masterClock;
    
            masterClock += temp;
            idleClock += temp;
            readyQ.push(waitQ.front());
            waitQ.front()->state = "ready";
            waitQ.pop(); 
        
            for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
                if(!strcmp((*it)->state.c_str(), "terminated"))
                    (*it)->idleTime += temp;      
        }

        if(!readyQ.empty()) {
            running = readyQ.front();
            running->state = "running";
            readyQ.pop();
            givePCBToVM();
        }  
    
    } 
    for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
        (*it)->turnaroundTime = (*it)->cpuTime + (*it)->waitingTime + (*it)->ioTime + (*it)->idleTime + (*it)->contextSwitchTime;
    
        
    int systemTime = contextSwitchClock + idleClock; // get sys time
         
    double floatingPointMasterClock = masterClock;  // get cpu utilize
        
    double cpuUtilization = (floatingPointMasterClock - idleClock) / floatingPointMasterClock;  //calculate cpu
                  
    int allCpuTime = 0;
           
    // iterate through processes
    for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
        allCpuTime += (*it)->cpuTime;    
            
    double userCpuUtilization = allCpuTime / floatingPointMasterClock; // user cpu
                   
    double throughPut = jobs.size() / (floatingPointMasterClock / 10000.0);  
        
    // iterate through pcb list 
        
     for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++) {
             
        fstream writeAccounting;
        writeAccounting.open((*it)->writefilename.c_str(), ios::in | ios::out | ios::ate);
	
        if (!writeAccounting.is_open()) {
            cout << (*it)->writefilename << " failed to open.\n";
            exit(1);
        }
            
        writeAccounting << "\nProcess Specific Accounting Data\n\n";
        writeAccounting << "CPU Time = " << (*it)->cpuTime << " cycles\n";
        writeAccounting << "Waiting Time = " << (*it)->waitingTime << " cycles\n";
        writeAccounting << "Turnaround Time = " << (*it)->turnaroundTime << " cycles\n";
        writeAccounting << "IO Time = " << (*it)->ioTime << " cycles\n";
        writeAccounting << "Largest Stack Size = " << (*it)->largestStackSize << endl;
          
        writeAccounting << "\nSystem Specific Accounting Data\n\n";            
        writeAccounting << "System Time = " << systemTime << " cycles\n";
        writeAccounting << "System CPU Utilization = " << cpuUtilization * 100 << "%\n";
        writeAccounting << "User CPU Utilization = " << userCpuUtilization * 100 << "%\n";
        writeAccounting << "ThroughPut = " << throughPut << " processes per second\n";

        writeAccounting.close();
    }
        
   
    // delete the st files
   
    system("rm *.st");
}

void OS::givePCBToVM() 
{
    //copy contents of PCB running into VM
    
    vm.pc = running->pc;
    vm.sr = running->sr;
    vm.sp = running->sp;
    vm.base = running->base;
    vm.limit = running->limit;
    
    for (int i = 0;i < vm.REG_FILE_SIZE;i++)
        vm.reg[i] = running->reg[i];
    
    if (running->sp < 256) {
        fstream readProgs;
            readProgs.open(running->stfilename.c_str(), ios::in);
	
        if (!readProgs.is_open()) {
            cout << running->stfilename << " failed to open.\n";
            exit(1);
        }
    
        int stackPointer = running->sp;
        
        string line;
        getline(readProgs, line);
    
        while (!readProgs.eof()) {
            vm.mem[stackPointer++] = atoi(line.c_str());
             getline(readProgs, line);
        } 
    }
}



void OS::getPCBFromVM()
{
    //copy contents of VM into running PCB
     
    running->pc= vm.pc; 
    running->sr= vm.sr; 
    running->sp=vm.sp;
    running->base= vm.base;
    running->limit= vm.limit;
    
    for (int i = 0;i <= vm.REG_FILE_SIZE - 1;i++) {
        running->reg[i] = vm.reg[i];
    }
        
    if(running->sp < 256)
    {
        fstream readProgs;
        readProgs.open(running->stfilename.c_str(), ios::out);
	
        if (!readProgs.is_open()) {
            cout << running->stfilename << " failed to open.\n";
            exit(1);
        }
    
        int stackPointer = running->sp;
        
        int temp = 256 - stackPointer;
        
        if(running->largestStackSize < temp)
            running->largestStackSize = temp;
    
        while (stackPointer<256) {
             readProgs<<vm.mem[stackPointer++]<<endl;
        } 
   }
}

void OS::outputErrorCode(string s)
{
    fstream output;
    output.open(running->writefilename.c_str(), ios::in | ios::out | ios::ate);
   
    output << "\nError Code = " << s << endl << endl;
    output.close();
}

// check for prcoesses killed
bool OS::done() 
{
    if(readyQ.empty() && waitQ.empty() && !running)
        return true;
    
    return false;
}

//run the OS
int main()
{
  OS os;
  os.start(); 
} 


