/*
   (Part of Linderdaum Engine)
   Version 0.05.60
   (17/12/2005)
   (C) Sergey Kosarevsky, 2005
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "I_FSGraph.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/Console.h"
#include "Core/CVars.h"

#include "Math/LRandom.h"
#include "Geometry/VertexAttribs.h"

#include "LColors.h"

void clGUIFullScreenGraph::Event_Registered( iGUIResponder* Source )
{
	clGUIPanel::Event_Registered( Source );

	FGeometry->SetVertexAttribs( clVertexAttribs::Create( LIST_SIZE, L_TEXCOORDS_BIT ) );
	FGeometry->GetVertexAttribs()->FPrimitiveType = L_PT_LINE_STRIP;
}

float clGUIFullScreenGraph::GetValue( int Index ) const
{
	int IntIndex = FStart + Index;

	if ( IntIndex > LIST_SIZE - 1 )
	{
		IntIndex = IntIndex % ( LIST_SIZE - 1 );
	}

	return FList[ IntIndex ];
}

float clGUIFullScreenGraph::GetAverage() const
{
	float Sum = 0;

	for ( int i = 0; i != LIST_SIZE; ++i )
	{
		Sum += GetValue( i );
	}

	return Sum / static_cast<float>( LIST_SIZE );
}

float clGUIFullScreenGraph::GetMax() const
{
	float Max = 1;

	for ( int i = 0; i != LIST_SIZE; ++i )
	{
		float Value = GetValue( i );

		if ( Value > Max )
		{
			Max = Value;
		}
	}

	return Max;
}

void clGUIFullScreenGraph::Event_Timer( iGUIResponder* Source, float DeltaTime )
{
	if ( GetConsoleVariable() )
	{
		SetVisible( GetConsoleVariable()->GetBool() );
	}

	clCVar* FPS = Env->Console->GetVar( FReadVarName );

	FList[ FStart ] = static_cast<float>( FPS->GetDouble() );

	FStart = ( FStart == 0 ) ? LIST_SIZE - 1 : FStart - 1;

	float Average = GetMax();

	//
	for ( int i = 0; i != LIST_SIZE; ++i )
	{
		float Value = GetValue( i );

		FGeometry->GetVertexAttribs()->FVertices.GetPtr()[i].X = 1.0f - static_cast<float>( i ) / static_cast<float>( LIST_SIZE );
		FGeometry->GetVertexAttribs()->FVertices.GetPtr()[i].Y = 1.0f - ( Value / Average );
		FGeometry->GetVertexAttribs()->FVertices.GetPtr()[i].Z = 0.0f;
	}

	FGeometry->CommitChanges();

	clGUIPanel::Event_Timer( Source, DeltaTime );
}

/*
 * 18/11/2005
     It's here
*/
