#pragma once

#include "Linderdaum.h"

void Test_4( sEnvironment* Env )
{
	// picking traverser
	{
		/*
		iVertexArray* VA1 = Env->Renderer->AllocateVA(6, 1, false, L_TRIANGLES, true, 0);
		iVertexArray* VA2 = Env->Renderer->AllocateVA(6, 1, false, L_TRIANGLES, true, 0);

		GeomServer->CreateRect2D(VA1,
		                         -10.0f, -10.0f,
		                         10.0f,  10.0f,
		                         +2.0f, false);
		GeomServer->CreateRect2D(VA2,
		                         -10.0f, -10.0f,
		                         10.0f, 10.0f,
		                         -2.0f, false);

		clCameraNode*    NodeC = Env->SceneManager->CreateCameraNode( "" );
		clMaterialNode*  NodeM = Env->SceneManager->CreateMaterialNode( "", NULL, "" );
		clSceneNode* Node1 = Env->SceneManager->CreateTransformNode( "", NULL );
		clGeometryNode*  Node2 = Env->SceneManager->CreateGeometryNode( "" );
		clGeometryNode*  Node3 = Env->SceneManager->CreateGeometryNode( "" );

		NodeC->ddSubObject( NodeM );
		NodeM->ddSubObject( Node1 );
		Node1->ddSubObject( Node2 );
		Node1->ddSubObject( Node3 );

		Node2->SetVertexArray( VA1 );
		Node3->SetVertexArray( VA2 );

		   NodeM->SetSurfaceShader( Env->Resources->LoadShader("tests\\shaders\\board.shader") );

		LRotator Rotator;
		Rotator.SetAxis( LVector3(1, 0, 0) );
		Rotator.SetAngle( 90.0f );

		Node1->SetLocalTransform( Rotator.ToMatrix4() );

		// projection
		const float Aspect  = static_cast<float>( Env->Engine->Viewport->GetWidth() ) / Env->Engine->Viewport->GetHeight();

		NodeC->Projection() = Math::Perspective( 80.0f, Aspect, 0.4f, 200000.0f);

		clIndexPicker* Picker = Env->SceneManager->GetIndexPicker();

		clGeometryNode* FNode1 = Picker->Pick( NodeC->GetIndexedSceneGraph(), LVector2(0.5f, 0.1f), NodeC );
		clGeometryNode* FNode2 = Picker->Pick( NodeC->GetIndexedSceneGraph(), LVector2(0.5f, 0.9f), NodeC );
		clGeometryNode* FNode3 = Picker->Pick( NodeC->GetIndexedSceneGraph(), LVector2(0.0f, 0.0f), NodeC );
		clGeometryNode* FNode4 = Picker->Pick( NodeC->GetIndexedSceneGraph(), LVector2(0.5f, 1.02f), NodeC );

		TEST_ASSERT( FNode1 != Node2 );
		TEST_ASSERT( FNode2 != Node3 );
		TEST_ASSERT( FNode3 != NULL );
		TEST_ASSERT( FNode4 != NULL );

		delete( NodeC );
		     */
	}
}

/*
 * 23/03/2005
     It's here
*/
