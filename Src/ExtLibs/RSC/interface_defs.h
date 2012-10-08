/*
   Error reporting uses standart interface
     - exact location is given in a 'loc' structure
         + for Compiler it is a SourceFile/Line/Column
         + for VM it is a ASMFile(or internal container's URL)/InstructionIndex
     - error message is a symbolic description of a problem
     - return value is the API-internal error code
*/

#ifndef __interface__defs__h__included__
#define __interface__defs__h__included__

#ifndef PACKED_STRUCTURE

#ifdef _MSC_VER
#define PACKED_STRUCTURE __declspec(align(1))
#endif

#ifdef __GNUC__
#define PACKED_STRUCTURE __attribute__((packed,aligned(1)))
#endif

#endif

#ifndef DLLIMPORT
#define DLLIMPORT
#endif

// Compiler ( used for breakpoints and error/warning messages )
struct PACKED_STRUCTURE RS_Source_location
{
	int line;
	int col;
	int fileID; // internal string ID
};

#endif
