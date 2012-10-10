//

vec4 GetAmbient()
{
   float Ka = 0.05;

   vec4 Color = vec4(Ka) + MTL_AMBIENT_COLOR;

#ifdef MATSYS_USE_AMBIENT_MAP
   Color += texture(Texture1, TexCoord0);
#endif

   return Color;
}

vec4 GetDiffuse()
{
   vec4 Color = MTL_DIFFUSE_COLOR;

#ifdef MATSYS_USE_DIFFUSE_MAP
   Color += texture(Texture2, TexCoord0);
#endif

   return Color;
}

vec4 GetSpecular()
{
   vec4 Color = MTL_SPECULAR_COLOR;

#ifdef MATSYS_USE_SPECULAR_MAP    
   Color += texture( Texture3, TexCoord0 );
#endif

   return Color;
}
