/**
 * \file Gizmos.cpp
 * \brief Transformation gizmos
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Gizmos.h"

#include "Math/LGeomUtils.h"
#include "Math/LMathStrings.h"

#include "VisualScene.h"

#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Environment.h"
#include "Resources/ResourcesManager.h"

#include "Utils/Viewport.h"

#include "Scene/Scene.h"

#include "Geometry/Geom.h"
#include "Scene/Material.h"

#include "Core/Console.h"
#include "Core/CVars.h"

#include "LKeys.h"

clGizmo::clGizmo()
{
	FUseMatrix = false;
	FUseVector = false;
	FVar = NULL;
	FObject = NULL;
	FProp.clear();
	FDirectPtr = NULL;

	FHandleEvents = false;

	FPerformingAction = false;
	FClickedPart = -1;

	FLocalTransform.IdentityMatrix();
	FLocalTransformInv.IdentityMatrix();

	FCoords = COORDS_World;

	LocalScene = NULL;
}

clGizmo::~clGizmo()
{
	ClearGeoms();
}

void clGizmo::Register()
{
	if ( FHandleEvents ) { return; }

	FTimerHandler = Utils::Bind( &clGizmo::Event_TIMER, this );
	Env->Connect( L_EVENT_TIMER,  FTimerHandler );
	FKeyHandler = Utils::Bind( &clGizmo::Event_KEY, this );
	Env->Connect( L_EVENT_KEY,  FKeyHandler );

	FHandleEvents = true;
}

void clGizmo::Unregister()
{
	if ( !FHandleEvents ) { return; }

	FHandleEvents = false;

	Env->Disconnect( L_EVENT_TIMER, FTimerHandler );
	Env->Disconnect( L_EVENT_KEY,   FKeyHandler );
}

void clGizmo::Event_TIMER( LEvent Event, const LEventArgs& Args )
{
	/// Redirect to update
	Update( Args.FFloatArg );
}

void clGizmo::Event_KEY( LEvent Event, const LEventArgs& Args )
{
	int Key = ( int )Args.FFloatArg;
	bool State = Args.FBoolArg;

	LVector2 Pos = Env->Viewport->GetMousePosition();

	if ( Key != LK_LBUTTON ) { return; }

	State ? OnMouseDown( FProj, FView, Pos.X, Pos.Y ) : OnMouseUp();
//	HandleKey( Key, State );
}

void clGizmo::Update( float DT )
{
	clVisualModifier::Update( DT );

	if ( FHandleEvents )
	{
		if ( LocalScene )
		{
			FProj = LocalScene->GetCameraProjection();
			FView = LocalScene->GetCameraTransform();
		}

		UpdateTransform();

		LVector2 Pos = Env->Viewport->GetMousePosition();
		OnMouseMove( Pos.X, Pos.Y );

		SetCurrentValue();
	}

	if ( FCoords == COORDS_Screen )
	{
		SetTransform( AlignToScreen( FScene->GetCurrentView().ExtractMatrix3(), FLocalTransform ) );
	}
	else if ( FCoords == COORDS_World )
	{
		// do nothing
	}
	else if ( FCoords == COORDS_Local )
	{
		// get attachment entity and align the coords
	}
}

void clGizmo::UpdateTransform()
{
	clVisualModifier::UpdateTransform();

	FLocalTransform = FTransform.FMatrix;
	FLocalTransformInv = FTransform.FMatrix.GetInversed();

	if ( LocalScene )
	{
		for ( size_t i = 0 ; i < Parts.size() ; i++ )
		{
			PartTransform[i] = FLocalTransform;
			PartTransformInv[i] = FLocalTransformInv;
			LocalScene->SetLocalTransform( PartID[i], FLocalTransform );
		}
	}

	SetCurrentValue();
}

void clGizmo::SetTransform( const LMatrix4& Mtx )
{
	clVisualModifier::SetTransform( Mtx );
}

int clGizmo::GetClosestPart( const LMatrix4& Proj, const LMatrix4& View, float X, float Y, LVector3& isect )
{
	int Part = -1;
	float Dist = +1000000.0f;

	// 1. calculate ray in world coordinates
	LVector3 srcPoint, dir;
	MouseCoordsToWorldPointAndRay( Proj, View, X, Y, srcPoint, dir );



	for ( size_t i = 0 ; i < Parts.size() ; i++ )
	{
		LVector3 local_isect;

		if ( InstersectsWithPart( ( int )i, Proj, View, X, Y, local_isect ) )
		{
			/*
			         // 2. transform ray to gizmo's local coordinates
			         LVector3 Pt = PartTransformInv[i] * srcPoint;
			         LVector3 Aa = PartTransformInv[i].ToMatrix3() * dir;

			         // 3. intersect the ray with part of the gizmo
			         clVertexAttribs* VA = Parts[i]->GetCurrentMesh()->GetRigid( 0 );
			         LVector3 local_isect;
			         if (VA->IntersectWithRay( Pt, Aa, local_isect ))
			         {
			            // 3. convert instersection back to world coordinates
			//          local_isect = PartTransform[i] * local_isect;

			            float NewDist = (local_isect - srcPoint).SqrLength();

			            if (NewDist < Dist)
			            {
			               Dist = NewDist;
			               isect = local_isect;
			               Part = (int)i;
			            }
			         }
			*/
			float NewDist = ( local_isect - srcPoint ).SqrLength();

			if ( NewDist < Dist )
			{
				Dist = NewDist;
				isect = local_isect;
				Part = ( int )i;
			}
		}
	}

	return Part;
}

