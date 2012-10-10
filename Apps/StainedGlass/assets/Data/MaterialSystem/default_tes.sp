//

layout(triangles, equal_spacing) in;

// comes from tesselation control program
in vec3 oLocalPosition[];
in vec2 oTexCoord0[];
in vec3 oeyeNormal[];
in vec3 omodelNormal[];
in vec3 oviewVec[];
in vec4 oProjectedVertex[];
in vec4 oShadowMapCoord[];

// goes to fragment program
out vec3 LocalPosition;
out vec2 TexCoord0;
out vec3 eyeNormal;
out vec3 modelNormal;
out vec3 viewVec;
out vec4 ProjectedVertex;
out vec4 ShadowMapCoord;

uniform sampler2D Texture4; // heightmap
uniform float ShapeFactor;

const float DisplacementStrength = 0.4;

vec3 project(vec3 p, vec3 c, vec3 n)
{
	return p - dot(p - c, n) * n;
}

vec3 PhongGeometry(float u, float v, float w)
{
   vec3 p0 = gl_in[0].gl_Position.xyz;
   vec3 p1 = gl_in[1].gl_Position.xyz;
   vec3 p2 = gl_in[2].gl_Position.xyz;
	vec3 n0 = normalize(oeyeNormal[0]);
	vec3 n1 = normalize(oeyeNormal[1]);
	vec3 n2 = normalize(oeyeNormal[2]);
	//
	vec3 p = u * p0 + v * p1 + w * p2;
	vec3 c0 = project(p, p0, n0);
	vec3 c1 = project(p, p1, n1);
	vec3 c2 = project(p, p2, n2);
	vec3 q = u * c0 + v * c1 + w * c2;
	vec3 r = mix(p, q, ShapeFactor);
	return r;
}

vec3 PhongNormal(float u, float v, float w)
{
	vec3 n0 = oeyeNormal[0];
	vec3 n1 = oeyeNormal[1];
	vec3 n2 = oeyeNormal[2];
	//
	return normalize(u * n0 + v * n1 + w * n2);
}

// www.gris.informatik.tu-darmstadt.de/~thokalbe/WS0607/seminar/files/development_pn_triangles_dasbach.pdf
vec3 PNGeometry(float u, float v, float w)
{
	vec3 P1 = gl_in[0].gl_Position.xyz;
	vec3 P2 = gl_in[1].gl_Position.xyz;
	vec3 P3 = gl_in[2].gl_Position.xyz;
	vec3 N1 = normalize(oeyeNormal[0]);
	vec3 N2 = normalize(oeyeNormal[1]);
	vec3 N3 = normalize(oeyeNormal[2]);

	// precalculate temporaries w_ij = (Pj-Pi) dot Ni, (see p.5)
	float w12 = dot( P2 - P1, N1 );
	float w23 = dot( P3 - P2, N2 );
	float w31 = dot( P1 - P3, N3 );

	float w21 = dot( P1 - P2, N2 );
	float w32 = dot( P2 - P3, N3 );
	float w13 = dot( P3 - P1, N1 );

	// construct a Bezier patch control points b_ijk
	vec3 b300 = P1;
	vec3 b030 = P2;
	vec3 b003 = P3;

	vec3 b210 = ( 2.0 * P1 + P2 - w12 * N1 ) / 3.0;
	vec3 b021 = ( 2.0 * P2 + P3 - w23 * N2 ) / 3.0;
	vec3 b201 = ( 2.0 * P3 + P1 - w31 * N3 ) / 3.0;

	vec3 b120 = ( 2.0 * P2 + P1 - w21 * N2 ) / 3.0;
	vec3 b012 = ( 2.0 * P3 + P2 - w32 * N3 ) / 3.0;
	vec3 b102 = ( 2.0 * P1 + P3 - w13 * N1 ) / 3.0;

	vec3 E = (b210 + b120 + b021 + b012 + b201 + b102) / 6.0;

	vec3 V = (P1 + P2 + P3) / 3.0;

	vec3 b111 = E + (E - V) / 2.0;

	// calculate the Bezier patch
	vec3 PNPos = b300*w*w*w + b030*u*u*u + b003*v*v*v + b210*3.0*w*w*u + b120*3.0*w*u*u + b201*3.0*w*w*v + b021*3.0*u*u*v + b102*3.0*w*v*v + b012*3.0*u*v*v + b111*6.0*w*u*v;

	vec3 p = u * P1 + v * P2 + w * P3;

	return mix(p, PNPos, ShapeFactor);
}

vec3 PNNormal(float u, float v, float w)
{
	vec3 P1 = gl_in[0].gl_Position.xyz;
	vec3 P2 = gl_in[1].gl_Position.xyz;
	vec3 P3 = gl_in[2].gl_Position.xyz;
	vec3 N1 = normalize(oeyeNormal[0]);
	vec3 N2 = normalize(oeyeNormal[1]);
	vec3 N3 = normalize(oeyeNormal[2]);

	vec3 n200 = N1;
	vec3 n020 = N2;
	vec3 n002 = N3;

	// v_ij = 2 * [ (Pj-Pi) dot (Ni+Nj) ] / [ (Pj-Pi) dot (Pj-Pi) ]
	float v12 = 2.0 * dot(P2-P1,N1+N2) / dot(P2-P1, P2-P1);
	float v23 = 2.0 * dot(P3-P2,N2+N3) / dot(P3-P2, P3-P2);
	float v31 = 2.0 * dot(P1-P3,N3+N1) / dot(P1-P3, P1-P3);

	vec3 h110 = N1+N2 - v12*(P2-P1);
	vec3 h011 = N2+N3 - v23*(P3-P2);
	vec3 h101 = N3+N1 - v31*(P1-P3);

	vec3 n110 = normalize(h110);
	vec3 n011 = normalize(h011);
	vec3 n101 = normalize(h101);

	return n200*w*w + n020*u*u + n002*v*v + n110*w*u + n011*u*v + n101*w*v;
}

void main(void)
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;

	vec4 Vertex = u * gl_in[0].gl_Position + v * gl_in[1].gl_Position + w * gl_in[2].gl_Position;

	LocalPosition = u * oLocalPosition[0] + v * oLocalPosition[1] + w * oLocalPosition[2];
	TexCoord0     = u * oTexCoord0[0]     + v * oTexCoord0[1]     + w * oTexCoord0[2];
//	eyeNormal     = normalize( u * oeyeNormal[0]   + v * oeyeNormal[1]   + w * oeyeNormal[2] );
	eyeNormal     = PhongNormal(u, v, w);
//	eyeNormal     = PNNormal(u, v, w);
	modelNormal   = normalize( u * omodelNormal[0] + v * omodelNormal[1] + w * omodelNormal[2] );
	viewVec       = normalize( u * oviewVec[0]     + v * oviewVec[1]     + w * oviewVec[2] );
	ProjectedVertex = u * oProjectedVertex[0] + v * oProjectedVertex[1] + w * oProjectedVertex[2];
	ShadowMapCoord  = u * oShadowMapCoord[0]  + v * oShadowMapCoord[1]  + w * oShadowMapCoord[2];

	vec3 Position = PhongGeometry(u, v, w);

#ifdef MATSYS_USE_BUMP_MAP
   // Displacement mapping
   Position += DisplacementStrength * eyeNormal * textureLod(Texture4, vec2( TexCoord0 ), 0.0).x;
#endif

	gl_Position = vec4( Position, Vertex.w );

//	gl_Position = vec4( PhongGeometry(u, v, w), Vertex.w );
//	gl_Position = vec4( PNGeometry(u, v, w), Vertex.w );
}
