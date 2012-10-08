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

in vec2 TexCoord0;

out vec4 out_FragColor;

void main()
{
   vec4 Color = texture( Texture0, TexCoord0 );
   vec4 Depth = texture( Texture1, TexCoord0 );

   out_FragColor = Color;
   gl_FragDepth  = Depth.x;
//   out_FragColor =  vec4( 1.0, 0.0, 0.0, 1.0 );


//   const float Far  = 0.04;
//   const float Near = 10.0;

//   gl_FragColor = vec4( vec3(Far * Near / (Near - Far) ) / ( Color.x - Far / (Far - Near) ), 0.5 );
}
