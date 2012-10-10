/*VERTEX_PROGRAM*/

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

out vec4 Color;

void main()
{
   gl_Position = ProjectionMatrix * ModelViewMatrix * in_Vertex;

   Color = in_TexCoord;
}

/*FRAGMENT_PROGRAM*/

in vec4 Color;

out vec4 out_FragColor;

void main()
{
	out_FragColor = Color;
}
