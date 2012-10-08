/**
 * \file I_Bubbles.cpp
 * \brief Bubbles 2D particle system
 * \version 0.5.89a
 * \date 28/06/2009
 * \author Sergey Kosarevsky, 2005-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "I_Bubbles.h"

#include "Math/LRandom.h"
#include "Environment.h"
#include "Geometry/VertexAttribs.h"
#include "Renderer/iRenderContext.h"

LVector3 clGUIBubbles::RandomVelocity()
{
	return LVector3( Math::RandomInRange( -0.00025f, 0.00025f ),
	                 Math::RandomInRange(   -0.001f,  -0.002f ),
	                 0.0f );
}

clGUIBubbles::~clGUIBubbles()
{
	delete( FGeometry );
}

void clGUIBubbles::Event_Registered( iGUIResponder* Source )
{
	FGeometry = Env->Renderer->AllocateEmptyVA();
	FGeometry ->SetVertexAttribs( clVertexAttribs::Create( 6 * MAX_BUBBLES, L_TEXCOORDS_BIT ) );

	LVector4* TexCoords = FGeometry->GetVertexAttribs()->FTexCoords.GetPtr();

	for ( int i = 0; i != MAX_BUBBLES; ++i )
	{
		// generate bubble position
		FBubbles[i] = LVector3( Math::RandomInRange( GetX1(), GetX2() ), Math::RandomInRange( GetY1(), GetY2() ), 0.0f );

		// generate bubble velocity
		FVelocities[i] = RandomVelocity();

		// prefetch texture coordinates for our blazing fast pure GPU billboarder
		TexCoords[i*6+0] = LVector4( 0, 0, 0, 0 );
		TexCoords[i*6+1] = LVector4( 0, 1, 0, 0 );
		TexCoords[i*6+2] = LVector4( 1, 1, 0, 0 );

		TexCoords[i*6+3] = LVector4( 0, 0, 0, 0 );
		TexCoords[i*6+4] = LVector4( 1, 1, 0, 0 );
		TexCoords[i*6+5] = LVector4( 1, 0, 0, 0 );
	}

	FGeometry->CommitChanges();
}

void clGUIBubbles::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	static const float SIZE_X = 0.020f;
	static const float SIZE_Y = 0.025f;

	LVector3* Vertices = FGeometry->GetVertexAttribs()->FVertices.GetPtr();

	for ( int i = 0; i != MAX_BUBBLES; ++i )
	{
		FBubbles[i] += FVelocities[i] * DeltaTime * 100;

		if ( ( FBubbles[i].Y < GetY1() - SIZE_Y ) ||
		     ( FBubbles[i].Y > GetY2() + SIZE_Y ) ||
		     ( FBubbles[i].X < GetX1() - SIZE_X ) ||
		     ( FBubbles[i].X > GetX2() + SIZE_X ) )
		{
			// create new particle
			FBubbles[i] = LVector3( Math::RandomInRange( GetX1(), GetX2() ), GetY2() + SIZE_Y, 0.0f );

			FVelocities[i] = RandomVelocity();
		}

		// replicate vertices for shader-based billboarder
		Vertices[i*6+0] = FBubbles[i];
		Vertices[i*6+1] = FBubbles[i];
		Vertices[i*6+2] = FBubbles[i];
		Vertices[i*6+3] = FBubbles[i];
		Vertices[i*6+4] = FBubbles[i];
		Vertices[i*6+5] = FBubbles[i];
	}

	FGeometry->CommitChanges();

	clGUIPanel::Event_Timer( Source, DeltaTime );
}

/*
 * 28/06/2009
     Implemented using triangles
 * 25/01/2007
     Moved to ComCtls
 * 17/04/2005
     It's here
*/
