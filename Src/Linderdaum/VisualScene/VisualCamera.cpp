/**
 * \file VisualCamera.cpp
 * \brief Visual camera
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VisualCamera.h"
#include "CameraPositioner.h"

clVisualCamera::clVisualCamera()
{
	FDebug = false;
	FPositioner = NULL;
}

clVisualCamera::~clVisualCamera()
{
}

void clVisualCamera::SetTransform( const LMatrix4& Mtx )
{
	clVisualProjector::SetTransform( Mtx );
}

LMatrix4 clVisualCamera::GetTransform() const
{
	return clVisualProjector::GetTransform();
}

void clVisualCamera::RenderDebug( const LMatrix4& Proj, const LMatrix4& View )
{
	if ( FDebug )
	{
		clVisualProjector::RenderDebug( Proj, View );
	}
}

void clVisualCamera::Update( float dt )
{
}

/*
 * 09/12/2010
     Log section added
*/
