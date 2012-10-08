/*VERTEX_PROGRAM*/

varying vec2 TexCoord0;
varying vec4 TexCoord1;
varying vec4 TexCoord2;

void main()
{
   vec3 Position = vec3(gl_Vertex);

   vec3 X = vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]);
   vec3 Y = vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]);
  
   float SizeX = 0.5;
   float SizeY = 7.0;

   vec3 A1 = -X*SizeX - Y*SizeY;
   vec3 B1 =  X*SizeX - Y*SizeY;
   vec3 C1 =  X*SizeX + Y*SizeY;
   vec3 D1 = -X*SizeX + Y*SizeY;

   /* make parallel to Z-axis */

   A1.x = D1.x;
   B1.x = C1.x;
   A1.y = D1.y;
   B1.y = C1.y;

   if (vec2(gl_MultiTexCoord0) == vec2(0.0, 0.0)) Position += A1;
   if (vec2(gl_MultiTexCoord0) == vec2(1.0, 0.0)) Position += B1;
   if (vec2(gl_MultiTexCoord0) == vec2(1.0, 1.0)) Position += C1;
   if (vec2(gl_MultiTexCoord0) == vec2(0.0, 1.0)) Position += D1;

   gl_Position = gl_ModelViewProjectionMatrix * vec4(Position, 1);

   TexCoord0 = vec2(gl_MultiTexCoord0);
}

/*FRAGMENT_PROGRAM*/

varying vec2 TexCoord0;

uniform sampler2D Texture0; /* Color texture */
                                              
void main()
{
   vec4 ParticleColor = vec4( texture(Texture0, TexCoord0) );

   gl_FragColor = ParticleColor;  
}
