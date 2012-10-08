#include "Scene/Particles/GPUParticles.h"
#include "Scene/Scene.h"

#include "Environment.h"
#include "Resources/ResourcesManager.h"

#include "Renderer/iTexture.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/Canvas.h"
#include "Renderer/iVertexArray.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/RenderState.h"

#include "Geometry/VAMender.h"
#include "Geometry/VertexAttribs.h"
#include "Geometry/Geom.h"

/// Create a vertex array for particle position storage
clVertexAttribs* CreateBillboardCloud( int BufferWidth, float SzX, float SzY )
{
	clVAMender m;

	int NumPoints = BufferWidth * BufferWidth;
	int NumX = BufferWidth;

	LVector3 vtx1, vtx2, vtx3, vtx4;

	m.ReserveVertices( NumPoints * 6 );
	m.ReserveNormals( 0 );
	m.ReserveTangents( 0 );
	m.ReserveBinormals( 0 );
	m.ReserveFaces( NumPoints * 2 );
	m.ReserveTFaces( NumPoints * 2 );
	m.ReserveTVertices( NumPoints * 6 );

	int k = 0;

	for ( int pt = 0 ; pt < NumPoints ; pt++ )
	{
		float _x = static_cast<float>( pt % NumX ) * SzX;
		float _u = _x / static_cast<float>( NumX );
		_u /= SzX;
		_x -= static_cast<float>( NumX / 2 ) * SzX;
		float _y = static_cast<float>( pt / NumX ) * SzY;
		float _v = _y / static_cast<float>( NumX );
		_v /= SzY;
		float _z = 0.0f;

		vtx1 = vec3( _x - SzX / 3.0f, _y - SzY / 3.0f, _z );
		vtx2 = vec3( _x + SzX / 3.0f, _y - SzY / 3.0f, _z );
		vtx3 = vec3( _x + SzX / 3.0f, _y + SzY / 3.0f, _z );
		vtx4 = vec3( _x - SzX / 3.0f, _y + SzY / 3.0f, _z );

		m.EmitVertex( k + 0, vtx1, -1, -1 );
		m.EmitVertex( k + 1, vtx2, -1, -1 );
		m.EmitVertex( k + 2, vtx3, -1, -1 );
		m.EmitFace( pt * 2 + 0, k + 0, k + 1, k + 2, -1, -1 );

		m.EmitTextureVertex( k + 0, vec3( _u, _v, 0.0f ) );
		m.EmitTextureVertex( k + 1, vec3( _u, _v, 0.25f ) );
		m.EmitTextureVertex( k + 2, vec3( _u, _v, 0.5f ) );
		m.EmitTextureFace( pt * 2 + 0, k + 0, k + 1, k + 2 );

		k += 3;

		m.EmitVertex( k + 0, vtx1, -1, -1 );
		m.EmitVertex( k + 1, vtx3, -1, -1 );
		m.EmitVertex( k + 2, vtx4, -1, -1 );
		m.EmitFace( pt * 2 + 1, k + 0, k + 1, k + 2, -1, -1 );

		m.EmitTextureVertex( k + 0, vec3( _u, _v, 0.0f ) );
		m.EmitTextureVertex( k + 1, vec3( _u, _v, 0.5f ) );
		m.EmitTextureVertex( k + 2, vec3( _u, _v, 0.75f ) );
		m.EmitTextureFace( pt * 2 + 1, k + 0, k + 1, k + 2 );

		k += 3;
	}

	return m.CreateVertexAttribs( -1 );
}

Particles::Particles():
	FMaxLifeTime( 10.0f ),
	FMinPosition( -5, -5, -5 ),
	FMaxPosition( +5, +5, +5 ),
	FMinVelocity( -1, -1, -1 ),
	FMaxVelocity( 1, 1, 1 ),
	FParticleColor( 1.0f, 0.0f, 0.0f, 0.0f ),
	FParticleSize( 0.1f ),
	FGravity( -1.0f ),
	FGeomHandle( -1 ),
	FGeom( NULL )
{
}

Particles::~Particles()
{
}

