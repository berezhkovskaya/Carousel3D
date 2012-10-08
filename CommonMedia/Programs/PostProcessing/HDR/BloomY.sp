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
  const float texScaler =  2.0 / 256.0;
  const float texOffset = -0.5 / 256.0;

  vec4 gaussFilter[11];

  gaussFilter[0]  = vec4(-5.0, 0.0, 0.0,  3.0/133.0);
  gaussFilter[1]  = vec4(-4.0, 0.0, 0.0,  6.0/133.0);
  gaussFilter[2]  = vec4(-3.0, 0.0, 0.0, 10.0/133.0);
  gaussFilter[3]  = vec4(-2.0, 0.0, 0.0, 15.0/133.0);
  gaussFilter[4]  = vec4(-1.0, 0.0, 0.0, 20.0/133.0);
  gaussFilter[5]  = vec4( 0.0, 0.0, 0.0, 25.0/133.0);
  gaussFilter[6]  = vec4( 1.0, 0.0, 0.0, 20.0/133.0);
  gaussFilter[7]  = vec4( 2.0, 0.0, 0.0, 15.0/133.0);
  gaussFilter[8]  = vec4( 3.0, 0.0, 0.0, 10.0/133.0);
  gaussFilter[9]  = vec4( 4.0, 0.0, 0.0,  6.0/133.0);
  gaussFilter[10] = vec4( 5.0, 0.0, 0.0,  3.0/133.0);

  vec4 Color = vec4( 0.0, 0.0, 0.0, 0.0 );

  for ( int i = 0; i < 11; i++ )
  {
     vec2 Coord = vec2( TexCoord0.x + gaussFilter[i].y * texScaler + texOffset,
                        TexCoord0.y + gaussFilter[i].x * texScaler + texOffset );

     Color += texture( Texture0, Coord ) * gaussFilter[i].w;
  }

  Color.a *= 2.0;

  out_FragColor = Color;
}
