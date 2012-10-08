//

vec4 GetAmbient()
{
   float Ka = 0.05;

   return vec4(Ka);
}

vec4 GetDiffuse()
{
   return MTL_DIFFUSE_COLOR;
}

vec4 GetSpecular()
{
   return MTL_SPECULAR_COLOR;
}

float GetCustomAlphaTransparency()
{
   return GetAlphaTransparency();
}
