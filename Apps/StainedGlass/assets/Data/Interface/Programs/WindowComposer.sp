/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec4 GUI_Pos;
uniform float ENGINE_TIME;

out vec2 TexCoord;
out vec2 ObjectSpace;

void main()
{
   float X1 = GUI_Pos.x;
   float Y1 = GUI_Pos.y;
   float X2 = GUI_Pos.z;
   float Y2 = GUI_Pos.w;

   float Width  = X2 - X1;
   float Height = Y2 - Y1;

   const float PI = 3.14159;
   float Offset = ENGINE_TIME * 2.0 * PI;

   vec4 VertexPos = vec4( X1 + in_Vertex.x * Width,
                          Y1 + in_Vertex.y * Height,
                          in_Vertex.z, in_Vertex.w );

   TexCoord = vec2( VertexPos.x, 1.0 - VertexPos.y );

   VertexPos.y += 0.001 * sin( 15.0 * VertexPos.x + Offset );
   VertexPos.x += 0.001 * cos( 15.0 * VertexPos.y + Offset );

   gl_Position = in_ModelViewProjectionMatrix * VertexPos;

   ObjectSpace = vec2( in_TexCoord );
}


/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0;

in vec2 TexCoord;
in vec2 ObjectSpace;

uniform float GUI_Opacity;

out vec4 out_FragColor;

void main()
{
   vec4 Color1 = texture(Texture0, TexCoord);
   vec4 Color2 = Color1;

   Color2.w = 0.8;

   out_FragColor = mix( Color1, Color2, ObjectSpace.x ) * vec4( GUI_Opacity );
}
