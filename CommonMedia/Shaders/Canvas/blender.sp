/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord0;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   TexCoord0 = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0; // color
uniform sampler2D Texture1; // depth

uniform sampler2D Texture2; // primary color
uniform sampler2D Texture3; // primary depth

in vec2 TexCoord0;

out vec4 out_FragColor;

void main()
{
   vec4 Color = texture( Texture0, TexCoord0 );
   float Depth = texture( Texture1, TexCoord0 ).x;

   vec4 Color2 = texture ( Texture2, TexCoord0 );
   float Depth2 = texture ( Texture3, TexCoord0 ).x;

   vec4  FinalColor;
   float FinalDepth;

   if (Depth > Depth2)
   {
     FinalColor = Color2;
     FinalDepth  = Depth;
   }
   else
   { 
     FinalColor = Color;
     FinalDepth = Depth2;
   }

   out_FragColor = FinalColor;
   gl_FragDepth = FinalDepth;
}
