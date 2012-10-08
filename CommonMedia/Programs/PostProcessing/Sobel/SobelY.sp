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
   float dx = 1.0 / 800.0;
   float dy = 1.0 / 600.0;

   vec4 S[3][3];

   S[0][0] = texture( Texture0, TexCoord + vec2( -dx, -dy ) );
   S[0][1] = texture( Texture0, TexCoord + vec2(   0, -dy ) );
   S[0][2] = texture( Texture0, TexCoord + vec2( +dx, -dy ) );

   S[1][0] = texture( Texture0, TexCoord + vec2( -dx,   0 ) );
   S[1][1] = texture( Texture0, TexCoord + vec2(   0,   0 ) );
   S[1][2] = texture( Texture0, TexCoord + vec2( +dx,   0 ) );
   
   S[2][0] = texture( Texture0, TexCoord + vec2( -dx, +dy ) );
   S[2][1] = texture( Texture0, TexCoord + vec2(   0, +dy ) );
   S[2][2] = texture( Texture0, TexCoord + vec2( +dx, +dy ) );

   mat3 Sx;
   mat3 Sy;

   Sx[0][0] = -1.0;
   Sx[0][1] =  0.0;
   Sx[0][2] = +1.0;

   Sx[1][0] = -2.0;
   Sx[1][1] =  0.0;
   Sx[1][2] = +2.0;

   Sx[2][0] = -1.0;
   Sx[2][1] =  0.0;
   Sx[2][2] = +1.0;

   vec4 Ry = vec4( 0 );

   for ( int i = 0 ; i != 3 ; i++ )
   {
      for ( int j = 0 ; j != 3 ; j++ )
      {
         Ry += Sx[j][i] * S[i][j];
      }
   }

   out_FragColor = Ry;
}
