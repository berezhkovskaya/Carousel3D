/**
 * \file ParticleSystem.cpp
 * \brief Particle system
 * \version 0.5.89a
 * \date 28/06/2009
 * \author Sergey Kosarevsky, 2006-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "ParticleSystem.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iVertexArray.h"
#include "Resources/ResourcesManager.h"
#include "Geometry/VertexAttribs.h"
#include "Math/LRandom.h"

clParticleSystem::clParticleSystem()
{
}

void clParticleSystem::AfterConstruction()
{
//   FTransformNode = Env->SceneManager->CreateTransformNode( "PARTICLE_SYSTEM_TRANSFORM", NULL );
//   FGeometryNode  = Env->SceneManager->CreateGeometryNode( "PARTICLE_SYSTEM_GEOMETRY" );
//   FShaderNode    = Env->SceneManager->CreateMaterialNode( "PARTICLE_SYSTEM_MATERIAL", NULL, "" );

//   FTransformNode->ddSubObject( FShaderNode );
//   FShaderNode->ddSubObject( FGeometryNode );

	SetMaxParticles( 500 );

	// default shader
	clRenderState* Shader = Env->Resources->LoadShader( "shaders\\particles\\particle.shader" );

	SetShader( Shader );
}

clParticleSystem::~clParticleSystem()
{
}

void clParticleSystem::SetMaxParticles( int MaxParticles )
{
	FCurrentMaxParticles = MaxParticles;

	FParticles.reserve( FCurrentMaxParticles );

	iVertexArray* VA = Env->Renderer->AllocateEmptyVA();

	VA->SetVertexAttribs( clVertexAttribs::Create( 6 * MaxParticles, L_TEXCOORDS_BIT | L_NORMALS_BIT | L_COLORS_BIT ) );

//   FGeometryNode->SetVertexArray( VA );

	// prefetch tex coordinates for our GPU billboarder
	LVector4* Vec = VA->GetVertexAttribs()->FTexCoords.GetPtr();

	for ( int i = 0; i != MaxParticles; ++i )
	{
		int IdxI = i * 6;

		Vec[IdxI + 0] = LVector4( 0.0f, 0.0f, 0.0f, 0.0f );
		Vec[IdxI + 1] = LVector4( 1.0f, 0.0f, 0.0f, 0.0f );
		Vec[IdxI + 2] = LVector4( 1.0f, 1.0f, 0.0f, 0.0f );

		Vec[IdxI + 3] = LVector4( 0.0f, 0.0f, 0.0f, 0.0f );
		Vec[IdxI + 4] = LVector4( 1.0f, 1.0f, 0.0f, 0.0f );
		Vec[IdxI + 5] = LVector4( 0.0f, 1.0f, 0.0f, 0.0f );
	}
}

void clParticleSystem::SetShader( clRenderState* Shader )
{
	//FShaderNode->SetSurfaceShader( Shader );
}

void clParticleSystem::AddParticle( const sParticle& Particle )
{
	FParticles.push_back( Particle );

	if ( FCurrentMaxParticles < static_cast<int>( FParticles.size() ) )
	{
		SetMaxParticles( FCurrentMaxParticles * 2 );
	}

	FIXME( "" )

	//iVertexArray* VA = FGeometryNode->GetVertexArray();

	//VA->GetVertexAttribs()->SetActiveVertexCount( 6 * static_cast<int>( FParticles.size() ) );
}

void clParticleSystem::UpdateParticles( float DeltaSeconds )
{
	/*
	iVertexArray* VA = FGeometryNode->GetVertexArray();

	LVector3* Vec = VA->GetVertexAttribs()->FVertices.GetPtr();
	FIXME( "use AUX attribs" )
	LVector3* Tex = VA->GetVertexAttribs()->FNormals.GetPtr();
	LVector4* RGB = VA->GetVertexAttribs()->FColors.GetPtr();

	for (size_t i = 0; i != FParticles.size(); ++i)
	{
	   size_t IdxI = i * 6;

	   FParticles[i].FTTL -= DeltaSeconds;

	   if ( FParticles[i].FTTL < 0 )
	   {
	      FParticles[i] = FParticles[ FParticles.size() - 1 ];
	      FParticles.pop_back();

	      VA->GetVertexAttribs()->SetActiveVertexCount( 6 * static_cast<int>( FParticles.size() ) );

	      i--;

	      continue;
	   }

	   FParticles[i].FVelocity += FParticles[i].FAcceleration * DeltaSeconds;
	   FParticles[i].FPosition += FParticles[i].FVelocity * DeltaSeconds;

	   // save positions
	   Vec[IdxI + 0] = FParticles[i].FPosition;
	   Vec[IdxI + 1] = FParticles[i].FPosition;
	   Vec[IdxI + 2] = FParticles[i].FPosition;
	   Vec[IdxI + 3] = FParticles[i].FPosition;
	   Vec[IdxI + 4] = FParticles[i].FPosition;
	   Vec[IdxI + 5] = FParticles[i].FPosition;

	   // save lifetimes and size
	   LVector3 TTL = LVector3( FParticles[i].FTTL, FParticles[i].FLifeTime, FParticles[i].FSize );

	   Tex[IdxI + 0] = TTL;
	   Tex[IdxI + 1] = TTL;
	   Tex[IdxI + 2] = TTL;
	   Tex[IdxI + 3] = TTL;
	   Tex[IdxI + 4] = TTL;
	   Tex[IdxI + 5] = TTL;

	   // save RGB
	   RGB[IdxI + 0] = FParticles[i].FRGBA;
	   RGB[IdxI + 1] = FParticles[i].FRGBA;
	   RGB[IdxI + 2] = FParticles[i].FRGBA;
	   RGB[IdxI + 3] = FParticles[i].FRGBA;
	   RGB[IdxI + 4] = FParticles[i].FRGBA;
	   RGB[IdxI + 5] = FParticles[i].FRGBA;
	}

	VA->SetActiveVertexCount( 6 * static_cast<int>( FParticles.size() ) );

	if ( FParticles.size() > 0 )
	{
	   VA->SetActiveVertexCount( 0 );
	}

	VA->CommitChanges();
	*/
}

/*
 * 28/06/2009
     Implemented using triangles
 * 25/01/2007
     Particles size
 * 18/01/2007
     A lot of improvements
 * 19/09/2006
     First working version
 * 10/09/2006
     Development...
 * 07/09/2006
     It's here
*/
