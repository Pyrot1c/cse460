/*Assembler for cs460 Ammar Alsibai, Michael Monaghan*/

#include "Assembler.h"


// Assembler constructor
Assembler::Assembler() {

    //Followed according to table. all Instructions needed for assembly assembler constructor
    instr["load"] = &Assembler::load;
    instr["loadi"] = &Assembler::loadi;
    instr["store"] = &Assembler::store;
    instr["add"] = &Assembler::add;
    instr["addi"] = &Assembler::addi;
    instr["addc"] = &Assembler::addc;
    instr["addci"] = &Assembler::addci;
    instr["sub"] = &Assembler::sub;
    instr["subi"] = &Assembler::subi;
    instr["subc"] = &Assembler::subc;
    instr["subci"] = &Assembler::subci;
    instr["and"] = &Assembler::_and;
    instr["andi"] = &Assembler::andi;
    instr["xor"] = &Assembler::_xor;
    instr["xori"] = &Assembler::xori;
    instr["compl"] = &Assembler::_compl;
    instr["shl"] = &Assembler::shl;
    instr["shla"] = &Assembler::shla;
    instr["shr"] = &Assembler::shr;
    instr["shra"] = &Assembler::shra;
    instr["compr"] = &Assembler::compr;
    instr["compri"] = &Assembler::compri;
    instr["getstat"] = &Assembler::getstat;
    instr["putstat"] = &Assembler::putstat;
    instr["jump"] = &Assembler::jump;
    instr["jumpl"] = &Assembler::jumpl;
    instr["jumpe"] = &Assembler::jumpe;
    instr["jumpg"] = &Assembler::jumpg;
    instr["call"] = &Assembler::call;
    instr["return"] = &Assembler::_return;
    instr["read"] = &Assembler::read;
    instr["write"] = &Assembler::write;
    instr["halt"] = &Assembler::halt;
    instr["noop"] = &Assembler::noop;
}



//conversion to binary opcode
void Assembler::assemble( string fileName, string outFileName ) {
    string buffer, opcode;


    inputFile = fileName;
    outputFile = outFileName;

    fstream output;

    //create output files
    output.open( outputFile.c_str(), ios::out );
    output.close();

    fstream assemblyInput;

    // Open .s files
    assemblyInput.open( inputFile.c_str(), ios::in );

    //error handling, if file cannot be opened. throw error and exit
    if ( !assemblyInput.is_open() ) {
        cout << "Failed to open " << inputFile << endl;
        exit( 1 );
    }

    // Load initial line of assembly into buffer
    getline( assemblyInput, buffer );

    // go through the assembly
    while ( !assemblyInput.eof() ) {
        // Check if line is a comment
        if ( buffer[0] == '!' ) {
            getline( assemblyInput, buffer );
            continue;
        }

        istringstream str( buffer.c_str() );

        try {
            str >> opcode;

            if ( not instr[opcode] ) {
                throw NullPointerException();
            } else {
                ( this->*instr[opcode] )( buffer.c_str() );
            }
        }

        catch ( NullPointerException e ) {
            cerr << e.what() << endl;
            exit( 1 );
        }

        getline( assemblyInput, buffer );
    }

    assemblyInput.close();
}


// write function
// Write binary source code to file
void Assembler::write( int binaryNum ) {
    fstream outFile;

    // Open output file
    outFile.open( outputFile.c_str(), ios::in | ios::out | ios::ate );

    // Check if output file opened correctly
    if ( !outFile.is_open() ) {
        cout << "File cannot be opened." << outputFile << endl;
        exit( 1 );
    }

    // write instruction code
    outFile << binaryNum << endl;

    outFile.close();
}

//make sure that the Rd value is usable
void Assembler::CheckRD( int RD ) {
    if ( RD > 3 || RD < 0 ) {
        cout << "RD out of range!" << endl;
        exit( 1 );
    }
}

//check if Rs is usable
void Assembler::CheckRS( int RS ) {
    if ( RS > 3 || RS < 0 ) {
        cout << "RS out of range!" << endl;
        exit ( 1 );
    }
}

//check if addr is usable
void Assembler::CheckADDR( int ADDR ) {
    if ( ADDR > 255 || ADDR < 0 ) {
        cout << "Addr Out of range!" << endl;
        exit( 1 );
    }
}

//check if const in range
void Assembler::CheckCONST( int CONST ) {
    if ( CONST > 127 || CONST < -128 ) {
        cout << "Const not in range!" << endl;
        exit( 1 );
    }
}


// load function
void Assembler::load( string s ) {
    int RD, ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> ADDR;

    // Confirm RD and ADDR are valid
    CheckRD( RD );
    CheckADDR( ADDR );

    // Insert OPCODE, RD and ADDR
    int binary = 0; // 00000
    binary += RD << 9;
    binary += ADDR;

    write( binary );
} // end load function


// loadi function
void Assembler::loadi( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 0; // 00000
    binary += RD << 9;
    binary += I << 8;

    // If CONST is negative, take 2's complement
    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
} // end loadi function


// store function
void Assembler::store( string s ) {
    int RD, ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> ADDR;

    // Confirm RD and ADDR are valid
    CheckRD( RD );
    CheckADDR( ADDR );

    // Insert OPCODE, RD and ADDR
    int binary = 1; // 00001
    binary = binary << 11;
    binary += RD << 9;
    binary += ADDR;

    write( binary );
}


