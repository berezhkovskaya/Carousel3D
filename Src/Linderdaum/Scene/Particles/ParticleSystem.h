/**
 * \file ParticleSystem.h
 * \brief Particle system
 * \version 0.5.74
 * \date 16/06/2007
 * \author Sergey Kosarevsky, 2006-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _ParticleSystem_
#define _ParticleSystem_

#include "Platform.h"
#include "Core/iObject.h"
#include "Math/LVector.h"
#include "Utils/LArray.h"

class clRenderState;

struct sParticle
{
	sParticle(): FPosition(),
		FVelocity(),
		FAcceleration(),
		FLifeTime( 0.0f ),
		FTTL( 0.0f ),
		FRGBA( 1.0f, 1.0f, 1.0f, 1.0f ),
		FSize( 0.5f ) {};
	//
	LVector3    FPosition;             // current position
	LVector3    FVelocity;             // current velocity
	LVector3    FAcceleration;         // current accelleration
	float       FLifeTime;             // total life time
	float       FTTL;                  // time to live left
	LVector4    FRGBA;                 // overlay color
	float       FSize;                 // particle size
};

class scriptfinal clParticleSystem: public iObject
{
public:
	clParticleSystem();
	virtual ~clParticleSystem();
	//
	// iObject interface
	//
	virtual void                       AfterConstruction();
	//
	// clParticleSystem
	//
	virtual void                  SetShader( clRenderState* Shader );
	virtual void                  UpdateParticles( float DeltaSeconds );
	virtual void                  AddParticle( const sParticle& Particle );
	virtual LArray<sParticle>&    GetParticles()
	{
		return FParticles;
	};
private:
	void    SetMaxParticles( int MaxParticles );
private:
	//
	LArray<sParticle>    FParticles;
	int                  FCurrentMaxParticles;
};

#endif

/*
 * 16/06/2007
     Merged with iParticleSystem.h
 * 25/01/2007
     FSize
 * 15/01/2007
     SetShader()
     AddParticle()
     GetParticles()
 * 27/08/2006
     It's here
*/
