/*
   Internal compiler structures

   RealScript Compiler SDK
   (C) Viktor Latypov, 2007
*/

#ifndef __compiler__types__h__included__
#define __compiler__types__h__included__

/////////////////
//  Types      //
/////////////////

#include "interface_defs.h"

// prototypical compiler handle
struct PACKED_STRUCTURE RS_Compiler_Handle
{
	void* CompilerObject;
	void* Reserved;
};

struct PACKED_STRUCTURE RS_Class
{
	int nameID;

	int classID;
};

struct PACKED_STRUCTURE RS_Method
{
	int nameID;

	int classID;
	int stateID;

	int methodID;

	int paramCount;
	// boolean modifiers
};

struct PACKED_STRUCTURE RS_Type
{
	int nameID;

	int typeID;
};

#define RS_Operator_Unknown     (0)
#define RS_Operator_Assignment  (1)
#define RS_Operator_Conversion  (2)
#define RS_Operator_Binary      (3)
#define RS_Operator_Pre         (4)
#define RS_Operator_Post        (5)

struct PACKED_STRUCTURE RS_Operator
{
	int nameID;

	int operatorType;
	int resTypeID, opType1ID, opType2ID;
};

struct PACKED_STRUCTURE RS_State
{
	int nameID;

	int classID;
	int stateID;
};

struct PACKED_STRUCTURE RS_Symbol /* field,param,global,local*/
{
	int nameID;

	int classID; // where it is in ?
	int typeID;
	// boolean modifiers ?
};

/////////////////
// Constants   //
/////////////////

// Target code definitions

#define RS_TargetCode_Error      (0)
#define RS_TargetCode_Expression (1)
#define RS_TargetCode_Statement  (2)
#define RS_TargetCode_Package    (3)
#define RS_TargetCode_Program    (4)

#endif
