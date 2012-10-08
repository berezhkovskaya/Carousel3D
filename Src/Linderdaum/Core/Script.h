/**
 * \file Script.h
 * \brief Code fragment, LinderScript virtual machine, Scripted classes/methods
 * \version 0.5.91
 * \date 09/03/2010
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __ScriptCore__h__included__
#define __ScriptCore__h__included__

#include "Core/iObject.h"

#include "Generated/VM/LOpCodes.h"

#include "Core/RTTI/Parameters.h"
#include "Core/RTTI/Method.h"
#include "Core/RTTI/Field.h"
#include "Core/RTTI/Method.h"
#include "Core/RTTI/iStaticClass.h"

#include "Utils/iStack.h"

#include "LString.h"

#include <map>
#include <memory.h>

class iOStream;
class iIStream;

class clScriptMethod;

class iSymbolDeclaration;
class clVarSymbolDeclaration;
class clFieldSymbolDeclaration;
class clParamSymbolDeclaration;

/// convert textual opcode representation into opcode
inline LOpCodes SymOpCode2OpCode( const LString& SymOpCode )
{
	for ( int i = 0; i != MAX_OPCODES + 1; ++i )
	{
		if ( LString( SymOpCodes[i] ) == SymOpCode )
		{
			return LOpCodes( i );
		}
	}

	return OP_INVALID;
}

const int FLAG_LABEL = 0x01;

enum LArgsType
{
	ARGS_STACK_STACK,
	ARGS_STACK_PARAM,
	ARGS_PARAM_PARAM,
	ARGS_STRING
};

/// LinderScript virtual machine instruction representation
struct sInstruction
{
	sInstruction(): FOpCode( OP_NOP ),
		FFlag( 0 )
	{
		FArgInt[0]  = 0;
		FArgInt[1]  = 0;
		FArgInt[2]  = 0;
		FArgInt[3]  = 0;
		FIntArgs[0]   = true;
		FIntArgs[1]   = true;
		FDeclaredVars = 0;
	};
	sInstruction( LOpCodes OpCode ): FOpCode( OpCode ),
		FFlag( 0 )
	{
		FArgInt[0] = 0;
		FArgInt[1] = 0;
		FArgInt[2] = 0;
		FArgInt[3] = 0;
		FIntArgs[0]   = true;
		FIntArgs[1]   = true;
		FDeclaredVars = 0;
	};
	LOpCodes    FOpCode;
	LArgsType   FArgsType;
	Lint        FDeclaredVars; // HACK : replace by OP_LABEL with argument
	Lubyte      FFlag;
	bool        FIntArgs[2];
	union
	{
		struct
		{
			Lint     FArgInt[4];
		};
		struct
		{
			double   FArgFloat[4];
		};
	};
};

typedef std::vector<sInstruction> clInstructions;

/// Code fragment within LinderScript virtual machine
class scriptfinal clCodePatch: public iObject
{
public:
	clCodePatch();
	virtual ~clCodePatch() {};

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	virtual bool BeginSave() { return Disassemble(); }
	virtual bool EndSave() { return true; }

	virtual bool BeginLoad()
	{
		TODO( "clear source and other stuff" )

		return true;
	}

	virtual bool EndLoad() { return Reassemble(); }

	/// Convert FSource<> to binary representation
	scriptmethod bool Reassemble();

	/// Convert instructions and operands to symbolic representation
	scriptmethod bool Disassemble();

	LString InstructionToString( const sInstruction& Instr ) const;

	bool AssembleInstruction( const LString& Line, LString& ErrorCode );

	//
	// clCodePatch
	//
	scriptmethod bool                 AssembleLine( const LString& InstrName, const LString& Args, LString& ErrorCode );
	scriptmethod bool                 AssembleCode( const std::vector<LString>& ACode, LString& ResultType, LString& ErrorCode );
	scriptmethod int                  GetLabelLink( const LString& String );
	scriptmethod LString              FindLabelByIP( int IP ) const;
	scriptmethod LString              GetString( int Index ) const;
	noexport     LString*             GetStringPtr( int Index );
	scriptmethod int                  GetStringsCount() const;
	virtual      clInstructions&      GetInstructions() { return FInstructions; };
	scriptmethod bool                 Link( LString& ErrorCode );

	/// FIXME : the following two methods are 'virtual' to make them  exportable to .NET, but they are really only 'scriptmethod'.

	virtual      LString              GetSourceLine( int i ) const { return FSource[i]; }
	virtual      void                 SetSourceLine( int i, const LString& S ) { FSource[i] = S; }
public:
	/** Property(Description="Assembler source code", Category="Assembly", Name=Code, Type=string, IndexType=int, FieldName=FSource, NetIndexedSetter=SetSourceLine, NetIndexedGetter=GetSourceLine) */

	/// Assembler source code
	std::vector<LString>  FSource;

