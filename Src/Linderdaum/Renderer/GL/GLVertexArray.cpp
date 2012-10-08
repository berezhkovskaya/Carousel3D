/**
 * \file GLVertexArray.cpp
 * \brief OpenGL vertex array
 * \version 0.5.91
 * \date 07/02/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "GLVertexArray.h"
#include "GLSL.h"
#include "GLRenderContext.h"

#include "Environment.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "Geometry/VertexAttribs.h"
#include "Generated/LGL/LGL.h"

#if defined(USE_OPENGL_3)
#	define USE_VAO
#else
#	undef USE_VAO
#endif

#ifdef USE_VAO
#	define VAO_BIND()    LGL3->glBindVertexArray( FVAOID )
#	define VAO_UNBIND()  LGL3->glBindVertexArray( 0 )
#else
#	define VAO_BIND()    BindVertexArray(SP)
#	define VAO_UNBIND()  UnBindVertexArray(SP)
#endif

clGLVertexArray::clGLVertexArray(): FGLPrimitive( 0 ),
	FVAOID( 0 ),
	FVBOID( 0 ),
	FVBOIndicesID( 0 ),
	FAttribVBOOffset( L_VS_TOTAL_ATTRIBS ),
	FEnumeratedStreams( L_VS_TOTAL_ATTRIBS ),
	FVALocksCount( NULL ),
	FTriangles( 0 ),
	FCount( 0 ),
	FIndicesType( 0 ),
	FIndicesPtr( NULL ),
	FDontUseIndices( false )
{
	for ( int i = 0; i != L_VS_TOTAL_ATTRIBS; i++ )
	{
		FAttribVBOOffset[ i ] = NULL;
	}
}

clGLVertexArray::~clGLVertexArray()
{
	Env->DisconnectObject( this );

#ifdef USE_VAO

	if ( FVAOID        )
	{
		LGL3->glDeleteVertexArrays( 1, &FVAOID );
	}

#endif

	if ( FVBOID        )
	{
		LGL3->glDeleteBuffers( 1, &FVBOID );
	}

	if ( FVBOIndicesID )
	{
		LGL3->glDeleteBuffers( 1, &FVBOIndicesID );
	}
}

void clGLVertexArray::AfterConstruction()
{
	iVertexArray::AfterConstruction();

	GetAPI( Env );

	Env->Connect( L_EVENT_SURFACE_ATTACHED, BIND( &clGLVertexArray::Event_SURFACE_ATTACHED ) );
	Env->Connect( L_EVENT_SURFACE_DETACHED, BIND( &clGLVertexArray::Event_SURFACE_DETACHED ) );
}

void clGLVertexArray::EVENT_HANDLER( Event_SURFACE_DETACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLVertexArray::EVENT_HANDLER( Event_SURFACE_DETACHED )" );

#ifdef USE_VAO
	FVAOID = 0;
#endif

	FVBOID = 0;
	FVBOIndicesID = 0;
}

void clGLVertexArray::EVENT_HANDLER( Event_SURFACE_ATTACHED )
{
	Env->Logger->LogP( L_DEBUG, "clGLVertexArray::EVENT_HANDLER( Event_SURFACE_ATTACHED )" );

	SetVertexAttribs( GetVertexAttribs() );
}

void clGLVertexArray::FeedIntoGPU( bool Wireframe, iShaderProgram* SP ) const
{
	VAO_BIND();

	bool Tesselation = SP->IsTesselationEnabled();

	if ( FDontUseIndices )
	{
		LGL3->glDrawArrays( Wireframe ? GL_LINE_LOOP : ( Tesselation ? GL_PATCHES : FGLPrimitive ), 0, FCount );
	}
	else
	{
		LGL3->glDrawElements( Wireframe ? GL_LINE_LOOP : ( Tesselation ? GL_PATCHES : FGLPrimitive ), FCount, FIndicesType, FIndicesPtr );
	}

	Env->Renderer->UpdateStatsTriangles( FTriangles );

	VAO_UNBIND();
}

void clGLVertexArray::FeedIntoGPUInstanced( bool Wireframe, int NumInstances, iShaderProgram* SP ) const
{
	VAO_BIND();

	bool Tesselation = SP->IsTesselationEnabled();

	if ( FDontUseIndices )
	{
		LGL3->glDrawArraysInstanced( Wireframe ? GL_LINE_LOOP : ( Tesselation ? GL_PATCHES : FGLPrimitive ), 0, FCount, NumInstances );
	}
	else
	{
		LGL3->glDrawElementsInstanced( Wireframe ? GL_LINE_LOOP : ( Tesselation ? GL_PATCHES : FGLPrimitive ), FCount, FIndicesType, FIndicesPtr, NumInstances );
	}

	Env->Renderer->UpdateStatsTriangles( NumInstances * FTriangles );

	VAO_UNBIND();
}

void clGLVertexArray::BindVertexArray( iShaderProgram* SP ) const
{
#if defined( USE_OPENGL_3 )
	if ( FVBOID        )
	{
		LGL3->glBindBuffer( GL_ARRAY_BUFFER, FVBOID );
	}

	if ( FVBOIndicesID )
	{
		LGL3->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, FVBOIndicesID );
	}
#else
	FLocalRenderer->SetActiveVBO( FVBOID, FVBOIndicesID );
#endif

	LGL3->glVertexAttribPointer( L_VS_VERTEX, L_VS_VEC_COMPONENTS[ 0 ], GL_FLOAT, GL_FALSE, 0, FAttribVBOOffset[ 0 ] );
	LGL3->glEnableVertexAttribArray( L_VS_VERTEX );

	// start from 1
	//	for ( int i = 1; i != L_VS_TOTAL_ATTRIBS; i++ )
	// this is due to Mali-400 firmware bug
//	int NumStreams = SP ? SP->GetActiveAttribStreams() : L_VS_TOTAL_ATTRIBS;

	const LArray<bool>& Streams = SP ? SP->EnumerateVertexStreams() : LArray<bool>( L_VS_TOTAL_ATTRIBS );

	for ( int i = 1; i < L_VS_TOTAL_ATTRIBS; i++ )
	{
		LGL3->glVertexAttribPointer( i, L_VS_VEC_COMPONENTS[ i ], GL_FLOAT, GL_FALSE, 0, FAttribVBOOffset[ i ] );

		if ( FAttribVBOOffset[ i ] && Streams[ i ] )
		{
			LGL3->glEnableVertexAttribArray( i );
		}
//		else
//		{
//			LGL3->glDisableVertexAttribArray( i );
//		}
	}
}

void clGLVertexArray::UnBindVertexArray( iShaderProgram* SP ) const
{
	// nothing to do here
}


void clGLVertexArray::CommitChanges()
{
	if ( !FVALocksCount )
	{
		FVALocksCount = Env->Console->GetVar( "Stats.VALocksCount" );
	}

	FVALocksCount->AddInt( 1 );

	// setup OpenGL rendering primitive type
	if ( GetVertexAttribs()->FPrimitiveType == L_PT_POINT          )
	{
		FGLPrimitive = GL_POINTS;
	}
	else if ( GetVertexAttribs()->FPrimitiveType == L_PT_LINE           )
	{
		FGLPrimitive = GL_LINES;
	}
	else if ( GetVertexAttribs()->FPrimitiveType == L_PT_LINE_STRIP     )
	{
		FGLPrimitive = GL_LINE_STRIP;
	}
	else if ( GetVertexAttribs()->FPrimitiveType == L_PT_TRIANGLE       )
	{
		FGLPrimitive = GL_TRIANGLES;
	}
	else if ( GetVertexAttribs()->FPrimitiveType == L_PT_TRIANGLE_STRIP )
	{
		FGLPrimitive = GL_TRIANGLE_STRIP;
	};

	if ( FVBOID )
	{
		LGL3->glBindBuffer( GL_ARRAY_BUFFER, FVBOID );

		int VCount = static_cast<int>( GetVertexAttribs()->FVertices.size() );
		int AVCount = static_cast<int>( GetVertexAttribs()->GetActiveVertexCount() );

		size_t DataSize = 0;

		for ( int i = 0; i != L_VS_TOTAL_ATTRIBS; i++ )
		{
			// using precached enumerated streams
			DataSize += FEnumeratedStreams[i] ? sizeof( float ) * L_VS_VEC_COMPONENTS[ i ] * VCount : 0;
		}

		// drop old buffer
		LGL3->glBufferData( GL_ARRAY_BUFFER, DataSize, NULL, GetVertexAttribs()->FDynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW );

		for ( int i = 0; i != L_VS_TOTAL_ATTRIBS; i++ )
		{
			if ( FEnumeratedStreams[i] )
			{
				LGL3->glBufferSubData( GL_ARRAY_BUFFER, ( GLintptrARB )FAttribVBOOffset[ i ], AVCount * sizeof( float ) * L_VS_VEC_COMPONENTS[ i ], FEnumeratedStreams[ i ] );
			}
		}
	}

	if ( GetVertexAttribs()->FIndices.size() == 0 )
	{
		FDontUseIndices = true;

		FIndicesPtr = NULL;

		FCount = static_cast<GLsizei>( GetVertexAttribs()->FActiveVertexCount );
	}
	else
	{
		FDontUseIndices = false;

		FIndicesType = GL_UNSIGNED_INT; // Use GL_UNSIGNED_SHORT for 16-bit indices
		FIndicesPtr  = FVBOIndicesID ? 0 : GetVertexAttribs()->FIndices.GetPtr();

		FCount = static_cast<GLsizei>( GetVertexAttribs()->FIndices.size() );
	}

	switch ( FGLPrimitive )
	{
		case GL_POINTS:
		case GL_TRIANGLE_STRIP:
			FTriangles = FCount;
			break;
		case GL_LINES:
		case GL_TRIANGLES:
		case GL_PATCHES:
			FTriangles = FCount / 3;
			break;
		case GL_LINE_STRIP:
			FTriangles = FCount / 2;
			break;
	}
}

void clGLVertexArray::RegenerateOffsets()
{
	int  VertexCount = static_cast<int>( GetVertexAttribs()->FVertices.size() );

	LArray<int> Sizes( L_VS_TOTAL_ATTRIBS );

	// these pointers are Offsets inside a VBO buffer
	for ( int i = 0; i != L_VS_TOTAL_ATTRIBS; i++ )
	{
		Sizes[ i ] = FEnumeratedStreams[i] ? sizeof( float ) * L_VS_VEC_COMPONENTS[ i ] * VertexCount : 0;

		int Offset = 0;

		for ( int j = 0; j != i; j ++ )
		{
			Offset += Sizes[ j ];
		}

		FAttribVBOOffset[ i ] = ( void* )( size_t )Offset;
	}
}

void clGLVertexArray::SetVertexAttribs( clVertexAttribs* Attribs )
{
	iVertexArray::SetVertexAttribs( Attribs );

	FATAL( Attribs->FVertices.size() == 0, "Unable to use zero-size vertex array" );

	const bool UseVBO = true;

#ifdef USE_VAO

	if ( FVAOID )
	{
		LGL3->glDeleteVertexArrays( 1, &FVAOID );
	}

	LGL3->glGenVertexArrays( 1, &FVAOID );
#endif

	// setup 32-bit indices
	if ( UseVBO && Attribs->FIndices.GetPtr() )
	{
		if ( FVBOIndicesID )
		{
			LGL3->glDeleteBuffers( 1, &FVBOIndicesID );
		}

		LGL3->glGenBuffers( 1, &FVBOIndicesID );
		//LGL3->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, FVBOIndicesID );
		FLocalRenderer->SetActiveVBO( FVBOID, FVBOIndicesID );
		LGL3->glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		                    Attribs->FIndices.size() * sizeof( Luint ),
		                    Attribs->FIndices.GetPtr(),
		                    GL_STATIC_DRAW );
	}

	clVertexAttribs* SrcVA = GetVertexAttribs();
	FEnumeratedStreams = SrcVA->EnumerateVertexStreams();

	// setup vertices
	if ( UseVBO )
	{
		if ( FVBOID )
		{
			LGL3->glDeleteBuffers( 1, &FVBOID );
		}

		size_t VertexCount = Attribs->FVertices.size();

		size_t DataSize = 0;

		for ( int i = 0; i != L_VS_TOTAL_ATTRIBS; i++ )
		{
			DataSize += FEnumeratedStreams[i] ? sizeof( float ) * L_VS_VEC_COMPONENTS[ i ] * VertexCount : 0;
		}

		RegenerateOffsets();

//      Lenum UsageFlag = GetVertexAttribs()->FDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		Lenum UsageFlag = GetVertexAttribs()->FDynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW;

		LGL3->glGenBuffers( 1, &FVBOID );
//		LGL3->glBindBuffer( GL_ARRAY_BUFFER, FVBOID );
		FLocalRenderer->SetActiveVBO( FVBOID, FVBOIndicesID );
		LGL3->glBufferData( GL_ARRAY_BUFFER, DataSize, NULL, UsageFlag );
#ifdef USE_VAO
		LGL3->glBindVertexArray( FVAOID );
		BindVertexArray( NULL );
		LGL3->glBindVertexArray( 0 );
#endif
	}
	else
	{
		// just use pointers
		FAttribVBOOffset = GetVertexAttribs()->EnumerateVertexStreams();
	}

	CommitChanges();
}

/*
 * 09/08/2010
     Switched to patches
 * 02/07/2010
     Precached fields
 * 18/03/2010
     Instanced rendering
 * 18/02/2010
     Enumerated vertex streams
 * 06/02/2010
     Cleaned
 * 24/05/2007
     SetCurrentKeyframe()
 * 31/10/2006
     glVertexPointer() is now called last
 * 01/12/2005
     VertexCount is dependent on currently attached indices
 * 22/12/2004
     It's here
*/
