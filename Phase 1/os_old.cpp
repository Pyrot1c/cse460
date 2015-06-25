#include "Assembler.h"
#include "VirtualMachine.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  string s, o, in, out;
  if (argc == 2) {
    s = argv[1];
    o = argv[1];
    in = argv[1];
    out = argv[1];
    s += ".s";
    o += ".o";
    in += ".in";
    out += ".out";
  } else if (argc == 5) {
    s = argv[1];
    o = argv[2];
    in = argv[3];
    out = argv[4];
  } else {
    cout << "invalid arguments" << endl;
    return 1;
  }
  Assembler as;
  as.assemble(s,o);
  VirtualMachine vm;
  ifstream prog, input;
  ofstream output;
  prog.open(o.c_str());
  if (!prog.is_open()) {
    cerr << "unable to open " << o << endl;
    return 1;
  }
  input.open(in.c_str());
  if (!prog.is_open()) {
    cerr << "unable to open " << in << endl;
    return 1;
  }
  output.open(out.c_str());
  if (!prog.is_open()) {
    cerr << "unable to open " << out << endl;
    return 1;
  }
  vm.run( &prog, &input, &output);
  return 0;
}
