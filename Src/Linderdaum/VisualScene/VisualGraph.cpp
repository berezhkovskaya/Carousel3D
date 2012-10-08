/**
 * \file VisualGraph.cpp
 * \brief Visual graph
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "VisualGraph.h"
#include "VisualScene.h"

#include "Scene/Scene.h"
#include "Math/LGraph.h"

#include "Environment.h"
#include "Geometry/Geom.h"
#include "Scene/Material.h"
#include "Resources/ResourcesManager.h"

void clVisualGraph::SetMaterialName( const LString& Name, clMaterial** MatPtr )
{
	*MatPtr = Env->Resources->LoadMaterial( Name );
}

LString clVisualGraph::GetMaterialName( clMaterial* MatPtr ) const
{
	if ( !MatPtr ) { return ""; }

	return MatPtr->GetFileName();
}

void clVisualGraph::UpdateTransform()
{
	clVisualRenderable::UpdateTransform();

	if ( FScene )
	{
		FScene->GetScene()->SetLocalTransform( FGraphGeomID, FTransform.FMatrix );
	}
}

clGeom* clVisualGraph::GetGraphGeometry()
{
	if ( !FGraphGeometry )
	{
		// regenerate geometry
		FGraphGeometry = FGraph->CreateGraphGeometry( FBeadRadius, FStickRadius, FArrowSize, FUseCubes, FBeadMaterial, FStickMaterial, FArrowMaterial );
	}

	return FGraphGeometry;
}

void clVisualGraph::AddDebugGeometry( clScene* S )
{
	FGraphGeomID = S->AddGeom( GetGraphGeometry() );
//	S->SetMtl(FGraphGeomID, Env->Resources->LoadMaterial("MaterialSystem/default.material"));
}

void clVisualGraph::RemoveDebugGeometry( clScene* S )
{
	S->RemoveItem( FGraphGeomID );
}

clVisualGraph::clVisualGraph()
{
	FGraphGeometry = NULL;
}

clVisualGraph::~clVisualGraph()
{
	// We do not manage the graph itself
//	if (FGraph) FGraph->DisposeObject();

	if ( FGraphGeometry ) { FGraphGeometry->DisposeObject(); }
}

/*
 * 09/12/2010
     Log section added
*/
