/*VERTEX_PROGRAM*/

#include Layout.sp

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

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform vec4 u_Color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 u_p00;
uniform vec4 u_p01;
uniform vec4 u_p10;


out vec4 out_FragColor;

void main()
{
	vec4 p00 = ProjectionMatrix * ModelViewMatrix * u_p00;
	vec4 p10 = ProjectionMatrix * ModelViewMatrix * u_p10;
	vec4 p01 = ProjectionMatrix * ModelViewMatrix * u_p01;
	p00 = (p00 + vec4(1.0, 1.0, 1.0, 1.0)) * 0.5;
	p10 = (p10 + vec4(1.0, 1.0, 1.0, 1.0)) * 0.5;
	p01 = (p01 + vec4(1.0, 1.0, 1.0, 1.0)) * 0.5;
    vec4 v1 = p10 - p00;
    vec4 v2 = p01 - p00;
    
	vec4 Color1 = texture( Texture0, TexCoord );
	vec4 Color2 = texture( Texture1, p00.xy + v1.xy * TexCoord.x + v2.xy * TexCoord.y);
	vec4 Color = (Color1 + 0.5 * Color2) / 1.5;
	Color.a = 1.0; 

	out_FragColor = Color + vec4( 0.1, 0.1, 0.1, 0.0 );
}
