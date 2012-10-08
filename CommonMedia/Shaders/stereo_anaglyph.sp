/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   TexCoord = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0; // left eye
uniform sampler2D Texture1; // right eye
uniform vec4 PICKING_COLOR;

uniform mat4 LeftEyeCoefs;
uniform mat4 RightEyeCoefs;

out vec4 out_FragColor;
out vec4 out_PickMask;

void main()
{
	vec4 LeftColor  = texture( Texture0, TexCoord );
	vec4 RightColor = texture( Texture1, TexCoord );

	// linear transformation
	// refer to http://3dtv.at/Knowhow/AnaglyphComparison_en.aspx for mixing coeffs

	out_FragColor = LeftColor * LeftEyeCoefs + RightColor * RightEyeCoefs;

/*
	float r = dot( LeftColor, LeftEyeCoefs[0] ) + dot( RightColor, RightEyeCoefs[0] );
	float g = dot( LeftColor, LeftEyeCoefs[1] ) + dot( RightColor, RightEyeCoefs[1] );
	float b = dot( LeftColor, LeftEyeCoefs[2] ) + dot( RightColor, RightEyeCoefs[2] );
*/
//   out_FragColor = vec4( r, g, b, 1.0 );
   out_PickMask  = PICKING_COLOR;
}
