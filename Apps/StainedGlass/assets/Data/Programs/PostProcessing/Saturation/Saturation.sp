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

uniform sampler2D Texture0;   /* Color */
uniform sampler2D Texture1;   /* Depth */

in vec2 TexCoord;

out vec4 out_FragColor;

void main()
{
   vec4 Color = texture( Texture0, TexCoord );

   vec4 Sat = Color;

   if ( Color.r < 0.5 ) Sat.r = 0.0;
   if ( Color.g < 0.5 ) Sat.g = 0.0;
   if ( Color.b < 0.5 ) Sat.b = 0.0;
   if ( Color.r > 0.5 ) Sat.r = 1.0;
   if ( Color.g > 0.5 ) Sat.g = 1.0;
   if ( Color.b > 0.5 ) Sat.b = 1.0;

   float Mono   = dot( Sat, vec4( 0.3, 0.59, 0.11, 0.5 ) );
   float Bright = dot( Color, vec4( 1.0, 1.0,  1.0,  0.0 ) );

   out_FragColor = mix( Sat, vec4( Mono, Mono, Mono, 0.0 ), Bright );
}
