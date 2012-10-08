/*VERTEX_PROGRAM*/

//
// all ligting is done in camera space
//

#include Layout.sp

uniform vec3  in_LightPos;

in vec4 in_Vertex;
in vec4 in_TexCoord;
in vec3 in_Normal;
//in vec2 in_Bones;

//uniform mat4 in_ShadowMatrix;

out vec3 LocalPosition;
out vec2 TexCoord0;
out vec3 eyeNormal;
out vec3 modelNormal;
out vec3 viewVec;
//out vec4 ProjectedVertex;
//out vec4 ShadowMapCoord;

uniform float ENGINE_TIME;
//uniform float Skinned;

//uniform sampler2D Texture0; // packed skeleton
//uniform vec4 KeyFrame;      // Frame1, Frame2, Lerp

//#_include MaterialSystem/ShaderLib/QuaternionLib.sp
//#_include MaterialSystem/ShaderLib/Skinning.sp

mat4 GetProjScaleBiasMat()
{ 
   // transform from -1..1 to 0..1
   return mat4( 0.5, 0.0, 0.0, 0.0, 
                0.0, 0.5, 0.0, 0.0, 
                0.0, 0.0, 0.5, 0.0, 
                0.5, 0.5, 0.5, 1.0 );
}

void main()
{
 //  vec4 Position = ( Skinned == 1.0 ) ? SkinVertex() : in_Vertex;
	vec4 Position = in_Vertex;

   LocalPosition = Position.xyz;

   TexCoord0 = vec2( in_TexCoord );
   modelNormal = in_Normal;
   eyeNormal = mat3(in_NormalMatrix) * in_Normal;
   viewVec = -(in_ModelViewMatrix * Position).xyz;

   //ProjectedVertex = GetProjScaleBiasMat() * in_ModelViewProjectionMatrix * Position;

   //ShadowMapCoord = GetProjScaleBiasMat() * in_ShadowMatrix * in_ViewMatrixInverse * in_ModelViewMatrix * Position;

   gl_Position = in_ModelViewProjectionMatrix * Position;
}

/*FRAGMENT_PROGRAM*/

#include Layout.sp

in vec3 LocalPosition;
in vec2 TexCoord0;
in vec3 eyeNormal;
in vec3 modelNormal;
in vec3 viewVec;
in vec3 lightDir;
//in vec4 ProjectedVertex;
//in vec4 ShadowMapCoord;

uniform sampler2D   Texture0; // Packed skeleton
uniform sampler2D   Texture1; // Ambient
uniform sampler2D   Texture2; // Diffuse
uniform sampler2D   Texture3; // Specular
//uniform sampler2D   Texture4; // Bump
//uniform sampler3D   Texture5; // Noise volume
//#ifdef MATSYS_USE_REFLECTION_MAP
//uniform samplerCube Texture6; // Reflection map
//#endif
//uniform sampler2D   Texture7; // Planar reflection
//#ifdef MATSYS_PASS_NORMAL
//uniform sampler2D   Texture8; // Shadow map
//#endif

uniform float ENGINE_TIME;
uniform vec4 MTL_AMBIENT_COLOR;
uniform vec4 MTL_DIFFUSE_COLOR;
uniform vec4 MTL_SPECULAR_COLOR;
uniform float MTL_TRANSPARENCY;
uniform float MTL_REFLECTIVITY;
uniform float MTL_RECEIVESHADOW;
uniform float MTL_PLANARREFLECTION;
uniform float MTL_USEAMBIENTONLY;

out vec4 out_FragColor;

#include MaterialSystem/Params_~MATERIAL_CLASS_NAME.sp

#ifdef MATSYS_PASS_NORMAL
//   #_include MaterialSystem/ShaderLib/Shadows.sp
#endif

#include MaterialSystem/ShaderLib/Transparency.sp

vec3 GetNormal()
{
   vec3 LNormal = normalize( eyeNormal );

   return LNormal;
}

