/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;
in vec4 in_Color;

out vec2 TexCoord;
out vec4 Color;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   TexCoord = vec2( in_TexCoord.x, in_TexCoord.y );
   Color    = in_Color;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D Texture0;

out vec4 out_FragColor;

void main()
{
   out_FragColor = texture( Texture0, TexCoord ) * Color;
}
