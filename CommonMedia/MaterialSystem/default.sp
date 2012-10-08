/*TESSCONTROL_PROGRAM*/

#include default_tcs.sp

/*TESSEVAL_PROGRAM*/

#include default_tes.sp

/*VERTEX_PROGRAM*/

//
// All lighting is done in camera space
//

#pragma debug(on)

#include Layout.sp

const int MaxLights = 8;

in vec4 in_Vertex;
in vec4 in_TexCoord;
in vec3 in_Normal;
in vec2 in_Bones;

uniform mat4 in_ShadowMatrix;

out vec3 LocalPosition;
out vec2 TexCoord0;
out vec3 eyeNormal;
out vec3 modelNormal;
out vec3 viewVec;
// out vec3 lightDir[MaxLights]; // Yeah, I know. This must be calculated in vtx shader, not per-pixel. But things are now as they are
out vec4 ProjectedVertex;
out vec4 ShadowMapCoord;

#ifdef MATSYS_USE_BUMP_MAP || MATSYS_WATER_MATERIAL
   uniform sampler2D   Texture4; // Bump
#endif

uniform float ENGINE_TIME;
uniform float Skinned;

uniform sampler2D Texture0; // packed skeleton
uniform vec4 KeyFrame;      // Frame1, Frame2, Lerp
#include MaterialSystem/ShaderLib/QuaternionLib.sp
#include MaterialSystem/ShaderLib/Skinning.sp
#include MaterialSystem/ShaderLib/Projections.sp

void main()
{
   vec4 Position = ( Skinned == 1.0 ) ? SkinVertex() : in_Vertex;

#ifdef MATSYS_USE_BUMP_MAP || MATSYS_WATER_MATERIAL
   // Displacement mapping
   Position += 0.04 * vec4( in_Normal * textureLod(Texture4, vec2( in_TexCoord ), 0.0).x, 0.0 );
#endif

   LocalPosition = Position.xyz;

   gl_Position = in_ModelViewProjectionMatrix * Position;
   TexCoord0 = vec2( in_TexCoord );
   modelNormal = in_Normal;
   eyeNormal = mat3(in_NormalMatrix) * in_Normal;
   viewVec = -(in_ModelViewMatrix * Position).xyz;

   ProjectedVertex = GetProjScaleBiasMat() * in_ModelViewProjectionMatrix * Position;

   ShadowMapCoord = GetProjScaleBiasMat() * in_ShadowMatrix * in_ViewMatrixInverse * in_ModelViewMatrix * Position;
}

/*FRAGMENT_PROGRAM*/

//#define DISABLE_LIGHTS

#include Layout.sp

const int MaxLights = 8;

in vec3 LocalPosition;
in vec2 TexCoord0;
in vec3 eyeNormal;
in vec3 modelNormal;
in vec3 viewVec;
in vec4 ProjectedVertex;
in vec4 ShadowMapCoord;

uniform sampler2D   Texture0; // Packed skeleton
uniform sampler2D   Texture1; // Ambient
uniform sampler2D   Texture2; // Diffuse
uniform sampler2D   Texture3; // Specular
uniform sampler2D   Texture4; // Bump
uniform sampler3D   Texture5; // Noise volume
#ifdef MATSYS_USE_REFLECTION_MAP
uniform samplerCube Texture6; // Reflection map
#endif
uniform sampler2D   Texture7; // Planar reflection
#ifdef MATSYS_PASS_NORMAL
uniform sampler2D   Texture8; // Shadow map
#endif

// LV: I don't want to break the map order, so I just add another one at the end
#ifdef MATSYS_USE_REFRACTION_MAP
uniform sampler2D   Texture9; // Refraction map. It is just a scee, rendered with different matrix. Or am I mistaken ?
#endif

/// Globaly available engine time
uniform float ENGINE_TIME;

uniform vec4  MTL_AMBIENT_COLOR;
uniform vec4  MTL_DIFFUSE_COLOR;
uniform vec4  MTL_SPECULAR_COLOR;
uniform float MTL_TRANSPARENCY;
uniform float MTL_REFLECTIVITY;
uniform float MTL_RECEIVESHADOW;
uniform float MTL_PLANARREFLECTION;
uniform float MTL_USEAMBIENTONLY;
uniform float MTL_SHININESS;