vec4 ComputeLight( vec3 LightDir, vec3 ViewVec, vec3 Normal, vec4 Ambient, vec4 Diffuse, vec4 Specular )
{
   float Kd = 0.5 * clamp( dot( LightDir, Normal ), 0.0, 1.0 ) + 0.5;

   vec3 ReflectionVec = reflect(-normalize(ViewVec), Normal );

   float Ks = pow(clamp(dot(ReflectionVec, LightDir.xyz), 0.0, 1.0), 24.0);

   return ( Ambient + vec4(Kd) ) * Diffuse + Specular * Ks;
}

uniform vec4  ClipPlane;
uniform float EnableClipPlane;

uniform vec3  in_LightPos;

#ifdef MATSYS_PASS_SHADOW
void main()
{
   vec4 FinalColor = vec4( 1.0, 1.0, 1.0, 1.0 );

   // alpha transparent shadow
   FinalColor.a = GetAlphaTransparency();

   out_FragColor = FinalColor;
}
#endif

#ifdef MATSYS_PASS_NORMAL
void main()
{
   vec4 WorldPos = in_ViewMatrixInverse * ( in_ModelViewMatrix * vec4(LocalPosition, 1.0) );

/*
#ifdef MATSYS_PASS_REFL
   if ( EnableClipPlane > 0.5 )
   {
      if ( dot( WorldPos, ClipPlane ) < 0.0 ) discard;
   }
#endif
*/
   vec3 EN = GetNormal();

   vec4 Ambient  = GetAmbient();
   vec4 Diffuse  = GetDiffuse();
   vec4 Specular = GetSpecular();

   vec3 lightDir = normalize( in_LightPos - WorldPos.xyz );

   vec4 FinalColor = Ambient;

	if ( MTL_USEAMBIENTONLY < 0.5 )
	{
		FinalColor = ComputeLight( lightDir, viewVec, EN, Ambient, Diffuse, Specular );
	}

/*
#ifdef MATSYS_USE_REFLECTION_MAP
   // reflection cube map is in world space
   mat3 EyeToWorld = mat3( transpose( in_ViewMatrix ) );

   vec3 Normal = EyeToWorld * EN;
   vec3 EyeDir = EyeToWorld * (-viewVec);

   // sample from cube map
   vec4 Reflection = textureCube( Texture6, reflect( EyeDir, Normal ).xzy );

   FinalColor += Reflection * MTL_REFLECTIVITY;
#endif
*/

/*
   // overlay shadow
   if ( MTL_RECEIVESHADOW > 0.5 )
   {
      // shadow map
      vec4 ShadowCoords4 = ShadowMapCoord / ShadowMapCoord.w;

      vec2 ShadowCoord = vec2( ShadowCoords4 );

      float ShadowSample = PCF5x5( ShadowCoord );

      // Isidoro-ShadowMapping.pdf
      // float DepthBias = - ( abs( dFdx( ShadowCoords4.z ) ) + abs( dFdy( ShadowCoords4.z ) ) + 0.0001 );
      float DepthBias = -0.0001;

      if ( ShadowCoords4.z + DepthBias >= ShadowSample ) FinalColor = FinalColor * vec4 (0.5);
   }

   if ( MTL_PLANARREFLECTION > 0.5 )
   {
      float Fresnel = abs( dot( normalize(viewVec), EN ) );

      FinalColor += textureProj( Texture7, ProjectedVertex ) * MTL_REFLECTIVITY * Fresnel;
   }
*/
#ifdef MATSYS_CUSTOM_MATERIAL
   FinalColor.a = GetCustomAlphaTransparency();
#else
   FinalColor.a = GetAlphaTransparency(); 
#endif

   out_FragColor = FinalColor;
}
#endif

/*
 * 15/03/2011
     Special version for GLSL ES 1.00
 * 11/09/2010
     ShowNormals mode
 * 30/04/2010
     Alpha transparency (w-channel of diffuse map)
 * 19/03/2010
     Skinning
 * 21/04/2009
     Light sources
 * 20/04/2009
     Uniforms for ambient, diffuse and specular
*/
