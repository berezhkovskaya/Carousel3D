/**
 * \file VisualRenderables.cpp
 * \brief Visual renderables
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VisualRenderables.h"
#include "VisualScene.h"

#include "Renderer/Canvas.h"
#include "Core/Linker.h"
#include "Renderer/iRenderContext.h"
#include "Environment.h"
#include "Resources/ResourcesManager.h"

#include "Geometry/Surfaces.h"

#include "Scene/Scene.h"

clVisualGenerator::clVisualGenerator() : FGenerator( NULL ) //, FGeneratorType("")
{
}

clVisualGenerator::~clVisualGenerator()
{
	delete FGenerator;
}

void clVisualGenerator::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
	clSimplePlane* Plane = dynamic_cast<clSimplePlane*>( FGenerator );

	if ( Plane )
	{
		clCanvas* C = Env->Renderer->GetCanvas();
		C->SetMatrices( Proj, View );
		C->Plane( Plane->P, Plane->V1, Plane->V2, Plane->NumU, Plane->NumV, Plane->MaxU - Plane->MinU, Plane->MaxV - Plane->MinV, LC_Red );
	}
}

void clVisualGenerator::AddGeometry( clScene* S )
{
	RegenerateSurface();

	clVisualGeom::AddGeometry( S );
}

void clVisualGenerator::RemoveGeometry( clScene* S )
{
	clVisualGeom::RemoveGeometry( S );
}

void clVisualGenerator::RegenerateSurface()
{
	if ( FGeom ) { delete FGeom; }

	FGeom = FGenerator->CreateGeom();
}
/*
void VisualGenerator::SetGeneratorType( const LString& GeneratorType )
{
   if ( FGenerator ) { delete FGenerator; }

   FGeneratorType = GeneratorType;
   RegenerateSurface();
}

LString VisualGenerator::GetGeneratorType() const
{
   return FGeneratorType;
}

clUVSurfaceGenerator* VisualGenerator::GetGenerator()
{
   if ( !FGenerator ) { FGenerator = Env->Resources->CreateSurfaceGenerator( FGeneratorType ); }

   return FGenerator;
}
*/
void clVisualGeom::SetTransform( const LMatrix4& Mtx )
{
	clVisualRenderable::SetTransform( Mtx );
//	FScene->GetScene()->SetLocalTransform(FGeomID, Mtx);
}

void clVisualGeom::SetGeom( clGeom* G )
{
	FGeom = G;
}

void clVisualGeom::UpdateTransform()
{
	clVisualRenderable::UpdateTransform();

	if ( FScene )
	{
		FScene->GetScene()->SetLocalTransform( FGeomID, FTransform.FMatrix );
	}
}

LMatrix4 clVisualGeom::GetTransform() const
{
	return clVisualRenderable::GetTransform();
}

bool clVisualGeom::IntersectsWithRay( const LMatrix4& Proj, const LMatrix4& View, const LVector3& P, const LVector3& Dir, LVector3& isect )
{
	LMatrix4 Inv = FTransform.FMatrix.GetInversed();

	LVector3 Pt = Inv * P;
	LVector3 Aa = Inv.ToMatrix3() * Dir;

	bool res = false;

	float minDist = 100000.0f;

	for ( size_t i = 0 ; i < FGeom->GetCurrentMesh()->GetRigidsCount() ; i++ )
	{
		// 3. intersect the ray with part of the gizmo
		clVertexAttribs* VA = FGeom->GetCurrentMesh()->GetRigid( i );

		if ( VA->IntersectWithRay( Pt, Aa, isect ) )
		{
			res = true;

			// 4. calculate distance to camera
			float dist = ( isect - Pt ).Length();

			if ( dist < minDist )
			{
				minDist = dist;

				// convert instersection back to world coordinates
				isect = FTransform.FMatrix * isect;
			}
		}
	}

	return res;
}

void clVisualGeom::SetMaterialName( const LString& MaterialName )
{
	FMaterialName = MaterialName;

	FMaterial = MaterialName.empty() ? NULL : Env->Resources->LoadMaterial( MaterialName );
}

