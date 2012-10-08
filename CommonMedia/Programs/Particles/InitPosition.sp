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

/// Output position
out vec4 out_FragColor;

/// Minimum particle position
uniform vec3 MinPosition;

/// Maximum particle velocity
uniform vec3 MaxPosition; 

/// Maximum particle lifetime
uniform float MaxLifeTime;

#include "ParticleCommon.sp"

void main()
{
//	out_FragColor = vec4(MinPosition.x + TexCoord.x * (MaxPosition.x - MinPosition.x), MinPosition.y + TexCoord.y * (MaxPosition.y - MinPosition.y), 0.0, 0.0);

//	out_FragColor = 4.0 * textureLod(Texture3, vec3(TexCoord, 0.5), 0.0); // vec4(10.0, 0.0, 0.0, 0.0) //vec4(TexCoord.x * 20.0, TexCoord.y * 20.0, 0.0, 0.0);

	/// Regenerate the particle (calculate new position)
	out_FragColor = vec4( RandomVec3InBox(vec3(TexCoord, 0.0), MinPosition, MaxPosition), MaxLifeTime * RandomValue(vec3(TexCoord, 0.0)));
}
