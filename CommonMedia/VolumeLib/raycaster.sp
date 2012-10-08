// This is GLSL 1.50 compliant. 14/02/2010

/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;

out vec4  Pos;
out vec3  LightPosition; 
out mat3  ModelToEyeNormal;

uniform vec3 LIGHT_POS; // in world coordinates
uniform vec4 VOLUME_SIZE;

out vec3 EyeRay_Origin;
out vec3 EyeRay_Dir;

void main()
{
   vec4 EyePos = in_Vertex * VOLUME_SIZE;

   gl_Position = in_ModelViewProjectionMatrix * EyePos;

   Pos = in_ModelViewProjectionMatrix * EyePos;
   LightPosition = LIGHT_POS;
   ModelToEyeNormal = mat3(in_NormalMatrix);

   EyeRay_Origin = ( in_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0) ).xyz;
   // direction
   EyeRay_Dir = EyePos.xyz - EyeRay_Origin;
}

/*FRAGMENT_PROGRAM*/

#include Layout.sp

in vec4  Pos;
in vec3  LightPosition;
in mat3  ModelToEyeNormal;
in vec3  EyeRay_Origin;
in vec3  EyeRay_Dir;

uniform sampler3D   Texture0; // 3D volume 
uniform sampler2D   Texture1; // 2D transfer function 
uniform sampler3D   Texture2; // Noise 
uniform sampler3D   Texture3; // Gradients 
uniform sampler2D   Texture4; // Color material texture 
uniform sampler3D   Texture5; // ESL map

uniform float SATURATION;
uniform float CUT_OFF_LOW;
uniform float ISO_VALUE;
uniform float TRACING_STEPS;
uniform float CUT_PLANE_THICKNESS;
uniform vec4 CUT_PLANE;
uniform vec4 CUT_PLANE_COLOR;
uniform vec4 MAT_COLOR;
uniform vec3 SCALE_2DMAP;

out vec4 out_FragColor;

float FragDepth = 1.0; // on the Far clip plane

float EyeCoordsToFragDepth(float Z, float Near, float Far)
{
   return Far / (Far - Near) + ( (Far * Near / (Near - Far) ) / Z );
}

vec4 Sample_2DColorMap(vec3 V)
{
   float M = 2 * sqrt(V.x * V.x + V.y * V.y + (V.z + 1.0) * (V.z + 1.0));

   vec2 UV = ( V.xy / M ) + vec2( 0.5 );

   return textureLod( Texture4, 5.0 * UV * SCALE_2DMAP.xy, 0.0 );
}

vec4 SampleVol( vec3 Pos )
{
   return textureLod( Texture0, Pos, 0.0 );
}

#ifdef NORMAL_DYN_FILTERED
vec3 GetFilteredNormal( vec3 Vec )
{
   const vec3 W[27] = vec3[]( vec3( -1, -3, -1 ), vec3( -3, -6, -3 ), vec3( -1, -3, -1 ),
                              vec3(  0,  0,  0 ), vec3(  0,  0,  0 ), vec3(  0,  0,  0 ),
                              vec3(  1,  3,  1 ), vec3(  3,  6,  3 ), vec3(  1,  3,  1 ),
                              vec3( -1, -3, -1 ), vec3(  0,  0,  0 ), vec3(  1,  3,  1 ),
                              vec3( -3, -6, -3 ), vec3(  0,  0,  0 ), vec3(  3,  6,  3 ),
                              vec3( -1, -3, -1 ), vec3(  0,  0,  0 ), vec3(  1,  3,  1 ),
                              vec3( -1,  0,  1 ), vec3( -3,  0,  3 ), vec3( -1,  0,  1 ),
                              vec3( -3,  0,  3 ), vec3( -6,  0,  6 ), vec3( -3,  0,  3 ),
                              vec3( -1,  0,  1 ), vec3( -3,  0,  3 ), vec3( -1,  0,  1 ) );

   vec3 Normal = vec3( 0.0, 0.0, 0.0 );

   int Idx = 0;

   for ( int i = -1; i != 2; i++ )
   {
      for ( int j = -1; j != 2; j++ )
      {
         for ( int k = -1; k != 2; k++ )
         {
            Normal += W[ Idx ] * textureLod( Texture0, Vec + vec3( i, j, k ) / 256.0, 0.0 ).x;

            Idx += 1;
         }
      }
   }

   return normalize( Normal );
}
#endif

