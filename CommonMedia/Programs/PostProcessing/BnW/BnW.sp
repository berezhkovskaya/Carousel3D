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
   vec4 Sky = texture( Texture0, TexCoord);

   vec4 Sat = Sky;

   if ( Sky.r < 0.1 ) Sat.r = 0.0;
   if ( Sky.g < 0.1 ) Sat.g = 0.0;
   if ( Sky.b < 0.1 ) Sat.b = 0.0;
   if ( Sky.r > 0.9 ) Sat.r = 1.0;
   if ( Sky.g > 0.9 ) Sat.g = 1.0;
   if ( Sky.b > 0.9 ) Sat.b = 1.0;

//   float Mono   = dot( Sat, vec4( 0.3, 0.59, 0.11, 0.5 ) );
//   float Bright = dot( Sat, vec4( 1.0, 1.0, 1.0, 0.0) );

//   out_FragColor = mix( Sat, vec4(Mono, Mono, Mono, 1.0), Bright );

//   Sat.a = 1.0;
//   out_FragColor = Sat;

   float Mono   = dot( Sky, vec4( 0.3, 0.59, 0.11, 0.0 ) );

   out_FragColor = vec4(Mono, Mono, Mono, 1.0);
}
