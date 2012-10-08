/**
 * \file VisualProjector.cpp
 * \brief Visual projector
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VisualProjector.h"
#include "VisualScene.h"

#include "CameraPositioner.h"

#include "Math/LFrustum.h"
#include "Math/LProjection.h"

#include "Environment.h"
#include "Renderer/Canvas.h"
#include "Resources/ResourcesManager.h"

#include "Scene/Scene.h"

#include "LColors.h"

// indices of the plane-defining corner points
LVector3 BFrustumPlanePoints[] =
{
	// PLANE_LEFT
	LVector3( FRUSTUM_ntl, FRUSTUM_nbl, FRUSTUM_fbl ),
	// PLANE_RIGHT
	LVector3( FRUSTUM_nbr, FRUSTUM_ntr, FRUSTUM_fbr ),
	// PLANE_TOP
	LVector3( FRUSTUM_ntr, FRUSTUM_ntl, FRUSTUM_ftl ),
	// PLANE_BOTTOM
	LVector3( FRUSTUM_nbl, FRUSTUM_nbr, FRUSTUM_fbr ),
	// PLANE_NEAR
	LVector3( FRUSTUM_ntl, FRUSTUM_ntr, FRUSTUM_nbr ),
	// PLANE_FAR
	LVector3( FRUSTUM_ftr, FRUSTUM_ftl, FRUSTUM_fbl )
};

clVisualProjector::clVisualProjector() : FProjectionType( PROJECTION_ERROR ),
	FUpVector( 0, 0, 1 ), FSideVector( 1, 0, 0 ), FDirection( 0, 1, 0 ),
	FTarget( 0, 5, 0 ), FUseTarget( false ),
	FDebugGeom( NULL ), FDebugGeomID( -1 ),
	FPositioner( NULL )
{
}

clVisualProjector::~clVisualProjector()
{
}

void clVisualProjector::AddDebugGeometry( clScene* S )
{
	LString Name = GetDebugMeshName();

	if ( Name.empty() ) { return; }

	FDebugGeom = Env->Resources->LoadGeom( Name );

	FDebugGeomID = S->AddGeom( FDebugGeom );

	S->SetLocalTransform( FDebugGeomID, GetTransform() );
}

void clVisualProjector::RemoveDebugGeometry( clScene* S )
{
	if ( FDebugGeomID > -1 )
	{
		S->RemoveItem( FDebugGeomID );
	}
}

void clVisualProjector::Update( float dt )
{
	if ( FPositioner )
	{
		SetTransform( FPositioner->GetCurrentTransform() );
	}
}

void clVisualProjector::SetDirection( const LVector3& ADirection )
{
	FDirection = ADirection;

	FTarget = GetPosition() + FDirection * FDistanceToTarget;
}

void clVisualProjector::SetDistanceToTarget( float ADistance )
{
	FDistanceToTarget = ADistance;

	FTarget = GetPosition() + FDirection * FDistanceToTarget;
}

void clVisualProjector::SetTarget( const LVector3& ATarget )
{
	FTarget = ATarget;

	FDirection = ATarget - GetPosition();

	FDistanceToTarget = FDirection.Length();
	FDirection.Normalize();
}

void clVisualProjector::CalculateTransformFromDirUpTarget()
{
	LVector3 V1, V2;
	BuildComplementaryBasis( FDirection, V1, V2 );

	LVector3 Z = FDirection.GetNormalized();
	LVector3 X = FUpVector.Cross( Z ).GetNormalized();
	LVector3 Y = Z.Cross( X );

	LMatrix3 m;
	m[0] = X;
	m[1] = Y;
	m[2] = Z;

	LMatrix4 NewTransform = LMatrix4( m ) * LMatrix4::GetTranslateMatrix( GetPosition() );

	SetTransform( NewTransform );
}

void clVisualProjector::RecalculatePlanes()
{
	/*
	   LMatrix4 ProjModelView = FTransform.FMatrix * FProjection;
	   for ( int i = 0; i != 4; ++i )
	   {
	      FPlanes[0][i] = ProjModelView[i][3] + ProjModelView[i][0]; // left
	      FPlanes[1][i] = ProjModelView[i][3] - ProjModelView[i][0]; // right
	      FPlanes[2][i] = ProjModelView[i][3] - ProjModelView[i][1]; // top
	      FPlanes[3][i] = ProjModelView[i][3] + ProjModelView[i][1]; // bottom
	      FPlanes[4][i] = ProjModelView[i][3] + ProjModelView[i][2]; // near
	      FPlanes[5][i] = ProjModelView[i][3] - ProjModelView[i][2]; // far
	   }

	   for ( int i = 0; i != 6; ++i )
	   {
	      LVector3 Vec = FPlanes[i].ToVector3();

	      FPlanes[i] /= Vec.Length();
	   }
	*/
}