bool clGizmo::IntersectsWithRay( const LMatrix4& Proj, const LMatrix4& View, const LVector3& P, const LVector3& Dir, LVector3& isect )
{
	for ( size_t i = 0 ; i < Parts.size() ; i++ )
	{
		// 2. transform ray to gizmo's local coordinates
		LVector3 Pt = PartTransformInv[i] * P;
		LVector3 Aa = PartTransformInv[i].ToMatrix3() * Dir;

		// 3. intersect the ray with part of the gizmo
		clVertexAttribs* VA = Parts[i]->GetCurrentMesh()->GetRigid( 0 );

		if ( VA->IntersectWithRay( Pt, Aa, isect ) )
		{
			// 3. convert instersection back to world coordinates
			isect = PartTransform[i] * isect;

			return true;
		}
	}

	return false;
}

bool clGizmo::InstersectsWithPart( int Part, const LMatrix4& Proj, const LMatrix4& View, float X, float Y, LVector3& isect )
{
	// 1. calculate ray in world coordinates
	LVector3 srcPoint;
	LVector3 dir;
	MouseCoordsToWorldPointAndRay( Proj, View, X, Y, srcPoint, dir );

	// 2. transform ray to gizmo's local coordinates
	LVector3 Pt = PartTransformInv[Part] * srcPoint;
	LVector3 Aa = PartTransformInv[Part].ToMatrix3() * dir;

	// 3. intersect the ray with part of the gizmo
	clVertexAttribs* VA = Parts[Part]->GetCurrentMesh()->GetRigid( 0 );

	bool res = VA->IntersectWithRay( Pt, Aa, isect );

	// 3. convert instersection back to world coordinates
	isect = PartTransform[Part] * isect;

	return res;
}

void clGizmo::AddDebugGeometry( clScene* S )
{
	LocalScene = S;

	for ( size_t i = 0 ; i < Parts.size() ; i++ )
	{
		PartID.push_back( S->AddGeom( Parts[i] ) );
		S->SetMtl( PartID[i], Materials[i] );
//		S->Scale(PartID[i], 0.05f);
	}
}

void clGizmo::RemoveDebugGeometry( clScene* S )
{
	for ( size_t i = 0 ; i < Parts.size() ; i++ )
	{
		LocalScene->RemoveItem( PartID[i] );
	}

	PartID.clear();
}

