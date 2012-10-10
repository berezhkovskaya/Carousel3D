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
uniform float GUI_Opacity;

out vec4 out_FragColor;

void main()
{
   vec4 Color = texture(Texture0, TexCoord);

   out_FragColor = Color * vec4( 0.1, 0.1, 0.1, 1.0 ) * vec4( GUI_Opacity );
}
