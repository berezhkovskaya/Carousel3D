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

/// Output force
out vec4 out_FragColor;

/// Timestep
// uniform float DT;

/// Adjustable gravity
uniform float Gravity;

void main()
{
	/// (x,y,z) + LifeTime in w
	vec4 State = textureLod( Texture0, TexCoord, 0.0 );

	/// Fetch particle position
	vec3 Pos = State.xyz;

	/// Fetch particle velocity
	vec3 Vel = textureLod( Texture1, TexCoord, 0.0 ).xyz;

	/// Store gravity factor
	out_FragColor = vec4(0.0, 0.0, Gravity, 0.0);
}