vec4 ClippedColor( vec3 Vec, vec4 Color )
{
#ifdef ENABLE_CLIPPLANE
   float CPEpsilon = 1.0 / 256.0;
 
   float dp = dot( CUT_PLANE.xyz, Vec );

   if ( abs( dp + CUT_PLANE.w ) < CPEpsilon ) return Color * CUT_PLANE_COLOR;
   if ( abs( dp + CUT_PLANE.w + CUT_PLANE_THICKNESS ) < CPEpsilon ) return Color * CUT_PLANE_COLOR;
#endif

   return Color;
}






#ifdef UNUSED_CODE
void GetColor_Wood(vec3 Position, out vec4 Ambient, out vec4 Diffuse, out vec4 Specular)
{
   vec3 vScaledPosition = Position * 1.5;

   float Ka            = 0.2;
   float Kd            = 0.89;
   float Ks            = 0.66;
   vec4  darkWood      = vec4(0.44,     0.210526, 0.0,      1.0);
   vec4  liteWood      = vec4(0.917293, 0.503759, 0.127820, 1.0);
   float frequency     = 6.72;
   float noiseScale    = 0.96;
   float ringSharpness = 21.0;

   /* Signed noise */
   float snoise = 2.0 * textureLod(Texture2, vScaledPosition, 0.0).x - 1.0;
   /* Rings goes along z axis, perturbed with some noise */
   float r = fract(frequency * vScaledPosition.z + noiseScale * snoise);

   float invMax = pow(ringSharpness, ringSharpness / (ringSharpness - 1.0)) / (ringSharpness - 1.0);
   float ring = invMax * (r - pow(r, ringSharpness));

   /* Add some noise and get base color */
   float lrp = ring + snoise;
   vec4  base = mix(darkWood, liteWood, lrp);

   Ambient  = Ka * base;
   Diffuse  = Kd * base;
   Specular = vec4( Ks );
}
#endif




#ifdef MATERIAL_SCATTERED
float HalfLambert( vec3 Vec1, vec3 Vec2 )
{  
   return dot( Vec1, Vec2 ) * 0.5 + 0.5;
}  
   
float BlinnPhongSpecular( vec3 normalVec, vec3 lightVec, float specPower )
{  
     vec3 halfAngle = normalize(normalVec + lightVec);
     
     return pow( clamp(0.0, 1.0, dot(normalVec,halfAngle)), specPower);
}  

vec4 Material_SubsurfaceScattering(vec3 lightPos, vec3 vertPos, vec3 eyePos, vec3 Normal, vec4 baseColor)
{  
     float matThickness = 0.56;
     vec3  extinction   = vec3( 0.74, 0.5, 0.5 );
     float specPower    = 35.0;
     float rimScalar    = 1.38;
     vec4 specColor     = baseColor;
     vec4 lightColor    = vec4( 1.0, 1.0, 1.0, 1.0 );

     float attenuation = 30.0 * (1.0 / distance(lightPos,vertPos)); 
     vec3  e           = normalize ( eyePos - vertPos      );
     vec3  l           = normalize ( lightPos - vertPos );
     vec3  n           = normalize ( Normal );  
     vec4 dotLN        = vec4 ( HalfLambert ( l, n ) * attenuation );

     dotLN *= baseColor;  
   
     float   inFactor = max ( 0.0, dot ( -n, l ) ) + HalfLambert ( -e, l );
     vec3    indirectLightComponent = vec3 ( matThickness * inFactor * attenuation ) * extinction;
     vec3    rim = vec3(1.0 - max ( 0.0, dot ( n, e ) ) );  
     
     rim *= rim;  
     rim *= max ( 0.0, dot ( n, l ) ) * specColor.rgb;  
   
     vec4 finalCol = dotLN + vec4 ( indirectLightComponent, 1.0 );
     finalCol.rgb += vec3(BlinnPhongSpecular ( n, l, specPower) * attenuation * specColor * finalCol.a * 0.25 );
     finalCol.rgb += (rim * rimScalar * attenuation * finalCol.a);
     finalCol.rgb *= lightColor.rgb;
   
     return finalCol;  
}
#endif

