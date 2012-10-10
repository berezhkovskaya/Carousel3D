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

uniform mat4 LeftEyeCoefs;
uniform mat4 RightEyeCoefs;

out vec4 out_FragColor;

void main()
{
	vec4 LeftColor  = texture( Texture0, TexCoord );
	vec4 RightColor = texture( Texture1, TexCoord );

	// linear transformation
	// refer to http://3dtv.at/Knowhow/AnaglyphComparison_en.aspx for mixing coeffs

	out_FragColor = LeftColor * LeftEyeCoefs + RightColor * RightEyeCoefs;
}
