/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec4 Color;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   Color = vec4( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

in vec4 Color;

out vec4 out_FragColor;

void main()
{
	out_FragColor = Color;
}