void Particles::InitBuffersAndShaders( int NumParticles )
{
	/// Number of particles in a squared array
	int BufferWidth = ( int )( sqrtf( ( float )NumParticles ) );

	/// Shader loading
	FRenderColorShader      = Env->Resources->CreateCustomShader( "Programs/Particles/RenderColor.sp", "" );
	FRenderTexturedShader   = Env->Resources->CreateCustomShader( "Programs/Particles/RenderTexture.sp", "" );

	FCalcForcesShader       = Env->Resources->CreateCustomShader( "Programs/Particles/UpdateForces.sp", "" );
	FMakePositionStepShader = Env->Resources->CreateCustomShader( "Programs/Particles/UpdatePosition.sp", "" );
	FMakeVelocityStepShader = Env->Resources->CreateCustomShader( "Programs/Particles/UpdateVelocity.sp", "" );

	FInitPositionShader     = Env->Resources->CreateCustomShader( "Programs/Particles/InitPosition.sp", "" );
	FInitVelocityShader     = Env->Resources->CreateCustomShader( "Programs/Particles/InitVelocity.sp", "" );

	/// Aux noise texture

	FNoiseTexture = Env->Renderer->CreateTexture();
	FNoiseTexture->SetImage( Env->Resources->LoadImg( "Textures/NoiseVolume.dds", L_TEXTURE_3D ) );

	FMakePositionStepShader->SetTexture( 3, FNoiseTexture, false );
	FMakeVelocityStepShader->SetTexture( 3, FNoiseTexture, false );
	FInitPositionShader->SetTexture( 3, FNoiseTexture, false );
	FInitVelocityShader->SetTexture( 3, FNoiseTexture, false );

	/// Get pointers to SPs

	FIV_SP = FInitVelocityShader->GetShaderProgram();
	FIP_SP = FInitPositionShader->GetShaderProgram();

	FPosSP = FMakePositionStepShader->GetShaderProgram();
	FVelSP = FMakeVelocityStepShader->GetShaderProgram();
	FForSP = FCalcForcesShader->GetShaderProgram();

	FRC_SP = FRenderColorShader->GetShaderProgram();
	FRT_SP = FRenderTexturedShader->GetShaderProgram();

	/// Uniforms' creation

	FInitVelocity_Minimum = FIV_SP->CreateUniform( "MinVelocity" );
	FInitVelocity_Maximum = FIV_SP->CreateUniform( "MaxVelocity" );

	FInitPosition_Minimum = FIP_SP->CreateUniform( "MinPosition" );
	FInitPosition_Maximum = FIP_SP->CreateUniform( "MaxPosition" );

	FPosition_TimeStep    = FPosSP->CreateUniform( "DT" );
	FPosition_MaxLifeTime = FPosSP->CreateUniform( "MaxLifeTime" );
	FPosition_Minimum     = FPosSP->CreateUniform( "MinPosition" );
	FPosition_Maximum     = FPosSP->CreateUniform( "MaxPosition" );

	FVelocity_TimeStep    = FVelSP->CreateUniform( "DT" );
	FVelocity_MaxLifeTime = FVelSP->CreateUniform( "MaxLifeTime" );
	FVelocity_Minimum     = FVelSP->CreateUniform( "MinVelocity" );
	FVelocity_Maximum     = FVelSP->CreateUniform( "MaxVelocity" );

	FForces_Gravity = FForSP->CreateUniform( "Gravity" );

	FRenderColor_ParticleSize = FRC_SP->CreateUniform( "ParticleSize" );
	FRenderColor_MaxLifeTime  = FRC_SP->CreateUniform( "MaxLifeTime" );
	FRenderColor_Color        = FRC_SP->CreateUniform( "ParticleColor" );

	/// Render targets

	FForces      = Env->Renderer->CreateRenderTarget( BufferWidth, BufferWidth, 1, 32, false, 1 );

	FNewPosition = Env->Renderer->CreateRenderTarget( BufferWidth, BufferWidth, 1, 32, false, 1 );
	FOldPosition = Env->Renderer->CreateRenderTarget( BufferWidth, BufferWidth, 1, 32, false, 1 );

	FNewVelocity = Env->Renderer->CreateRenderTarget( BufferWidth, BufferWidth, 1, 32, false, 1 );
	FOldVelocity = Env->Renderer->CreateRenderTarget( BufferWidth, BufferWidth, 1, 32, false, 1 );

	/// Vertex array

	const float SzX = 0.1f;
	const float SzY = 0.1f;

	FBuffer = CreateBillboardCloud( BufferWidth, SzX, SzY );
	FGeom = Env->Resources->CreateGeomForVA( FBuffer );

	FVA = Env->Renderer->AllocateEmptyVA();
	FVA->SetVertexAttribs( FBuffer );
}

void Particles::DestroyBuffersAndShaders()
{
	FVA->DisposeObject();
	delete FBuffer;

	FForces->DisposeObject();
	FOldPosition->DisposeObject();
	FNewPosition->DisposeObject();
	FOldVelocity->DisposeObject();
	FNewVelocity->DisposeObject();

	FRenderColorShader->DisposeObject();
	FRenderTexturedShader->DisposeObject();

	FCalcForcesShader->DisposeObject();
	FMakePositionStepShader->DisposeObject();
	FMakeVelocityStepShader->DisposeObject();

	FInitPositionShader->DisposeObject();
	FInitVelocityShader->DisposeObject();

	FNoiseTexture->DisposeObject();
}

