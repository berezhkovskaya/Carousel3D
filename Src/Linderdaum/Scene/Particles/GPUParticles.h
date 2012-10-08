#ifndef __GPUParticles__h_included__
#define __GPUParticles__h_included__

#include "Core/iObject.h"

class iTexture;
class iVertexArray;
class clVertexAttribs;
class iRenderTarget;
class iShaderProgram;
class clRenderState;
class clScene;
class clGeom;

// sMatrices definition
#include "Renderer/iRenderContext.h"

/**
   \brief GPU particle system

   It can be completely integrated into the Scene/Sequencer
   (the method RenderWithShader() should be managed by the Scene and a new material type must be created with appropriate passes)

   I.e., we add the FVA to the scene, sort the particles internally, and the scene only renders them as a single piece.

   If we need to add these particles to some postprocessing step (e.g, heat and haze),
   the material return appropriate shader.

   TODO: sorting by camera distance (bitonic sort on GPU)
*/
class netexportable scriptfinal Particles: public iObject
{
public:
	Particles();
	virtual ~Particles();

	/// Load shader programs and create uniform handles
	scriptmethod void InitBuffersAndShaders( int NumParticles );

	/// Clean everything
	scriptmethod void DestroyBuffersAndShaders();

	/// Make each required step for the simulation
	scriptmethod void UpdateDynamics( float dt );

	/// Reset system
	scriptmethod void ResetDynamics();

	/// Send current property values to the uniforms
	scriptmethod void UpdateUniforms();

	scriptmethod void AddToScene( clScene* S );
	scriptmethod void RemoveFromScene( clScene* S );

	/**
	   \brief Single-pass rendering of the particle system

	   This can be used for testing of the rendering itself, for the variance shadow map building,
	   for the heat-and-haze mask building etc.
	*/
	scriptmethod void DebugRenderWithShader( clRenderState* Shader, const LMatrix4& Projection, const LMatrix4& ModelView );

	/// Utility function: generate time-limited explosion
	scriptmethod void MakeExplosion( const LVector3& Position, const LVector3& MinVelocity, const LVector3& MaxVelocity, float ExtinctionTime );

	scriptmethod clRenderState* GetColorShader() const { return FRenderColorShader; }

#pragma region Render targets and aux buffers
private:

	/// (u,v) geometry
	clVertexAttribs* FBuffer;

	/// Vertex array with 4 vertices for each particle (rectangular billboards)
	iVertexArray*    FVA;

	/// Noise volume for random number generation
	iTexture*  FNoiseTexture;

	/// Old velocity buffer
	iRenderTarget* FOldVelocity;

	/// Old position buffer
	iRenderTarget* FOldPosition;

	/// New position buffer
	iRenderTarget* FNewPosition;

	/// New velocity buffer
	iRenderTarget* FNewVelocity;

	/// Temporary force accumulator
	iRenderTarget* FForces;

	/// Exchange new/old pointers for velocity and position
	inline void SwapTargets()
	{
		iRenderTarget* Tmp = FOldVelocity;
		FOldVelocity = FNewVelocity;
		FNewVelocity = Tmp;

		Tmp = FOldPosition;
		FOldPosition = FNewPosition;
		FNewPosition = Tmp;
	}

#pragma endregion

#pragma region Particle shaders and uniform handles

private:

	/// Default rendering (colored) shader
	clRenderState* FRenderColorShader;

	/// Default rendering (textured) shader
	clRenderState* FRenderTexturedShader;

	/// Forces = Calculate(Pos, Vel);
	clRenderState* FCalcForcesShader;

	/// Pos += Vel * dT;
	clRenderState* FMakePositionStepShader;

	/// Vel += F * dT;
	clRenderState* FMakeVelocityStepShader;

	/// Generate random positions
	clRenderState* FInitPositionShader;

	/// Generate random velocities
	clRenderState* FInitVelocityShader;

#pragma region Uniform handles

	Lint32 FVelocity_Maximum, FVelocity_Minimum, FVelocity_TimeStep, FVelocity_MaxLifeTime;
	Lint32 FPosition_Maximum, FPosition_Minimum, FPosition_TimeStep, FPosition_MaxLifeTime;
	Lint32 FInitPosition_Maximum, FInitPosition_Minimum, FInitVelocity_Maximum, FInitVelocity_Minimum;
	Lint32 FRenderColor_ParticleSize, FRenderColor_MaxLifeTime, FRenderColor_Color;
	Lint32 FForces_Gravity;

#pragma endregion

	/// Cached shader program pointers
	iShaderProgram* FPosSP, *FVelSP, *FForSP, *FRC_SP, *FRT_SP, *FIV_SP, *FIP_SP;

	/// Local copy of Matrix buffer for direct rendering
	sMatrices FLocalMatrices;

#pragma endregion

#pragma region Properties and corresponding members

public:

	/// Maximum lifetime of a particle
	float FMaxLifeTime;

	/// Current simulation timestep
	float FTimeStep;

	/// Particle billboard size
	float FParticleSize;

	/// Particle color
	LVector4  FParticleColor;

	/// Velocity bounds
	LVector3  FMinVelocity, FMaxVelocity;

	/// Position bounds
	LVector3  FMinPosition, FMaxPosition;

	/// Gravity factor
	float FGravity;

	/** Property(Description="Force of gravity", Category="Simulation", Name=Gravity,       Type=float, FieldName=FGravity)       */
	/** Property(Description="Current timestep", Category="Simulation", Name=TimeStep,      Type=float, FieldName=FTimeStep)      */
	/** Property(Description="Maximum lifetime", Category="Simulation", Name=MaxLifeTime,   Type=float, FieldName=FMaxLifeTime)   */
	/** Property(Description="Minimum position", Category="General parameters", Name=MinPosition,   Type=vec3,  FieldName=FMinPosition)   */
	/** Property(Description="Maximum position", Category="General parameters", Name=MaxPosition,   Type=vec3,  FieldName=FMaxPosition)   */
	/** Property(Description="Minimum velocity", Category="General parameters", Name=MinVelocity,   Type=vec3,  FieldName=FMinVelocity)   */
	/** Property(Description="Maximum velocity", Category="General parameters", Name=MaxVelocity,   Type=vec3,  FieldName=FMaxVelocity)   */
	/** Property(Description="Particle billboard size", Category="Rendering parameters", Name=ParticleSize,  Type=float, FieldName=FParticleSize)  */
	/** Property(Description="Particle color",          Category="Rendering parameters", Name=ParticleColor, Type=vec4,  FieldName=FParticleColor) */

#pragma endregion

	/// Handle for the geometry added to some scene
	int FGeomHandle;

	/// Actual geometry container (for the FBuffer)
	clGeom* FGeom;
};
/*
/// Manages a number of randomly fired rockets
Firework
{
};

/// Produces smoke trace and explodes at the end
FireworkRocket
{
};
*/
#endif
