/*VERTEX_PROGRAM*/

//
// Apply lighting
//

#pragma debug(on)

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord0;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
   TexCoord0 = vec2( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

#include Layout.sp

in vec2 TexCoord0;

uniform sampler2D   Texture0; // Diffuse
uniform sampler2D   Texture1; // Picking
uniform sampler2D   Texture2; // Normals
uniform sampler2D   Texture3; // Positions


/// Globaly available engine time
uniform float ENGINE_TIME;

/// For simple forward rendering we use the single output buffer
out vec4 out_FragColor;
out vec4 out_PickMask;

vec4 ComputeLight( vec3 LightDir, vec3 ViewVec, vec3 Normal, vec4 Ambient, vec4 Diffuse, vec4 Specular )
{
   float Kd = 0.5 * clamp( dot( LightDir, Normal ), 0.0, 1.0 ) + 0.5;

   vec3 ReflectionVec = reflect(-normalize(ViewVec), Normal );

   float Ks = pow(clamp(dot(ReflectionVec, LightDir.xyz), 0.0, 1.0), 24.0); // 24 is shininess

   return ( Ambient + vec4(Kd) ) * Diffuse + Specular * Ks;
}

void main()
{
	vec3 WorldPos = texture( Texture3, TexCoord0 ).xyz;
	vec3 Normal   = normalize( texture( Texture2, TexCoord0 ).xyz );

	vec4 Diffuse  = texture( Texture0, TexCoord0 );

	vec3 lightDir = normalize( vec3(-10, -4, -18) - WorldPos.xyz );
	vec3 viewVec  = normalize( WorldPos );

	out_FragColor = ComputeLight( lightDir, viewVec, Normal, vec4(0), Diffuse, vec4(0) );
	out_PickMask  = texture( Texture1, TexCoord0 );
}

/*
 * 02/10/2011
     It is here
*/
