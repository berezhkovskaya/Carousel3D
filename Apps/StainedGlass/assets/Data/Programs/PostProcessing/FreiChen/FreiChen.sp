/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

out vec2 TexCoord;

void main()
{
   gl_Position = in_ModelViewProjectionMatrix * in_Vertex;

   TexCoord = vec2( in_TexCoord );
}


/*FRAGMENT_PROGRAM*/

uniform sampler2D Texture0;   /* Color */
uniform sampler2D Texture1;   /* Depth */

in vec2 TexCoord;

out vec4 out_FragColor;

void main()
{
   float dx = 1.0 / 1600.0;
   float dy = 1.0 / 1200.0;

	mat3 G[9];

	G[0] = 1.0/(2.0*sqrt(2.0)) * mat3( 1.0, sqrt(2.0), 1.0, 0.0, 0.0, 0.0, -1.0, -sqrt(2.0), -1.0 );
	G[1] = 1.0/(2.0*sqrt(2.0)) * mat3( 1.0, 0.0, -1.0, sqrt(2.0), 0.0, -sqrt(2.0), 1.0, 0.0, -1.0 );
	G[2] = 1.0/(2.0*sqrt(2.0)) * mat3( 0.0, -1.0, sqrt(2.0), 1.0, 0.0, -1.0, -sqrt(2.0), 1.0, 0.0 );
	G[3] = 1.0/(2.0*sqrt(2.0)) * mat3( sqrt(2.0), -1.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, -sqrt(2.0) );
	G[4] = 1.0/2.0 * mat3( 0.0, 1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 1.0, 0.0 );
	G[5] = 1.0/2.0 * mat3( -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0 );
	G[6] = 1.0/6.0 * mat3( 1.0, -2.0, 1.0, -2.0, 4.0, -2.0, 1.0, -2.0, 1.0 );
	G[7] = 1.0/6.0 * mat3( -2.0, 1.0, -2.0, 1.0, 4.0, 1.0, -2.0, 1.0, -2.0 );
	G[8] = 1.0/3.0 * mat3( 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 );

	mat3 I;
	
	// fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			vec3 s = texture( Texture0, TexCoord + vec2( dx * (i-1), dy * (j-1) ) ).rgb;
			I[i][j] = length(s); 
		}
	}

	float cnv[9];
	
	// calculate the convolution values for all the masks
	for (int i=0; i<9; i++)
	{
		float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
		cnv[i] = dp3 * dp3; 
	}

	float M = (cnv[0] + cnv[1]) + (cnv[2] + cnv[3]);
	float S = (cnv[4] + cnv[5]) + (cnv[6] + cnv[7]) + (cnv[8] + M); 
	float R = sqrt(M/S);

	out_FragColor = vec4( R, R, R, 1.0 );
}