/// Refraction index, depends on two media, not a single surface
// uniform float MTL_REFRACTIVITY;

/// Cook-Torrance roughness coefficient
uniform float MTL_COOK_TORRANCE_ROUGHNESS;

uniform vec4  PICKING_COLOR;

/// Global switch for the lighting model type: old - 0, new (4-type lighting: spot, point, dir, ambient) - 1
uniform float LIGHTING_MODEL;

/// For simple forward rendering we use the single output buffer
out vec4 out_FragColor;

#ifdef MATSYS_PASS_NORMAL
out vec4 out_PickMask;
#endif

#include MaterialSystem/ShaderLib/Transparency.sp

#include MaterialSystem/Params_~MATERIAL_CLASS_NAME.sp

#ifdef MATSYS_PASS_NORMAL
   #include MaterialSystem/ShaderLib/Shadows.sp
#endif

vec4 WorldPos;

vec3 GetNormal()
{
	vec3 LNormal = normalize( eyeNormal );

#ifdef MATSYS_USE_BUMP_MAP
	LNormal = normalize( LNormal + ( 2.0 * texture(Texture4, TexCoord0).xyz - vec3( 1.0 ) ) );
#endif

	return LNormal;
}

uniform vec4  ClipPlane;
uniform float EnableClipPlane;
uniform float ShowNormals;

//#ifdef MATSYS_PASS_NORMAL

#include "Lights.sp"

//#endif


#ifdef MATSYS_PASS_SHADOW
void main()
{
	// alpha transparent shadow
	out_FragColor = vec4( 1.0, 1.0, 1.0, GetAlphaTransparency() );
}
#endif

#ifdef MATSYS_PASS_NORMAL

void main()
{
	WorldPos = in_ViewMatrixInverse * ( in_ModelViewMatrix * vec4(LocalPosition, 1.0) );

#ifdef MATSYS_PASS_REFL
	if ( EnableClipPlane > 0.5 )
	{
		if ( dot( WorldPos, ClipPlane ) < 0.0 ) discard;
	}
#endif

	vec3 EN = GetNormal();

	vec4 FinalColor;

	if(LIGHTING_MODEL > 0.5)
	{
		// LV: new version
		/// Convert normal to world space
		FinalColor = ComputeActiveLighting( mat3( transpose( in_ViewMatrix ) ) * EN);
	}
	else
	{
		/////// Old stuff
		vec4 Ambient  = GetAmbient();
		vec4 Diffuse  = GetDiffuse();
		vec4 Specular = GetSpecular();

		vec3 lightDir = normalize( vec3(-10, -4, -18) - WorldPos.xyz );

		FinalColor = Ambient;

		if ( MTL_USEAMBIENTONLY < 0.5 )
		{
			FinalColor = ComputeLight( lightDir, viewVec, EN, Ambient, Diffuse, Specular );
		}
	}

#ifdef MATSYS_USE_REFLECTION_MAP
	// reflection cube map is in world space
	mat3 EyeToWorld = mat3( transpose( in_ViewMatrix ) );

	vec3 Normal = EyeToWorld * EN;
	vec3 EyeDir = EyeToWorld * (-viewVec);
 
	// sample from cube map
	vec4 Reflection = texture( Texture6, reflect( EyeDir, Normal ).xzy );

	FinalColor += Reflection * MTL_REFLECTIVITY;
#endif

#ifdef MATSYS_USE_REFRACTION_MAP

	// sample from another cubemap. TODO: write refraction() function
	vec4 Refraction = texture( Texture9, refract(EyeDir, Normal) );

	FinalColor += Refraction * MTL_REFRACTIVITY;
#endif

	// overlay normals
	if ( ShowNormals > 0.5 ) FinalColor = vec4( 0.5*( EN + vec3(1.0) ), 0.0 );

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

	FinalColor.a =
#ifdef MATSYS_CUSTOM_MATERIAL
	GetCustomAlphaTransparency(); 
#else
	GetAlphaTransparency(); 
#endif

	out_FragColor = FinalColor;
	out_PickMask  = PICKING_COLOR;
}
#endif

/*
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
