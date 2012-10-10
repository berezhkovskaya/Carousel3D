/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord0;

void main()
{
   TexCoord0 = vec2( in_TexCoord );

   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord0;

uniform sampler2D Texture0;

out vec4 out_FragColor;

void main()
{
  vec4 Color = vec4( texture(Texture0, TexCoord0) );

  if ( dot( Color, vec4( 0.33, 0.34, 0.33, 0.0) ) < 1.0 ) Color = vec4( 0.0, 0.0, 0.0, 0.0 );

  out_FragColor = Color;
}
