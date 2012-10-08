#ifndef __h__edit__included__
#define __h__edit__included__

#include "Core/iObject.h"

#include "Utils/LArray.h"

class clSelectionGroup;

class iStaticClass;
class clScene;

class iTransaction;
class iTool;

/**
   \brief High-level editing process control

   This class controls changes of any property and provides facilities for object selection.

   LEditor is not a visual component, it is just a selection/tool state holder
*/
class scriptfinal netexportable clEditor: public iObject
{
public:
	clEditor();
	virtual ~clEditor();

#pragma region Selection tracking

	/// Select a single object by name. False if object not found
	scriptmethod bool SelectByName( const LString& ID );

	/// Select a single object
	scriptmethod void Select( iObject* Obj );

	/// Clear current selection
	scriptmethod void Unselect();

	/// Get current selection as it is (iObject or SelectionGroup)
	scriptmethod iObject* GetSelection() const { return FSelectedObject; }

	/// If the group is selected, an editable proxy is returned
	scriptmethod iObject* GetEditableObject() const;

	/// Create a group selection object. False if some objects are not found
	bool SelectGroup( const std::vector<LString>& IDs );

	/// Shortcut to the Linker->Create() for SelectionGroup. Also assigns owner for the created group
	scriptmethod clSelectionGroup* CreateSelectionGroup();

	/// Inform the editor about some unregistered object change
	scriptmethod void ObjectChanged( iObject* Obj );

#pragma endregion

#pragma region Tool management

	/// Change current tool
	scriptmethod void SetToolType( const LString& ToolType );

	/// Set concrete tool using current selection
	scriptmethod void SetTool( iObject* T );

	/// Return active tool
	scriptmethod iTool* GetTool() const { return FCurrentTool; }

#pragma endregion

#pragma region Editing transaction management

	/// Can the "Undo" action be done
	scriptmethod bool CanUndo();

	/// Can the "Redo" action be done
	scriptmethod bool CanRedo();

	/// Undo last transaction
	scriptmethod void Undo();

	/// Redo last transaction
	scriptmethod void Redo();

	/// Current transaction index
	scriptmethod int GetCurrentTransaction() const { return FCurrentTransaction; }

	scriptmethod size_t GetTransactionCount() const { return FTransactions.size(); }

	scriptmethod iTransaction* GetTransaction( int i ) const { return FTransactions[i]; }

	/// Make a transaction and store it in internal list
	scriptmethod iTransaction* MakeTransaction( const LString& TransType );

#pragma endregion

private:
	/// Currently selected object
	iObject* FSelectedObject;

	/// Currently active tool
	iTool*   FCurrentTool;

	/// List of transactions made
	LArray<iTransaction*> FTransactions;

	int FCurrentTransaction;

private:
	void RegisterCommands();
	void UnregisterCommands();

	/// Command handlers. No script yet, so we just use old-school cmds
	void SelectC( const LString& Cmd );
	void UnselectC( const LString& Cmd );
	void SetToolC( const LString& Cmd );
};

/**
   \brief Virtual object representing a selection group

   Selection group is a collection of object handles.

   This is a tricky little class which extract all common properties from each of selected objects
   and changes them appropriately. E.g., if some Light and Mesh are selected, the properties in this class would be Position etc.

   Contains special metaclass with a combined list of properties
   Subscribed to each object's EVENT_OBJECT_DESTROYED to keep track of object deletion
*/
class scriptfinal netexportable clSelectionGroup: public iObject
{
public:
	clSelectionGroup();
	virtual ~clSelectionGroup();

	/// Add object to selection
	scriptmethod void     AddObject( iObject* Object );

	/// Remove object from selection
	scriptmethod void     RemoveObject( iObject* Object );

	/// Selected object count
	scriptmethod int      GetObjectCount() const { return ( int )( FObjects.size() ); }

	/// Get indexed subobject
	scriptmethod iObject* GetObjectIdx( int idx ) const { return FObjects[idx]; }

	/// Delete all objects from group
	scriptmethod void     Clear();

	/// Create a proxy object
	scriptmethod iObject* GetEditableProxy();

	/// Set link to editor
	scriptmethod void     SetOwner( clEditor* Ed ) { FOwner = Ed; }

	/// Get link to editor
	scriptmethod clEditor* GetOwner() const { return FOwner; }

private:
	/// List of selected objects
	LArray<iObject*> FObjects;

	/// Link to owner
	clEditor* FOwner;

