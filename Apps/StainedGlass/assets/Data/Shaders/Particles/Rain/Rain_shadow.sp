/*VERTEX_PROGRAM*/

void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

/*FRAGMENT_PROGRAM*/
                                              
void main()
{
   discard;
}
