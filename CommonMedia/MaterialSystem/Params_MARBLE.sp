//

vec4 GetAmbient()
{
   return vec4( 0.0 );
}

vec4 GetDiffuse()
{
   vec4 Color = vec4(1.0, 0.842717, 0.625152, 1.0); 

   vec3 vScaledPosition = LocalPosition.xzy * 0.05;

   float noisy = texture(Texture5, vScaledPosition).x;

   /* Base marble color */
   float Marble = (0.2 + 5.0 * abs(noisy - 0.5));

#ifdef MATSYS_USE_DIFFUSE_MAP
   Color += texture(Texture2, TexCoord0);
#endif

   return Color * vec4( Marble );
}

vec4 GetSpecular()
{
   vec3 vScaledPosition = LocalPosition.xzy * 0.05;

   float noisy = texture(Texture5, vScaledPosition).x;

   /* Base marble color */
   float Marble = (0.2 + 5.0 * abs(noisy - 0.5));

   /* We assume dark parts of the marble reflects light better */
   float Ks = clamp(1.1 - 1.3 * Marble, 0.0, 1.0);

   return vec4( Ks );
}