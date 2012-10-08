/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec4 GUI_Pos;

out vec2 TexCoord;

void main()
{
   float X1 = GUI_Pos.x;
   float Y1 = GUI_Pos.y;
   float X2 = GUI_Pos.z;
   float Y2 = GUI_Pos.w;

   float Width  = X2 - X1;
   float Height = Y2 - Y1;

   vec4 VertexPos = vec4( X1 + in_Vertex.x * Width,
                          Y1 + in_Vertex.y * Height,
                          in_Vertex.z, in_Vertex.w );

   gl_Position = in_ModelViewProjectionMatrix * VertexPos;

   TexCoord = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0;
uniform sampler2D Texture1;

in vec2 TexCoord;

uniform float ENGINE_TIME;

out vec4 out_FragColor;

void main()
{
   vec4 Color1 = 0.45 * texture( Texture0, TexCoord );
//   vec4 Color2 = texture( Texture1, TexCoord0 + ENGINE_TIME * vec2( -0.05 , -0.04 ) );

   Color1.a = 0.95;

   out_FragColor = Color1 /*+ 0.2 * Color2*/ ;
}