void clGizmo::AfterConstruction()
{
	clVisualObject::AfterConstruction();
	LoadGeoms();
}

void clGizmo::LoadGeoms()
{
	for ( size_t i = 0 ; i < PartNames.size() ; i++ )
	{
		Parts.push_back( Env->Resources->LoadGeom( PartNames[i] ) );
		Materials.push_back( Env->Resources->LoadMaterial( MaterialNames[i] ) );
	}

	// create yellow material for selected part
	FSelectedColorMaterial = Env->Resources->CreateMaterial();
	FSelectedColorMaterial->SetPropertyValue( "DiffuseColor", LStr::Vec4ToStr( LC_Yellow ) );
	FSelectedColorMaterial->SetPropertyValue( "ReceiveShadow", "false" );
	FSelectedColorMaterial->SetPropertyValue( "CastShadow",    "false" );
}

void clGizmo::ClearGeoms()
{
}

clRotateGizmo::~clRotateGizmo()
{
}

clRotateGizmo::clRotateGizmo()
{
	FAngleSystem = Euler_zyxs;
	FBaseTransform = mtx4::Identity();

	PartNames.push_back( "Manipulators/rotate_axisx.ase" );
	PartNames.push_back( "Manipulators/rotate_axisy.ase" );
	PartNames.push_back( "Manipulators/rotate_axisz.ase" );
	PartNames.push_back( "Manipulators/rotate_screen.ase" );

	for ( int i = 0 ; i < 4 ; i++ ) { MaterialNames.push_back( "Manipulators/move.material" ); }

	PartTransform.resize( 4 );
	PartTransformInv.resize( 4 );
}

void clRotateGizmo::Update( float DT )
{
	clGizmo::Update( DT );

	// align 'rotate_screen' to be rotated
	LMatrix3 NewCam = FScene->GetCurrentView().ExtractMatrix3() * ( LMatrix4::GetRotateMatrixAxis( Math::DegToRad( 90.0f ), vec3( 1, 0, 0 ) ) ).ExtractMatrix3().GetInversed();
	/*
	   for (int i = 0 ; i < 3 ; i++)
	   {
	      for (int j = 0 ; j < 3 ; j++)
	      {
	         PartTransform[3][i][j] = FixedRotate[i][j];
	      }
	   }

	   PartTransformInv[3] = PartTransform[3].GetInversed();
	*/
	PartTransform[3] = AlignToScreen( NewCam, PartTransform[3] );
	PartTransformInv[3] = PartTransform[3].GetInversed();

	LocalScene->SetLocalTransform( PartID[3], PartTransform[3] );
}

clTranslateGizmo::clTranslateGizmo()
{
	PartNames.push_back( "Manipulators/move_axisx.ase" );
	PartNames.push_back( "Manipulators/move_axisy.ase" );
	PartNames.push_back( "Manipulators/move_axisz.ase" );
	PartNames.push_back( "Manipulators/move_axis_xy.ase" );
	PartNames.push_back( "Manipulators/move_axis_xz.ase" );
	PartNames.push_back( "Manipulators/move_axis_yz.ase" );
	PartNames.push_back( "Manipulators/move_root.ase" );

	for ( int i = 0 ; i < 6 ; i++ ) { MaterialNames.push_back( "Manipulators/move.material" ); }

	// something semitransparent ?
	MaterialNames.push_back( "Manipulators/move.material" );

	PartTransform.resize( 7 );
	PartTransformInv.resize( 7 );
}

clTranslateGizmo::~clTranslateGizmo()
{
}

LMatrix4 clGizmo::AlignToScreen( const LMatrix3& Cam, const LMatrix4& Src )
{
	LMatrix4 NewTransform = Src;
	// align 'rotate_screen' to be rotated
	LMatrix3 FixedRotate = Cam.GetInversed();

	for ( int i = 0 ; i < 3 ; i++ )
	{
		for ( int j = 0 ; j < 3 ; j++ )
		{
			NewTransform[i][j] = FixedRotate[i][j];
		}
	}

	return NewTransform;
}