#ifdef MATERIAL_WOOD
vec4 Material_Wood(vec3 Position, vec3 Normal, vec3 Eye, vec4 Opacity)
{
   vec3 vViewVec = Eye - Position;
   vec3 vScaledPosition = Position * 1.5;

   float Ka            = 0.2;
   float Kd            = 0.89;
   float Ks            = 0.66;
   vec4  darkWood      = vec4(0.44,     0.210526, 0.0,      1.0);
   vec4  liteWood      = vec4(0.917293, 0.503759, 0.127820, 1.0);
   float frequency     = 6.72;
   float noiseScale    = 0.96;
   float ringSharpness = 21.0;
   vec4  lightDir      = normalize( vec4( LightPosition, 1.0 ) - vec4( Position, 0.0 ) );

   /* Signed noise */
   float snoise = 2.0 * textureLod(Texture2, vScaledPosition, 0.0).x - 1.0;
   /* Rings goes along z axis, perturbed with some noise */
   float r = fract(frequency * vScaledPosition.z + noiseScale * snoise);

   float invMax = pow(ringSharpness, ringSharpness / (ringSharpness - 1.0)) / (ringSharpness - 1.0);
   float ring = invMax * (r - pow(r, ringSharpness));

   /* Add some noise and get base color */
   float lrp = ring + snoise;
   vec4  base = mix(darkWood, liteWood, lrp);

   vec3 normal = normalize(Normal);
   /* Soft diffuse */
   float diffuse = 0.3 + 0.7 * max( dot( lightDir.xyz, normal ), 0.0 );
   /* Standard specular */
   float specular = pow(clamp(dot(reflect(-normalize(vViewVec), normal), lightDir.xyz),0.0, 1.0), 12.0);

   vec4 Color = Ka * base + Kd * diffuse * base + vec4(Ks * specular);

#ifdef ENABLE_CLIPPLANE
   return ClippedColor( Position, Color );
#else
   return Color;
#endif
}
#endif


#ifdef MATERIAL_MARBLE
vec4 Material_Marble(vec3 Position, vec3 Normal, vec3 Eye, vec4 Opacity)
{
   vec3 vViewVec = Eye - Position;
   vec3 vScaledPosition = Position * 1.5;

   vec4 color = vec4(1.0, 0.842717, 0.625152, 1.0); 

   vec4  light_color = vec4( 1.0, 1.0, 1.0, 1.0 );
   vec4  lightDir = normalize( vec4( LightPosition, 1.0 ) - vec4( Position, 0.0 ) );

   // noise
   float noisy = textureLod(Texture2, vScaledPosition, 0.0).x;

   // Base marble color
   float marble = (0.2 + 5.0 * abs(noisy - 0.5));

   vec3 normal = normalize(Normal);

   // Simple lighting
   vec3 reflectionVec = reflect(-normalize(vViewVec), normal);
   float diffuse = 0.3 * max( dot( lightDir.xyz, normal ), 0.0 ) + 0.7;
   float specular = pow(clamp(dot(reflectionVec, lightDir.xyz), 0.0, 1.0), 24.0);

   // We assume dark parts of the marble reflects light better
   float Ks = clamp(1.1 - 1.3 * marble, 0.0, 1.0);

   vec4 Color = diffuse * marble * color + vec4(Ks * specular);

#ifdef ENABLE_CLIPPLANE
   return ClippedColor( Position, Color );
#else
   return Color;
#endif
}
#endif

float GetDiffuse(vec3 Normal)
{
   vec3 lightDir = vec3( 0.408250, 0.408250, 0.8165 );

   return 0.8 * dot(lightDir, Normal) + 0.2;
}

