/*VERTEX_PROGRAM*/

#include Layout.sp

in vec4 in_Vertex;
in vec4 in_TexCoord;

uniform vec4 GUI_Pos;

out vec2 TexCoord;

void main()
{
   float X1 = GUI_Pos.x;
   float Y1 = GUI_Pos.y;
   float X2 = GUI_Pos.z;
   float Y2 = GUI_Pos.w;

   float Width  = X2 - X1;
   float Height = Y2 - Y1;

   vec4 VertexPos = vec4( X1 + in_Vertex.x * Width,
                          Y1 + in_Vertex.y * Height,
                          in_Vertex.z, in_Vertex.w );

   gl_Position = in_ModelViewProjectionMatrix * VertexPos;

   TexCoord = in_TexCoord.xy;
}

/*FRAGMENT_PROGRAM*/

in vec2 TexCoord;

uniform sampler2D Texture0;

uniform float GUI_Opacity;

uniform float DialAlpha;
uniform float r_min;
uniform float r_max;

uniform vec3 MousePoint;

out vec4 out_FragColor;

float RadToDeg(float RR) { return RR * 180.0 / 3.141592654; }
float DegToRad(float RR) { return RR * 3.141592654 / 180.0; }

float cosD(float R) { return cos(DegToRad(R)); }
float sinD(float R) { return sin(DegToRad(R)); }

vec2 ToPolar( float RR, float Angle) { return RR * vec2(cos(DegToRad(Angle)), sin(DegToRad(Angle))); }

float Vec2Angle( float x, float y ) { return 180.0 + RadToDeg( atan(y,x) ); }

float FactorAdjust(float Color, float Factor, float IntensityMax, float Gamma)
{
	return (Color > 0.001) ? IntensityMax * pow(Color * Factor, Gamma) : 0.0;
}

/// http://miguelmoreno.net/sandbox/wavelengthtoRGB/
/// and http://www.midnightkite.com/color.html
/// Wavelength from 350 to 780
vec3 ColorFromWaveLength(float W)
{
	float Gamma  = 1.0;
	float Blue   = 0.0;
	float Green  = 0.0;
	float Red    = 0.0;
	float Factor = 0.0;

	if( ( W >= 350.0 ) && ( W < 440.0 ) )
	{
		Red    = -(W - 440.0) / (440.0 - 350.0);
		Blue   = 1.0;
	}
	else if( ( W >= 440.0 ) && ( W < 490.0 ) )
	{
		Green  = (W - 440.0) / (490.0 - 440.0);
		Blue   = 1.0;
	}
	else if( ( W >= 490.0 ) && ( W < 510.0 ) )
	{
		Green  = 1.0;
		Blue   = -(W - 510.0) / (510.0 - 490.0);
	}
	else if( ( W >= 510.0 ) && ( W < 580.0 ) )
	{ 
		Red    = (W - 510.0) / (580.0 - 510.0);
		Green  = 1.0;
	}
	else if( ( W >= 580.0 ) && ( W < 645.0 ) )
	{
		Red    = 1.0;
		Green  = -(W - 645.0) / (645.0 - 580.0);
	}
	else if( ( W >= 645.0 ) && ( W <= 780.0 ) )
	{
		Red    = 1.0;
	}

	if( ( W >= 350.0 ) && ( W < 420.0 ) )
	{
		Factor = 0.3 + 0.7 *(W - 350.0) / (420.0 - 350.0);
	}
	else if( ( W >= 420.0 ) && ( W < 700.0 ) )
	{
		Factor = 1.0;
	}
	else if( ( W >= 700.0 ) && ( W <= 780.0 ) )
	{
		Factor = 0.3 + 0.7*(780.0 - W) / (780.0 - 700.0);
	}

	Red   = FactorAdjust(Red,   Factor, 255.0, Gamma);
	Green = FactorAdjust(Green, Factor, 255.0, Gamma);
	Blue  = FactorAdjust(Blue,  Factor, 255.0, Gamma);

	return vec3(Red, Green, Blue) / 255.0;
}

