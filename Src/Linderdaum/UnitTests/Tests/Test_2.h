#pragma once

#include "Engine.h"
#include "Scene/Scene.h"
#include "Renderer/iRenderContext.h"

void Test_2( sEnvironment* Env )
{
	// identity transform initialization
	/*
	{
	   clSceneNode* Node = Env->SceneManager->CreateTransformNode( "", NULL );

	   LMatrix4 Mat = Node->GetLocalTransform();

	   TEST_ASSERT( Mat[0][0] != 1.0f );
	   TEST_ASSERT( Mat[1][1] != 1.0f );
	   TEST_ASSERT( Mat[2][2] != 1.0f );
	   TEST_ASSERT( Mat[3][3] != 1.0f );
	}
	// calculating simple AABB
	{
	   iVertexArray* VA = Env->Renderer->AllocateVA(4, false, L_POINTS, true, 0 );

	   VA->GetVertices()[0] = LVector3(0.0f, 0.0f, 0.0f);
	   VA->GetVertices()[1] = LVector3(1.0f, 1.0f, 0.0f);
	   VA->GetVertices()[2] = LVector3(0.0f, 2.0f, 1.0f);
	   VA->GetVertices()[3] = LVector3(1.0f, 0.0f, 3.0f);

	   VA->CommitChanges();

	   clGeometryNode* Node = Env->SceneManager->CreateGeometryNode( "" );

	   Node->SetVertexArray( VA );

	   LAABoundingBox Box = Node->GetBoundingBox();

	   TEST_ASSERT( Box.FMin.X != 0.0f );
	   TEST_ASSERT( Box.FMin.Y != 0.0f );
	   TEST_ASSERT( Box.FMin.Z != 0.0f );
	   TEST_ASSERT( Box.FMax.X != 1.0f );
	   TEST_ASSERT( Box.FMax.Y != 2.0f );
	   TEST_ASSERT( Box.FMax.Z != 3.0f );
	}

	// calculating more complex AABB
	{
	   iVertexArray* VA1 = Env->Renderer->AllocateVA(4, false, L_POINTS, true, 0);
	   iVertexArray* VA2 = Env->Renderer->AllocateVA(4, false, L_POINTS, true, 0);

	   VA1->GetVertices()[0] = LVector3(0.0f, 0.0f, 0.0f);
	   VA1->GetVertices()[1] = LVector3(1.0f, 1.0f, 0.0f);
	   VA1->GetVertices()[2] = LVector3(0.0f, 2.0f, 1.0f);
	   VA1->GetVertices()[3] = LVector3(1.0f, 0.0f, 3.0f);
	   VA1->CommitChanges();

	   VA2->GetVertices()[0] = LVector3( 0.0f, 4.0f, 0.0f);
	   VA2->GetVertices()[1] = LVector3(-1.0f, 1.0f, 0.0f);
	   VA2->GetVertices()[2] = LVector3( 0.0f, 2.0f, 1.0f);
	   VA2->GetVertices()[3] = LVector3( 1.0f, 0.0f, 5.0f);
	   VA2->CommitChanges();

	   clSceneNode* Node1 = Env->SceneManager->CreateTransformNode( "", NULL );
	   clGeometryNode*  Node2 = Env->SceneManager->CreateGeometryNode( "" );
	   clGeometryNode*  Node3 = Env->SceneManager->CreateGeometryNode( "" );

	   Node1->ddSubObject( Node2 );
	   Node1->ddSubObject( Node3 );

	   Node2->SetVertexArray( VA1 );
	   Node3->SetVertexArray( VA2 );

	   LAABoundingBox Box = Node1->GetBoundingBox();

	   TEST_ASSERT( Box.FMin.X != -1.0f );
	   TEST_ASSERT( Box.FMin.Y != 0.0f );
	   TEST_ASSERT( Box.FMin.Z != 0.0f );
	   TEST_ASSERT( Box.FMax.X != 1.0f );
	   TEST_ASSERT( Box.FMax.Y != 4.0f );
	   TEST_ASSERT( Box.FMax.Z != 5.0f );
	}
	// searching traverser
	{
	   clSceneNode* Node1 = Env->SceneManager->CreateTransformNode( "Root", NULL  );
	   clGeometryNode*  Node2 = Env->SceneManager->CreateGeometryNode(  "Geom1" );
	   clGeometryNode*  Node3 = Env->SceneManager->CreateGeometryNode(  "Geom2" );

	   Node1->ddSubObject( Node2 );
	   Node1->ddSubObject( Node3 );

	   clSceneNode* FNode1;
	   clSceneNode* FNode2;
	   clSceneNode* FNode3;
	   clSceneNode* FNode4;

	   FNode1 = Node1->FindSceneNode( "Root" );
	   FNode2 = Node1->FindSceneNode( "Geom1" );
	   FNode3 = Node1->FindSceneNode( "Geom2" );
	   FNode4 = Node1->FindSceneNode( "not found" );

	   TEST_ASSERT( FNode1 != Node1 );
	   TEST_ASSERT( FNode2 != Node2 );
	   TEST_ASSERT( FNode3 != Node3 );
	   TEST_ASSERT( FNode4 != NULL );
	}
	*/
}

/*
 * 23/03/2005
     It's here
*/
