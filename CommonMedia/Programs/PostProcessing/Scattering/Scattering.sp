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
uniform sampler2D Texture2;   /* WorldCoords */

in vec2 TexCoord;

uniform float MIE_FACTOR;
uniform float RAYLEIGH_FACTOR;
uniform float INSCATTER_FACTOR;
/*uniform float DIRECTIONAL_COEF;*/
/*uniform float SUN_INTENSITY;*/

out vec4 out_FragColor;

void main()
{
   const float DIRECTIONAL_COEF = 0.5;
   const float SUN_INTENSITY    = 1.0;

	/* calculate useful constants */
	const float pi = 3.14159265358;
	const float n = 1.003; /* refractive index */
	const float N = 2.545e25;
	const float pn = 0.035;

	vec3 fLambda,fLambda2,fLambda4;

	fLambda[0]  = 1.0/650e-9; /* red // note 650e-9 m = 650nm. */
	fLambda[1]  = 1.0/570e-9; /* green */
	fLambda[2]  = 1.0/475e-9; /* blue  */
	for (int i = 0 ; i < 3 ; i++)
	{
		fLambda2[i] = fLambda[i]*fLambda[i];
		fLambda4[i] = fLambda2[i]*fLambda2[i];
	}

	/* Rayleigh scattering constants. */
	float fTemp = pi*pi*(n*n-1.0)*(n*n-1.0)*(6.0+3.0*pn)/(6.0-7.0*pn)/N;
	float fBeta = 8.0*fTemp*pi/3.0;
	vec3 BetaRayleigh = fBeta * fLambda4 * RAYLEIGH_FACTOR;

	float fBetaDash = fTemp/2.0;
	vec3 BetaDashRayleigh = fBetaDash * fLambda4;

	/* Mie scattering constants. */
	const float T = 2.0;
	float c = (6.544*T - 6.51)*1e-17; /* from page 57 of my thesis.*/
	float fTemp2 = 2.0*0.434*c*(pi*pi);
	vec3 BetaDashMie = fTemp2 * fLambda2;

	vec3 K = vec3(0.685, 0.679, 0.670); /* from pg 64 of my thesis. */
	float fTemp3 = 2.0*fTemp2*pi; /* 4.0*0.434*c*pi*pi*pi; */

	vec3 vBetaMieTemp = K*fLambda2;
	vec3 BetaMie = fTemp3*vBetaMieTemp * MIE_FACTOR;


	/* 1) extinction distance */
   vec3 worldPos  = vec3( texture( Texture2, TexCoord ) );
   vec3 eyeVector = /* cam pos is Zero ! */ -worldPos;
	float s = dot( eyeVector, eyeVector );
	s = 1.0/sqrt(s);
	eyeVector *= s;
	s = 1.0/s;

	/* FIXME : multiply by inverse model view */
	const vec3 sun_vec = vec3(1.0/1.4142, 1.0/1.4142, 0.0);

	float cosTheta = dot(eyeVector, sun_vec);

	/* 2) sun energy*/
	const float /* ln(2) = 1.0/0.6931 */ l2e = 1.442;

	vec3 Beta1PlusBeta2 = BetaMie + BetaRayleigh;
	vec3 E = -( Beta1PlusBeta2 * l2e) * s;
	vec3 Esun = exp( E );

	/* 3) phase functions */

	float cosSqr = cosTheta*cosTheta;

	const float g = DIRECTIONAL_COEF;

	/* a) Rayleigh phase func */
	float Phase_Rayleigh = ( 3.0 * ( 1.0 + cosSqr) ) / ( 16.0 * pi );
	/* b) Henyey/Greenstein   */
	float Phase_HG = ( (1.0 - g) * (1.0 - g) ) / (4.0 * pi);

	float denom = 1.0 + g * g - 2.0 * g * cosTheta;

	Phase_HG /= sqrt(denom)*denom;

	/* 4) calculate inscattering */
/*
 Inscattering (I) = (Beta'_1 * Phase_1(theta) + Beta'_2 * Phase_2(theta)) * 
        [1-exp(-Beta_1*s).exp(-Beta_2*s)] / (Beta_1 + Beta_2)
*/
	vec3 I = ( BetaDashRayleigh*Phase_Rayleigh + BetaDashMie*Phase_HG )*(1.0-exp(-Beta1PlusBeta2*s)) / (BetaMie + BetaRayleigh);

	/* FIXME : read from another texture ???? */
/*
	vec3 pixelNormal = -worldPos;

	float NormalDotLight = dot ( pixelNormal , sun_vec );

	I = I * clamp(NormalDotLight + 0.33, 0.0, 1.0);
*/

	/* 5) calculate extinction */

	/* FIXME : normal sun color !!!! */
	const vec3 sun_color = vec3(1.0, 1.0, 1.0);
	vec3 InScattering = I * sun_color * SUN_INTENSITY * INSCATTER_FACTOR; 

	vec3 Extinction = Esun * sun_color * SUN_INTENSITY;

	/* 6) at the long last, output the color ! */
	/*  Final_color =  Surf_color * extinction + inscattering  */

	vec4 TexColor = texture(Texture0, TexCoord);

   out_FragColor = TexColor * vec4( Extinction, 1.0) + vec4(InScattering, 0.0); 

/*   gl_FragColor = vec4( BetaDashRayleigh*Phase_Rayleigh + BetaDashMie*Phase_HG, 1.0 );  */
}
