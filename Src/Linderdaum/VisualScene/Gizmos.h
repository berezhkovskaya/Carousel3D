/**
 * \file Gizmos.h
 * \brief Transformation gizmos
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Gizmos__h__included__
#define __Gizmos__h__included__

#include "VisualModifier.h"

enum LGizmoCoordSys
{
	COORDS_Local  = 0,
	COORDS_World  = 1,
	COORDS_Screen = 2 // View
};

class clGeom;
class clMaterial;
class clCVar;

/**
   \brief Generic visual modifier for 3d property tweaking

   Gizmos affect Rotation, Translation and Scale of some object.
   They can be binded to Matrix4/Vec3/Quat property, direct pointer or console variable.

   To modify something in an interactive mode one must use the following template code:

   "cl_SomeGizmo" * G = VisScene->Add...PropMtx( Object, "PropName" );
   G->Register(); // add event handlers for key/mouse/timer
   .. the framework itself handles gizmos
   G->DisposeObject(); // remove after use
*/
class scriptfinal netexportable clGizmo : public clVisualModifier
{
public:
	clGizmo();
	virtual ~clGizmo();

	virtual void AfterConstruction();

	scriptmethod void BindToPropVector( iObject* Obj, const LString& Prop );
	scriptmethod void BindToPropMatrix( iObject* Obj, const LString& Prop );
	scriptmethod void BindToPropQuat( iObject* Obj, const LString& Prop );
	scriptmethod void BindToVarVector( const LString& VarName );
	scriptmethod void BindToVarMatrix( const LString& VarName );
	scriptmethod void BindToVarQuat( const LString& VarName );
	void BindToVectorPtr( LVector3* Ptr );
	void BindToMatrixPtr( LMatrix4* Ptr );
	void BindToQuatPtr( LQuaternion* Ptr );

	/// Tear down any link
	scriptmethod void Unbind();

	/// Called inside a Bind() call for initial value fetch
	virtual void FetchInitialPropertyValue() {}

	/// Called in Update() or somewhere else to store immediate current value
	virtual void SetCurrentValue() {}

	/// Autonomous work mode
	scriptmethod void Register();

	/// Autonomouse work mode - event handler deregistration
	scriptmethod void Unregister();

	// looks like something from Entity...
	virtual void AddDebugGeometry( clScene* S );
	virtual void RemoveDebugGeometry( clScene* S );

	// looks like it is from Entity too
	virtual void SetTransform( const LMatrix4& Mtx );
	virtual LMatrix4 GetTransform() const { return FLocalTransform; }

	virtual void UpdateTransform();

	// load parts
	void LoadGeoms();
	void ClearGeoms();

	int GetClosestPart( const LMatrix4& Proj, const LMatrix4& View, float X, float Y, LVector3& isect );

	bool InstersectsWithPart( int Part, const LMatrix4& Proj, const LMatrix4& View, float X, float Y, LVector3& isect );

	virtual bool IntersectsWithRay( const LMatrix4& Proj, const LMatrix4& View, const LVector3& P, const LVector3& Dir, LVector3& isect );

	virtual void Update( float DT );

	static LMatrix4 AlignToScreen( const LMatrix3& Cam, const LMatrix4& Src );

	virtual void OnMouseDown( const LMatrix4& Proj, const LMatrix4& View, float MX, float MY );
	virtual void OnMouseMove( float MX, float MY );
	virtual void OnMouseUp();

	scriptmethod void SetStartTransform( const LMatrix4& M ) { FStartTransform = M; }

	virtual LMatrix4 CalculateTransform( float MX, float MY ) { return LMatrix4::Identity(); };

public:
	/// Cached camera transform and projection
	LMatrix4 FProj, FView;

	LGizmoCoordSys FCoords;

	clScene* LocalScene;

	LMatrix4 FLocalTransform;
	LMatrix4 FLocalTransformInv;

	std::vector<LMatrix4> PartTransform;
	std::vector<LMatrix4> PartTransformInv;

	std::vector<clGeom*> Parts;
	std::vector<clMaterial*> Materials;
	std::vector<int> PartID;

	std::vector<LString> PartNames;
	std::vector<LString> MaterialNames;

	/// Yellow material
	clMaterial* FSelectedColorMaterial;

public:
	/// Are we performing action or not
	bool FPerformingAction;

	/// Stored index
	int FClickedPart;

	float FOldMouseX, FOldMouseY;
	float FMouseX, FMouseY;

	/// Initial 3d point (in MouseDown handler)
	LVector3 FStartOffset;

	/// Original transformation
	LMatrix4 FStartTransform;

	/// Current transformation
	LMatrix4 FCurrentTransform;

	/// Get the point under mouse cursor on the selected plane/line/gizmo_circle
	LVector3 GetActivePoint( const LMatrix4& CoordSys, float MX, float MY );

	FWD_EVENT_HANDLER( Event_TIMER );
	FWD_EVENT_HANDLER( Event_KEY );

private:
	/// Autonomous mode
	bool FHandleEvents;

	/// Local timer/key event handlers
	LEventSubscriber::LEventHandler FTimerHandler, FKeyHandler;

protected:
	/// Binding to some vector
	bool FUseVector;

	/// Binding to the fourth row of some transform matrix
	bool FUseMatrix;

	/// Bind to 4-vector with unit quaternion
	bool FUseQuat;

	/// Link to variable
	clCVar* FVar;

	/// Link to property
	iObject* FObject;

	/// Property name
	LString FProp;

	/// Pointer to "somewhere" where the translation/transform resides
	void* FDirectPtr;

	/// It is a property and it is an index in some array. I.e., we are moving some Obj->P[i]
//	int FArrayIndex;
};

/**
   \brief Visual orientation modifier

   Rotation gizmo only modifies the rotation. Translation ('base transform') must be specified
   as a separate parameter in FBaseTransform

   Since orientation can be represnted in a number of ways,
   this modifier supports binding to unit quaterniton, 4x4 rotation matrix (3x3 part) or 3-component
   vector with Euler angles. Concrete angular system is specified in FAngleSystem.

   E.g., if one wants to set up the X,Y,Z angles, FAngleSystem must be Euler_zyxs
*/
class scriptfinal netexportable clRotateGizmo : public clGizmo
{
public:
	clRotateGizmo();
	virtual ~clRotateGizmo();

	virtual void Update( float DT );

	/// Load orientation from object
	virtual void FetchInitialPropertyValue();

	/// Store translation to object
	virtual void SetCurrentValue();

	virtual LMatrix4 CalculateTransform( float MX, float MY );

	scriptmethod void SetAngleSystem( LEulerAngleSystem Sys );
	scriptmethod LEulerAngleSystem GetAngleSystem() const { return FAngleSystem; }

	/// Set base transform for this modifier
	scriptmethod void SetBaseTransform( const mtx4& BaseTransform ) { FBaseTransform = BaseTransform; }

	/// Get base transform for this modifier
	scriptmethod mtx4 GetBaseTransform() const { return FBaseTransform; }

public:
	/// Currently used angular system (zxz, xyz etc.)
	LEulerAngleSystem FAngleSystem;

	/// Base transformation for this modifier
	LMatrix4 FBaseTransform;
};

class scriptfinal netexportable clTranslateGizmo : public clGizmo
{
public:
	clTranslateGizmo();
	virtual ~clTranslateGizmo();

	virtual LMatrix4 CalculateTransform( float MX, float MY );

	/// Load translation from object
	virtual void FetchInitialPropertyValue();

	/// Store translation to object
	virtual void SetCurrentValue();
};

#endif

/*
 * 09/12/2010
     Log section added
*/
