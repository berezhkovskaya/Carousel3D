/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec3 CURSOR_POS;

out vec2 TexCoord;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * ( in_Vertex + vec4( CURSOR_POS, 0.0 ) );

   TexCoord = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;

out vec4 out_FragColor;

void main()
{
   out_FragColor = texture( Texture0, TexCoord );
}
