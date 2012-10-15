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
uniform sampler2D Texture0;
out vec4 out_FragColor;

void main()
{
	vec4 Color = texture( Texture0, TexCoord );  
	Color.a = 0.5; 
 
   	out_FragColor = Color;
}