#ifdef MATERIAL_PLASTIC
vec4 Material_Plastic(vec3 Position, vec3 Normal, vec3 Eye, vec4 Opacity)
{
   vec3 vViewVec = Eye - Position;

#ifdef USE_2D_TEXTURE_MAP
   vec4 color = ClippedColor( Position, Sample_2DColorMap( Position ) );
#else
   vec4 color = ClippedColor( Position, MAT_COLOR ); 
#endif

   color = Opacity * Opacity.a + color * (1.0 - Opacity.a);

   vec4  light_color = vec4( 1.0, 1.0, 1.0, 1.0 );
   vec4  lightDir = normalize( vec4( LightPosition, 1.0 ) - vec4( Position, 0.0 ) );

   // noise 
   vec3 vScaledPosition = Position * 0.5;
   vec3 normal = normalize( Normal );

   vec3 reflectionVec = reflect(-normalize(vViewVec), normal);
   float diffuse = 0.6 * max( dot( lightDir.xyz, normal ), 0.0 ) + 0.4;
   float specular = pow(clamp(dot(reflectionVec, lightDir.xyz), 0.0, 1.0), 24.0);

   float Ks = clamp(1.1 - 1.3 * color.r, 0.0, 1.0);

   return diffuse * color + vec4(Ks * specular);
}
#endif

#ifdef MATERIAL_GRADIENTS
vec4 Material_ColorGradient(vec3 Position, vec3 Normal, vec3 Eye, vec4 Opacity)
{
   return vec4( 0.5 * Normal, 1.0 ) + vec4( 0.5, 0.5, 0.5, 0.0 );
}
#endif

#ifdef MATERIAL_IVORY
vec4 Material_Ivory(vec3 Position, vec3 Normal, vec3 Eye, vec4 Opacity)
{
   // Compute normalized vector from vertex to light in eye space  (Leye)
   vec3 Leye = normalize( LightPosition - Position);

   // Normalize interpolated normal
   // vNormalES    = in_NormalMatrix * in_Normal;
   vec3 Neye = normalize( Normal );

    // Compute Veye
   vec3 Veye = -normalize( Position );

    // Compute half-angle
   vec3 Heye = normalize(Leye + Veye);

   // N.L
   float NdotL = dot(Neye, Leye);

   // "Half-Lambert" technique for more pleasing diffuse term
   float diffuse = clamp( NdotL * 0.5 + 0.5, 0.1, 1.0 );

   // Compute N.H
   float NdotH = pow(clamp(dot(Neye, Heye), 0.0, 1.0), 64.0);

   float intensity = NdotH + diffuse;

#ifdef USE_2D_TEXTURE_MAP
   vec4 Color = intensity * Sample_2DColorMap( Position );
#else
   vec4 Color = intensity * MAT_COLOR;
#endif

   return ClippedColor( Position, Color );
}
#endif

vec4 Lighting(vec3 n, vec3 lightDir, vec4 color, vec4 Opacity)
{
   float diffuse = max( dot( lightDir, n ), 0.0 );

   return color * (diffuse + 0.4) + pow(diffuse, 32.0);
}

#ifdef NORMAL_CENTERDIFF
vec3 GetNormal( vec3 Vec, float Eps )
{
   vec3 Smp1 = 0.5 * vec3(  Eps,  0.0,  0.0 );
   vec3 Smp2 = 0.5 * vec3(  0.0,  Eps,  0.0 );
   vec3 Smp3 = 0.5 * vec3(  0.0,  0.0,  Eps );

   vec3 Normal;

   Normal.x = textureLod( Texture0, Vec + Smp1, 0.0 ).x - textureLod( Texture0, Vec - Smp1, 0.0 ).x;
   Normal.y = textureLod( Texture0, Vec + Smp2, 0.0 ).x - textureLod( Texture0, Vec - Smp2, 0.0 ).x;
   Normal.z = textureLod( Texture0, Vec + Smp3, 0.0 ).x - textureLod( Texture0, Vec - Smp3, 0.0 ).x;

   return normalize( Normal );
}
#endif

#ifdef NORMAL_FORWARDDIFF
// Calculate the gradient based on the A channel using forward differences.
vec3 GetNormalFD( vec3 Vec, float Delta )
{
   float Eps = 0.5 * Delta;

   float v =  textureLod( Texture0, Vec, 0.0).x;
   float v0 = textureLod( Texture0, Vec + vec3(Eps, 0.0, Eps ), 0.0 ).x;
   float v1 = textureLod( Texture0, Vec + vec3(0.0, Eps, 0.0 ), 0.0 ).x;
   float v2 = textureLod( Texture0, Vec + vec3(0.0, 0.0, Eps ), 0.0 ).x;
   return normalize( vec3(v0-v, v1-v, v2-v) );
}
#endif

