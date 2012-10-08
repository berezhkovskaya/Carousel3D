/*VERTEX_PROGRAM*/

#include "Layout.sp"

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

uniform float ENGINE_TIME;
uniform float EFFECT_START_TIME;
uniform float EFFECT_DURATION;
uniform vec4  EFFECT_COLOR1;
uniform vec4  EFFECT_COLOR2;
uniform float SHRINK_COEF;

void main()
{
   float Correction = 0.0;

   if ( TexCoord.x + TexCoord.y < 0.6 ) Correction += (0.6 - TexCoord.x - TexCoord.y);
   if ( TexCoord.x + TexCoord.y > 1.4 ) Correction += (TexCoord.x + TexCoord.y - 1.4);
   if ( TexCoord.x - TexCoord.y > 0.5 ) Correction += (TexCoord.x - TexCoord.y - 0.5);
   if ( TexCoord.y - TexCoord.x > 0.5 ) Correction += (TexCoord.y - TexCoord.x - 0.5);

   float EffectTime = ( ENGINE_TIME - EFFECT_START_TIME ) / EFFECT_DURATION;

   vec4 EffectColor = mix( EFFECT_COLOR1, EFFECT_COLOR2, EffectTime );

   if ( EffectTime >= 0.5 ) 
   {
      EffectTime = 1.0 - ( EffectTime - 0.5 ) * 2.0;
   } else
   if ( EffectTime < 0.5 ) EffectTime *= 2.0;

   vec2 Coord = TexCoord - vec2( 0.5, 0.5 );

   Coord *= 1.0 + SHRINK_COEF * EffectTime;

   Coord += vec2( 0.5, 0.5 );

   vec4 Color = texture( Texture0, Coord );

   Color += 2.0 * EffectColor * Correction * EffectTime;
 
   out_FragColor = Color;
}