//void OnMouseDown(const LMatrix4& Proj, const LMatrix4& View, float MX, float MY);
//void OnMouseMove(float MX, float MY);
//void OnMouseUp();

void clGizmo::OnMouseDown( const LMatrix4& Proj, const LMatrix4& View, float MX, float MY )
{
	FProj = Proj;
	FView = View;

	LVector3 isect;

	FMouseX = MX;
	FMouseY = MY;
	FOldMouseX = FMouseX;
	FOldMouseY = FMouseY;

	FClickedPart = GetClosestPart( Proj, View, MX, MY, isect );

	if ( FClickedPart < 0 ) { return; }

	FStartTransform = GetTransform();

	FStartOffset = FStartTransform.GetInversed() * GetActivePoint( LMatrix4::Identity(), MX, MY );

	FCurrentTransform = FStartTransform;
	FPerformingAction = true;

	/// Change color for selected part
	LocalScene->SetMtl( PartID[FClickedPart], FSelectedColorMaterial );
}

void clGizmo::OnMouseMove( float MX, float MY )
{
	if ( FPerformingAction )
	{
		// Calculate new transform for this mouse position
		// Store new mouse position ?
		LMatrix4 T = CalculateTransform( MX, MY );
		FCurrentTransform = FStartTransform * T;
		FStartTransform = T;

		SetTransform( FCurrentTransform );
	}
}

void clGizmo::OnMouseUp()
{
	if ( FClickedPart > -1 )
	{
		LocalScene->SetMtl( PartID[FClickedPart], Materials[FClickedPart] );
		FClickedPart = -1;

		FPerformingAction = false;
		// assign new transform, invoke owner about change ?
	}
}

LVector3 ProjectPointToLine( const LVector3& Orig, const LVector3& Dir, const LVector3& Pt )
{
	return Orig + ( ( Pt - Orig ).Dot( Dir ) / Dir.Dot( Dir ) ) * Dir;
}

LVector3 clGizmo::GetActivePoint( const LMatrix4& CoordSys, float MX, float MY )
{
	LVector3 Origin;
	LMatrix4 Rotation;

	DecomposeTransformation( CoordSys, Origin, Rotation );

	LVector3 Xaxis = Rotation[0].ToVector3();
	LVector3 Yaxis = Rotation[1].ToVector3();
	LVector3 Zaxis = Rotation[2].ToVector3();

	LVector3 srcPoint;
	LVector3 dir;
	MouseCoordsToWorldPointAndRay( FProj, FView, MX, MY, srcPoint, dir );

	LVector3 Pt = Origin;

	// project mouse to the given axis or plane
	if ( FClickedPart < 3 )
	{
		LVector3 N = Xaxis;

		if ( FClickedPart == 1 ) { N = Yaxis; }

		if ( FClickedPart == 2 ) { N = Zaxis; }

		// axis selected
		float t1, tClose;
		ClosestPointsOnLines( srcPoint, dir, Origin, N, t1, tClose );

		Pt = Origin + N * tClose;
	}
	else
	{
		if ( FClickedPart < 7 )
		{
			// plane or root selected (screen-aligned movement)
			LVector3 N = Zaxis; // xy-plane

			if ( FClickedPart == 4 ) { N = Yaxis; } // xz

			if ( FClickedPart == 5 ) { N = Xaxis; } // yz

			// FIXME: must be FDirection from camera, not just the Y-vector. Sometimes it is Z
			if ( FClickedPart == 6 ) { N = FView.GetInversed()[2].ToVector3(); } // screen-aligned plane

			// build a plane
			LPlane Plane( Origin, N );

			// intersect with ray from the eye
			LVector3 isect;
			IntersectRayToPlane( srcPoint, dir, N, -Origin.Dot( N ), isect );

			// 'isect' is the new Origin. So, FCurrentTransform becomes TranslateMatrix(Origin-isect)
			Pt = isect;
		}
	}

	return Pt;
}