// add function
void Assembler::add( string s ) {
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RS are valid
    CheckRD(RD);
    CheckRS(RS);

    // Insert OPCODE, RD, I and RS
    int binary = 2; // 00010
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
}


// addi function
void Assembler::addi( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 2; // 00010
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if (CONST < 0) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// addc function
void Assembler::addc( string s ) {
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RS are valid
    CheckRD( RD );
    CheckRS( RS );

    // Insert OPCODE, RD, I and RS
    int binary = 3; // 00011
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
}


// addci function
void Assembler::addci( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 3; // 00011
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// sub function
void Assembler::sub( string s ) {
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RS are valid
    CheckRD( RD );
    CheckRS( RS );

    // Insert OPCODE, RD, I and RS
    int binary = 4; // 00100
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
}


// subi function
void Assembler::subi( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 4; // 00100
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// subc function
void Assembler::subc( string s ) {
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RS are valid
    CheckRD( RD );
    CheckRS( RS );

    // Insert OPCODE, RD, I and RS
    int binary = 5; // 00101
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
}


// subci function
void Assembler::subci( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 5; // 00101
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// _and function
void Assembler::_and( string s ) {
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RS are valid
    CheckRD( RD );
    CheckRS( RS );

    // Insert OPCODE, RD, I and RS
    int binary = 6; // 00110
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
}


// andi function
void Assembler::andi( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 6; // 00110
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// _xor function
void Assembler::_xor( string s ) {
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RS are valid
    CheckRD( RD );
    CheckRS( RS );

    // Insert OPCODE, RD, i and RS
    int binary = 7; // 00111
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
}


// xori function
void Assembler::xori( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 7; // 00111
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// _compl function
void Assembler::_compl( string s ) {
    int RD, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 8; // 01000
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}

// shl function
void Assembler::shl( string s ) {
    int RD, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 9; // 01001
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// shla function
void Assembler::shla( string s ) {
    int RD, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 10; // 01010
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}

// shr function
void Assembler::shr( string s ) {
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 11; // 01011
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// shra function
void Assembler::shra( string s ) {
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 12; // 01100
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// compr function
void Assembler::compr( string s ) {
    int RD, RS;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> RS;

    // Confirm RD and RD are valid
    CheckRD( RD );
    CheckRS( RS );

    // Insert OPCODE, RD and RS
    int binary = 13; // 01101
    binary = binary << 11;
    binary += RD << 9;
    binary += RS << 6;

    write( binary );
}


// compri function
void Assembler::compri( string s ) {
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD >> CONST;

    // Confirm RD and CONST are valid
    CheckRD( RD );
    CheckCONST( CONST );

    // Insert OPCODE, RD, I and CONST
    int binary = 13; // 01101
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 ) {
        CONST = CONST & 0x00FF;
    }

    binary += CONST;

    write( binary );
}


// getstat function
void Assembler::getstat( string s ) {
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 14; // 01110
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// putstat function
void Assembler::putstat( string s ) {
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 15; // 01111
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// jump function
void Assembler::jump( string s ) {
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> ADDR;

    // Confirm ADDR is valid
    CheckADDR( ADDR );

    // Insert OPCODE and ADDR
    int binary = 16; // 10000
    binary = binary << 11;
    binary += ADDR;

    write( binary );
}


// jumpl function
void Assembler::jumpl( string s ) {
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> ADDR;

    // Confirm ADDR is valid
    CheckADDR( ADDR );

    // Insert OPCODE and ADDR
    int binary = 17; // 10001
    binary = binary << 11;
    binary += ADDR;

    write( binary );
}

// jumpe function
void Assembler::jumpe( string s ) {
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> ADDR;

    // Confirm ADDR is valid
    CheckADDR( ADDR );

    // Insert OPCODE and ADDR
    int binary = 18; // 10010
    binary = binary << 11;
    binary += ADDR;

    write( binary );
}


// jumpg function
void Assembler::jumpg( string s ) {
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> ADDR;

    // Confirm ADDR is valid
    CheckADDR( ADDR );

    // Insert OPCODE and ADDR
    int binary = 19; // 10011
    binary = binary << 11;
    binary += ADDR;

    write( binary );
}


// call function
void Assembler::call( string s ) {
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> ADDR;

    // Confirm ADDR is valid
    CheckADDR( ADDR );

    // Insert OPCODE and ADDR
    int binary = 20; // 10100
    binary = binary << 11;
    binary += ADDR;

    write( binary );
}


// _return function
// Pop and restore status
void Assembler::_return( string s ) {
    // Insert OPCODE
    int binary = 21; // 10101
    binary = binary << 11;

    write( binary );
}


// read function
void Assembler::read( string s ) {
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 22; // 10110
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// write function
void Assembler::write( string s ) {
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

    // Read in instruction
    iStream >> OPCODE >> RD;

    // Confirm RD is valid
    CheckRD( RD );

    // Insert OPCODE and RD
    int binary = 23; // 10111
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
}


// halt function
void Assembler::halt( string s ) {
    // Insert OPCODE
    int binary = 24; // 11000
    binary = binary << 11;

    write( binary );
}



// No operation
void Assembler::noop( string s ) {

    int binary = 25; // 11001
    binary = binary << 11;

    write( binary );
}
