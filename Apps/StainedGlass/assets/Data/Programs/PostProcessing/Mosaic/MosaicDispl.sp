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
uniform sampler3D Texture2;   /* Noise */
uniform sampler2D Texture3;   /* Pattern */

in vec2 TexCoord;

out vec4 out_FragColor;

void main()
{
   float Tiles = 30.0;

   vec4 Pattern = texture( Texture3, vec2( Tiles, Tiles * 3.0 / 4.0 ) * TexCoord );

   /* convert [0,1] to [-1,1] ! */
   Pattern = 2.0 * Pattern - vec4( 1.0, 1.0, 1.0, 0.0 ); 

   vec4 Color   = texture( Texture0, TexCoord + 0.01 * Pattern.rb );
 
   out_FragColor = Color;
}
