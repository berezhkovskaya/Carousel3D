/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;

uniform vec4 RectPos;

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
}

/*FRAGMENT_PROGRAM*/

uniform vec4 RectColor;

out vec4 out_FragColor;

void main()
{
   out_FragColor = RectColor;
}
