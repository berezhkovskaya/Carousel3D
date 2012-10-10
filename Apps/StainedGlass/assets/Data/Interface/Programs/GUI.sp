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

in vec2 TexCoord;

uniform sampler2D Texture0;

uniform float GUI_Opacity;

out vec4 out_FragColor;

void main()
{
   out_FragColor = texture( Texture0, TexCoord ) * vec4( GUI_Opacity );
}
