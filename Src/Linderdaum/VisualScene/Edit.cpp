#include "Environment.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/RTTI/iStaticClass.h"
#include "Core/Script.h"

#include "Edit.h"

template<class T> int array_find( const LArray<T>& Array, const T& Value )
{
	for ( size_t j = 0 ; j < Array.size() ; j++ )
		if ( Array[j] == Value )
		{
			return ( int )j;
		}

	return -1;
}

/// Move the value with order breaking
template<class T> void erase_with_movement( LArray<T>& Array, int idx )
{
	if ( idx < Array.size() )
	{
		Array[idx].~T();
		Array[idx] = Array[Array.size() - 1];
	}

	Array.pop_back();
}

clEditor::clEditor(): FSelectedObject( NULL ), FCurrentTool( NULL )
{
}

clEditor::~clEditor()
{
}

void clEditor::RegisterCommands()
{
}

void clEditor::UnregisterCommands()
{
}

void clEditor::SelectC( const LString& Cmd )
{
	std::vector<std::string> Parts;
//	LStr::FastSplitLine(0, 100, Cmd, Parts);

	SelectGroup( Parts );
}

void clEditor::SetToolType( const LString& ToolType ) {}
void clEditor::SetTool( iObject* T ) {}
bool clEditor::CanUndo() { return false; }
bool clEditor::CanRedo() { return false; }
void clEditor::Undo() {}
void clEditor::Redo() {}

iTransaction* clEditor::MakeTransaction( const LString& TransType )
{
	return NULL;
}

void clEditor::UnselectC( const LString& Cmd )
{
	Unselect();
}

void clEditor::SetToolC( const LString& Cmd )
{
}

void clEditor::ObjectChanged( iObject* Obj )
{
	if ( !Obj ) { return; }

	if ( Obj == FSelectedObject )
	{
		/// handle object change. maybe inform the owner ?
	}
}

clSelectionGroup* clEditor::CreateSelectionGroup()
{
	clSelectionGroup* Group = Env->Linker->Instantiate( "clSelectionGroup" );

	Group->SetOwner( this );

	return Group;
}

bool clEditor::SelectGroup( const std::vector<LString>& IDs )
{
	if ( IDs.empty() ) { return false; }

	if ( IDs.size() == 1 ) { return SelectByName( IDs[0] ); }

	clSelectionGroup* Group = CreateSelectionGroup();

	for ( size_t j = 0 ; j < IDs.size() ; j++ )
	{
		iObject* Obj = Env->FindObject( IDs[j] );

		Group->AddObject( Obj );
	}

	Select( Group );

	return true;
}

void clEditor::Select( iObject* Obj )
{
	if ( !Obj ) { return; }

	Unselect();

	FSelectedObject = Obj;
}

bool clEditor::SelectByName( const LString& ID )
{
	iObject* Obj = Env->FindObject( ID );

	if ( !Obj ) { Env->Logger->LogP( L_NOTICE, "Object not found for selection: %s", ID.c_str() ); return false; }

	Select( Obj );

	return true;
}

void clEditor::Unselect()
{
	if ( !FSelectedObject ) { return; }

	if ( clSelectionGroup* G = dynamic_cast<clSelectionGroup*>( FSelectedObject ) )
	{
		// group is a special case, we have to destroy it
		G->SetOwner( NULL );

		delete G;
	}

	FSelectedObject = NULL;
}

iObject* clEditor::GetEditableObject() const
{
	iObject* Res = FSelectedObject;

	if ( clSelectionGroup* G = dynamic_cast<clSelectionGroup*>( Res ) )
	{
		Res = G->GetEditableProxy();
	}

	return Res;
}

clSelectionGroup::clSelectionGroup():
	FObjects(),
	FOwner( NULL ),
	FVirtualClass( NULL ),
	FEditableProxy( NULL )
{
}

clSelectionGroup::~clSelectionGroup()
{
	DeleteProxies();
}

void clSelectionGroup::AddObject( iObject* Object )
{
	/*
	   LArray<iObject*>::iterator ptr = FObjects.find(Object);

	   if (ptr != FObjects.end()) return;
	*/
	FObjects.push_back( Object );

	if ( FOwner ) { FOwner->ObjectChanged( this ); }
}

void clSelectionGroup::RemoveObject( iObject* Object )
{
	/*
	LArray<iObject*>::iterator ptr = FObjects.find(Object);

	if (ptr == FObjects.end()) return;

	FObjects.erase(ptr);
	*/
	if ( FOwner ) { FOwner->ObjectChanged( this ); }
}

void clSelectionGroup::DeleteProxies()
{
	if ( FEditableProxy ) { FEditableProxy->DisposeObject(); }

	if ( FVirtualClass ) { delete FVirtualClass; }

	FEditableProxy = NULL;
	FVirtualClass = NULL;
}

void clSelectionGroup::Clear()
{
	FObjects.clear();

	if ( FOwner ) { FOwner->ObjectChanged( this ); }

	DeleteProxies();
}

/// Get closest common ancestor in class hierarchy
iStaticClass* GetCommonAncestor( iStaticClass* Class1, iStaticClass* Class2 )
{
	/// Find common part
	if ( Class1->ClassName() == Class2->ClassName() ) { return Class1; }

	if ( Class1->InheritsFrom( Class2->ClassName() ) )
	{
		return GetCommonAncestor( Class1->GetSuperClass(), Class2 );
	}
	else
	{
		return GetCommonAncestor( Class1, Class2->GetSuperClass() );
	}
}

iStaticClass* clSelectionGroup::CreateCombinedMetaclass()
{
	if ( FObjects.empty() ) { return NULL; }

	clScriptClass* Res = Env->Linker->Instantiate( "clVirtualEditorProxyClass" );

	/// Find common ancestor class for each selected object
	/// Depth-first search on the hierarchy tree


	/// FObjects array is not empty
	iStaticClass* CommonClass = FObjects[0]->GetStaticClass();

	if ( FObjects.size() > 1 )
	{
		for ( LArray<iObject*>::iterator i = FObjects.begin() + 1; i != FObjects.end() ; ++i )
		{
			CommonClass = GetCommonAncestor( CommonClass, ( *i )->GetStaticClass() );
		}
	}

	/// set found class as the tuneller
	Res->SetSuperClass( CommonClass );

	return Res;
}

/// Just check if we need to recreate virtual metaclass and proxy object
iObject* clSelectionGroup::GetEditableProxy()
{
	bool Changed = false;

	if ( !FVirtualClass )
	{
		FVirtualClass = CreateCombinedMetaclass();
	}
	else
	{
		iStaticClass* NewClass = CreateCombinedMetaclass();

		if ( NewClass->GetSuperClassName() == FVirtualClass->GetSuperClassName() )
		{
			delete NewClass;
		}
		else
		{
			delete FVirtualClass;
			FVirtualClass = NewClass;

			Changed = true;
		}
	}

	if ( !FVirtualClass ) { return NULL; }

	if ( Changed )
	{
		if ( FEditableProxy ) { FEditableProxy->DisposeObject(); }

		FEditableProxy = NULL;
	}

	if ( !FEditableProxy )
	{
		FEditableProxy = FVirtualClass->VirtualConstructor( clParametersList() );
	}

	return FEditableProxy;
}


LString clCompositeConstraint::CanPlaceHere( iObject* Obj )
{
	// TODO: check each constraint
	return "";
}

clPicker::clPicker() {}
clPicker::~clPicker() {}

void clPicker::StartSelection( const LVector3& StartPos ) {}
void clPicker::CancelSelection() {}
iObject* clPicker::EndSelection( const LVector3& EndPos ) { return NULL; }
