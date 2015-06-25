// Copyright 2015 Michael Monaghan, Ammar Alsibai

#include "VirtualMachine.h"


VirtualMachine::VirtualMachine() {
  reg = vector <int> (REG_FILE_SIZE);
  mem = vector <int> (MEM_SIZE);

  // Fill the map with the appropriate functions.
  instr[0] = &VirtualMachine::loadOP;
  instr[1] = &VirtualMachine::storeOP;
  instr[2] = &VirtualMachine::addOP;
  instr[3] = &VirtualMachine::addc;
  instr[4] = &VirtualMachine::subOP;
  instr[5] = &VirtualMachine::subcOP;
  instr[6] = &VirtualMachine::andOP;
  instr[7] = &VirtualMachine::xorOP;
  instr[8] = &VirtualMachine::complOP;
  instr[9] = &VirtualMachine::shlOP;
  instr[10] = &VirtualMachine::shlaOP;
  instr[11] = &VirtualMachine::shrOP;
  instr[12] = &VirtualMachine::shraOP;
  instr[13] = &VirtualMachine::comprOP;
  instr[14] = &VirtualMachine::getstatOP;
  instr[15] = &VirtualMachine::putstatOP;
  instr[16] = &VirtualMachine::jumpOP;
  instr[17] = &VirtualMachine::jumplOP;
  instr[18] = &VirtualMachine::jumpeOP;
  instr[19] = &VirtualMachine::jumpgOP;
  instr[20] = &VirtualMachine::callOP;
  instr[21] = &VirtualMachine::returnOP;
  instr[22] = &VirtualMachine::readOP;
  instr[23] = &VirtualMachine::writeOP;
  instr[24] = &VirtualMachine::haltOP;
  instr[25] = &VirtualMachine::noopOP;
}



void VirtualMachine::run(istream * prog, istream * input, ostream * output) {
  inputStream = input;
  outputStream = output;
	// read stream into mem
  int l = 0;
  while (prog->good()) {
    *prog >> mem[l++];
  }
  limit = l;

  // Begin execution

  // Reset state
  pc = 0;
  ir = 0;
	sr = 0;
	sp = 256;
	base = 0;
	clock = 0;
	reg[0] = 0;
	reg[1] = 0;
	reg[2] = 0;
	reg[3] = 0;

  // Main fetch execute loop
  while (true) {
    // Fetch
    ir = mem[pc++];
    ins.i = ir;
    // Execute
    (this->*instr[ins.f1.OP])();
  }
}

// Set carry using reg RD
void VirtualMachine::checkSetCarryBit() {
  if (reg[ins.f1.RD] & 0x10000) {
    sr |= 1;
  } else {
    sr &= 0xfffe;
  }
}


// Get the carry bit
int VirtualMachine::getCarryBit() {
  return ( sr & 0x1 );
}


// Set overflow
void VirtualMachine::setOverflow() {
  sr |= 0x10;
}


// Set the greater than
void VirtualMachine::setGreaterThan() {
  sr |= 0x02;
}


// Get greater than
int VirtualMachine::getGreaterThan() {
  status.i = sr;
  return status.bits.G;
}


// Reset the greater than bit
void VirtualMachine::resetGreaterThan() {
  sr &= 0xfffffffd;
}


// Set less than bit
void VirtualMachine::setLessThan() {
  sr |= 0x08;
}


// Get the less than status bit
int VirtualMachine::getLessThan() {
  status.i = sr;
  return status.bits.L;
}


// Reset less than status bit
void VirtualMachine::resetLessThan() {
  sr &= 0xfffffff7;
}


// Set equal to bit
void VirtualMachine::setEqualTo() {
  sr |= 0x04;
}


// Get equal to bit
int VirtualMachine::getEqualTo() {
  status.i = sr;
  return status.bits.E;
}


// Reset equal to bit
void VirtualMachine::resetEqualTo() {
  sr &= 0xfffffffb;
}


