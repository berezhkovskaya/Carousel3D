/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
}

/*FRAGMENT_PROGRAM*/

out vec4 out_FragColor;

void main()
{
  // Initial value for adapted luminance
  out_FragColor = vec4( 10.0, 10.0, 10.0, 0.0 );
}
