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

uniform vec4 RectColor;

out vec4 out_FragColor;

void main()
{
   out_FragColor = vec4( 1.0, TexCoord.x, TexCoord.y, 1.0 );
}
