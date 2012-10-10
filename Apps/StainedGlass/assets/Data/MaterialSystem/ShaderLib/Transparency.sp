// Alpha transparency utils

float GetAlphaTransparency()
{
#ifdef MATSYS_TRANSPARENT
   return 1.0 - MTL_TRANSPARENCY;
#else
   #ifdef MATSYS_ALPHATRANSPARENCY
      return texture( Texture2, TexCoord0 ).a;
   #else
      return 1.0;
   #endif

#endif
}