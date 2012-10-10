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
   float dx = 1.0 / 1600.0;
   float dy = 1.0 / 1200.0;

   vec4 S00, S01, S02;
   vec4 S10, S11, S12;
   vec4 S20, S21, S22;

   S00 = texture( Texture0, TexCoord + vec2( -dx, -dy ) );
   S01 = texture( Texture0, TexCoord + vec2(   0, -dy ) );
   S02 = texture( Texture0, TexCoord + vec2( +dx, -dy ) );

   S10 = texture( Texture0, TexCoord + vec2( -dx,   0 ) );
   S11 = texture( Texture0, TexCoord + vec2(   0,   0 ) );
   S12 = texture( Texture0, TexCoord + vec2( +dx,   0 ) );
   
   S20 = texture( Texture0, TexCoord + vec2( -dx, +dy ) );
   S21 = texture( Texture0, TexCoord + vec2(   0, +dy ) );
   S22 = texture( Texture0, TexCoord + vec2( +dx, +dy ) );

/*
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
*/
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

   vec4 Rx = vec4( 0 );
/*
   for ( int i = 0 ; i != 3 ; i++ )
   {
      for ( int j = 0 ; j != 3 ; j++ )
      {
         Rx += Sx[i][j] * S[i][j];
      }
   }
*/
   Rx += Sx[0][0] * S00;
   Rx += Sx[0][1] * S01;
   Rx += Sx[0][2] * S02;

   Rx += Sx[1][0] * S10;
   Rx += Sx[1][1] * S11;
   Rx += Sx[1][2] * S12;

   Rx += Sx[2][0] * S20;
   Rx += Sx[2][1] * S21;
   Rx += Sx[2][2] * S22;

   vec4 Ry = vec4( 0 );
/*
   for ( int i = 0 ; i != 3 ; i++ )
   {
      for ( int j = 0 ; j != 3 ; j++ )
      {
         Ry += Sx[j][i] * S[i][j];
      }
   }
*/
   Ry += Sx[0][0] * S00;
   Ry += Sx[1][0] * S01;
   Ry += Sx[2][0] * S02;

   Ry += Sx[0][1] * S10;
   Ry += Sx[1][1] * S11;
   Ry += Sx[2][1] * S12;

   Ry += Sx[0][2] * S20;
   Ry += Sx[1][2] * S21;
   Ry += Sx[2][2] * S22;

   out_FragColor = sqrt( Rx * Rx + Ry * Ry );
}
