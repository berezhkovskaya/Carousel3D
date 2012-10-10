/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
}

/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0;     /* Current luminance */
uniform sampler2D Texture1;     /* Adapted luminance */

/*uniform float ElapsedTime;*/

out vec4 out_FragColor;

void main()
{
   float ElapsedTime = 0.1;

   float CurrentLum = texture( Texture0, vec2(0.5, 0.5) ).x;
   float AdaptedLum = texture( Texture1, vec2(0.5, 0.5) ).x;

   float NewAdaptation = AdaptedLum + (CurrentLum - AdaptedLum) * ( 1.0 - pow( 0.98, 30.0 * ElapsedTime ) );

   out_FragColor = vec4( NewAdaptation, NewAdaptation, NewAdaptation, 0.0 );
}