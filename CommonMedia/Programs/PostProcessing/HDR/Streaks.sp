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
uniform sampler2D Texture1; // streaks rotation pattern

out vec4 out_FragColor;

void main()
{
  const float texOffset = 1.0 / 256.0;
  const int NumStreaks = 4;

  float streakSamples = 7.0;
  float attenuation   = 0.94;
  int iteration = 1;

  vec4 Color = vec4( 0.0 );

  float b = pow(streakSamples, iteration);

  vec2 streakDirection = texture( Texture1, TexCoord0 ).xy;

  for ( int k = 0; k < NumStreaks; k++ )
  {
     vec4 cOut = vec4( 0.0 );
  
     for ( int s = 0; s < streakSamples; s++)
     {
        float weight = pow(attenuation, b * s);

        // Streak direction is a 2D vector in image space
        vec2 texCoordSample = TexCoord0 + (streakDirection * b * s * texOffset);
     
        // Scale and accumulate
        cOut += clamp(weight, 0.0, 1.0) * texture( Texture0, texCoordSample ) / 4.0;
     };
     Color = max( Color, cOut );

     // rotate streak 90 degrees
     streakDirection = vec2( -streakDirection.y, streakDirection.x );
  }

  out_FragColor = Color;
}