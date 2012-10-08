/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   TexCoord = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;
uniform vec4 PICKING_COLOR;

out vec4 out_FragColor;
out vec4 out_PickMask;

void main()
{
   out_FragColor = texture( Texture0, TexCoord );
   out_PickMask  = PICKING_COLOR;
}
