/** Common part for lists */

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 TexCoord;

void main()
{
  gl_Position = ProjectionMatrix * ModelViewMatrix * in_Vertex;

  TexCoord = vec2( in_TexCoord );
}