LMatrix4 clTranslateGizmo::CalculateTransform( float MX, float MY )
{
	// calculate new _relative_ transform
	return LMatrix4::GetTranslateMatrix( FStartOffset ).GetInversed() * LMatrix4::GetTranslateMatrix( GetActivePoint( LMatrix4::Identity() /*FStartTransform*/, MX, MY ) ) * FStartTransform.GetInversed();
}

void clTranslateGizmo::FetchInitialPropertyValue()
{
	LVector3 Pos( 0, 0, 0 );

	if ( FObject )
	{
		LString PropVal = FObject->GetPropertyValue( FProp );

		if ( FUseMatrix )
		{
			LMatrix4 M = LStr::StrToMtx4( PropVal );
			LMatrix4 Rot;
			DecomposeTransformation( M, Pos, Rot );
		}
		else
		{
			Pos = LStr::StrToVec3( PropVal );
		}
	}
	else if ( FVar )
	{
		if ( FUseMatrix )
		{
			LMatrix4 Rot;
			DecomposeTransformation( FVar->GetMatrix4(), Pos, Rot );
		}
		else if ( FUseVector )
		{
			Pos = FVar->GetVector3();
		}
	}
	else if ( FDirectPtr != NULL )
	{
		if ( FUseMatrix )
		{
			LMatrix4 Rot;
			DecomposeTransformation( *( ( LMatrix4* )FDirectPtr ), Pos, Rot );
		}
		else if ( FUseVector )
		{
			Pos = *( LVector3* )FDirectPtr;
		}
	}

	FStartTransform = FCurrentTransform = LMatrix4::GetTranslateMatrix( Pos );
	SetTransform( FCurrentTransform );
}

void clRotateGizmo::FetchInitialPropertyValue()
{
	LMatrix4 Rotation = mtx4::Identity();

	if ( FObject )
	{
		LString PropVal = FObject->GetPropertyValue( FProp );

		if ( FUseMatrix )
		{
			LMatrix4 M = LStr::StrToMtx4( PropVal );
			LVector3 Pos;
			DecomposeTransformation( M, Pos, Rotation );
		}
		else if ( FUseQuat )
		{
			Rotation = mtx4( quat( LStr::StrToVec4( PropVal ) ).ToMatrix3() );
		}
		else if ( FUseVector )
		{
			vec3 A = LStr::StrToVec3( PropVal );
			Rotation = mtx4::Identity();
			AnglesToMatrix( FAngleSystem, Rotation, A.X, A.Y, A.Z );
		}
	}
	else if ( FVar )
	{
		if ( FUseMatrix )
		{
			vec3 Pos;
			DecomposeTransformation( FVar->GetMatrix4(), Pos, Rotation );
		}
		else if ( FUseQuat )
		{
			Rotation = mtx4( quat( FVar->GetVector4() ).ToMatrix3() );
		}
		else if ( FUseVector )
		{
			vec3 A = FVar->GetVector3();
			Rotation = mtx4::Identity();
			AnglesToMatrix( FAngleSystem, Rotation, A.X, A.Y, A.Z );
		}
	}
	else if ( FDirectPtr != NULL )
	{
		if ( FUseMatrix )
		{
			vec3 Pos;
			DecomposeTransformation( *( ( LMatrix4* )FDirectPtr ), Pos, Rotation );
		}
		else if ( FUseQuat )
		{
			Rotation = mtx4( quat( *( LVector4* )FDirectPtr ).ToMatrix3() );
		}
		else if ( FUseVector )
		{
			vec3 A = *( LVector3* )FDirectPtr;
			Rotation = mtx4::Identity();
			AnglesToMatrix( FAngleSystem, Rotation, A.X, A.Y, A.Z );
		}
	}

	FStartTransform = FCurrentTransform = Rotation;
	SetTransform( FCurrentTransform );
}

