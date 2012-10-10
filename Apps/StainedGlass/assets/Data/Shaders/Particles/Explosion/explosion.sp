/*VERTEX_PROGRAM*/

varying vec2 TexCoord0;
varying vec4 TexCoord1;
varying vec4 TexCoord2;

void main()
{
   vec3 Position = vec3( gl_Vertex );

   vec3 X = vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]);
   vec3 Y = vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]);
  
   float SizeX = gl_MultiTexCoord1.z;
   float SizeY = gl_MultiTexCoord1.z;

   vec2 Coord = 2.0 * vec2( gl_MultiTexCoord0 ) - vec2( 1.0, 1.0 );

   Position += X * Coord.x;
   Position += Y * Coord.y;

/*
   vec3 A1 = -X*SizeX - Y*SizeY;
   vec3 B1 =  X*SizeX - Y*SizeY;
   vec3 C1 =  X*SizeX + Y*SizeY;
   vec3 D1 = -X*SizeX + Y*SizeY;

   vec2 Coord = vec2( gl_MultiTexCoord0 );

   if ( Coord == vec2(0.0, 0.0) ) Position += A1;
   if ( Coord == vec2(1.0, 0.0) ) Position += B1;
   if ( Coord == vec2(1.0, 1.0) ) Position += C1;
   if ( Coord == vec2(0.0, 1.0) ) Position += D1;
*/

   vec4 TransformedPos = gl_ModelViewProjectionMatrix * vec4(Position, 1);

   gl_Position = TransformedPos;

   TexCoord0 = vec2( gl_MultiTexCoord0 );
   TexCoord1 = vec4( gl_MultiTexCoord1 );
   TexCoord2 = vec4( gl_MultiTexCoord2 );
}

/*FRAGMENT_PROGRAM*/

varying vec2 TexCoord0;
varying vec4 TexCoord1; /* TTL, LifeTime, Size, unused */
varying vec4 TexCoord2; /* RGBA overlay */

uniform sampler2D Texture0; /* Color texture */
                                              
void main()
{
   vec4 ParticleColor = vec4( texture(Texture0, TexCoord0) );

   float TTL      = TexCoord1.x;
   float LifeTime = TexCoord1.y;

/*  if ( bool(lessThan(ParticleColor, vec4(0.1, 0.1, 0.1, 0))) ) discard; */

/*   gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 ); */

   gl_FragColor = 1.5 * TexCoord2 * ParticleColor * TTL / LifeTime; 
}
