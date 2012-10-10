/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;
in vec4 in_Color;

//uniform vec4 TextPosSize;
//uniform vec4 TextTexCoords;

out vec2 TexCoord;
out vec4 Color;

void main()
{
//	float X = TextPosSize.x;
//	float Y = TextPosSize.y;
//	float W = TextPosSize.z;
//	float H = TextPosSize.w;
/*
 
	vec4 VertexPos = vec4( X + in_Vertex.x * W, Y + in_Vertex.y * H, in_Vertex.z, in_Vertex.w );
*/
//	vec4 VertexPos = vec4( X + in_Vertex.x, Y + in_Vertex.y, in_Vertex.z, in_Vertex.w );

	vec4 VertexPos = in_Vertex;

	gl_Position = in_ModelViewProjectionMatrix * VertexPos;

//	TexCoord = in_TexCoord.xy * TextTexCoords.xy;
	TexCoord = in_TexCoord.xy;
	Color    = in_Color;
}

/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0;

//uniform vec4 TextColor;

in vec2 TexCoord;
in vec4 Color;

out vec4 out_FragColor;

void main()
{
	float Outline = texture( Texture0, TexCoord ).r;

	vec4 TextColor = Color;

	out_FragColor = vec4( TextColor.rgb, Outline * TextColor.a );
}
