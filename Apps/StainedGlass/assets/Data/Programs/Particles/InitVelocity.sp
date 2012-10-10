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

/// Output velocity
out vec4 out_FragColor;

/// Minimum particle velocity
uniform vec3 MinVelocity;

/// Maximum particle velocity
uniform vec3 MaxVelocity; 

#include "ParticleCommon.sp"

void main()
{
	/// Regenerate the particle (calculate new velocity)
	out_FragColor = vec4(RandomVec3InBox(vec3(TexCoord, 0.0), MinVelocity, MaxVelocity), 0.0 );

//	out_FragColor = 4.0 * textureLod(Texture3, vec3(TexCoord, 0.5), 0.0); // vec4(10.0, 0.0, 0.0, 0.0) //vec4(TexCoord.x * 20.0, TexCoord.y * 20.0, 0.0, 0.0);
}