void clVisualProjector::SplitFrustum( std::vector<float>& Dists )
{
	int NumParts = static_cast<int>( Dists.size() );

	const float Lambda = 1.0f;

	float Delta = 1.0f / static_cast<float>( NumParts );

	float n = FNearClipPlane;
	float f = FFarClipPlane;

	float Power = 0.0f;

	for ( int i = 0 ; i < NumParts ; i++, Power += Delta )
	{
		float NewZ1 = n * pow ( f / n, Power );
		float NewZ2 = n + Power * ( f - n );

		Dists[i] = NewZ1 * Lambda + NewZ2 * ( 1 - Lambda );
	}
}

void clVisualProjector::UpdateProjection()
{
	vec3 TheDir = GetDirection();
	vec3 ThePos = GetPosition();

	// 1. calculate projection matrix
	if ( FProjectionType == PROJECTION_ORTHOGRAPHIC )
	{
		FProjection = Linderdaum::Math::Ortho( FLeft, FRight, FBottom, FTop, FNearClipPlane, FFarClipPlane );

		// corner points
		if ( FUseTarget )
		{
			CalcOrthographicFrustumPoints( FLeft, FRight, FBottom, FTop, FNearClipPlane, FFarClipPlane, ThePos, FTarget, FUpVector );
		}
		else
		{
			CalcOrthographicFrustumPoints( FLeft, FRight, FBottom, FTop, FNearClipPlane, FFarClipPlane, ThePos, ThePos + TheDir * 10.0f, FUpVector );
		}

	}
	else if ( FProjectionType == PROJECTION_PERSPECTIVE )
	{
		FProjection = Linderdaum::Math::Perspective( FFieldOfView, FAspectRatio, FNearClipPlane, FFarClipPlane );

		// corner points
		if ( FUseTarget )
		{
			CalcPerspectiveFrustumPoints( FFieldOfView, FAspectRatio, FNearClipPlane, FFarClipPlane, ThePos, FTarget, FUpVector );
		}
		else
		{
			CalcPerspectiveFrustumPoints( FFieldOfView, FAspectRatio, FNearClipPlane, FFarClipPlane, ThePos, ThePos + TheDir * 10.0f, FUpVector );
		}
	}
	else
	{
		// we do not do anything if the projection type is erroneous
		return;
	}

	// 2. calculate clipping planes
	RecalculatePlanes();
}

void clVisualProjector::SetTransform( const LMatrix4& Mtx )
{
	clVisualObject::SetTransform( Mtx );
	SetProjection( FProjection );
}

void clVisualProjector::SetAngles( const LVector3& A )
{
	clVisualObject::SetAngles( A );
	SetProjection( FProjection );
}

void clVisualProjector::SetPosition( const LVector3& P )
{
	clVisualObject::SetPosition( P );
	SetProjection( FProjection );
}

void clVisualProjector::SetProjectionType( int /*LProjectionType*/ PT )
{
	FProjectionType = ( LProjectionType )PT;
	UpdateProjection();
}