vec3 ColorFromAngle( float phi )
{
	float startA = 0.0;

	float dA = 10.0;

	if(phi < startA + dA || phi > 360.0 + startA - dA) 
	{
		float t = 0.0;
		if(phi > startA + dA)
		{
			t = (phi - startA + dA - 360.0) / (2.0 * dA);
		} else
		{
			t = (phi - startA + dA) / (2.0 * dA);
		}

		// difference from CPU version... atan works inverted ?
		t = 1.0 - t;

		return t * ColorFromWaveLength(350.0) + (1.0 - t) * ColorFromWaveLength(780.0);
	}

	// map [startA + dA, 360 + startA - dA] to [0..360] and then to [0..1]
        float phiN = (phi - dA - startA) / (360.0 - 2 * dA);

	return ColorFromWaveLength( 780.0 + phiN * (350.0 - 780.0) );
}

vec3 Barycentric2D( float x, float y, float x1, float y1, float x2, float y2, float x3, float y3 )
{
	float detT = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);

	float l1 = ( (y2-y3)*(x-x3) + (x3-x2)*(y-y3) ) / detT;
	float l2 = ( (y3-y1)*(x-x3) + (x1-x3)*(y-y3) ) / detT;

	return vec3( l1, l2, 1.0 - l1 - l2 );
}

void main()
{
	float u = TexCoord.x;
	float v = TexCoord.y;

	float nx = (u - 0.5) * 2.0;
	float ny = (v - 0.5) * 2.0;

	float r = nx * nx + ny * ny;

	float r_max2 = r_max * r_max;
	float r_min2 = r_min * r_min;

	float phi = Vec2Angle(nx, ny);

	if( r < r_max2 && r > r_min2 )
	{
		out_FragColor = vec4( ColorFromAngle( phi ), 1.0) * vec4( GUI_Opacity );
	}
	else
	{

//		vec2 p_color = -r_min * vec2( cosD( DialAlpha ), sinD(DialAlpha) );
//		vec2 p_white = -r_min * vec2( cosD( DialAlpha + 120.0 ), sinD( DialAlpha + 120.0 ) );
//		vec2 p_black = -r_min * vec2( cosD( DialAlpha + 240.0 ), sinD( DialAlpha + 240.0 ) );

		vec2 p_color = -ToPolar(r_min, DialAlpha );
		vec2 p_white = -ToPolar(r_min, DialAlpha + 120.0 );
		vec2 p_black = -ToPolar(r_min, DialAlpha + 240.0 );

		vec3 B = Barycentric2D( nx, ny, p_black.x, p_black.y, p_white.x, p_white.y, p_color.x, p_color.y );

		if( B.x >= 0 && B.x <= 1 && B.y >= 0 && B.y <= 1 && B.z >= 0 && B.z <= 1)
		{
			vec3 TheColor = ColorFromAngle( DialAlpha );

			out_FragColor = vec4( B.x * vec3(0,0,0) + B.y * vec3(1,1,1) + B.z * TheColor, 1.0 )  * vec4( GUI_Opacity );
		}
		else
		{
			out_FragColor = vec4(0, 0, 0, 0);
		}

		vec3 MB = Barycentric2D( MousePoint.x, MousePoint.y, p_black.x, p_black.y, p_white.x, p_white.y, p_color.x, p_color.y );

		if( MB.x >= 0 && MB.x <= 1 && MB.y >= 0 && MB.y <= 1 && MB.z >= 0 && MB.z <= 1)
		{
			// now check the mouse mark
			vec2 dd = vec2( MousePoint.x - nx, MousePoint.y - ny);
			float D = sqrt(dd.x * dd.x + dd.y * dd.y);

			if( D < 0.04 && D > 0.02) out_FragColor = vec4(1,1,1,1) * vec4( GUI_Opacity );
		}
	}
}
