/**
 * \file VisualObject.cpp
 * \brief Visual object
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VisualObject.h"
#include "VisualScene.h"

clVisualObject::clVisualObject()
{
	FScene = NULL;
}

clVisualObject::~clVisualObject()
{
}

void clVisualObject::AddToScene( clVisualScene* VS )
{
	AddGeometry( VS->GetScene() );
//	AddDebugGeometry( VS->GetScene()/*VS->GetDebugScene()*/ ); //Now we have one scene
	SetScene( VS );
}

void clVisualObject::RemoveFromScene( clVisualScene* VS )
{
	RemoveGeometry( VS->GetScene() );
//	RemoveDebugGeometry( VS->GetScene()/*VS->GetDebugScene()*/ ); //Now we have one scene
	SetScene( NULL );
}

bool clVisualObject::BeginLoad()
{
	return true;
}

bool clVisualObject::EndLoad()
{
	UpdateTransform();
	return true;
}

void clVisualObject::MakeLocalRotation( const LMatrix4& Mtx )
{
	LMatrix3 M = Mtx.ExtractMatrix3() * FTransform.FMatrix.ExtractMatrix3();

	for ( int i = 0 ; i < 3 ; i++ )
		for ( int j = 0 ; j < 3 ; j++ )
		{
			FTransform.FMatrix[i][j] = M[i][j];
		}
}

bool clVisualObject::IntersectsWithRay( const LMatrix4& Proj, const LMatrix4& View, const LVector3& P, const LVector3& Dir, LVector3& isect )
{
	return false;
}

void clVisualObject::MakeGlobalTranform( const LMatrix4& Mtx )
{
	FTransform.FMatrix = Mtx * FTransform.FMatrix;
	UpdateTransform();
}

void clVisualObject::UpdateTransform()
{
}

void clVisualObject::SetTransform( const LMatrix4& Mtx )
{
	FTransform.FMatrix = Mtx;
	UpdateTransform();
}

LMatrix4 clVisualObject::GetTransform() const
{
	return FTransform.FMatrix;
}

void clVisualObject::SetAngles( const LVector3& A )
{
	FTransform.SetAngles( A );
	UpdateTransform();
}

LVector3 clVisualObject::GetAngles() const
{
	return FTransform.GetAngles();
}

void clVisualObject::SetPosition( const LVector3& P )
{
	FTransform.SetPosition( P );
	UpdateTransform();
}

LVector3 clVisualObject::GetPosition() const
{
	return FTransform.GetPosition();
}

/*
 * 09/12/2010
     Log section added
*/
