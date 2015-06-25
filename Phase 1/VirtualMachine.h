/* Virtual Machine for cs460 Ammar Alsibai, Michael Monaghan*/
#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <ostream>
#include <istream>

using namespace std;

const static char* OUT_OF_RANGE = "Address out of range!";
const static char* STACK_FULL = "Stack full!";

union instruction {
  int i;

  struct {
    unsigned UNUSED:6;
    unsigned RS:2;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
  }
  f1;

  struct {
    unsigned ADDR:8;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
  }
  f2;

  struct {
    int CONST:8;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
  }
  f3;
};

class VirtualMachine {
private:
  typedef void(VirtualMachine::*FP)();

  static const int REG_FILE_SIZE = 4;
  static const int MEM_SIZE = 256;
  vector < int > reg;
  vector < int > mem;
  instruction ins;
  int pc, ir, sr, sp, base, limit, clock;
  map<int, FP> instr;
  string oFileName, inFileName, outFileName;
  istream * inputStream;
  ostream * outputStream;

  union {
    int i;
    struct {
      unsigned C:1;
      unsigned G:1;
      unsigned E:1;
      unsigned L:1;
      unsigned V:1;
    } bits;
  } status;


  // Execute
  void loadOP();
  void loadiOP();
  void storeOP();
  void addOP();
  void addiOP();
  void addc();
  void addciOP();
  void subOP();
  void subiOP();
  void subcOP();
  void subciOP();
  void andOP();
  void andi();
  void xorOP();
  void xoriOP();
  void complOP();
  void shlOP();
  void shlaOP();
  void shrOP();
  void shraOP();
  void comprOP();
  void compriOP();
  void getstatOP();
  void putstatOP();
  void jumpOP();
  void jumplOP();
  void jumpeOP();
  void jumpgOP();
  void callOP();
  void returnOP();
  void readOP();
  void writeOP();
  void haltOP();
  void noopOP();

  // Check
  void checkSetCarryBit();
  void setOverflow();
  void setGreaterThan();
  void setLessThan();
  void setEqualTo();
  void resetGreaterThan();
  void resetLessThan();
  void resetEqualTo();
  int getCarryBit();
  int getGreaterThan();
  int getLessThan();
  int getEqualTo();

public:
  VirtualMachine();
  /**
     Runs the Virtual Machine with the program read from prog, input from input,
     and output from output.
   */
  void run( istream * prog, istream * input, ostream * output );
};

#endif
