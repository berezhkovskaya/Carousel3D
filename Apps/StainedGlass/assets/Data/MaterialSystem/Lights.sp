/** */

uniform float in_NumLights;

/// Type of the light: Spot, Point or Directional
uniform float in_LightType[ 8 /* MaxLights */ ];

uniform float in_LightEnabled[ 8 /* MaxLights */ ];

/// k0, k1, k2 in attenuation formula (const, linear, quadratic)
uniform vec3 in_LightAttenuation[ 8 /* MaxLights */ ];

uniform vec4 in_LightAmbientColor[ 8 /* MaxLights */ ];
uniform vec4 in_LightDiffuseColor[ 8 /* MaxLights */ ];
uniform vec4 in_LightSpecularColor[ 8 /* MaxLights */ ];

/// For spot lights the First axis in matrix is the SpotDirection
/// For directional/point lights the transform is useless in light calculations.
/// Light transform is always used in ShadowMapping calculations (it is set in ModelViewProj, however)
uniform mat4 in_LightTransform[ 8 /* MaxLights */ ];

/// Spot light "main" direction of the light cone
uniform vec3  in_LightSpotDirections[ 8 /* MaxLights */ ];

/// X is the Spot CosCutoff, Y is the Spot Exponent in Blinn-Phong model
uniform vec3  in_LightSpotParameters[ 8 /* MaxLights */ ];

/// LV: Some black-magic for the unknown old lighting model
vec4 ComputeLight( vec3 LightDir, vec3 ViewVec, vec3 Normal, vec4 Ambient, vec4 Diffuse, vec4 Specular )
{
   float Kd = 0.5 * clamp( dot( LightDir, Normal ), 0.0, 1.0 ) + 0.5;

   vec3 ReflectionVec = reflect(-normalize(ViewVec), Normal );

   float Ks = pow(clamp(dot(ReflectionVec, LightDir.xyz), 0.0, 1.0), 24.0); // 24 is shininess

   return ( Ambient + vec4(Kd) ) * Diffuse + Specular * Ks;
}


////  Lighting computations

/// Calculate 'c0 + c1 * Dist + c2 * Dist * Dist' polynomial to get the attenuation factor
float AttenuationForLight(int LightIdx, float DistToLight)
{
	float constAtt = in_LightAttenuation[LightIdx].x;
	float linAtt   = in_LightAttenuation[LightIdx].y;
	float quadAtt  = in_LightAttenuation[LightIdx].z;

	return 1.0 / (constAtt + (linAtt + quadAtt * DistToLight) * DistToLight);
}

/// Helper for Cook-Torrance specular highlight
float CookTorranceCoefficient(float NdotL, float NdotV, float NdotH, float VdotH)
{
	const float SmallValue = 1.0e-07;

	/// Geometric attenuation
	float G = 2.0 * NdotH / VdotH;
	G = min( 1.0, G * min(NdotV, NdotL) );

	/// Surface roughness squared, used multiple times
	float R2 = MTL_COOK_TORRANCE_ROUGHNESS * MTL_COOK_TORRANCE_ROUGHNESS;

	float NdotH2  = NdotH * NdotH;
	float NdotH2i = 1.0 / (NdotH2 * R2);

	float ExpVar  = (NdotH2 - 1.0) * NdotH2i;

	/// Roughness factor (Beckmann distribution)
	float R = 0.25 * exp(ExpVar) * NdotH2 / NdotH2;

	/// Really rough approximation for Fresnel's term (Schlick's variations)
	float F = 1.0 / (1.0 + NdotV);

	/// Final Cook-Torrance term
	return min( 1.0, (F * G * R ) / (NdotV * NdotL + SmallValue) );
}

///// Actual light computations

vec4 GetGlobalAmbient()
{
	// TODO: fetch from uniform, like ENGINE_TIME
	return vec4(0.05, 0.05, 0.05, 0.0);
}

