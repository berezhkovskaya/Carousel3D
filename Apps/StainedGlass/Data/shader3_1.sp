/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec3 u_Light;
uniform vec3 u_Normal;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 TexCoord;

out vec3 l;
out vec3 n;

void main()
{
  gl_Position = ProjectionMatrix * ModelViewMatrix * in_Vertex;

  l = normalize ( u_Light - vec3(in_Vertex));        
  n = normalize ( u_Normal );

  TexCoord = vec2( in_TexCoord );
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

in vec3 l;
in vec3 n;

uniform vec4 u_Color;

out vec4 out_FragColor;

void main()
{
   vec3 n2   = normalize ( n );
   vec3 l2   = normalize ( l );
   out_FragColor = vec4(vec3(u_Color * max(dot(n2, l2), 0.0)), 1.0) ;
}