void clTranslateGizmo::SetCurrentValue()
{
	LVector3 Pos; // = FCurrentTransform[3].ToVector3();
	LMatrix4 Rot;
	DecomposeTransformation( FCurrentTransform, Pos, Rot );

	if ( FObject )
	{
		if ( FUseMatrix )
		{
			LString PropVal = FObject->GetPropertyValue( FProp );
			LMatrix4 M = LStr::StrToMtx4( PropVal );
			LMatrix4 Rot;
			LVector3 OldPos;
			DecomposeTransformation( M, OldPos, Rot );
			M = ComposeTransformation( Pos, Rot );
			FObject->SetPropertyValue( FProp, LStr::Mat4ToStr( M ) );
		}
		else
		{
			FObject->SetPropertyValue( FProp, LStr::Vec3ToStr( Pos ) );
		}
	}
	else if ( FVar )
	{
		if ( FUseMatrix )
		{
			LMatrix4 M = FVar->GetMatrix4();
			LMatrix4 Rot;
			LVector3 OldPos;
			DecomposeTransformation( M, OldPos, Rot );
			FVar->SetMatrix4( ComposeTransformation( Pos, Rot ) );
		}
		else if ( FUseVector )
		{
			FVar->SetVector3( Pos );
		}
	}
	else if ( FDirectPtr != NULL )
	{
		if ( FUseMatrix )
		{
			LMatrix4 M = *( LMatrix4* )FDirectPtr;
			LMatrix4 Rot;
			LVector3 OldPos;
			DecomposeTransformation( M, OldPos, Rot );
			*( LMatrix4* )FDirectPtr = ComposeTransformation( Pos, Rot );
		}
		else if ( FUseVector )
		{
			*( LVector3* )FDirectPtr = Pos;
		}
	}
}

void clRotateGizmo::SetCurrentValue()
{
	LVector3 CurPos;
	LMatrix4 CurRot;
	DecomposeTransformation( FCurrentTransform, CurPos, CurRot );

	// Property binding ?
	if ( FObject )
	{
		if ( FUseMatrix )
		{
			LString PropVal = FObject->GetPropertyValue( FProp );
			LMatrix4 M = LStr::StrToMtx4( PropVal );
			LMatrix4 OldRot;
			LVector3 OldPos;
			DecomposeTransformation( M, OldPos, OldRot );
			M = ComposeTransformation( OldPos, CurRot );
			FObject->SetPropertyValue( FProp, LStr::Mat4ToStr( M ) );
		}
		else if ( FUseVector )
		{
			float Angle1, Angle2, Angle3;
			MatrixToAngles( FAngleSystem, CurRot.ToMatrix3(), Angle1, Angle2, Angle3 );
			FObject->SetPropertyValue( FProp, LStr::Vec3ToStr( vec3( Angle1, Angle2, Angle3 ) ) );
		}
		else if ( FUseQuat )
		{
			FObject->SetPropertyValue( FProp, LStr::Vec4ToStr( quat( CurRot ).ToVector4() ) );
		}
	}
	else if ( FVar )
	{
		if ( FUseMatrix )
		{
			LMatrix4 M = FVar->GetMatrix4();
			LMatrix4 OldRot;
			LVector3 OldPos;
			DecomposeTransformation( M, OldPos, OldRot );
			FVar->SetMatrix4( ComposeTransformation( OldPos, CurRot ) );
		}
		else if ( FUseVector )
		{
			float Angle1, Angle2, Angle3;
			MatrixToAngles( FAngleSystem, CurRot.ToMatrix3(), Angle1, Angle2, Angle3 );
			FVar->SetVector3( vec3( Angle1, Angle2, Angle3 ) );
		}
		else if ( FUseQuat )
		{
			FVar->SetVector4( quat( CurRot ).ToVector4() );
		}
	}
	else if ( FDirectPtr != NULL )
	{
		if ( FUseMatrix )
		{
			LMatrix4 M = *( LMatrix4* )FDirectPtr;
			LMatrix4 OldRot;
			LVector3 OldPos;
			DecomposeTransformation( M, OldPos, OldRot );
			*( LMatrix4* )FDirectPtr = ComposeTransformation( OldPos, CurRot );
		}
		else if ( FUseVector )
		{
			float Angle1, Angle2, Angle3;
			MatrixToAngles( FAngleSystem, CurRot.ToMatrix3(), Angle1, Angle2, Angle3 );
			*( LVector3* )FDirectPtr = vec3( Angle1, Angle2, Angle3 );
		}
		else if ( FUseQuat )
		{
			*( LVector4* )FDirectPtr = quat( CurRot ).ToVector4();
		}
	}
}