/**
	\brief Calculate total lighting from all the active light sources using Normal as a normal for the surface being lit

	Normal is in the World Space (!)

		- E is the vector to the camera or eye
		- H is the half-angle vector
		- L is the vector to the light source
		- N is the normal vector

	No shadow or global illumination calculations are there.

	Each light has a type: Spot (0), Point (1) or directional (2). The types are stored in in_LightType array.

	Light "activity" (is it enabled or disabled) is specified by the in_LightEnabled[] array.

	The transformation from world to light space is given in in_LightTransform array.

	Each light has three color properties: ambient, diffuse and specular, stored
	in the arrays in_LightAmbientColor[], in_LightDiffuseColor[] and in_LightSpecularColor[].

	Spot and Point light types support FP-like attenuations. The const/linear/quadratic
	attenuation coefficients are given in the in_LightAttenuation[] array.

	All three light types are handled in one cycle body simultaneously,
	because they are basically the same. For each light
	the diffuse, specular and ambient components are calculated.

	a) Diffuse lighting

	The diffuse term is calculated using simple Lambert's law for each point
	Id = LightDiff * MDiff * Cos ( Theta )

	LightDiff - light diffuse property
	MDiff - material diffuse property

	Theta is the angle between surface normal and the direction from point to the viewer.

	b) Ambient lighting is a simple product: Ia = LightAmbient * MAmbient

	c) Specular lighting

	// "Half-vector"
	H = Eye - L

	// Specular component (Blinn-Phong)
	Spec = LightSpec * MSpec * k

	Blinn-Phong:
		k = (N . H) ^ s

	Cook-Torrance (if the MTL_COOK_TORRANCE_ROUGHNESS is more than zero):

		k = D * F * G  / ( E . N)

		D =  exp(-tan^2(alpha) / (m * m) ) / (pi * m * m * cos^4(alpha) )    -  Beckmann distribution, m is Roughness

		alpha = arccos( N . H)

		Of course, tan^2 = (1-cos^2) / cos^2

		F = (1 + E.N ) ^ lambda

		Geometric attenuation

		G = min(1, 2 * (H.N) * (E.N) / (E.H), 2 * (H.N) * (L.N) / (E.H) )

		F is approximated by F0 + (1-F0)(1-cos(Theta))
		Theta - reflection angle, cos(Theta) = E.N (??)

		Finally, F is just 1 / ( 1 + (N.V) )

	1) Directional light is completely determined by its direction
	   which is encoded as the light position (literaly, in_LightTransform[i][3].xyz)

	2) Point light (or omni-directional) is specified by the transformation matrix

	3) Spot light has position, direction and CosCutoff/SpotExponent parameters

*/
vec4 ComputeActiveLighting(vec3 Normal)
{
	float NdotL = 0.0;
	float NdotH = 0.0;
	float NdotV = 0.0; // used in CT
	float VdotH = 0.0; // used in CT
	float dist, att;
	float spotEffect;
	float shineFactor;

	/// Extract eye position from camera transform
	vec3 eyePos = -in_ViewMatrix[3].xyz/in_ViewMatrix[3].w;

	/// Temp vars for calculated light contriution components
	vec4 dif, amb, spec;

	/// Read Ambient, Diffuse and Specular material properties
	vec4 Ambient  = GetAmbient();

	/// Check if the material is "ambient-only"
	if ( MTL_USEAMBIENTONLY > 0.5 ) return Ambient;

	vec4 Diffuse  = GetDiffuse();
	vec4 Specular = GetSpecular();

	vec4 FinalColor = vec4(0); //Ambient;

	vec4 LightSpec = vec4(0);

	int NL = int(in_NumLights);

	/// "Light everything out" to avoid errors with incorrect number of lights
	if(in_NumLights < 0 || in_NumLights > 8) return vec4(1,1,1,1);

	vec3 lightPos, lightDir, spotDirection;
	mat4 lightMtx;

	/// Half-vector (Eye - LightDir)
	vec3 H;

	/// Add contribution to the FinalColor from each light
	for(int i = 0 ; i < NL ; i++)
	{
		if(in_LightEnabled[i] < 0.5) continue;

		shineFactor = 0.0;

		/// Cached light transform
		lightMtx = in_LightTransform[i];

		/// Light position in world space
		lightPos = lightMtx[3].xyz /* / lightMtx[i].w */;

		/// Specific parameter for the spot light
		spotDirection = in_LightSpotDirections[i];

		att = 0.0;

		/// Handle spot and point lights
		/// For spot and point lights we use attenuation
		if(in_LightType[i] != 2 )
		{
			/// Direction from point to the light (for spots and point lights only, directionals are handled below)
			lightDir = normalize( lightPos - WorldPos.xyz );

			NdotL = clamp(dot(Normal, lightDir), 0.0, 1.0);

			/// Check if the light is really reflected to the viewer
			if(NdotL > 0.0)
			{
				dist = sqrt(dot( lightPos, WorldPos.xyz));

				att = AttenuationForLight(i, dist);

				/// Spot light is clamped to the cone region
				if(in_LightType[i] == 0)
				{
					spotEffect = dot(normalize( spotDirection ), normalize(-lightDir));

					if (spotEffect > in_LightSpotParameters[i].x)
					{
						spotEffect = pow(spotEffect, in_LightSpotParameters[i].y);
					} else
					{
						spotEffect = 0.0;
					}

					// clamp() spotEffect ?
					att *= spotEffect;
				}

			}
		} else
		{
			/// Directional light only uses the directional
			lightDir = normalize( lightPos );

			NdotL = clamp(dot(Normal, lightDir), 0.0, 1.0);

			att = 1.0;
		}

		spec = vec4(0);
		dif  = vec4(0);

		amb = Ambient * in_LightAmbientColor[i];

		H = vec3(0);

		/// Compute the specular term if NdotL is larger than zero
		if (NdotL > 0.0 && att > 0.0)
		{
			/// CHECK: do we use it correctly or is the V vector's sign screwed up ?
			vec3 V = normalize(eyePos - WorldPos.xyz);

			H = (V - lightDir);

			NdotH = clamp(dot(Normal, H), 0.0, 1.0);

			/// Blinn-Phong approximation
			if(MTL_SHININESS >= 0.0)
				shineFactor = pow(NdotH, MTL_SHININESS);

/** TODO: uncomment when done with uniforms
			if(MTL_COOK_TORRANCE_ROUGHNESS > 0.0)
			{
				/// Cook-Torrance model
				NdotV = max(0.0, dot(Normal, V));
				VdotH = max(0.0, dot(V, H) );
				shineFactor = CookTorrance(NdotL, NdotV, NdotH, VdotH);
			}
*/

			spec = att * Specular * shineFactor * in_LightSpecularColor[i];

			dif = NdotL * Diffuse * in_LightDiffuseColor[i];
		}

		/// Add light contribution: Diffuse, Ambient and Specular components
		if(NdotL > 0.0)
		{
			FinalColor = clamp(FinalColor + dif, 0.0, 1.0);
			FinalColor = clamp(FinalColor + spec, 0.0, 1.0);
		}

		/// Problems with ambient, I don't know yet what to do. Looks like something is not initialized here
		if(FinalColor.r < 1.0) FinalColor.r = clamp(FinalColor.r + amb.r, 0.0, 1.0);
		if(FinalColor.g < 1.0) FinalColor.g = clamp(FinalColor.g + amb.g, 0.0, 1.0);
		if(FinalColor.b < 1.0) FinalColor.b = clamp(FinalColor.b + amb.b, 0.0, 1.0);

		// This line produces some eally strange results, that's why it is converted to three lines above
//		FinalColor = clamp(FinalColor + amb, 0.0, 1.0);

		/** The following are the debug lines to see each calculated entity in this shader */
//		FinalColor = 
//		vec4( vec3(normalize( lightDir ) ), 1.0);
//		vec4( vec3(normalize( lightPos.xyz )), 1.0);
//		vec4( vec3(normalize( eyePos.xyz )), 1.0);
//		vec4( vec3(normalize( WorldPos.xyz )), 1.0);
//		vec4(NdotL, NdotL, NdotL, 1.0); // This is correct if the Normal is in WorldSpace
//		vec4(NdotH, NdotH, NdotH, 1.0);
//		vec4(H, 1.0);
//		vec4(spotEffect, 0.0, 0.0, 1.0);
//		vec4(att, att, att, 1.0);
//		vec4(shineFactor, shineFactor, shineFactor, 1.0);
//		LightSpec;
//		vec4(vec3(normalize(spotDirection)), 1.0);
//		dif;
//		spec + Ambient;
//		in_LightAmbientColor[i];
//		amb;
	}

	/// Add global ambient light
	return FinalColor; // + GetGlobalAmbient();
}