// r[RD] = mem[ADDR]
void VirtualMachine::loadOP() {
  clock += 4;
  // check I
	if ( ins.f2.I == 0 ) {
    if ( ins.f2.ADDR >= base + limit || ins.f2.ADDR < base ) {
      cout << OUT_OF_RANGE << endl;
      exit(1);
    } else {
      reg[ins.f2.RD] = mem[ins.f2.ADDR];
    }
	} else {
    loadiOP();
  }
}


// r[RD] = CONST
void VirtualMachine::loadiOP() {
  clock += 1;
  reg[ins.f3.RD] = ins.f3.CONST;
}


// mem[ADDR] = r[RD]
void VirtualMachine::storeOP() {
clock += 4;
  if ( ins.f2.ADDR >= base + limit || ins.f2.ADDR < base ) {
    cout << OUT_OF_RANGE << endl;
    exit(1);
  }
  mem[ins.f2.ADDR] = reg[ins.f2.RD];
}


//  r[RD] = r[RD] + r[RS]
void VirtualMachine::addOP() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
    int sum = reg[ins.f1.RD] + reg[ins.f1.RS];
    if ( reg[ins.f1.RD] >= 0 && reg[ins.f1.RS] >= 0 && sum < 0 )
      setOverflow();
    else if ( reg[ins.f1.RD] < 0 && reg[ins.f1.RS] < 0 && sum >= 0 )
      setOverflow();
    reg[ins.f1.RD] = sum;
    checkSetCarryBit();
  } else {
    addiOP();
  }
}


//  r[RD] = r[RD] + CONST
void VirtualMachine::addiOP() {
  clock += 1;
  int sum = reg[ins.f3.RD] + ins.f3.CONST;
  if ( reg[ins.f3.RD] >= 0 && ins.f3.CONST >= 0 && sum < 0 )
    setOverflow();
  else if ( reg[ins.f3.RD] < 0 && ins.f3.CONST < 0 && sum >= 0 )
    setOverflow();
  reg[ins.f3.RD] = sum;
  checkSetCarryBit();
  }


//  r[RD] = r[RD] + r[RS] + CARRY
void VirtualMachine::addc() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
    int carry = getCarryBit();
    int sum = reg[ins.f1.RD] + reg[ins.f1.RS] + carry;
    if (reg[ins.f1.RD] >= 0 && reg[ins.f1.RD] >= 0 &&  carry >= 0 && sum < 0) {
      setOverflow();
    } else if (reg[ins.f1.RD] < 0 &&
      reg[ins.f1.RD] < 0 && carry < 0 && sum >= 0) {
      setOverflow();
    }
    reg[ins.f1.RD] = sum;
    checkSetCarryBit();
  } else {
    addciOP();
  }
}

//  r[RD] = r[RD] + CONST + CARRY
void VirtualMachine::addciOP() {
  clock += 1;
  int carry = getCarryBit();
  int sum = reg[ins.f3.RD] + ins.f3.CONST + carry;
  if ( reg[ins.f3.RD] >= 0 && ins.f3.CONST >= 0 &&  carry >= 0 && sum < 0 ) {
    setOverflow();
  } else if  ( reg[ins.f3.RD] < 0 &&
    ins.f3.CONST < 0 && carry < 0 && sum >= 0 ) {
    setOverflow();
  }
  reg[ins.f3.RD] = sum;
  checkSetCarryBit();
}


// r[RD] = r[RD] - r[RS]
void VirtualMachine::subOP() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
      int temp = reg[ins.f1.RD] - reg[ins.f1.RS];
      int complRS = ~reg[ins.f1.RS] + 1;
      if ( reg[ins.f1.RD] >= 0 && complRS >= 0 && temp < 0 )
        setOverflow();
      else if ( reg[ins.f1.RD] < 0 && complRS < 0 && temp >= 0 )
        setOverflow();
      reg[ins.f1.RD] = temp;
      checkSetCarryBit();
    } else {
      subiOP();
    }
}


//  r[RD] = r[RD] - CONST
void VirtualMachine::subiOP() {
  clock += 1;
	int temp = reg[ins.f1.RD] - ins.f3.CONST;
	int complCONST = ~ins.f3.CONST + 1;
	if ( reg[ins.f1.RD] >= 0 && complCONST >= 0 && temp < 0 )
		setOverflow();
	else if ( reg[ins.f1.RD] < 0 && complCONST < 0 && temp >= 0 )
		setOverflow();
	reg[ins.f1.RD] = temp;
	checkSetCarryBit();
}