void Particles::UpdateUniforms()
{
	FIV_SP->BindUniforms();
	FIV_SP->SetUniformVec3Array( FInitVelocity_Minimum, 1, FMinVelocity );
	FIV_SP->SetUniformVec3Array( FInitVelocity_Maximum, 1, FMaxVelocity );

	FIP_SP->BindUniforms();
	FIP_SP->SetUniformVec3Array( FInitPosition_Minimum, 1, FMinPosition );
	FIP_SP->SetUniformVec3Array( FInitPosition_Maximum, 1, FMaxPosition );
	FIP_SP->SetUniformNameFloat( "MaxLifeTime", FMaxLifeTime );

	FPosSP->BindUniforms();
	FPosSP->SetUniformFloat( FPosition_TimeStep,    FTimeStep );
	FPosSP->SetUniformFloat( FPosition_MaxLifeTime, FMaxLifeTime );
	FPosSP->SetUniformVec3Array( FPosition_Minimum, 1, FMinPosition );
	FPosSP->SetUniformVec3Array( FPosition_Maximum, 1, FMaxPosition );

	FVelSP->BindUniforms();
	FVelSP->SetUniformFloat( FVelocity_TimeStep,    FTimeStep );
	FVelSP->SetUniformFloat( FVelocity_MaxLifeTime, FMaxLifeTime );
	FVelSP->SetUniformVec3Array( FVelocity_Minimum, 1, FMinVelocity );
	FVelSP->SetUniformVec3Array( FVelocity_Maximum, 1, FMaxVelocity );

	FForSP->BindUniforms();
	FForSP->SetUniformFloat( FForces_Gravity, FGravity );

	FRC_SP->BindUniforms();
	FRC_SP->SetUniformFloat( FRenderColor_ParticleSize, FParticleSize );
	FRC_SP->SetUniformFloat( FRenderColor_MaxLifeTime, FMaxLifeTime );
	FRC_SP->SetUniformVec4Array( FRenderColor_Color, 1, FParticleColor );
}

void Particles::ResetDynamics()
{
	FOldVelocity->Bind( 0 );
	Env->Renderer->GetCanvas()->FullscreenRect( FInitVelocityShader );
	FOldVelocity->UnBind();

	FOldPosition->Bind( 0 );
	Env->Renderer->GetCanvas()->FullscreenRect( FInitPositionShader );
	FOldPosition->UnBind();
}

void Particles::MakeExplosion( const LVector3& Position, const LVector3& MinVelocity, const LVector3& MaxVelocity, float ExtinctionTime )
{
	FMaxLifeTime = ExtinctionTime;

	FMinPosition = Position;
	FMaxPosition = Position;

	FMinVelocity = MinVelocity;
	FMaxVelocity = MaxVelocity;

	/// Fill coordinates and velocities
	ResetDynamics();
}

/// Make each required step for the simulation
void Particles::UpdateDynamics( float dt )
{
	FPosSP->BindUniforms();
	FPosSP->SetUniformFloat( FPosition_TimeStep, dt );

	FVelSP->BindUniforms();
	FVelSP->SetUniformFloat( FVelocity_TimeStep, dt );

	/// 0. Forces
	FForces->Bind( 0 );
	Env->Renderer->GetCanvas()->FullscreenRect( FCalcForcesShader );
	FForces->UnBind();

	/// 2. Positions
	FMakePositionStepShader->SetTexture( 0, FOldPosition->GetColorTexture( 0 ), false );
	FMakePositionStepShader->SetTexture( 1, FOldVelocity->GetColorTexture( 0 ), false );

	FNewPosition->Bind( 0 );
	Env->Renderer->GetCanvas()->FullscreenRect( FMakePositionStepShader );
	FNewPosition->UnBind();
	iRenderTarget* Tmp = FOldPosition;
	FOldPosition = FNewPosition;
	FNewPosition = Tmp;

	/// 1. Velocities
	FMakeVelocityStepShader->SetTexture( 0, FOldPosition->GetColorTexture( 0 ), false );
	FMakeVelocityStepShader->SetTexture( 1, FOldVelocity->GetColorTexture( 0 ), false );
	FMakeVelocityStepShader->SetTexture( 2, FForces->GetColorTexture( 0 ), false );

	FNewVelocity->Bind( 0 );
	Env->Renderer->GetCanvas()->FullscreenRect( FMakeVelocityStepShader );
	FNewVelocity->UnBind();
	Tmp = FOldVelocity;
	FOldVelocity = FNewVelocity;
	FNewVelocity = Tmp;

	/// 3. Swap the buffers
//	SwapTargets();

	GetColorShader()->SetTexture( 0, FOldPosition->GetColorTexture( 0 ), false );
}

/// Basically, this is the same as for any other object in scene
void Particles::DebugRenderWithShader( clRenderState* Shader, const LMatrix4& Projection, const LMatrix4& ModelView )
{
	Shader->SetTexture( 0, FOldPosition->GetColorTexture( 0 ), false );

	FLocalMatrices.SetModelMatrix( LMatrix4::Identity() );
	FLocalMatrices.SetViewMatrix( ModelView );
	FLocalMatrices.SetProjectionMatrix( Projection );

	Shader->GetShaderProgram()->SetTransformationUniforms( FLocalMatrices );
	Env->Renderer->AddBuffer( FVA, Shader, 1, false );
}

void Particles::AddToScene( clScene* S )
{
	if ( FGeomHandle > -1 ) { return; }

	FGeomHandle = S->AddGeom( FGeom );
	S->SetMtlFromShader( FGeomHandle, GetColorShader(), NULL, NULL );
}

void Particles::RemoveFromScene( clScene* S )
{
	S->RemoveItem( FGeomHandle );
	FGeomHandle = -1;
}
