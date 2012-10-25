/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 TexCoord;

void main()
{
  gl_Position = ProjectionMatrix * ModelViewMatrix * in_Vertex;

  TexCoord = vec2( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform vec4 u_Color;
uniform vec4 RectColor;
uniform sampler2D Texture1;
uniform sampler2D Texture0;
out vec4 out_FragColor;

void main()
{
	vec4 Color1 = texture( Texture0, TexCoord );
    vec4 Color2 = texture( Texture1, TexCoord );
	vec4 Color = (Color1 + 5.0 * Color2) / 6.0;
	Color.a = 1.0; 
 
   	out_FragColor = Color;
}
