/*VERTEX_PROGRAM*/

#include StdList.sp

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

uniform vec4 OverlayColor;

out vec4 out_FragColor;

void main()
{
	vec4 Color = texture( Texture0, TexCoord );
	vec4 Mask  = texture(Texture1, vec2(TexCoord.x, 1.0 - TexCoord.y) );

	Color.w = Mask.r;

   out_FragColor = Color * OverlayColor;
}
