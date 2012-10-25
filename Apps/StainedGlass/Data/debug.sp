/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;
out vec2 TexCoord;

void main()
{
  gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * in_Vertex;

  TexCoord = vec2( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

out vec4 out_FragColor;

uniform sampler Texture0;

void main()
{
   out_FragColor = vec4( texture( Texture0, TexCoord ) );
}
