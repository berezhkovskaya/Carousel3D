/**
	\brief Rendering of a single colored particle

	\author Viktor Latypov
*/

/*VERTEX_PROGRAM*/

/**
	Vertex shader converts each quadruple of vertices to appropriate billboard corners

	(v1,v2,v3,v4) contain the same coordinates (particle center)
	and are transformed to v[i] + VtxOfs(i),
	where VtxOfs is the offset of the corner

	This technique does not require geometry shaders,
	but, of cource, it uses 4 times more memory for the vertices

	If the lifetime is elapsed, we just output a single point
*/

#include Layout.sp

/// Default particle color
uniform vec4 ParticleColor;

/// Size of the billboard
uniform float ParticleSize;

in vec4 in_Vertex;
in vec4 in_TexCoord;

/// Calculated particle color (TODO: use texture ?)
out vec4 Color;

/// Billboard vertex coordinates
out vec4 OutPos;

/// First texture is the particle position container
uniform sampler2D Texture0;

/// Maximum lifetime of a particle (only positive values matter)
uniform float MaxLifeTime;

/**
	\brief Decode floating point index of a vertex to (0,1,2,3) range

	in_TexCoord.z is the local vertex index : 0.0 - 1, 0.25 - 2, 0.5 - 3, 0.75 - 4
*/
int VtxIndex(float idx)
{
	if(idx < 0.2) return 0;
	if(idx < 0.4) return 1;
	if(idx < 0.7) return 2;

	return 3;
}

/**
	\brief Calculate vertex coordinate inside a billboard
*/
vec4 VtxOfs(float idx)
{
	if(idx < 0.2) return vec4(-1.0, -1.0, 0.0, 0.0);
	if(idx < 0.4) return vec4(+1.0, -1.0, 0.0, 0.0);
	if(idx < 0.7) return vec4(+1.0, +1.0, 0.0, 0.0);

	return vec4(-1.0, +1.0, 0.0, 0.0);
}

void main()
{
	/// Just output the color
	Color = ParticleColor;

	/// Fetch complete particle state
	vec4 State = textureLod(Texture0, in_TexCoord.xy, 0.0);

	/// Fetch particle center from position texture
	vec3 ParticlePos = State.xyz;

	if(MaxLifeTime > 0)
	{
		if(State.w > MaxLifeTime)
		{
			/// The particle is inactive, do not create billboard, leave the point
			OutPos = vec4(0,0,0,0);

			/// Output final particle position
			gl_Position =  in_ModelViewProjectionMatrix * vec4( ParticlePos, 1.0 );

			return;
		}
	}

	/// Two vectors parallel to the screen plane
	vec3 X = ParticleSize * vec3( in_ModelViewMatrix[0][0], in_ModelViewMatrix[1][0], in_ModelViewMatrix[2][0] );
	vec3 Y = ParticleSize * vec3( in_ModelViewMatrix[0][1], in_ModelViewMatrix[1][1], in_ModelViewMatrix[2][1] );

	/// Convert float to int index
	int idx = VtxIndex(in_TexCoord.z);

	/// Add offsets of the corners for the particle billboard
	if ( idx == 0 ) ParticlePos += (-X - Y);
	if ( idx == 1 ) ParticlePos += (+X - Y);
	if ( idx == 2 ) ParticlePos += (+X + Y);
	if ( idx == 3 ) ParticlePos += (-X + Y);

	/// Local coordinate in the billboard [0..1]x[0..1]
	OutPos = VtxOfs(in_TexCoord.z);

	/// Output final particle position
	gl_Position =  in_ModelViewProjectionMatrix * vec4( ParticlePos, 1.0 );
}

/*FRAGMENT_PROGRAM*/

/// Particle color (TODO: replace with some texture)
in vec4 Color;

/// Calculated/interpolated pixel position in billboard coordinates
in vec4 OutPos;

/// Fragment color
out vec4 out_FragColor;

void main()
{
	/// Calculate transparency factor
	float OutAlpha = 0.5 * abs( (1.0-abs(OutPos.x)) * (1.0-abs(OutPos.y)));

	/// Dump the color (TODO: use texture ?)
	out_FragColor = vec4(Color.r, Color.g, Color.b, OutAlpha);
}
