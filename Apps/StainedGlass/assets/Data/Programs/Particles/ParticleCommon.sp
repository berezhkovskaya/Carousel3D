/* */

uniform float ENGINE_TIME;

/// Noise (for the random generator)
uniform sampler3D Texture3;

float RandomValue(vec3 TexCoord)
{
	float t = (sin(ENGINE_TIME) + 1.0) / 2.0;

	return textureLod(Texture3, vec3(TexCoord.xy, t), 0.0).x;
}

/** Uniform */
vec3 RandomVec3InBox(vec3 TexCoord, vec3 Min, vec3 Max)
{
	/// How to generate a uniform random sample ?

//	vec3 Noise = textureLod(Texture3, vec3(TexCoord, ENGINE_TIME), 0.0).xyz;

	float t = (sin(ENGINE_TIME) + 1.0) / 2.0;

	float Noise1 = textureLod(Texture3, vec3(TexCoord.xy, t), 0.0).x;
	float Noise2 = textureLod(Texture3, vec3(TexCoord.yx, t), 0.0).x;
	float Noise3 = textureLod(Texture3, vec3(TexCoord.xx, t), 0.0).x;

	vec3 Noise = vec3(Noise1, Noise2, Noise3);

	return Min + Noise * (Max - Min);

//	return Min + vec3(TexCoord, 0.0) * (Max - Min);
}
