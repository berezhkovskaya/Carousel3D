/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

//uniform vec4 LineStart;
//uniform vec4 LineEnd;
//uniform float Thickness;

// start, end, color, thickness
uniform mat4 LineParams;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

out float DistanceToLine;
out vec2 TexCoord;
out vec4 Vertex;

void main()
{
	vec4 LineStart  = LineParams[0];
	vec4 LineEnd    = LineParams[1];
	float Thickness = LineParams[3].x;

	mat4 MVP = ProjectionMatrix * ModelViewMatrix;

	vec4 Start = MVP * LineStart;
	vec4 End   = MVP * LineEnd;

	vec2 Start2D = Start.xy / Start.w;
	vec2 End2D   = End.xy / End.w;

	vec2 D = Thickness * normalize(Start2D - End2D);

   vec4 A = vec4( -D.y, D.x, 0.0, 0.0 );
   vec4 B = vec4(  D.y,-D.x, 0.0, 0.0 );

   vec2 Coord = in_TexCoord.xy;

	// here we use dummy in_Vertex to cheat the stupud OpenGL ES 2 implementation...
	vec4 Position = MVP * in_Vertex;
	Vertex = in_Vertex;

	// ..overwrite with the correct initial value
	Position = ( 1.0 - Coord.x ) * Start + (Coord.x) * End;

	// offset
	Position += ( 1.0 - Coord.y ) * A + (Coord.y) * B;

	DistanceToLine = (-1.0) * Coord.y + (+1.0) * ( 1.0 - Coord.y );

	TexCoord = vec2( 0.5, (DistanceToLine+1.0)*0.5 );

	gl_Position = Position;
}

/*FRAGMENT_PROGRAM*/

in float DistanceToLine;
in vec2 TexCoord;
in vec4 Vertex;

uniform sampler2D Texture0;
uniform mat4 LineParams;

out vec4 out_FragColor;

void main()
{
	vec4 Mask = texture( Texture0, TexCoord );

	float Alpha = Mask.x * ( 1.0 - sqrt(abs(DistanceToLine)) );

	out_FragColor = LineParams[2] * vec4( 1.0, 1.0, 1.0, Alpha );
}
