/**
	\brief Particle velocity updater

	\author

	We fetch the particle velocity and add acting forces to it.

	If the lifetime of the particle is ended, we regenerate its velocity
*/

/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
	TexCoord = vec2( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

/// Auxillary 2d particle index
in vec2 TexCoord;

/// Input particle position
uniform sampler2D Texture0;

/// Input particle velocity
uniform sampler2D Texture1;

/// Input forces
uniform sampler2D Texture2;

/// Output velocity
out vec4 out_FragColor;

/// Timestep
uniform float DT;

/// Maximum life time (we check this if it is positive)
uniform float MaxLifeTime;

/// Minimum particle velocity
uniform vec3 MinVelocity;

/// Maximum particle velocity
uniform vec3 MaxVelocity; 

#include "ParticleCommon.sp"

void main()
{
//	out_FragColor = textureLod( Texture1, TexCoord, 0.0 );

	float LifeTime = textureLod( Texture0, TexCoord, 0.0).w;

	if(MaxLifeTime > 0.0)
	{
		/// Check lifetime
		if(LifeTime > MaxLifeTime)
		{
			/// Regenerate the particle (calculate new velocity)

			/// Store calculated position and update lifetime
			out_FragColor = vec4(RandomVec3InBox(vec3(TexCoord, 0.0), MinVelocity, MaxVelocity), 0.0);

			/// terminate
			return;
		}
	}

	/// Fetch particle velocity
	vec3 Vel = textureLod( Texture1, TexCoord, 0.0 ).xyz;

	/// Fetch acting forces
	vec3 Force = textureLod( Texture2, TexCoord, 0.0 ).xyz;

	/// Make euler integration step
	vec3 NewVel = Vel + Force * DT;

	/// Store calculated position and update lifetime
	out_FragColor = vec4(NewVel, 0.0);
}
