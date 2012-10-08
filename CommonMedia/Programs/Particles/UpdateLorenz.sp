/**
	\brief Lorenz system integrator

	\author Viktor Latypov
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

/// Output position
out vec4 out_FragColor;

/// Timestep
uniform float DT;

const float rho   = 28.0;
const float sigma = 10.0;
const float beta  = 8.0/3.0;

void main()
{
	/// Fetch particle position
	vec4 Pos = textureLod( Texture0, TexCoord, 0.0 ).xyz;

/*
	const float A = 2.0;
	const float B = 0.5;
	const float C = -1.0;
	const float D = -1.0;
	const float E = 2.0;

	vec3 Delta( sin(A * y) - z * cos( B * x), z * sin ( C * x) - cos( D * y), E * sin( x ) );
*/

	/// Lorenz equations - Right hand side
	vec3 Delta((sigma * (y - x)), (x * ( rho - z) - y), (x * y - beta * z));

	/// Make euler integration step
	vec3 NewPos = Pos + Delta * DT;

	/// Store calculate position
	out_FragColor = vec4(NewPos, 1.0);
}
