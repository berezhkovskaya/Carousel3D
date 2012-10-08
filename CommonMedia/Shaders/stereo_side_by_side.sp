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

out vec4 out_FragColor;
out vec4 out_PickMask;

void main()
{
	vec4 LeftColor  = texture( Texture0, vec2( 2.0, 1.0 ) * TexCoord );
	vec4 RightColor = texture( Texture1, vec2( 2.0, 1.0 ) * TexCoord - vec2(1.0, 0.0) );

	out_FragColor = LeftColor * step( TexCoord.x, 0.5 ) + RightColor * step( 0.5, TexCoord.x );
   out_PickMask  = PICKING_COLOR;
}