	/// Dynamically created metaclass with common properties for the objects in group
	iStaticClass* FVirtualClass;

	/// Object proxy with FVirtualClass inside
	iObject* FEditableProxy;

	/// Iterate objects and extract properties from their common metaclass
	iStaticClass* CreateCombinedMetaclass();

	/// Delete temporary combined objects
	void DeleteProxies();
};


class scriptfinal iTransaction: public iObject
{
public:
	iTransaction() {}
	virtual ~iTransaction() {}
};

/*
class PropertyChangeTransaction: public iTransaction
{
};

class ObjectDeletionTransaction: public iTransaction
{
};

class ObjectCreationTransaction: public iTransaction
{
};

class Transactor
{
public:
   Undo()
   Redo()

   Undoable()
   Redoable()

   Apply()
};

/// Change a single propery
class PropertyModifier: public Transactor
{
};
*/

/**
   \brief Unified tool object. Used for selection/item creation/placement/modification

   Handles clicks, key presses and other user input. Also can provide gui for keyboard param entry
*/
class netexportable scriptfinal iTool: public iObject
//Transactor
{
public:
	iTool(): FEditor( NULL ) {}
	virtual ~iTool() {}

public:
	/// Link to owner
	clEditor* FEditor;
};


/**
   Picker handles object selection.

   Selection object(s) is/are stored in Editor

   When the tool changes it (the new tool) uses selected objects
*/
class netexportable scriptfinal clPicker: public iTool
{
public:
	clPicker();
	virtual ~clPicker();

#pragma region Multiple section

	/// Start selection at given position
	scriptmethod void StartSelection( const LVector3& StartPos );

	/// Cancel selection
	scriptmethod void CancelSelection();

	/// Finalize selection and return selected object
	scriptmethod iObject* EndSelection( const LVector3& EndPos );

#pragma endregion
public:
	/// Selection start pos
	LVector3 FStartPosition;

	/// Link to scene where the selection goes
	clScene* FScene;
};

#pragma region Placement constraints

/**
   Positioning/Orientation can be limited by a number of constraints
*/
class netexportable iPlacementConstraint: public iObject
{
public:
	iPlacementConstraint() {}
	virtual ~iPlacementConstraint() {}

	/// Supports snap-to operation
	virtual bool CanSnap() { return false; }

	/// Snap to grid, heightmap or something else
	virtual bool Snap() { return true; }

	/// Check if an object can be placed in current location. Returns error message
	virtual LString CanPlaceHere( iObject* Obj ) { return ""; }

public:
	/// Enable flag
	bool FEnabled;
};
/*
/// Bounding box which limits position and orientation
class netexportable BoundingBoxConstraint: public iPlacementConstraint
{
};

/// Constraint allowing object placement on a heightmap
class netexportable HeightmapConstraint: public iPlacementConstraint
{
public:
   HeightmapConstraint();

public:
   /// Up and direction vectors
   LVector3 FV1, FV2, FUp;
};

/// Regular grid supporting snap operations
class netexportable GridConstraint: public iPlacementConstraint
{
public:
   GridConstraint() {}
};
*/
class scriptfinal netexportable clCompositeConstraint: public iPlacementConstraint
{
public:
	clCompositeConstraint() {}
	virtual ~clCompositeConstraint() { ClearConstraints(); }

	scriptmethod void AddConstraint( iPlacementConstraint* C ) { FConstraints.push_back( C ); }
	scriptmethod void ClearConstraints() { for ( size_t i = 0 ; i < FConstraints.size() ; i++ ) { delete FConstraints[i]; } FConstraints.clear(); }
	scriptmethod size_t ConstraintCount() { return FConstraints.size(); }

	/// Check each constaint
	virtual LString CanPlaceHere( iObject* Obj );

public:
	/// List of placement constraints
	LArray<iPlacementConstraint*> FConstraints;
};

#pragma endregion

/**
   \brief Something which affects geometric properties of an object

   Can contain an attached gizmo object which does all the job
*/
class scriptfinal netexportable clSpatialTool: public iTool
{
public:
	clSpatialTool() {}
	virtual ~clSpatialTool() {}

public:
	/** Pr_operty(Name=Constraints, Type=CompositeConstraint, FieldName=FConstraints) */

	clCompositeConstraint* FConstraints;
};
/*
class netexportable Placer: public SpatialTool {};

class netexportable Rotator: public iTool
{
};
*/
#endif