private:
	int     EmitString( const LString& String );
	/// run peephole optimizer for this code patch
	bool    Optimize();
	/// return 'true' if instruction can be removed with no side effects
	bool    IsDeadInstruction( const sInstruction& Instr );
	/// return 'true' if two instructions can be replaced by a single one
	bool    PeepholeOptimizePair( const sInstruction& Instr1,
	                              const sInstruction& Instr2,
	                              sInstruction* OutInstruction );
	/// return 'true' if three instructions can be replaced by a single one
	bool    PeepholeOptimizeTriplet( const sInstruction& Instr1,
	                                 const sInstruction& Instr2,
	                                 const sInstruction& Instr3,
	                                 sInstruction* OutInstruction );
	/// remove Skip instructions starting from FromIP and update label tables
	void    CollapseInstructions( int FromIP, int Skip );
private:
	typedef std::map<LString, int> clLabelsTable;
	clInstructions                 FInstructions;
	LStr::clStringsVector          FStringTable;
	LStr::clStringsVector          FLabelLinks;
	/// map label names to IP jump offsets
	clLabelsTable                  FLabelsTable;
};

/// LinderScript virtual machine execution thread
class clExecutionThread
{
public:
	EXCEPTIONABLE;
	EXCEPTION( clExcept_InvalidOpCode, "Invalid opcode encountered while executing byte-code" );
public:
	clExecutionThread();
	virtual ~clExecutionThread();
	//
	// iExecutionThread interface
	//
	scriptmethod iStack*  GetStack() { return &FStack; };
	scriptmethod void     Execute( clScriptMethod* Method, clCodePatch* CodePatch, void* Self, clParametersList& Params );
	static  void          ClearExecutionStats();
	static  void          LogExecutionStats( sEnvironment* Env );
	/// opcodes executors (autogenerated stuff)
#  include "Generated/VM/ExecThread_MtdList.h"
private:
	// symbol stuff
	// field handler
	clFieldSymbolDeclaration* FFieldSymbol;
	iSymbolDeclaration*       FindSymbol( const LString& SymbolName );
private:
	LString     ExtractString();
	LString*    ExtractStringPtr();
	void        RecursiveEnter();
	void        RecursiveLeave();
	void        CallMethod( iObject* NativeObject, iMethod* Method, int Inherited );
	void        UndeclareVars();
private:
	typedef std::vector<clParamSymbolDeclaration*> clParamSymbols;
	typedef std::vector<clVarSymbolDeclaration*> clVarSymbols;
public:
	sEnvironment* Env;
private:
	iStack                  FStack;
	iStack                  FRecursionStack;
//   iStack                  FLocalVarsStack;
	clVarSymbols            FLocalVars;
	int                     FInstructionPointer;
	sInstruction            FCurrentInstruction;
	sInstruction*           FCurrentInstructionPtr;
	void*                   FCurrentNativeObject;
	iStaticClass*           FCurrentStaticClass;
	clParametersList*       FCurrentParameters;
	clCodePatch*            FCurrentCodePatch;
	clScriptMethod*         FCurrentMethod;
	int                     FMaxInstructions;
	clParamSymbols          FParamSymbols;
};

class scriptfinal clScriptField : public iField
{
public:
	clScriptField() : FFieldSize( 0 ) {}
	virtual ~clScriptField() {}

	//
	// iField interface
	//
	virtual void*    GetFieldPtr( iObject* obj );
	virtual int      GetFieldSize() { return FFieldSize; }

	virtual void     Resize( iObject* obj, int NewSize );
	//
	// clScriptField
	//
	inline void SetFieldSize( int sz ) { FFieldSize = sz; }
private:
	int FFieldSize;
};

class scriptfinal clScriptClass: public iStaticClass
{
public:
	clScriptClass() {}
	virtual ~clScriptClass() {}

	virtual LString     ClassName() const;

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	//
	// iStaticClass interface
	//
	virtual iObject*    VirtualConstructor( const clParametersList& Params ) const;
	virtual int         GetConstructorParamsCount( size_t ConstructorID ) const;
	virtual bool        IsAbstract() const { return false; };
	//
	// iStaticClass interface
	//
	virtual void    RegisterNonNativeFields( iObject* Object ) const;
	//
	// clScriptedStaticClass
	//
	virtual bool BeginLoad();

	/// register custom states, scripted fields and methods
	virtual bool EndLoad();

	/** Property(Name=Name, Type=string, FieldName=FClassName) */

	/// scripted classes support loading of custom states

	TODO( "figure out how to deal with the fields" )
	/** Property(Name=Fields, Type=iField, IndexType=int, FieldName=FScriptFields, NetIndexedGetter=GetScriptField, NetIndexedSetter=SetScriptField) */

	virtual iField*  GetScriptField( int i ) const { return FScriptFields[i]; }
	virtual void     SetScriptField( int i, iField* FF ) { FScriptFields[i] = FF; }

