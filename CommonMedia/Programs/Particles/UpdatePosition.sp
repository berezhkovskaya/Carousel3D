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

/// Output position
out vec4 out_FragColor;

/// Timestep
uniform float DT;

/// Maximum life time (we check this if it is positive)
uniform float MaxLifeTime;

/// Minimum particle position
uniform vec3 MinPosition;

/// Maximum particle velocity
uniform vec3 MaxPosition; 

#include "ParticleCommon.sp"

void main()
{
//	out_FragColor = textureLod( Texture0, TexCoord, 0.0 );

	/// (x,y,z) + LifeTime in w
	vec4 State = textureLod( Texture0, TexCoord, 0.0 );

	float LifeTime = State.w;

	if(MaxLifeTime > 0.0)
	{
		/// Check lifetime
		if(LifeTime > MaxLifeTime)
		{
			/// Regenerate the particle (calculate new position)

			/// Store calculated position and reset lifetime
			out_FragColor = vec4(RandomVec3InBox(vec3(TexCoord, 0.0), MinPosition, MaxPosition), 0.0);

			/// terminate
			return;
		}
	}

	/// Fetch particle position
	vec3 Pos = State.xyz;

	/// Fetch particle velocity
	vec3 Vel = textureLod( Texture1, TexCoord, 0.0 ).xyz;

	/// Make euler integration step
	vec3 NewPos = Pos + Vel * DT;

	/// Store calculated position and update lifetime
	out_FragColor = vec4(NewPos, LifeTime + 10.0 * DT);
}