void clVisualProjector::SetProjection( const LMatrix4& Projection )
{
	LProjectionType NewProjectionType = Math::DetermineProjectionType( Projection );

	if ( NewProjectionType != PROJECTION_ERROR )
	{
		// 1. store the matrix
		FProjection = Projection;

		vec3 ThePos = GetPosition();
		vec3 TheDir = GetDirection();

		// 2. decompose the projection into parameters and calculate clipping planes
		if ( NewProjectionType == PROJECTION_PERSPECTIVE )
		{
			Linderdaum::Math::PerspectiveToParams( Projection, FFieldOfView, FAspectRatio, FNearClipPlane, FFarClipPlane );
			FNearClipPlane = -FNearClipPlane;
			FFarClipPlane = -FFarClipPlane;

			if ( FUseTarget )
			{
				CalcPerspectiveFrustumPoints( FFieldOfView, FAspectRatio, FNearClipPlane, FFarClipPlane, ThePos, FTarget, FUpVector );
			}
			else
			{
				CalcPerspectiveFrustumPoints( FFieldOfView, FAspectRatio, FNearClipPlane, FFarClipPlane, ThePos, ThePos + TheDir * 10.0f, FUpVector );
			}
		}
		else if ( NewProjectionType == PROJECTION_ORTHOGRAPHIC )
		{
			Linderdaum::Math::OrthoToParams( Projection, FLeft, FRight, FBottom, FTop, FNearClipPlane, FFarClipPlane );

			if ( FUseTarget )
			{
				CalcOrthographicFrustumPoints( FLeft, FRight, FBottom, FTop, FNearClipPlane, FFarClipPlane, ThePos, FTarget, FUpVector );
			}
			else
			{
				CalcOrthographicFrustumPoints( FLeft, FRight, FBottom, FTop, FNearClipPlane, FFarClipPlane, ThePos, ThePos + TheDir * 10.0f, FUpVector );
			}
		}

		// 3. recalculate planes
		RecalculatePlanes();
	}
}

void clVisualProjector::CalcFrustumInBasis( const LVector3& Pos, const LVector3& To, const LVector3& Up, float fw, float fh, float nw, float nh )
{
	LVector3 Z = To - Pos;
	Z.Normalize();

	LVector3 X = Up.Cross( Z );
	X.Normalize();

	LVector3 Y = Z.Cross( X );

	LVector3 nc = Pos + Z * FNearClipPlane;
	LVector3 fc = Pos + Z * FFarClipPlane;

	FCornerPoints[FRUSTUM_ntl] = nc + Y * nh - X * nw;
	FCornerPoints[FRUSTUM_ntr] = nc + Y * nh + X * nw;
	FCornerPoints[FRUSTUM_nbl] = nc - Y * nh - X * nw;
	FCornerPoints[FRUSTUM_nbr] = nc - Y * nh + X * nw;

	FCornerPoints[FRUSTUM_ftl] = fc + Y * fh - X * fw;
	FCornerPoints[FRUSTUM_ftr] = fc + Y * fh + X * fw;
	FCornerPoints[FRUSTUM_fbl] = fc - Y * fh - X * fw;
	FCornerPoints[FRUSTUM_fbr] = fc - Y * fh + X * fw;

	for ( int i = PLANE_LEFT ; i < PLANE_FAR + 1 ; i++ )
	{
		LVector3 P1 = FCornerPoints[ static_cast<int>( BFrustumPlanePoints[i].X ) ];
		LVector3 P2 = FCornerPoints[ static_cast<int>( BFrustumPlanePoints[i].Y ) ];
		LVector3 P3 = FCornerPoints[ static_cast<int>( BFrustumPlanePoints[i].Z ) ];


		// plane from 3 points
		LPlane Pl( P1, P2, P3 );

		FPlanes[i] = Pl.GetEquation();
	}
}

void clVisualProjector::CalcOrthographicFrustumPoints( float L, float R, float B, float T, float N, float F, const LVector3& Pos, const LVector3& To, const LVector3& Up )
{
	float nh = T - B;
	float nw = R - L;
	float fh = nh;
	float fw = nw;

	CalcFrustumInBasis( Pos, To, Up, fw, fh, nw, nh );
}

