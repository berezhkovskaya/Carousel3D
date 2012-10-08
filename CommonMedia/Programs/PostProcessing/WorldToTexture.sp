/*VERTEX_PROGRAM*/

varying vec4 TexCoord;

void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   TexCoord = gl_ModelViewMatrix * gl_Vertex;
}


/*FRAGMENT_PROGRAM*/

varying vec4 TexCoord;

void main()
{
   gl_FragColor = TexCoord;
}