//  r[RD] = r[RD] - r[RS] - CARRY
void VirtualMachine::subcOP() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
    int temp = reg[ins.f1.RD] - reg[ins.f1.RS] - getCarryBit();
    int complRS = ~reg[ins.f1.RS] + 1;
    if ( reg[ins.f1.RD] >= 0 && complRS >= 0 && temp < 0 ) {
      setOverflow();
    } else if ( reg[ins.f1.RD] < 0 && complRS < 0 && temp >= 0 ) {
      setOverflow();
    }
    reg[ins.f1.RD] = temp;
    checkSetCarryBit();
  } else {
    subciOP();
  }
}


// r[RD] = r[RD] - CONST - CARRY
void VirtualMachine::subciOP() {
  clock += 1;
  int temp = reg[ins.f1.RD] - ins.f3.CONST - getCarryBit();
  int complRS = ~ins.f3.CONST + 1;

  if ( reg[ins.f1.RD] >= 0 && complRS >= 0 && temp < 0 ) {
    setOverflow();
  } else if ( reg[ins.f1.RD] < 0 && complRS < 0 && temp >= 0 ) {
      setOverflow();
    }
  reg[ins.f1.RD] = temp;
  checkSetCarryBit();
}


// r[RD] = r[RD] & r[RS]
void VirtualMachine::andOP() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
    reg[ins.f1.RD] = reg[ins.f1.RD] & reg[ins.f1.RS];
  } else {
    andi();
  }
}


// r[RD] = r[RD] & CONST
void VirtualMachine::andi() {
  clock += 1;
  reg[ins.f1.RD] = reg[ins.f1.RD] & ins.f3.CONST;
}


// r[RD] = r[RD] ^ r[RS]
void VirtualMachine::xorOP() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
    reg[ins.f1.RD] = reg[ins.f1.RD] ^ reg[ins.f1.RS];
  } else {
    xoriOP();
  }
}


// r[RD] = r[RD] ^ CONST
void VirtualMachine::xoriOP() {
  clock += 1;
  reg[ins.f1.RD] = reg[ins.f1.RD] ^ ins.f3.CONST;
}


// r[RD] = ~ r[RD]
void VirtualMachine::complOP() {
  clock += 1;
  reg[ins.f1.RD] = ~reg[ins.f1.RD];
}


// r[RD] = r[RD] << 1, shift-in-bit = 0
void VirtualMachine::shlOP() {
  clock += 1;
  reg[ins.f1.RD] = reg[ins.f1.RD] << 1;
  checkSetCarryBit();
}


// shl arithmetic  Set CARRY & Sign Ext
void VirtualMachine::shlaOP() {
  clock += 1;
  if ( reg[ins.f1.RD] < 0 ) {
    reg[ins.f1.RD] = reg[ins.f1.RD] << 1;
    reg[ins.f1.RD] = reg[ins.f1.RD] | 0xffff8000;
  } else {
    reg[ins.f1.RD] = reg[ins.f1.RD] << 1;
  }
  checkSetCarryBit();
}


// r[RD] = r[RD] >> 1, shift-in-bit = 0
void VirtualMachine::shrOP() {
  clock += 1;
  int temp = reg[ins.f1.RD];
  temp = temp & 0x1;
  if ( temp ) {
    sr |= 1;
  }
  reg[ins.f1.RD] = reg[ins.f1.RD] >> 1;
}


// shrOP arithmetic  Set CARRY & Sign Ext
void VirtualMachine::shraOP() {
  clock += 1;
  int temp = reg[ins.f1.RD];
  temp = temp & 0x1;
  if ( temp ) {
    checkSetCarryBit();
  }
  if ( reg[ins.f1.RD] < 0 ) {
    reg[ins.f1.RD] = reg[ins.f1.RD] >> 1;
    reg[ins.f1.RD] = reg[ins.f1.RD] | 0xffff8000;
  } else {
    reg[ins.f1.RD] = reg[ins.f1.RD] >> 1;
  }
}