void clVisualProjector::CalcPerspectiveFrustumPoints( float Angle, float Ratio, float NearCP, float FarCP, const LVector3& Pos, const LVector3& To, const LVector3& Up )
{
	float Tang = ( float )tan( Linderdaum::Math::DegToRad( Angle * 0.5f ) );

	float nh = NearCP * Tang;
	float nw = nh * Ratio;
	float fh = FarCP  * Tang;
	float fw = fh * Ratio;

	CalcFrustumInBasis( Pos, To, Up, fw, fh, nw, nh );
}

LMatrix4 clVisualProjector::CalculateCropMatrix()
{
	return LMatrix4::Identity();
}


void clVisualProjector::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
	DebugRenderFrustum( Proj, View );

	DebugRenderFrustumNormals( Proj, View );

	clCanvas* C = Env->Renderer->GetCanvas();

	if ( FUseTarget )
	{
		C->SetMatrices( Proj, View );
		C->Arrow3D( GetPosition(), FTarget, 0.1f, LC_Yellow, LC_White );
	}
}


void DrawRect3D( sEnvironment* Env, const LMatrix4& _proj, const LMatrix4& _view, const LVector3& p1, const LVector3& p2, const LVector3& p3, const LVector3& p4, const LVector4& col )
{
	clCanvas* C = Env->Renderer->GetCanvas();
	C->SetMatrices( _proj, _view );
	C->Line3D( p1, p2, col );
	C->Line3D( p2, p3, col );
	C->Line3D( p3, p4, col );
	C->Line3D( p4, p1, col );
}