void clVisualMesh::SetMesh( const LString& MeshName )
{
	FGeom = Env->Resources->LoadGeom( MeshName );

	FMeshName = MeshName;
}

void clVisualGeom::SetAnimation( const LString& AnimName )
{
	FScene->GetScene()->SetAnimation( FGeomID, AnimName );
}

void clVisualGeom::SetAnimationSpeed( float Speed )
{
	FScene->GetScene()->SetAnimationSpeed( FGeomID, Speed );
}

clVisualMesh::clVisualMesh()
{
}

clVisualMesh::~clVisualMesh()
{
}

clVisualGeom::clVisualGeom() : FGeom( NULL ), FMaterial( NULL ), FMatID( -1 ), FGeomID( -1 )
{
}

clVisualGeom::~clVisualGeom()
{
	if ( FGeom ) { delete FGeom; }
}

void clVisualGeom::UpdateMaterial( clMaterial* M )
{
	if ( FGeomID > -1 )
	{
		SetMaterial( M );
		FScene->GetScene()->SetMtl( FGeomID, M );
	}
}

void clVisualGeom::UpdateGeom( clGeom* G )
{
}

/// Add renderables to the scene
void clVisualGeom::AddGeometry( clScene* S )
{
	if ( !FGeom ) { return; }

	FGeomID = S->AddGeom( FGeom );

	if ( FMaterial != NULL )
	{
		/*FMatID =*/ S->SetMtl( FGeomID, FMaterial );
	}
}

/// Remove renderables from the scene
void clVisualGeom::RemoveGeometry( clScene* S )
{
	if ( FGeomID > -1 ) { S->RemoveItem( FGeomID ); }
}

void clVisualDebugBox::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
	clCanvas* C = Env->Renderer->GetCanvas();

	C->SetMatrices( Proj, View );
	C->BoundingBox( FBox, FColor );
}

void clVisualDebugPlane::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
	clCanvas* C = Env->Renderer->GetCanvas();
	C->SetMatrices( Proj, View );
	C->Plane( LVector3( 0, 0, 0 ), LVector3( 1, 0, 0 ), LVector3( 0, 1, 0 ), 20, 20, 0.2f, 0.2f, FColor );
}

/// Find the segment in array where the Val resides
template <class T> int FindSegFast( const LArray<T>& Array, float Val )
{
	int N = static_cast<int>( Array.size() );

	if ( Val <= Array[0] )
	{
		return -1;
	}

	for ( int i = 0 ; i < N - 1 ; i++ )
	{
		if ( Val > Array[i] && Val <= Array[i + 1] )
		{
			return i;
		}
	}

	return N - 1;
}

clSplineKB::clSplineKB()
{
}

clSplineKB::~clSplineKB()
{
}

LVector3 clSplineKB::ValueForSegment( int Seg, float t )
{
///   t - T[Seg]
	return LVector3( 0, 0, 0 );
}

LVector3 clSplineKB::DerivativeForSegment( int Seg, float t )
{
	return LVector3( 0, 0, 0 );
}

LVector3 clSplineKB::SecondDerivativeForSegment( int Seg, float t )
{
	return LVector3( 0, 0, 0 );
}

LMatrix3 clSplineKB::GetCanonicalFrame( float t )
{
	return LMatrix3::Identity();
}

LVector3 clSplineKB::GetPosition( float t )
{
	return LVector3( 0, 0, 0 );
}

LVector3 clSplineKB::GetDerivative( float t )
{
	return LVector3( 0, 0, 0 );
}

LVector3 clSplineKB::GetSecondDerivative( float t )
{
	return LVector3( 0, 0, 0 );
}

clVisualCurve::clVisualCurve()
{
}

clVisualCurve::~clVisualCurve()
{
}

void clVisualCurve::AddGeometry( clScene* S )
{
}

void clVisualCurve::RemoveGeometry( clScene* S )
{
}

void clVisualCurve::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
}


/*
 * 12/06/2010
     Log section added
*/
