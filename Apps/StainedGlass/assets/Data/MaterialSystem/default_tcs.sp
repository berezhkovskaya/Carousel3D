//

uniform mat4 in_ModelViewProjectionMatrix;

uniform float MTL_TESSINNERLEVEL;
uniform float MTL_TESSOUTERLEVEL;

layout(vertices = 3) out;

// comes from vertex program
in vec3 LocalPosition[];
in vec2 TexCoord0[];
in vec3 eyeNormal[];
in vec3 modelNormal[];
in vec3 viewVec[];
in vec4 ProjectedVertex[];
in vec4 ShadowMapCoord[];

// goes to tesselation evaluation program
out vec3 oLocalPosition[];
out vec2 oTexCoord0[];
out vec3 oeyeNormal[];
out vec3 omodelNormal[];
out vec3 oviewVec[];
out vec4 oProjectedVertex[];
out vec4 oShadowMapCoord[];

const float LodFactor = 1.0;

vec4 Project( vec4 v )
{
	vec4 R = in_ModelViewProjectionMatrix*v;
	return R / R.w;
}

void main(void)
{
	gl_TessLevelInner [0] = MTL_TESSINNERLEVEL;

	gl_TessLevelOuter [0] = MTL_TESSOUTERLEVEL;
	gl_TessLevelOuter [1] = MTL_TESSOUTERLEVEL;
	gl_TessLevelOuter [2] = MTL_TESSOUTERLEVEL;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	oLocalPosition[gl_InvocationID] = LocalPosition[gl_InvocationID];
	oTexCoord0[gl_InvocationID] = TexCoord0[gl_InvocationID];
	oeyeNormal[gl_InvocationID] = eyeNormal[gl_InvocationID];
	omodelNormal[gl_InvocationID] = modelNormal[gl_InvocationID];
	oviewVec[gl_InvocationID] = viewVec[gl_InvocationID];
	oProjectedVertex[gl_InvocationID] = ProjectedVertex[gl_InvocationID];
	oShadowMapCoord[gl_InvocationID] = ShadowMapCoord[gl_InvocationID];
}