#define _FRect3D(p1,p2,p3,p4) \
   DrawRect3D(Env, _proj, _view, FCornerPoints[FRUSTUM_##p1], FCornerPoints[FRUSTUM_##p2], FCornerPoints[FRUSTUM_##p3], FCornerPoints[FRUSTUM_##p4], LC_Yellow);

void clVisualProjector::DebugRenderFrustum( const LMatrix4& _proj, const LMatrix4& _view )
{
	// near plane
	_FRect3D( ntl, ntr, nbr, nbl );
	//far plane
	_FRect3D( ftr, ftl, fbl, fbr );
	//bottom plane
	_FRect3D( nbl, nbr, fbr, fbl );
	//top plane
	_FRect3D( ntr, ntl, ftl, ftr );
	//left plane
	_FRect3D( ntl, nbl, fbl, ftl );
	// right plane
	_FRect3D( nbr, ntr, ftr, fbr );
}

#define _FSum(PlaneIdx,p1,p2,p3,p4) \
   a[PLANE_##PlaneIdx] = (FCornerPoints[FRUSTUM_##p1], FCornerPoints[FRUSTUM_##p2], FCornerPoints[FRUSTUM_##p3], FCornerPoints[FRUSTUM_##p4]) * 0.25;

void clVisualProjector::DebugRenderFrustumNormals( const LMatrix4& _proj, const LMatrix4& _view )
{
	Env->Renderer->GetCanvas()->SetMatrices( _proj, _view );

	LVector3 a[6];

	_FSum( NEAR,   ntr, ntl, nbr, nbl )
	_FSum( FAR,    ftr, ftl, fbr, fbl )
	_FSum( LEFT,   ftl, fbl, nbl, ntl )
	_FSum( RIGHT,  ftr, nbr, fbr, ntr )
	_FSum( TOP,    ftr, ftl, ntr, ntl )
	_FSum( BOTTOM, fbr, fbl, nbr, nbl )

	for ( int i = 0 ; i < 6 ; i++ )
	{
		//
		LVector3 b = a[i] + 5.0f * FPlanes[i].ToVector3();
		Env->Renderer->GetCanvas()->Arrow3D( a[i], b, 0.3f, LC_White, LC_White );
	}
}

#undef _FSum
#undef _FRect3D

clVisualLight::clVisualLight()
{
}
clVisualLight::~clVisualLight()
{
}

void clVisualLight::UpdateTransform()
{
	clVisualProjector::UpdateTransform();

	if ( !FScene ) { return; }

	if ( FDebugGeomID > -1 ) { FScene->GetScene()->SetLocalTransform( FDebugGeomID, GetTransform() ); }

	SyncWithScene( FScene->GetScene() );
}

void clVisualLight::SyncWithScene( clScene* Scene )
{
	LMatrix4 View = GetTransform();
	LMatrix4 Proj = GetProjection();

	Scene->FLightTransforms[FInternalID]  = View;
	Scene->FLightProjections[FInternalID] = Proj;

	Scene->FLightShadowMatrices[FInternalID] = View * Proj;

	Scene->FLightEnabled[FInternalID] = ( float )FEnabled;

	Scene->FLightAmbientColors[FInternalID] = FAmbientColor;
	Scene->FLightSpecularColors[FInternalID] = FSpecularColor;
	Scene->FLightDiffuseColors[FInternalID] = FDiffuseColor;

	Scene->FLightAttenuations[FInternalID] = FAttenuations;
}

clVisualSpotLight::clVisualSpotLight(): FSpotParams( 0.3f, 1.0f, 0.0f )
{
}

clVisualSpotLight::~clVisualSpotLight()
{
}

void clVisualLight::UpdateLight()
{
	if ( !FScene ) { return; }

	SyncWithScene( FScene->GetScene() );
}

void clVisualSpotLight::SyncWithScene( clScene* Scene )
{
	clVisualLight::SyncWithScene( Scene );

	Scene->FLightSpotParams[FInternalID] = FSpotParams;
	mtx4 View = GetTransform();
	Scene->FLightSpotDirections[FInternalID] = FUseTarget ? ( FTarget - GetPosition() ).GetNormalized() /*FDirection*/ : View[2].ToVector3();
}

void clVisualSpotLight::AddToScene( clVisualScene* VS )
{
	clVisualLight::AddToScene( VS );

	clScene* S = VS->GetScene();
//	AddDebugGeometry(S);

	int Idx = S->AddSpotLight( GetPosition(), GetDirection(), FAmbientColor, FAttenuations, vec3( 0, 0, 0 ) );
	FInternalID = Idx;
}

void clVisualSpotLight::RemoveFromScene( clVisualScene* VS )
{
	VS->GetScene()->RemoveLight( FInternalID );

	clVisualLight::RemoveFromScene( VS );
}

clVisualPointLight::clVisualPointLight()
{
}

clVisualPointLight::~clVisualPointLight()
{
}

void clVisualPointLight::AddToScene( clVisualScene* VS )
{
	clVisualLight::AddToScene( VS );

	clScene* S = VS->GetScene();
//	AddDebugGeometry(S);

	int Idx = S->AddPointLight( GetPosition(), LC_White, vec3( 1, 0, 0 ), vec3( 0, 0, 0 ) );
	FInternalID = Idx;
}

void clVisualPointLight::RemoveFromScene( clVisualScene* VS )
{
	VS->GetScene()->RemoveLight( FInternalID );

	clVisualLight::RemoveFromScene( VS );
}

void clVisualPointLight::SyncWithScene( clScene* Scene )
{
	clVisualLight::SyncWithScene( Scene );
}

clVisualDirectionalLight::clVisualDirectionalLight()
{
}

void clVisualDirectionalLight::SyncWithScene( clScene* Scene )
{
	clVisualLight::SyncWithScene( Scene );
}

clVisualDirectionalLight::~clVisualDirectionalLight()
{
}

void clVisualDirectionalLight::AddToScene( clVisualScene* VS )
{
	clVisualLight::AddToScene( VS );

	clScene* S = VS->GetScene();
//	AddDebugGeometry(S);

	int Idx = S->AddDirLight( GetPosition(), LC_White, vec3( 1, 0, 0 ) );
	FInternalID = Idx;
}

void clVisualDirectionalLight::RemoveFromScene( clVisualScene* VS )
{
	VS->GetScene()->RemoveLight( FInternalID );

	clVisualLight::RemoveFromScene( VS );
}

clVisualCSMDirectionalLight::clVisualCSMDirectionalLight()
{
}

clVisualCSMDirectionalLight::~clVisualCSMDirectionalLight()
{
}

/*
 * 12/06/2010
     Log section added
*/
