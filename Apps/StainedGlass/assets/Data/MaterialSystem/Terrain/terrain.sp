/*VERTEX_PROGRAM*/

varying vec2 TexCoord0;
varying vec2 TexCoord1;
varying vec4 TexCoord2;

varying vec3 Normal;
varying vec3 LightDir;

uniform mat4 SHADOW_PROJECTION;

void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   TexCoord0 = vec2(gl_MultiTexCoord0);
   TexCoord1 = 0.15 * vec2(gl_MultiTexCoord1);
   TexCoord2 = SHADOW_PROJECTION * gl_ModelViewMatrix * gl_Vertex;

   LightDir = vec3(gl_ModelViewMatrix * vec4(0.08248, 0.08249, -0.816497, 0.0) );

   /* Compute fake tangent space */
   Normal = vec3( 0.0, 0.0, 1.0 );
}


/*FRAGMENT_PROGRAM*/

uniform sampler2D       Texture0; /* diffuse */
uniform sampler2D       Texture1; /* -free- */
uniform sampler2D       Texture2; /* shadow map */
uniform sampler2D       Texture3; /* detail blending */
uniform sampler2D       Texture4; /* detail1         */
uniform sampler2D       Texture5; /* detail2         */
uniform sampler2D       Texture6; /* detail3         */
uniform sampler2D       Texture7; /* detail4         */

varying vec2 TexCoord0;
varying vec2 TexCoord1;
varying vec4 TexCoord2;

varying vec3 Normal;
varying vec3 LightDir;

void main()
{
   vec2 Coord = vec2( TexCoord0.x, 1.0 - TexCoord0.y );

   vec4 Color       = texture( Texture0, Coord );
   vec4 DetailBlend = texture( Texture3, Coord );

   vec3 lightDir = normalize(LightDir);

   vec3 normal = normalize( Normal );

   float diffuse = max( 0.0, 0.4 * dot(lightDir, normal) ) + 0.6;

   /* calculate details */   
   vec4 Detail1 = texture( Texture4, TexCoord1 );
   vec4 Detail2 = texture( Texture5, TexCoord1 );
   vec4 Detail3 = texture( Texture6, TexCoord1 );
   vec4 Detail4 = texture( Texture7, TexCoord1 );

   float Sum = dot( DetailBlend, vec4( 1.0, 1.0, 1.0, 1.0 ) );

   if ( Sum < 1.0 ) Sum = 1.0;

   vec4 Detail = (DetailBlend.x * Detail1 + 
                  DetailBlend.y * Detail2 +
                  DetailBlend.z * Detail3 +
                  DetailBlend.w * Detail4) / Sum;
/*   vec4 Detail = vec4( 0.5, 0.5, 0.5, 0.0 ); */

   Detail = Detail - vec4( 0.5, 0.5, 0.5, 0.0 );

   /* calculate shadows */
   float LightCoef = 1.0;

   float DepthBias = 0.01;

   float PCF = 1.0 / 4096.0;

   /* Shadow mapping */
   vec4 ShadowCoords4 = TexCoord2 / TexCoord2.w;
 
   vec2 ShadowCoord = vec2( ShadowCoords4 );

   float ShadowDepth = ShadowCoords4.z + DepthBias;

   vec4 ShadowSample;

   /* Percentage Closer Filtering */
   ShadowSample = texture( Texture2, ShadowCoord );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.63;

/*   ShadowSample = texture( Texture2, ShadowCoord + vec2( PCF, 0.0 ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( -PCF, 0.0 ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( 0.0, PCF ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( 0.0, -PCF ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( PCF, PCF ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( -PCF, -PCF ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( -PCF, PCF ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95;

   ShadowSample = texture( Texture2, ShadowCoord + vec2( PCF, -PCF ) );
   if ( ShadowDepth >= ShadowSample.x ) LightCoef *= 0.95; */

   /**/

   gl_FragColor = LightCoef * diffuse * 0.9 * Color + Detail;
}