float JitterPos( vec3 Vec )
{
#ifdef AMD
   return 0.0;
#else
   float Noise = ( textureLod( Texture2, 256.0 * Vec, 0.0 ).x - 0.5 ) / 256.0;

   return Noise;
#endif
}

vec3 SelectNormal( vec3 Vec, float CPEpsilon )
{
#ifdef ENABLE_CLIPPLANE
   float dp = dot( CUT_PLANE.xyz, Vec );

   if ( abs( dp + CUT_PLANE.w ) < CPEpsilon ) return ModelToEyeNormal * CUT_PLANE.xyz;
   if ( abs( dp + CUT_PLANE.w + CUT_PLANE_THICKNESS ) < CPEpsilon ) return ModelToEyeNormal * CUT_PLANE.xyz;
#endif

#ifdef NORMAL_DYN_FILTERED
   return ModelToEyeNormal * GetFilteredNormal( Vec );
#endif

#ifdef NORMAL_FORWARDDIFF
   return ModelToEyeNormal * GetNormalFD( Vec, CPEpsilon );
#endif

#ifdef NORMAL_CENTERDIFF
   return ModelToEyeNormal * GetNormal( Vec, CPEpsilon );
#endif

#ifdef NORMAL_PRECOMPUTED_RGBA
   return ModelToEyeNormal * ( ( textureLod( Texture3, Vec, 0.0 ).xyz * 2.0 ) - vec3( 1.0, 1.0, 1.0 ) );
#endif

#ifdef NORMAL_PRECOMPUTED_FLOAT
   return ModelToEyeNormal * textureLod( Texture3, Vec, 0.0 ).xyz;
#endif

   return vec3( 0.0, 0.0, 0.0 );
}

bool IsClippedPos( vec3 Vec )
{
#ifdef ENABLE_CLIPPLANE
   float dp = dot( CUT_PLANE.xyz, Vec );

   if ( dp + CUT_PLANE.w > 0 ) return true;
   if ( dp + CUT_PLANE.w + CUT_PLANE_THICKNESS < 0 ) return true;
#endif
  
   return false;
}

