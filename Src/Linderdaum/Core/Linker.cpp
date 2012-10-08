/**
 * \file Linker.cpp
 * \brief Main factory for all classes derived from iObject
 * \version 0.5.99
 * \date 13/12/2010
 * \author Sergey Kosarevsky, 2004-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Linker.h"

#include <algorithm>

#include "Engine.h"
#include "Core/RTTI/iStaticClass.h"
#include "Core/Logger.h"
#include "Core/Script.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"
#include "Environment.h"

#include "Core/VFS/ML.h"

#include "Core/RTTI/Method.h"
#include "Core/RTTI/Symbol.h"

clLinker::clLinker() : FStaticMethods()
{
}

bool clLinker::SetObjectProperty( const LString& ObjID, const LString& PropertyName, const LString& PropertyValue )
{
	if ( iObject* Obj = Env->FindObject( ObjID ) )
	{
		return Obj->SetPropertyValue( PropertyName, PropertyValue );
	}

	return false;
}

void clLinker::RegisterStaticClass( iStaticClass* StaticClass )
{
	iStaticClass* OldClass = FindStaticClass( LString( StaticClass->ClassName() ) );

	if ( OldClass != NULL )
	{
		LString OldClassName( StaticClass->ClassName() );
		FATAL_MSG( "Duplicate static class: " + OldClassName );
	}

	StaticClass->Env = Env;

	FClassesRepository[ LString( StaticClass->ClassName() ) ] = StaticClass;
}

void clLinker::RebuildVirtualTables()
{
	clClassesRepository::iterator Iter;

	for ( Iter = FClassesRepository.begin(); Iter != FClassesRepository.end(); ++Iter )
	{
		Iter->second->BuildVirtualTables();
	}
}

iObject* clLinker::InstantiateClassByName( const LString& ClassName ) const
{
	iStaticClass* StaticClass = FindStaticClass( ClassName );

	if ( this->IsFatalOnErrors() )
	{
		FATAL( StaticClass == NULL, "Class not found: " + ClassName );
	}
	else
	{
		if ( !StaticClass )
		{
			Env->Logger->LogP( L_WARNING, "Linker: Unable to instantiate Class <%s>", ClassName.c_str() );
			return NULL;
		}
	}

	iObject* Obj = StaticClass->DefaultConstructor();

	return Obj;
}

iStaticClass* clLinker::FindStaticClass( const LString& ClassName ) const
{
	clClassesRepository::const_iterator Iter = FClassesRepository.find( ClassName );

	return ( Iter == FClassesRepository.end() ) ? NULL : ( Iter->second );
}

void clLinker::LoadStaticMethod( const LString& FileName )
{
	clScriptMethod* ScriptMethod = Instantiate( "clScriptMethod" );

	iObject* Ptr = ScriptMethod;

	if ( !LoadObjectFromFile( FileName, &Ptr ) ) { return; }

	RegisterStaticMethod( ScriptMethod->GetMethodName(), ScriptMethod );
}

iStaticClass* clLinker::LoadStaticClass( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	clScriptClass* ScriptedClass = Instantiate( "clScriptClass" );

	iObject* Ptr = ScriptedClass;

	if ( !Env->Linker->LoadObjectFromFile( FileName, &Ptr ) ) { return NULL; }

	clScriptClass* OldClass = dynamic_cast<clScriptClass*>( FindStaticClass( LString( ScriptedClass->ClassName() ) ) );

	if ( OldClass )
	{
		delete( ScriptedClass );

		Ptr = OldClass;

		if ( LoadObjectFromFile( FileName, &Ptr ) ) { return NULL; }

		return OldClass;
	}
	else
	{
		RegisterStaticClass( ScriptedClass );

		return ScriptedClass;
	}

	unguard();
}

clLinker::~clLinker()
{
	// 1. deallocate all classes
	::Linderdaum::Utils::DeallocatePairs( FClassesRepository.begin(), FClassesRepository.end() );

	FClassesRepository.clear();

	// 2. deallocate all static methods
	for ( clMethodsRepository::iterator i = FStaticMethods.begin() ; i != FStaticMethods.end() ; ++i )
	{
		delete i->second;
	}

	FStaticMethods.clear();

	// 3. deallocate registered symbols
	for ( clSymbolsRepository::iterator i = FGlobalSymbols.begin() ; i != FGlobalSymbols.end() ; ++i )
	{
		delete i->second;
	}

	FGlobalSymbols.clear();
}

void clLinker::RegisterStaticMethod( const LString& MethodName, iMethod* Method )
{
	FStaticMethods.push_back( std::pair<LString, iMethod*>( MethodName, Method ) );
}

TODO( "move it to a separate file ?" )

class clGlobalPODSymbolDeclaration : public iSymbolDeclaration
{
public:
	clGlobalPODSymbolDeclaration( void* ptr )
	{
		DataPtr = ptr;
	}
	virtual ~clGlobalPODSymbolDeclaration() {}

	virtual VMPTR GetSymbolAddress()
	{
		return reinterpret_cast<VMPTR>( DataPtr );
	}

	virtual void Resize( int NewSize )
	{
//		FATAL_MSG( "Cannot resize global POD symbol" );
	}
private:
	void* DataPtr;
};

class clGlobalClassSymbolDeclaration : public iSymbolDeclaration
{
public:
	clGlobalClassSymbolDeclaration( iObject* ptr )
	{
		ClassPtr = ptr;
	}
	virtual ~clGlobalClassSymbolDeclaration() {}

	virtual VMPTR GetSymbolAddress()
	{
		return reinterpret_cast<VMPTR>( &ClassPtr );
	}

	virtual void Resize( int NewSize )
	{
//		FATAL_MSG( "Cannot resize global Ref symbol" );
	}
private:
	iObject*    ClassPtr;
};

void clLinker::RegisterPODSymbol( const LString& SName, void* Ptr )
{
	// allocate symbol handler with direct data pointer
	FGlobalSymbols.push_back( std::pair<LString, iSymbolDeclaration*>( SName, new clGlobalPODSymbolDeclaration( Ptr ) ) );
}

void clLinker::RegisterConstPODSymbol( const LString& SName, const void* Ptr )
{
	// allocate symbol handler with direct data pointer
	FGlobalSymbols.push_back( std::pair<LString, iSymbolDeclaration*>( SName, new clGlobalPODSymbolDeclaration( const_cast<void*>( Ptr ) ) ) );
}

void clLinker::RegisterClassRefSymbol( const LString& SName, iObject* Ref )
{
	// allocate symbol handler and convert class reference to simple pointer
	FGlobalSymbols.push_back( std::pair<LString, iSymbolDeclaration*>( SName, new clGlobalClassSymbolDeclaration( Ref ) ) );
}

void* clLinker::GetGlobalVarAddress( int idx )
{
	return FGlobalSymbols[idx].second->GetSymbolAddress();
}

iMethod* clLinker::GetStaticMethod( int idx )
{
	return FStaticMethods[idx].second;
}

int clLinker::GetStaticMethodIndex( const LString& MethodName )
{
	//
	for ( size_t i = 0 ; i < FStaticMethods.size() ; i++ )
	{
		if ( FStaticMethods[i].first == MethodName )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

iMethod* clLinker::FindStaticMethod( const LString& MethodName )
{
	iMethod* Method = NULL;

	int idx = GetStaticMethodIndex( MethodName );

	if ( idx > -1 )
	{
		Method = GetStaticMethod( idx );
	}

	return Method;
}

int clLinker::GetGlobalVarIndex( const LString& SymbolName )
{
	//
	for ( size_t i = 0 ; i < FGlobalSymbols.size() ; i++ )
	{
		if ( FGlobalSymbols[i].first == SymbolName )
		{
			return static_cast<int>( i );
		}
	}

	return -1;
}

void clLinker::LoadAsm( const LString& FileName )
{
	LString _method = "%Method.";

	if ( FileName.find( _method ) != std::string::npos )
	{
		// it is a method
		LoadStaticMethod( FileName );
	}
	else
	{
		// it is a class
		LoadStaticClass( FileName );
	}
}

void clLinker::LoadScriptDirectory( const LString& DirName )
{
	// scan directory and load each .asm file using Linker::LoadAsm()
	LStr::clStringsVector Files;

	Env->FileSystem->EnumerateFiles( DirName, true, &Files );

	for ( size_t i = 0; i < Files.size(); i++ )
	{
		// 1. extract extension
		LString FileExt;

		Env->FileSystem->SplitPath( Files[i], NULL, NULL, NULL, &FileExt );

		// 2. check if it is an .asm-file
		if ( FileExt == ".ASM" )
		{
			LoadAsm( Files[i] );
		}
	}
}

LString clLinker::ExecuteCode( const std::vector<LString>& Code )
{
	// 1. create temporary codepatch
	clCodePatch TmpCode;
	TmpCode.Env = Env;
	TmpCode.AfterConstruction();

	LString CodeError;
	LString ResultType;

	// FIXME: DOES NOT WORK

	if ( !TmpCode.AssembleCode( Code, ResultType, CodeError ) )
	{
		// error while assembling
		return "";
	}

	if ( !TmpCode.Link( CodeError ) )
	{
		// error while linking
		return "";
	}

	// 2. run execthread
	clExecutionThread Thread;
	Thread.Env = Env;

	clParametersList ZeroParams;

	Thread.Execute( NULL, &TmpCode, NULL, ZeroParams );

	TODO( "extract result" )

	// 3. extract result

	LString ResType = LStr::GetToken( ResultType, 1 );
	int ResSize     = LStr::ToInt( LStr::GetToken( ResultType, 2 ) );

// Env->Logger->Log(L_DEBUG, "Done assembling : "+Code);
// Env->Logger->Log(L_DEBUG, "ResultType = "+ResType);
// Env->Logger->Log(L_DEBUG, "ResultSize = "+ResultType.GetToken(2));

	if ( LStr::GetUpper( ResType ) == "VOID" || ResSize == 0 )
	{
		// "void" result
		return "";
	}

	FIXME( "should be handled in compiler?" )

	if ( ResType == "LString" )
	{
		ResType = "STRING";
	}
	else
	{
		if ( FindStaticClass( ResType ) != NULL )
		{
			ResType = "REF"; // the type of result is reference
		}
	}

	// use the code from ScriptMethod.cpp
	// to allocate iParameter by its type name
	clScriptedParam* TmpParam = Instantiate( "clScriptedParam" );
	TmpParam->FParamName = "$$TMP_LINKER_PARAM";
	TmpParam->FParamSize = ResSize;
	TmpParam->SetParamType( ResType );

	iParameter* ResultRetriever = TmpParam->CreateParameter();

	delete TmpParam;

	//
	ResultRetriever->PopFromStack( Thread.GetStack() );

	LString RealResult = ResultRetriever->ToString();

	// clean up
	delete ResultRetriever;

	return RealResult;
}

#if 0

bool Linker::RunAppletFile( const LString& FileName )
{
	// load Applet instance
	iObject* Object = NULL;

	// FIXME : use LoadTypedObject with custom error message

	if ( !Env->Linker->LoadObjectFromFile( FileName, &Object ) )
	{
		FLastError = Env->Linker->GetLastError();

		return false;
	}

	Applet* Applet = safe_cast<Applet*>( Object );

	if ( !Script )
	{
		FLastError = string( "Object loaded from " ) + FileName + string( " is not an Applet" );

		return false;
	}

	// call Run
	if ( !Applet->Run() )
	{
		// store error codes
		FLastError = Applet->GetLastError();

		return false;
	}

	delete Applet;

	return true;
}

#endif

bool clLinker::IsFatalOnErrors() const
{
#ifdef OS_ANDROID
	return true;
#else
	return true;
#endif
}

// Some code for new serialization scheme

bool clLinker::SaveObject( iObject* Obj, mlNode** Result ) const
{
	if ( !Obj->BeginSave() ) { return false; }

//	mlSectionNode* ObjNode = new mlSectionNode();
	mlNode* ObjNode = new mlNode();
	ObjNode->isSection = true;
	ObjNode->setID( "Object" );
	ObjNode->setValue( Obj->NativeClassName() );

	/// save each property
	for ( int i = 0 ; i < Obj->GetStaticClass()->GetPropertiesCount() ; i++ )
	{
		iProperty* Prop = Obj->GetStaticClass()->GetPropertyByIndex( i );

		// if this property must be serialized - do so
		LString Name = Prop->GetName();

		if ( Prop->Serialized() && !Prop->IsDefaultValue( Obj ) )
		{
			mlNode* SubNode = NULL;

			if ( !Prop->Save( Obj, &SubNode ) )
			{
				// delete created tree and return failure
				delete ObjNode;

				return false;
			}

			ObjNode->children.push_back( SubNode );
		}
	}

	if ( !Obj->EndSave() )
	{
		delete ObjNode;
		return false;
	}

	*Result = ObjNode;

	return true;
}

bool clLinker::LoadPropertyForObject( iObject* Obj, mlNode* Node ) const
{
	// skip comments
//   if ( dynamic_cast<mlCommentNode*>( Node ) ) return true;

	if ( Node->getID().empty() )
	{
		// pass to the first child. this is a quirk of the parser
		if ( Node->children.size() < 1 )
		{
			TODO( "store last error" )
			return false;
		}

		mlNode* Child = Node->children[0];
		return LoadPropertyForObject( Obj, Child );
	}

	// Continue result parametrization
	iProperty* Prop = Obj->GetStaticClass()->FindProperty( Node->getID() );

	if ( !Prop )
	{
		if ( this->IsFatalOnErrors() )
		{
			FATAL_MSG( "Property '" + Node->getID() + "' not found in class '" + Obj->ClassName() + "'" );
			// we are dead here
		}

		// property(section name) not found : handle this here
		return false;
	}

	return Prop->Load( Obj, Node );
}

bool clLinker::LoadObject( mlNode* Node, iObject** Result ) const
{
	if ( Result == NULL )
	{
		return false;
	}

	// we skip nodes with empty IDs and pass to the first child
	if ( Node->getID().empty() )
	{
		TODO( "store last error" )

		if ( Node->children.size() < 1 ) { return false; }

		return LoadObject( Node->children[0], Result );
	}

	if ( ( *Result == NULL ) || ( Node->getID() == "Object" ) )
	{
		if ( Node->getValue() == "NULL" )
		{
			*Result = NULL;

			return true;
		}

		/// if the object is null, then create it. otherwise - do the parametrization
		if ( *Result == NULL ) { *Result = InstantiateClassByName( Node->getValue() ); }

		if ( !IsFatalOnErrors() )
		{
			if ( !( *Result ) )
			{
				Env->Logger->Log( L_WARNING, "The result could not be instantiated" );

				return false;
			}
		}

		TODO( "save the error" )

		if ( !( *Result )->BeginLoad() ) { return false; }

		/// now iterate each property
//		mlSectionNode* Section = dynamic_cast<mlSectionNode*>( Node );

		// Node->isSection is irrelevant here
		for ( size_t i = 0 ; i < Node->children.size() ; i++ )
		{
			mlNode* Child = Node->children[i];

			if ( !LoadPropertyForObject( *Result, Child ) ) { return false; }
		}

		TODO( "save the error" )

		if ( !( *Result )->EndLoad() ) { return false; }

		return true;
	}
	else
	{
		/// Continue result parametrization
		return LoadPropertyForObject( *Result, Node );
	}
}

bool clLinker::LoadObjectFromFile( const LString& FileName, iObject** Result ) const
{
	mlNode* Node = Env->FileSystem->LoadXLML( FileName );

	TODO( "store error from FileSystem" )

	if ( !Node ) { return false; }

	bool Res = LoadObject( Node, Result );

	delete Node;

	return Res;
}

bool clLinker::SaveObjectToFile( const LString& FileName, iObject* Obj ) const
{
	mlNode* Node;

	// we already have the error here, so just return
	if ( !SaveObject( Obj, &Node ) ) { return false; }

	return Env->FileSystem->SaveXLML( Node, FileName );
}

bool clLinker::LoadObjectsListFromStream( iIStream* Stream, std::vector<iObject*>* ObjectList ) const
{
	mlNode* XLMLTree = Env->FileSystem->LoadXLMLFromStream( Stream );

	return LoadObjectsList( XLMLTree, ObjectList );
}

bool clLinker::LoadObjectsList( mlNode* XLMLTree, std::vector<iObject*>* ObjectList ) const
{
	/*
	if ( XLMLTree->getID().empty() )
	{
	   TODO( "store last error" )
	   if ( XLMLTree->children.size() < 1 )
	   {
	      return false;
	   }

	   return LoadObjectsList( XLMLTree->children[0], ObjectList );
	}
	*/

	ObjectList->reserve( XLMLTree->children.size() );
	ObjectList->resize( 0 );

	for ( size_t i = 0; i != XLMLTree->children.size(); i++ )
	{
		// skip commens
//      if ( dynamic_cast<mlCommentNode*>( XLMLTree->children[i] ) ) continue;

		// insert a placeholder for a new object
		ObjectList->push_back( NULL );

		bool Res = LoadObject( XLMLTree->children[i], &( ObjectList->back() ) );

		if ( !Res )
		{
			for ( size_t j = 0; j != ObjectList->size(); j++ ) { ( *ObjectList )[j]->DisposeObject(); }

			return false;
		}
	}

	return true;
}

/*
 * 13/12/2010
     RebuildVirtualTables()
 * 05/04/2009
     Memory leaks fixed
 * 13/08/2007
     Immediate code execution
 * 30/07/2007
     Package management : LoadASM / LoadDir
 * 09/07/2007
     CreateHierarchyFromStream()
 * 12/06/2007
     Global symbols repository
 * 10/06/2007
     Static methods repository
 * 26/04/2007
     RegisterClasses() deprecated
 * 13/04/2007
     CreateHierarchy()
 * 02/12/2005
     Fixed memory leak in static loaders repository
 * 15/11/2005
     Registration of clOGGLoader
 * 08/11/2005
     Registration of exported tunnellers
 * 03/11/2005
     Deploy() deprecated
 * 25/10/2005
     FindStaticClass()
 * 13/06/2005
     Clean up
 * 12/06/2005
     More logging information in Deploy()
 * 17/03/2005
     Static classes for scenegraph nodes
 * 21/01/2005
     std::find() usage in InstantiateClassByName()
 * 03/12/2004
     RegisterClass()
 * 02/12/2004
     First working version
     Deploy()
     InstantiateClassByName()
 * 01/12/2004
     It's here
*/
