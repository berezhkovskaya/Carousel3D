/*VERTEX_PROGRAM*/

#include StdList.sp

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;

uniform vec4 OverlayColor;

out vec4 out_FragColor;

void main()
{
   vec4 Color = texture( Texture0, TexCoord );

   out_FragColor = Color * OverlayColor;
}