#ifdef MODE_ISOSURFACE
vec4 TraceIsosurface( vec3 FrontPos, vec3 DeltaDir, int Steps, float MaxLength )
{
   vec3 Vec       = FrontPos + JitterPos( FrontPos ) * DeltaDir;
   vec3 PrevVec   = Vec;
   vec4 PrevColor = textureLod( Texture0, Vec, 0.0 );

   float LengthAcc = 0.0;
   float StepSize  = length( DeltaDir );
   float CPEpsilon = 1.0 / Steps;

   float AlphaAcc  = 0.0;
   vec4 ColorAcc   = vec4( 0.0, 0.0, 0.0, 0.0 );

   for(int i = 0; i < Steps; i++)
   {  
      vec4 Color = textureLod( Texture0, Vec, 0.0 );
      float Scalar = Color.r;
      float Scalar2 = PrevColor.r;
      PrevColor = Color;

      if ( Scalar > CUT_OFF_LOW )
      {
#ifdef TF_2D
         Color = texture( Texture1, vec2( Scalar, abs(Scalar-Scalar2) ) ); 
#endif
         ColorAcc += (1.0 - AlphaAcc) * Color * StepSize * SATURATION;
      }

      AlphaAcc += Scalar * StepSize;

      // Build isosurface
      if ( Scalar > ISO_VALUE )
      {
#ifndef ENABLE_CLIPPLANE
         vec3 Refine = -0.5 * DeltaDir;

         for ( int j = 0; j < 4; j++ )
         {
            Vec += Refine;
            Scalar = textureLod( Texture0, Vec, 0.0 ).x;

            Refine *= ( Scalar > ISO_VALUE ) ?  0.5 : -0.5;
         }
#endif

#ifdef ENABLE_CLIPPLANE
         if ( !IsClippedPos(Vec) )
#endif
         {
            vec3 Normal = SelectNormal( Vec, CPEpsilon );


#ifdef MATERIAL_MARBLE
            vec4 FinalColor = Material_Marble( Vec, Normal, vec3( 0.0, 0.0, 0.0 ), ColorAcc );
#endif

#ifdef MATERIAL_WOOD
            vec4 FinalColor = Material_Wood( Vec, Normal, vec3( 0.0, 0.0, 0.0 ), ColorAcc );
#endif
                           
#ifdef MATERIAL_PLASTIC
            vec4 FinalColor = Material_Plastic( Vec, Normal, vec3( 0.0, 0.0, -100.0 ), ColorAcc );
#endif

#ifdef MATERIAL_GRADIENTS
            vec4 FinalColor = Material_ColorGradient( Vec, Normal, vec3( 0.0, 0.0, 0.0 ), ColorAcc );
#endif

#ifdef MATERIAL_IVORY
            vec4 FinalColor = Material_Ivory( Vec, Normal, vec3( 0.0, 0.0, 0.0 ), ColorAcc );
#endif

#ifdef MATERIAL_SCATTERED
            FinalColor += Material_SubsurfaceScattering( vec3( 0.0, 0.0, -100.0 ), Vec, vec3( 0.0, 0.0, 0.0 ), Normal, ColorAcc );
#endif

            FinalColor.a = 1.0;

            // output depth
            vec4 ObjectSpace = vec4( Vec * 2.0 - vec3( 1.0, 1.0, 1.0 ), 1.0 );
            vec4 EyeSpace    = in_ModelViewProjectionMatrix * ObjectSpace;
//            FragDepth        = EyeCoordsToFragDepth( EyeSpace.z / EyeSpace.w, 0.04, 10.0 );
            FragDepth = ( EyeSpace.z / EyeSpace.w );

            return FinalColor;
/*
            // Some output for deferred shading

            // coords in camera space
//            return in_ModelViewProjectionMatrix*vec4(Vec, 1.0);
            // normal in model space
//            return vec4(Normal, 1.0);

            // 1. Fill all required buffers for deferred shading

            vec4 Diffuse, Ambient, Specular;
            GetColor_Wood(Vec, Ambient, Diffuse, Specular);

            // 2. build lightmaps : by now we only have one

            vec3 CameraSpaceVec = (in_ModelViewMatrix * vec4( Vec, 1.0)).xyz;
 
            vec3  lightDir = normalize( LightPosition - Vec );
            float Kd = max( dot( lightDir.xyz, Normal ), 0.0 );

            vec3 vViewVec = -CameraSpaceVec;

            // Standard specular
            float Ks = pow(clamp(dot(reflect(-normalize(vViewVec), Normal), lightDir.xyz),0.0, 1.0), 12.0);


            // 3. Emulation of combine shader from deferred shading
 
           //  only diffuse params
            FinalColor = Ambient + Diffuse*Kd + Specular*Ks;
            FinalColor.a = 1.0;

            return FinalColor;
*/
         }
      }

      Vec += DeltaDir;
      LengthAcc += StepSize;
 
      if ( LengthAcc >= MaxLength ) break;
   }

   return ColorAcc;
}
#endif