// Compare
void VirtualMachine::comprOP() {
  clock += 1;
  if ( ins.f1.I == 0 ) {
    if ( reg[ins.f1.RD] < reg[ins.f1.RS] ) {
      setLessThan();
      resetEqualTo();
      resetGreaterThan();
    } else if ( reg[ins.f1.RD] > reg[ins.f1.RS] ) {
      setGreaterThan();
      resetEqualTo();
      resetLessThan();
    } else {
      setEqualTo();
      resetLessThan();
      resetGreaterThan();
    }
  } else {
    compriOP();
  }
}

// compare with literal
void VirtualMachine::compriOP() {
  clock += 1;

  if ( reg[ins.f1.RD] < ins.f3.CONST ) {
    setLessThan();
    resetEqualTo();
    resetGreaterThan();
  } else if ( reg[ins.f1.RD] > ins.f3.CONST ) {
    setGreaterThan();
    resetEqualTo();
    resetLessThan();
  } else if ( reg[ins.f1.RD] == ins.f3.CONST ) {
    setEqualTo();
    resetLessThan();
    resetGreaterThan();
  }
}


// r[RD] = SR
void VirtualMachine::getstatOP() {
  clock += 1;
  reg[ins.f1.RD] = sr;
}


// SR = r[RD]
void VirtualMachine::putstatOP() {
  clock += 1;
  sr = reg[ins.f1.RD];
}


// pc = ADDR
void VirtualMachine::jumpOP() {
  if (ins.f2.ADDR >= base + limit || ins.f2.ADDR < base) {
    cerr << OUT_OF_RANGE << ins.f2.ADDR;
    exit(1);
  }
  clock += 1;
  pc = ins.f2.ADDR;
}


// if L pc = ADDR
void VirtualMachine::jumplOP() {
  if (ins.f2.ADDR >= base + limit || ins.f2.ADDR < base) {
    cout << OUT_OF_RANGE << ins.f2.ADDR;
    exit(1);
  }
  clock += 1;
  if ( getLessThan() ) {
    pc = ins.f2.ADDR;
  }
}


// if E, pc = ADDR
void VirtualMachine::jumpeOP() {
  if (ins.f2.ADDR >= base + limit || ins.f2.ADDR < base) {
    cout << OUT_OF_RANGE;
    exit(1);
    }
    clock += 1;
    if (getEqualTo()) {
      pc = ins.f2.ADDR;
    }
}


// if G pc = ADDR
void VirtualMachine::jumpgOP() {
  if (ins.f2.ADDR >= base + limit || ins.f2.ADDR < base) {
    cout << OUT_OF_RANGE;
    exit(1);
  }
  clock += 1;
  if (getGreaterThan())
  pc = ins.f2.ADDR;
}


// save status to stack and change pc
void VirtualMachine::callOP() {
  clock += 4;
  if (sp < limit + 6) {
    cout << STACK_FULL;
    exit(1);
  }
  mem[--sp] = sr;
  mem[--sp] = reg[0];
  mem[--sp] = reg[1];
  mem[--sp] = reg[2];
  mem[--sp] = reg[3];
  mem[--sp] = pc;
  pc = ins.f2.ADDR;
}


// restore status from stack and change pc
void VirtualMachine::returnOP() {
  clock += 4;
  pc = mem[sp++];
  reg[3] = mem[sp++];
  reg[2] = mem[sp++];
  reg[1] = mem[sp++];
  reg[0] = mem[sp++];
  sr = mem[sp++];
}


// read from input
void VirtualMachine::readOP() {
  clock += 28;
  *inputStream >> reg[ins.f1.RD];
}


// write to output
void VirtualMachine::writeOP() {
  clock += 28;
  // check the sign bit
  int val;
  if (reg[ins.f1.RD] & 0x8000) {
    val = reg[ins.f1.RD] | 0xffff0000;
  } else {
    val = reg[ins.f1.RD] & 0xffff;
  }
  *outputStream << val << endl;
}


// Stahpp!
void VirtualMachine::haltOP() {
    clock += 1;
    exit(0);
}


// Procrastinate the next instruction
void VirtualMachine::noopOP() {
    clock += 1;
}
