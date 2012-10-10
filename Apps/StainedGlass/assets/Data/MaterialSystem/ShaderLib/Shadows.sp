// Shadowing utils

// 5x5 percentage closer filtering
float PCF5x5( const vec2 ShadowCoord )
{
	float Size = 1.0 / 1024.0;
	
	vec4 Shadow = texture( Texture8, ShadowCoord );

   for ( int v=-2; v<=2; v++ ) for ( int u=-2; u<=2; u++ )
   {
      Shadow += texture( Texture8, ShadowCoord + Size * vec2(u, v) );
   }
	
   return Shadow.r / 26.0;
}

// 7x7 percentage closer filtering
float PCF7x7( const vec2 ShadowCoord )
{
	float Size = 1.0 / 1024.0;
	
	vec4 Shadow = texture( Texture8, ShadowCoord );

   for ( int v=-3; v<=3; v++ ) for ( int u=-3; u<=3; u++ )
   {
      Shadow += texture( Texture8, ShadowCoord + Size * vec2(u, v) );
   }
	
   return Shadow.r / 50.0;
}

// http://micro.web.sin.cvut.cz/viz/doc/shadows_en.pdf
float PCSS( const vec4 lpos )
{
   float pcssLightSize = 10.0;

   // Compute radius of neighbourhood for occluder scaning.
   float receiver = lpos.z/lpos.w;
   float radius = (pcssLightSize/receiver)/2.0;

   // Scan the neighbourhood for occluders.
   float occluder = 0.0;
   float occluderCount = 0.0;

   for (int v=-2; v<=2; v++) for (int u=-2; u<=2; u++)
   {
      // Get depth value from shadow map.
      float depthVal = textureProj( Texture8, lpos.xyw + radius*vec3(u, v, 0.0)).r;

      // Compute average of occluding depths.
      float isBlocker = step(depthVal, receiver);
      occluder += depthVal*isBlocker;
      occluderCount += isBlocker;
   }

   occluder /= occluderCount;

   // Estimate penumbra size.
   float penumbra = pcssLightSize*(receiver-occluder)/occluder;

   // Perform ordinary PCF with diameter of penumbra size.
   radius = penumbra*lpos.w/6.0;
   float shadow = 0.0;

   for (int v=-3; v<=3; v++) for (int u=-3; u<=3; u++)
   {
      shadow += textureProj( Texture8, lpos + radius*vec4(u, v, 0.0, 0.0) ).r;
   }
   shadow /= 49.0;

   // Compute fragment color based on given shadow information.
   return shadow;
}
