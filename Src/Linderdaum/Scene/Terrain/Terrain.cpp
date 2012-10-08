/**
 * \file Terrain.cpp
 * \brief Terrain
 * \version 0.5.89a
 * \date 06/04/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"

#include "Terrain.h"

#include "Engine.h"
#include "Core/Console.h"
#include "Core/Linker.h"
#include "Core/CVars.h"
#include "Core/Logger.h"
#include "Core/VFS/FileSystem.h"
#include "Scene/Heightmaps/HeightMapFacetter.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/RenderState.h"
#include "Renderer/iVertexArray.h"

LAABoundingBox clTerrain::GetBoundingBox()
{
	return FBoundingBox;
}

clTerrain::~clTerrain()
{
//  delete( FOptimizer );
}

void clTerrain::RenderDirect( iShaderProgram* SPOverride )
{
	/*
	if ( !FOptimizer )
	{
	   FOptimizer = Env->Linker->Instantiate<clSceneGraphOptimizer>( "clSceneGraphOptimizer" );
	   FOptimizer->OptimizeSceneGraph( FSceneNode );
	}

	Env->SceneManager->GetIndexRenderer()->Render( FOptimizer->GetOptimizedSceneGraph(), NULL, NULL );
	*/
}

void clTerrain::LoadHeightMap( const LString& FileName,
                               int SizeX, int SizeY, int BitsPerPixel,
                               const LAABoundingBox& InGameDimensions )
{
	FHeightMap.SetHeightDataProvider( Env->Renderer->CreateHeightDataProvider( SizeX, SizeY, BitsPerPixel, FileName ) );

	// define world-to-heightmap transform
	FBoundingBox = InGameDimensions;
	FHeightMap.SetWorldAABB( InGameDimensions );

	// init arrays
	clPtr<iHeightMapFacetter> Facetter = Env->Renderer->CreateHeightMapFacetter();

	FLandscapeVertices = Env->Renderer->AllocateEmptyVA();
	FLandscapeVertices->SetVertexAttribs( Facetter->FacetHeightMap( FHeightMap ) );
	//
//   FLandscapeNode = Env->SceneManager->CreateGeometryNode( "" );
	//FLandscapeNode->SetVertexArray( FLandscapeVertices );

	// terrain shader
//   clRenderState* TerrainShader = Env->Resources->LoadShader( "MaterialSystem/Terrain/Terrain.shader" );

	// materials
	/*
	FLandscapeShaderNode = Env->SceneManager->CreateMaterialNode( "", NULL, "" );
	FLandscapeShaderNode->ddSubObject( FLandscapeNode );
	FLandscapeShaderNode->SetSurfaceShader( TerrainShader );

	if (FSceneNode == NULL)
	{
	  FSceneNode = Env->SceneManager->CreateTransformNode(FileName+"_TerrainNode", NULL);
	}

	FSceneNode->ddSubObject( FLandscapeShaderNode );
	*/
}

/*
 * 10/04/2009
     It's here
*/
