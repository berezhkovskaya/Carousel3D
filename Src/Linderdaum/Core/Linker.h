/**
 * \file Linker.h
 * \brief Main factory for all classes derived from iObject
 * \version 0.5.99
 * \date 13/12/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _Linker_
#define _Linker_

#include <map>

#include "Platform.h"
#include "Core/iObject.h"

#include "Utils/Utils.h"

#define REGISTER_CLASS(linker, class)  { \
                                  iStaticClass* Class = (new clNativeStaticClass<class>); \
                                  linker->RegisterStaticClass( Class ); \
                               }

class iSymbolDeclaration;
class iMethod;
class iIStream;
class mlNode;

/// Main factory for all classes derived from iObject
class scriptfinal clLinker: public iObject
{
private:
	struct sInstantiationProxy
	{
		friend class clLinker;
	private:
		iObject* FObject;
		sEnvironment* Env;
		sInstantiationProxy( iObject* Obj, sEnvironment* E ): FObject( Obj ), Env( E ) {};
	public:
		template<typename T> operator T()
		{
			return ::Linderdaum::Utils::guarded_cast<T>( FObject, "Invalid class type for: " + FObject->ClassName() + ". Expected descendant of " + typeid( T ).name(), Env );
		}
	};

	/// Do we throw errors on incorrect ML-serialization
	bool    IsFatalOnErrors() const;

public:
	clLinker();
	virtual ~clLinker();

	NET_EXPORTABLE()

	//
	// clLinker
	//
	/**
	  Adds a static class to the global classes registry so it can be instantiated
	  lately via Instantiate() call
	**/
	scriptmethod void                RegisterStaticClass( iStaticClass* StaticClass );
	/**
	  Loads a static class from file and adds it to the global classes registry.
	**/
	scriptmethod iStaticClass*       LoadStaticClass( const LString& FileName );
	/**
	  Returns a static class that can instantiate objects of class ClassName
	**/
	scriptmethod iStaticClass*       FindStaticClass( const LString& ClassName ) const;

	/**
	  Rebuild virtual tables of all loaded classes
	**/
	scriptmethod void RebuildVirtualTables();
public:

	scriptmethod bool SetObjectProperty( const LString& ObjID, const LString& PropertyName, const LString& PropertyValue );

	noexport     bool LoadObject( mlNode* Node, iObject** Result ) const;
	scriptmethod bool LoadPropertyForObject( iObject* Obj, mlNode* Node ) const;
	noexport     bool SaveObject( iObject* Obj, mlNode** Result ) const;

	noexport     bool LoadObjectFromFile( const LString& FileName, iObject** Result ) const;
	scriptmethod bool SaveObjectToFile( const LString& FileName, iObject* Obj ) const;

	scriptmethod bool LoadObjectsListFromStream( iIStream* Stream, std::vector<iObject*>* ObjectList ) const;
	scriptmethod bool LoadObjectsList( mlNode* XLMLTree, std::vector<iObject*>* ObjectList ) const;
	template <class T> bool LoadTypedObjectsListFromStream( iIStream* Stream, std::vector<T*>* ResultsList ) const
	{
		std::vector<iObject*> ObjectsList;

		bool Res = LoadObjectsListFromStream( Stream, &ObjectsList );

		if ( !Res ) { return false; }

		ResultsList->resize( ObjectsList.size() );

		for ( size_t i = 0; i != ObjectsList.size(); i++ )
		{
			( *ResultsList )[i] = dynamic_cast<T*>( ObjectsList[i] );

			// cannot cast object
			if ( ( *ResultsList )[i] == NULL )
			{
				TODO( "set the error code here" )
				return false;
			}
		}

		return true;
	}

	/**
	  Instantiates a specified class and downcasts it to the desired subclass
	  (using some templates magic) performing all necessary typechecking.
	  This method generates a runtime exception if downcasting is impossible.
	**/
	sInstantiationProxy    Instantiate( const LString& ClassName ) const
	{
		return sInstantiationProxy( InstantiateClassByName( ClassName ), Env );
	};

	scriptmethod iObject*            InstantiateClassByName( const LString& ClassName ) const;

#pragma region Static methods management
	scriptmethod void                RegisterStaticMethod( const LString& MethodName, iMethod* Method );
	scriptmethod void                LoadStaticMethod( const LString& FileName );
	scriptmethod iMethod*            GetStaticMethod( int idx );
	scriptmethod int                 GetStaticMethodIndex( const LString& MethodName );
	scriptmethod iMethod*            FindStaticMethod( const LString& MethodName );
#pragma endregion

#pragma region Global symbols management
	scriptmethod void                RegisterPODSymbol( const LString& SName, void* Ptr );
	scriptmethod void                RegisterConstPODSymbol( const LString& SName, const void* Ptr );
	scriptmethod void                RegisterClassRefSymbol( const LString& SName, iObject* Ref );
	scriptmethod void*               GetGlobalVarAddress( int idx );
	scriptmethod int                 GetGlobalVarIndex( const LString& SymbolName );
#pragma endregion

#pragma region Plugins and scripted assembly management
	scriptmethod void                LoadAsm( const LString& FileName );
	scriptmethod void                LoadScriptDirectory( const LString& DirName );
#pragma endregion

	/// Immediate execution
	scriptmethod LString             ExecuteCode( const std::vector<LString>& Code );
private:
	typedef std::map<LString, iStaticClass*>                      clClassesRepository;
	typedef std::vector<std::pair<LString, iMethod*> >            clMethodsRepository;
	typedef std::vector<std::pair<LString, iSymbolDeclaration*> > clSymbolsRepository;

	clClassesRepository    FClassesRepository;
	clMethodsRepository    FStaticMethods;
	clSymbolsRepository    FGlobalSymbols;
};

#endif

/*
 * 18/01/2010
     Some 'noexport' defines to avoid errors with LSDC/Script compiler
 * 13/12/2010
     RebuildVirtualTables()
 * 30/04/2009
     Merged with iLinker
 * 30/09/2007
     Linear addressing for global fields/static methods
 * 09/07/2007
     CreateHierarchyFromStream()
 * 12/06/2007
     Global symbols repository
 * 10/06/2007
     Static methods repository
 * 13/04/2007
     CreateHierarchy()
 * 10/11/2005
     CreateExecutionThread()
 * 02/12/2004
     Deploy()
     Initialize()
     InstantiateClassByName()
 * 01/12/2004
     It's here
*/
