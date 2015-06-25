/*Assembler for cs460 Ammar Alsibai, Michael Monaghan*/

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>
#include <stdexcept>

using namespace std;

//main assembler class
class Assembler {

  private:

    typedef void(Assembler::*FP)(string);

    // methods
    void CheckRD( int RD );
    void CheckRS( int RS );
    void CheckADDR( int ADDR );
    void CheckCONST( int CONST );
    void write( int binaryCode );

    // Instruction methods..string s all the way! lol
    void load(string s);
    void loadi(string s);
    void store(string s);
    void add(string s);
    void addi(string s);
    void addc(string s);
    void addci(string s);
    void sub(string s);
    void subi(string s);
    void subc(string s);
    void subci(string s);
    void _and(string s);
    void andi(string s);
    void _xor(string s);
    void xori(string s);
    void _compl(string s);
    void shl(string s);
    void shla(string s);
    void shr(string s);
    void shra(string s);
    void compr(string s);
    void compri(string s);
    void getstat(string s);
    void putstat(string s);
    void jump(string s);
    void jumpl(string s);
    void jumpe(string s);
    void jumpg(string s);
    void call(string s);
    void _return(string s);
    void read(string s);
    void write(string s);
    void halt(string s);
    void noop(string s);

    //variables needed
    string inputFile, outputFile;
    map<string, FP> instr;

  public:
    //needs to be public
    Assembler();
    void assemble( string fileName, string outFileName );
};

class NullPointerException: public runtime_error {
  public:
    NullPointerException(): runtime_error("Error") { }
};

#endif
