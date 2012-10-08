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
  const float texScaler =  2.0 / 512.0;
  const float texOffset = -0.05 / 512.0;

  vec4 gaussFilter[3];

  gaussFilter[0]  = vec4(-1.0, 0.0, 0.0, 30.0/133.0);
  gaussFilter[1]  = vec4( 0.0, 0.0, 0.0, 73.15/133.0);
  gaussFilter[2]  = vec4( 1.0, 0.0, 0.0, 30.0/133.0);

  vec4 Color = vec4( 0.0, 0.0, 0.0, 0.0 );

  for ( int i = 0; i < 3; i++ )
  {
     vec2 Coord = vec2( TexCoord0.x + gaussFilter[i].y * texScaler + texOffset,
                        TexCoord0.y + gaussFilter[i].x * texScaler + texOffset );

     Color += texture( Texture0, Coord ) * gaussFilter[i].w;
  }

  Color.a *= 3.0;

  out_FragColor = Color;
}
