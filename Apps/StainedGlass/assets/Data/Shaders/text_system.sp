/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;
in vec4 in_Color;

uniform vec4 TextUVOfs;

out vec2 TexCoord;
out vec4 TextColor;

void main()
{
   float X = in_TexCoord.z;
   float Y = in_TexCoord.w;
   float W = TextUVOfs.z;
   float H = TextUVOfs.w;
 
   vec4 VertexPos = vec4( X + in_Vertex.x * W,
                          Y + in_Vertex.y * H,
                          in_Vertex.z, in_Vertex.w );

   gl_Position = in_ModelViewProjectionMatrix * VertexPos;

   TexCoord = in_TexCoord.xy + TextUVOfs.xy;

   TextColor = in_Color;
}

/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0;

// depends on viewport resolution - smaller for lower resolution
uniform float OUTLINE_DISCARD;

in vec2 TexCoord;
in vec4 TextColor;

out vec4 out_FragColor;

void main()
{
   float Outline = texture( Texture0, TexCoord ).r;

   if ( Outline < OUTLINE_DISCARD ) discard;

   out_FragColor = vec4( TextColor.rgb, Outline * TextColor.a );
}