void clRotateGizmo::SetAngleSystem( LEulerAngleSystem Sys )
{
	FAngleSystem = Sys;

	// refetch the values, angles can change
	FetchInitialPropertyValue();
}

LMatrix4 clRotateGizmo::CalculateTransform( float MX, float MY )
{
	return LMatrix4::Identity();
}

void clGizmo::BindToPropVector( iObject* Obj, const LString& Prop )
{
	FUseMatrix = false;
	FUseVector = true;
	FUseQuat = false;
	FVar = NULL;
	FObject = Obj;
	FProp = Prop;
	FDirectPtr = NULL;

	FetchInitialPropertyValue();
}

void clGizmo::BindToPropMatrix( iObject* Obj, const LString& Prop )
{
	FUseMatrix = true;
	FUseVector = false;
	FUseQuat = false;
	FVar = NULL;
	FObject = Obj;
	FProp = Prop;
	FDirectPtr = NULL;

	FetchInitialPropertyValue();
}

void clGizmo::BindToPropQuat( iObject* Obj, const LString& Prop )
{
	FUseMatrix = false;
	FUseVector = false;
	FUseQuat = true;
	FVar = NULL;
	FObject = Obj;
	FProp = Prop;
	FDirectPtr = NULL;

	FetchInitialPropertyValue();
}

void clGizmo::BindToVarVector( const LString& VarName )
{
	FUseMatrix = false;
	FUseVector = true;
	FUseQuat = false;
	FVar = Env->Console->GetVar( VarName );
	FObject = NULL;
	FProp.clear();
	FDirectPtr = NULL;

	FetchInitialPropertyValue();
}

void clGizmo::BindToVarMatrix( const LString& VarName )
{
	FUseMatrix = true;
	FUseVector = false;
	FVar = Env->Console->GetVar( VarName );
	FObject = NULL;
	FProp.clear();
	FDirectPtr = NULL;

	FetchInitialPropertyValue();
}

void clGizmo::BindToVectorPtr( LVector3* Ptr )
{
	FUseMatrix = false;
	FUseVector = true;
	FUseQuat = false;
	FVar = NULL;
	FObject = NULL;
	FProp.clear();
	FDirectPtr = Ptr;

	FetchInitialPropertyValue();
}

void clGizmo::BindToVarQuat( const LString& VarName )
{
	FUseMatrix = false;
	FUseVector = false;
	FUseQuat = true;
	FVar = Env->Console->GetVar( VarName );
	FObject = NULL;
	FProp.clear();
	FDirectPtr = NULL;

	FetchInitialPropertyValue();
}

void clGizmo::BindToMatrixPtr( LMatrix4* Ptr )
{
	FUseMatrix = true;
	FUseVector = false;
	FUseQuat = false;
	FVar = NULL;
	FObject = NULL;
	FProp.clear();
	FDirectPtr = Ptr;

	FetchInitialPropertyValue();
}

void clGizmo::Unbind()
{
	SetCurrentValue();

	FUseMatrix = false;
	FUseVector = false;
	FUseQuat   = false;
	FVar = NULL;
	FObject = NULL;
	FProp.clear();
	FDirectPtr = NULL;
}

/*
 * 09/12/2010
     Log section added
*/
