/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec4 RectPos;
uniform vec4 RectTiles;
uniform vec4 RectUV;

out vec2 TexCoord;

void main()
{
   float X1 = RectPos.x;
   float Y1 = RectPos.y;
   float X2 = RectPos.z;
   float Y2 = RectPos.w;

   float Width  = X2 - X1;
   float Height = Y2 - Y1;

   vec4 VertexPos = vec4( X1 + in_Vertex.x * Width,
                          Y1 + in_Vertex.y * Height,
                          in_Vertex.z, in_Vertex.w );

   gl_Position = in_ModelViewProjectionMatrix * VertexPos;

	float U0 = RectUV.x;
	float V0 = RectUV.y;
	float U1 = RectUV.z;
	float V1 = RectUV.w;

	float u = ( 1.0 - in_Vertex.x ) * U0 + ( in_Vertex.x ) * U1;
	float v = ( 1.0 - in_Vertex.y ) * V0 + ( in_Vertex.y ) * V1;

//	TexCoord = vec2( in_TexCoord );// * RectTiles.xy;

	TexCoord = vec2( u, v ) * RectTiles.xy;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;
uniform vec4 RectColor;

out vec4 out_FragColor;

void main()
{
   out_FragColor = texture( Texture0, TexCoord ) * RectColor;
//   out_FragColor = vec4( TexCoord.xy, 0.0, 1.0 );
}