	virtual clState* GetScriptState( int i ) const { return FScriptStates[i]; }
	virtual void     SetScriptState( int i, clState* SS ) { FScriptStates[i] = SS; }

	/// scripted classes support loading of custom states
	/** Property(Name=States, Type=clState, IndexType=int, FieldName=FScriptStates, NetIndexedGetter=GetScriptState, NetIndexedSetter=SetScriptState) */

public:
	LString                FClassName;

	std::vector<iField*>   FScriptFields;
	std::vector<clState*>  FScriptStates;
private:
	inline void SetClassName( const LString& Name ) { FClassName = Name; }
	inline void BindTunneller( iStaticClass* Tunneller ) { FTunneller = Tunneller; }

	void          CallInitCode( iObject* Tunneller, const iStaticClass* TheClass ) const;
	iStaticClass* FindFirstNativeClass( iStaticClass* TheClass ) const;

	//
	mutable iStaticClass*    FTunneller;
};


using std::vector;

/// Serializable representation of Scripted parameter
class scriptfinal clScriptedParam : public iObject
{
public:
	clScriptedParam() {}
	virtual ~clScriptedParam();

	scriptmethod iParameter*    CreateParameter() const;

	iParameter* CreateParameter_Buf( void* Where, void* BufForParam ) const;

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

public:
	/** Property(Name=Name, Type=string, FieldName=FParamName) */
	/** Property(Name=Size, Type=int, FieldName=FParamSize) */
	/** Property(Name=Type, Type=string, Setter=SetParamType, Getter=GetParamType) */

	LString    FParamName;
	int        FParamSize;

	virtual LString GetParamType() const { return FParamSourceTypeName;}

	virtual void SetParamType( const LString& PName )
	{
		FParamType = PName;
		FParamSourceTypeName = FParamType;
		LStr::ToUpper( &FParamType );
	}

private:
	LString    FParamSourceTypeName;
	LString    FParamType;
};


/// Script method (static, class)
class scriptfinal clScriptMethod: public iMethod
{
public:
	clScriptMethod();
	virtual ~clScriptMethod();

	virtual void    AfterConstruction();

	/// Prepare for loading
	virtual bool    BeginLoad();

	/// Assemble and link loaded code
	virtual bool    EndLoad();

	SERIALIZABLE_CLASS()

	//
	// iMethod interface
	//
	virtual void           Invoke( void* ObjectAddr, clParametersList& Params );
	virtual int            GetParamsCount() const { return static_cast<int>( FParameters.size() ); }
	virtual int            GetParamSize( int Index ) { return FParameters[Index]->FParamSize; }
	virtual iParameter*    CreateParameter( int Index, void* InitialValue );
	virtual bool           IsScripted() const { return true; }

	// new stuff: fast invoke
	virtual bool           CreateParameters( iParameter** Params, void** ParamBuffers, void** ValueBuffers, void** InitialValues );
	virtual bool           InvokeFast( void* ObjectAddr, iParameter** Params );

	//
	// clScriptethod
	//
	inline LString GetParamName( int i ) const { return FParameters[i]->FParamName; }

	virtual LString        GetModifier( int i ) const { return FModifiers[i]; }
	virtual void           SetModifier( int i, const LString& Str ) { FModifiers[i] = Str; }

	virtual void             SetParamDescription( int i, clScriptedParam* P ) { FParameters[i] = P; }
	virtual clScriptedParam* GetParamDescription( int i ) const { return FParameters[i]; }

	/** Property(Name=Code,   Type=clCodePatch,   FieldName=FCode)   */
	/** Property(Name=Name,   Type=string, Getter=GetMethodName, Setter=SetMethodName ) */
	/** Property(Name=Modifiers, Type=string, IndexType=int, FieldName=FModifiers, NetIndexedGetter=GetModifier, NetIndexedSetter=SetModifier) */
	/** Property(Name=Parameters, Type=clScriptedParam, IndexType=int, FieldName=FParameters, NetIndexedGetter=GetParamDescription, NetIndexSetter=SetParamDescription) */
	/** Property(Name=Result, Type=clScriptedParam, FieldName=FResultDescription) */

public:
	clCodePatch*              FCode;
	clScriptedParam*          FResultDescription;
	vector<clScriptedParam*>  FParameters;
	vector<LString>           FModifiers;
};

#endif

/*
 * 15/12/2010
     vector<string> instead of string in Assemble()
 * 09/03/2010
     More comments added
 * 04/12/2007
     Emit* instructions ceased
 * 19/07/2007
     Optimize()
     PeepholeOptimizeTriplet()
 * 14/06/2007
     Merged with iCodePatch and OpCodes.h
 * 04/06/2007
     GetStringsCount()
 * 07/09/2005
     GetInstructions()
 * 01/08/2005
     It's here
*/
