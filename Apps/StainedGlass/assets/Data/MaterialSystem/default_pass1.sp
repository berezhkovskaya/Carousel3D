/*VERTEX_PROGRAM*/

//
// Render to deferred buffers
//

#pragma debug(on)

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;
in vec3 in_Normal;
in vec2 in_Bones;

uniform mat4 in_ShadowMatrix;

out vec4 LocalPosition;
out vec2 TexCoord0;
out vec3 eyeNormal;
out vec3 modelNormal;

uniform float ENGINE_TIME;
uniform float Skinned;

uniform sampler2D Texture0; // packed skeleton

uniform vec4 KeyFrame;      // Frame1, Frame2, Lerp
#include MaterialSystem/ShaderLib/QuaternionLib.sp
#include MaterialSystem/ShaderLib/Skinning.sp

void main()
{
   vec4 Position = ( Skinned == 1.0 ) ? SkinVertex() : in_Vertex;
   LocalPosition = Position;

   gl_Position = in_ModelViewProjectionMatrix * Position;

   TexCoord0 = vec2( in_TexCoord );
   modelNormal = in_Normal;
   eyeNormal = mat3(in_NormalMatrix) * in_Normal;
}


/*FRAGMENT_PROGRAM*/

#include Layout.sp

in vec4 LocalPosition;
in vec2 TexCoord0;
in vec3 eyeNormal;
in vec3 modelNormal;

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

uniform vec4  PICKING_COLOR;

uniform sampler2D   Texture1; // Ambient
uniform sampler2D   Texture2; // Diffuse
uniform sampler2D   Texture3; // Specular
uniform sampler2D   Texture4; // Bump
uniform sampler3D   Texture5; // Noise volume
#ifdef MATSYS_USE_REFLECTION_MAP
uniform samplerCube Texture6; // Reflection map
#endif

#include MaterialSystem/ShaderLib/Transparency.sp

#include MaterialSystem/Params_~MATERIAL_CLASS_NAME.sp

/// For simple forward rendering we use the single output buffer
out vec4 out_FragColor; // r, g, b
out vec4 out_PickMask;
out vec4 out_Normals;   // nx, ny, nz
out vec4 out_WorldPos;  // x, y, z

void main()
{
	vec4 Ambient  = GetAmbient();
	vec4 Diffuse  = GetDiffuse();
	vec4 Specular = GetSpecular();

	vec4 WorldPos = in_ViewMatrixInverse * ( in_ModelViewMatrix * LocalPosition );

#ifdef MATSYS_USE_REFLECTION_MAP
	// reflection cube map is in world space

	vec3 viewVec = -(in_ModelViewMatrix * LocalPosition).xyz;

	mat3 EyeToWorld = mat3( transpose( in_ViewMatrix ) );

	vec3 Normal = EyeToWorld * eyeNormal;
	vec3 EyeDir = EyeToWorld * (-viewVec);
 
	// sample from cube map
	vec4 Reflection = texture( Texture6, reflect( EyeDir, Normal ).xzy );

	Diffuse += Reflection * MTL_REFLECTIVITY;
#endif

	out_FragColor = Ambient + Diffuse;
	out_PickMask  = PICKING_COLOR;
	out_Normals   = vec4( eyeNormal, 0.0 );
	out_WorldPos  = WorldPos;
}

/*
 * 02/10/2011
     It is here
*/
