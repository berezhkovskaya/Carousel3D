/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord;

void main()
{
  gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

  TexCoord = vec2( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform float in_ZSlice;

uniform sampler2D Texture0;

out vec4 out_FragColor;

void main()
{
   vec4 Color = vec4( 0.5 + 0.5 * sin( 15.0 * TexCoord.x - in_ZSlice), 
                      0.5 + 0.5 * cos( 15.0 * TexCoord.y + in_ZSlice ), 
                      0.5 + 0.5 * in_ZSlice, 
                      1.0 );

   out_FragColor = Color;
}