#ifdef MODE_OPACITY
vec4 TraceOpacity( vec3 FrontPos, vec3 DeltaDir, int Steps, float MaxLength )
{
   vec3 Vec       = FrontPos + JitterPos( FrontPos ) * DeltaDir;
   vec3 PrevVec   = Vec;
   vec4 PrevColor = textureLod( Texture0, Vec, 0.0 );

   float AlphaAcc  = 0.0;
   float LengthAcc = 0.0;
   float StepSize  = length( DeltaDir );
   float CPEpsilon = 1.0 / Steps;
   vec4 ColorAcc   = vec4( 0.0, 0.0, 0.0, 0.0 );

   for(int i = 0; i < Steps; i++)
   {
      vec4 Color = textureLod( Texture0, Vec, 0.0 );
//      float Scalar = Color.r;
      float Scalar = dot( vec3( 0.333, 0.333, 0.333 ), Color.xyz );
      float Scalar2 = PrevColor.r;
      PrevColor = Color;
      PrevVec = Vec;

#ifdef ENABLE_CLIPPLANE
      if ( !IsClippedPos(Vec) )
#endif
      {
         if ( Scalar > CUT_OFF_LOW )
         {
#ifdef TF_1D_PREINTEGRATION
            Color = texture( Texture1, vec2( Scalar, abs(Scalar-Scalar2) ) ); 
#endif 

#ifdef TF_1D
            Color = texture( Texture1, vec2( Scalar, 0.5 ) ); 
#endif 

#ifdef TF_2D
            Color = texture( Texture1, vec2( Scalar, length( SelectNormal(Vec, CPEpsilon) ) ) ); 
#endif 

#ifdef TF_NONE
//            Color = /*GetDiffuse( GetNormal( Vec, 1.0 / Steps ) ) */ MAT_COLOR;
            Color.a = Scalar;
#endif
            ColorAcc += (1.0 - AlphaAcc) * Color * StepSize * SATURATION;
         }

         AlphaAcc += Scalar * StepSize;
      }

      Vec += DeltaDir;
      LengthAcc += StepSize;

      if (LengthAcc >= MaxLength || AlphaAcc > 1.0) break;
   }

   ColorAcc.a = SATURATION * AlphaAcc;

   return ColorAcc;
}
#endif

#ifdef MODE_MIP
vec4 TraceMIP( vec3 FrontPos, vec3 DeltaDir, int Steps, float MaxLength )
{
   vec3 Vec       = FrontPos + JitterPos( FrontPos ) * DeltaDir;

   float LengthAcc = 0.0;
   float StepSize  = length( DeltaDir );

   float MaxScalar = 0.0;
   vec3 MaxVec    = Vec;

   for(int i = 0; i < Steps; i++)
   {
      float Scalar = textureLod( Texture0, Vec, 0.0 ).r;
                                                       
#ifdef ENABLE_CLIPPLANE
      if ( !IsClippedPos(Vec) )
#endif
      if ( Scalar > MaxScalar && Scalar > CUT_OFF_LOW )
      {
         MaxScalar = Scalar;
         MaxVec = Vec;
      }

      Vec += DeltaDir;
      LengthAcc += StepSize;

      if (LengthAcc >= MaxLength ) break;
   }

   vec4 Color = vec4( MaxScalar * ( MaxLength / LengthAcc ) );

   Color.a = MaxScalar;

   return Color;
}
#endif

#include "MaterialSystem/ShaderLib/VolumeRendering.sp"

void main()
{
   // calculate ray intersection with bounding box
   float tnear, tfar;
   Ray EyeRay;
   EyeRay.o = EyeRay_Origin;
   EyeRay.d = normalize( EyeRay_Dir );
   bool hit = IntersectBox( EyeRay, BBoxMin, BBoxMax, tnear, tfar);
   if (!hit) discard;
   if (tnear < 0.0) tnear = 0.0;

   int   Steps = int(TRACING_STEPS);

   vec2 Coord = ( ( Pos.xy / Pos.w ) + 1.0 ) / 2.0;

   vec3 BackPosition  = EyeRay.o + EyeRay.d * tfar;
   vec3 FrontPosition = EyeRay.o + EyeRay.d * tnear;

   // convert to texture space
   BackPosition  = BackPosition  * 0.5 + vec3( 0.5 );
   FrontPosition = FrontPosition * 0.5 + vec3( 0.5 );

   vec3 Dir = BackPosition - FrontPosition;
   float Length = length( Dir );

   vec3 DeltaDir = Dir / Steps;

#ifdef MODE_ISOSURFACE
   out_FragColor = TraceIsosurface( FrontPosition, DeltaDir, Steps, Length );
   gl_FragDepth = FragDepth;
#endif

#ifdef MODE_OPACITY
   out_FragColor = TraceOpacity( FrontPosition, DeltaDir, Steps, Length );
#endif

#ifdef MODE_MIP
   out_FragColor = TraceMIP( FrontPosition, DeltaDir, Steps, Length );
#endif

//   out_FragColor = vec4( vec3(FragDepth), 1.0 );
//   out_FragColor = vec4( Pos.xyz, 1.0 );
}
