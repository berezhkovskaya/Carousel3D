/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord0;

void main()
{
   TexCoord0 = vec2( in_TexCoord );

   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord0;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

out vec4 out_FragColor;

void main()
{
  vec4 Color = 2.0 * vec4( texture( Texture0, TexCoord0 ) );

  float Dim = 1.0 / 64.0;

  Color += 1.0 * vec4( texture(Texture0, TexCoord0 + vec2( 0.0, Dim ) ) );
  Color += 1.0 * vec4( texture(Texture0, TexCoord0 + vec2( Dim, 0.0   ) ) );
  Color += 1.0 * vec4( texture(Texture0, TexCoord0 - vec2( 0.0, Dim ) ) );
  Color += 1.0 * vec4( texture(Texture0, TexCoord0 - vec2( Dim, 0.0   ) ) );

  Color += 1.0 * vec4( texture(Texture0, TexCoord0 + vec2( Dim, Dim ) ) );
  Color += 1.0 * vec4( texture(Texture0, TexCoord0 + vec2( Dim, Dim ) ) );
  Color += 1.0 * vec4( texture(Texture0, TexCoord0 - vec2( Dim, Dim ) ) );
  Color += 1.0 * vec4( texture(Texture0, TexCoord0 - vec2( Dim, Dim ) ) );

  Color /= 10.0;

  out_FragColor = Color;
}
