/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   TexCoord = vec2( in_TexCoord );
}


/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0; /* Color */
uniform sampler2D Texture1; /* Depth */
uniform sampler3D Texture2; /* Noise */

uniform float ENGINE_TIME;

in vec2 TexCoord;

out vec4 out_FragColor;

vec2 HexagonGradient(vec2 TexCoord)
{
   vec4 Noise = texture( Texture2, vec3( TexCoord.x, TexCoord.y, ENGINE_TIME ) );

   return 0.02 * vec2( Noise.x, Noise.y );
}

void main()
{
   vec4 Sky = texture( Texture0, TexCoord + HexagonGradient(TexCoord) );

   out_FragColor = Sky;
}
