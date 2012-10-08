/**
 * \file Surfaces.cpp
 * \brief Analytic surfaces generator
 * \version 0.5.98
 * \date 17/10/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Surfaces.h"

#include "Geometry/VAMender.h"

#include "Math/LCurve.h"
#include "Math/LMath.h"
#include "Math/LMathStrings.h"
#include "Resources/ResourcesManager.h"

#include "Geometry/Geom.h"
#include "Geometry/Mesh.h"

clUVSurfaceGenerator::clUVSurfaceGenerator():
	MinV( -1.0f ), MaxV( +1.0f ), MinU( -1.0f ), MaxU( +1.0f ),
	GlueUEdges( false ),
	GlueVEdges( false )
{
}

clUVSurfaceGenerator::~clUVSurfaceGenerator()
{
}

void clLinearSurface::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	LVector3 V = C->GetPosition( u ) - P;

	dRdu = C->GetDerivative( u );
	dRdv = Axis;

	R = P + V + v * Axis;
}

clVertexAttribs* clUVSurfaceGenerator::CreateSurface()
{
	if ( NumU == 0 || NumV == 0 ) { return NULL; }

	float URange = ( MaxU - MinU );
	float VRange = ( MaxV - MinV );

	float du = URange / static_cast<float>( NumU );
	float dv = VRange / static_cast<float>( NumV );

	clVAMender Mender;

	// First pass : output rectangular grid of points
	for ( int i = 0 ; i < NumU + 1; i++ )
	{
		float u =  MinU + i * du;

		for ( int j = 0 ; j < NumV + 1; j++ )
		{
			float v =  MinV + j * dv;

			int v_idx = j * ( NumU + 1 ) + i;

			LVector3 Pos, Normal, Tangent, Binormal;

			/// calculate spatial position and tangent vectors
			R_and_dR( u, v, Pos, Tangent, Binormal );

			Normal   = Tangent.Cross( Binormal );

			/// finally, output vertices to mender
			LVector3 TVertex( ( u - MinU ) / URange, ( v - MinV ) / VRange, 1.0f );

			Mender.EmitNormal( v_idx, Normal );
			Mender.EmitVertex( v_idx, Pos, -1, -1 );
			Mender.EmitTextureVertex ( v_idx, TVertex );
			Mender.EmitBinormal( v_idx, Binormal );
			Mender.EmitTangent( v_idx, Tangent );

			// AddVertex( j * NumU + i, Pos);
			// AddNormal( , Normal);
			// AddTexVertex( u, v);
			// AddTBN( Normal, Tangent, Binormal );
		}
	}

	int FaceCount = 0;

	// Second pass : output triangle indices
	for ( int i = 0 ; i < NumU ; i++ )
	{
		int ThisI = i;
		int NextI = i + 1;

		// wrap U edge
		if ( GlueUEdges )
		{
			if ( i == NumU - 1 )
			{
				// wrap p2 and p4
				NextI = 0;
			}
		}

		for ( int j = 0 ; j < NumV ; j++ )
		{
			int ThisJ = j;
			int NextJ = j + 1;

			// wrap V edge
			if ( GlueVEdges )
			{
				if ( j == NumV - 1 )
				{
					// wrap p1 and p3
					NextJ = 0;
				}
			}

			// calculate point indices

			/**
			p4=(u1,v2)       p3=(u2,v2)
			   +------------+
			   |\--         |
			   |   \--\---- |
			   |           \|
			   +------------+
			p1=(u1,v1)       p2=(u2,v1)

			   Triangles are T1=(p1,p2,p4) and T2=(p2,p3,p4)
			*/

			int p1 = ThisJ * ( NumU + 1 ) + ThisI; // (u1, v1)
			int p2 = ThisJ * ( NumU + 1 ) + NextI; // (u2, v1)
			int p3 = NextJ * ( NumU + 1 ) + NextI; // (u2, v2)
			int p4 = NextJ * ( NumU + 1 ) + ThisI; // (u1, v2)

			// Add triangles ( p1, p2, p4 ) and ( p2, p3, p4 ) to topology container

			Mender.EmitFace( FaceCount * 2 + 0, p1, p2, p4, -1, -1 );
			Mender.EmitFace( FaceCount * 2 + 1, p2, p3, p4, -1, -1 );

			Mender.EmitTextureFace( FaceCount * 2 + 0, p1, p2, p4 );
			Mender.EmitTextureFace( FaceCount * 2 + 1, p2, p3, p4 );

			FaceCount++;
		}
	}

	clVertexAttribs* Result = Mender.CreateVertexAttribs( -1 );

	return Result;
}

/*
/// render vertex array with tangent field
void clUVSurfaceGenerator::DebugRenderTangentField()
{
   if (NumU == 0) return;
   if (NumV == 0) return;

   float du = (MaxU - MinU) / static_cast<float>(NumU);
   float dv = (MaxV - MinV) / static_cast<float>(NumV);

   for (int i = 0 ; i < NumU ; i++)
   {
      float u =  i * du;

      for (int j = 0 ; j < NumV ; j++)
      {
         float v =  i * dv;

         LVector3 Pos, Normal, Tangent, Binormal;

         R_and_dR(u, v, Pos, Tangent, Binormal);

         Normal = Tangent.Cross(Binormal);

         // Through drawing canvas

         _Arrow(Pos, Pos + Normal,   LC_Red  );
         _Arrow(Pos, Pos + Tangent,  LC_Green);
         _Arrow(Pos, Pos + Binormal, LC_Blue );
      }
   }
}
*/

clGeom* clUVSurfaceGenerator::CreateGeom()
{
	return Env->Resources->CreateGeomForVA( CreateSurface() );
}

clWhitneyUmbrella::clWhitneyUmbrella()
{
	MinV = -1.0f;
	MaxV = +1.0f;
	MinU = -1.0f;
	MaxU = +1.0f;

	NumU = 40;
	NumV = 40;

	GlueUEdges = false;
	GlueVEdges = false;
}

clWhitneyUmbrella::~clWhitneyUmbrella()
{
}

void clWhitneyUmbrella::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	R = vec3( u * v, u, v * v );

	dRdu = vec3( v, 1.0f, 0.0f  );
	dRdv = vec3( u, 0.0f, 2.0f * v );
}

clSimplePlane::clSimplePlane()
{
	MinV = -5.0f;
	MaxV = +5.0f;
	MinU = -5.0f;
	MaxU = +5.0f;

	NumU = 40;
	NumV = 40;

	GlueUEdges = false;
	GlueVEdges = false;
}

clSimplePlane::~clSimplePlane()
{
}

void clSimplePlane::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	// no multiplication yet
	R = P + V1 * u + V2 * v;

	dRdu = V1;
	dRdv = V2;
}

clRevolutionParaboloid::clRevolutionParaboloid()
{
	MinV = -1.0f;
	MaxV = +1.0f;
	MinU = -1.0f;
	MaxU = +1.0f;

	NumU = 40;
	NumV = 40;

	GlueUEdges = false;
	GlueVEdges = false;
}

clRevolutionParaboloid::~clRevolutionParaboloid()
{
}

void clRevolutionParaboloid::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	R = vec3( u, v, u * u + v * v );

	dRdu = vec3( 1.0f, 0.0f, 2.0f * u );
	dRdv = vec3( 0.0f, 1.0f, 2.0f * v );
}

clHyperbolicParaboloid::clHyperbolicParaboloid()
{
	MinV = -1.0f;
	MaxV = +1.0f;
	MinU = -1.0f;
	MaxU = +1.0f;

	NumU = 40;
	NumV = 40;

	GlueUEdges = false;
	GlueVEdges = false;
}

clHyperbolicParaboloid::~clHyperbolicParaboloid()
{
}

void clHyperbolicParaboloid::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	R = vec3( u, v, u * u - v * v );

	dRdu = vec3( 1.0f, 0.0f, +2.0f * u );
	dRdv = vec3( 0.0f, 1.0f, -2.0f * v );
}

clMonkeySaddle::clMonkeySaddle()
{
	MinV = -1.0f;
	MaxV = +1.0f;
	MinU = -1.0f;
	MaxU = +1.0f;

	NumU = 40;
	NumV = 40;

	GlueUEdges = false;
	GlueVEdges = false;
}

clMonkeySaddle::~clMonkeySaddle()
{
}

void clMonkeySaddle::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	R = vec3( u, v, u * u * u - v * v );

	dRdu = vec3( 1.0f, 0.0f, +3.0f * u * u - 3.0f * v * v );
	dRdv = vec3( 0.0f, 1.0f, -6.0f * u * v );
}


clTorusKnot::clTorusKnot()
{
	NumU = 250;
	NumV = 50;

	GlueUEdges = true;
	GlueVEdges = true;

	BaseRadius = 2.0f;
	SegmentRadius = 1.0f;
	TubeRadius = 0.1f;

	p = 2;
	q = 3;

	p = 5;
	q = 11;

	p = 3;
	q = 7;

// p = q = 1;

	MinV = 0.0f;
	MaxV = 2.0f * Math::PI;
	MinU = 0.0f;
	MaxU = 2.0f * static_cast<float>( p ) * Math::PI;
}

clTorusKnot::~clTorusKnot()
{
}

void clTorusKnot::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float ct = cos( u );
	float st = sin( u );

	float cv = cos( v );
	float sv = sin( v );

	float qp = static_cast<float>( q ) / static_cast<float>( p );
	float arg = u * qp;

	float sqp = sin( arg );
	float cqp = cos( arg );

	float BSQP = ( BaseRadius + SegmentRadius * cqp );

	float _x = BSQP * ct;
	float _y = BSQP * st;
	float _z = SegmentRadius * sqp;

	float _dxdt = -qp * SegmentRadius * sqp * ct - st * BSQP;
	float _dydt = -qp * SegmentRadius * sqp * st + ct * BSQP;
	float _dzdt =  qp * SegmentRadius * cqp;

	float _d2xdt2 = -qp * SegmentRadius * sqp * st - qp * qp * SegmentRadius * cqp * ct - ct * BSQP + qp * st * SegmentRadius * sqp;
	float _d2ydt2 = -qp * SegmentRadius * cqp * st - qp * SegmentRadius * sqp * ct - st * BSQP - qp * ct * SegmentRadius * sqp;
	float _d2zdt2 = -qp * qp * SegmentRadius * sqp;

	LVector3 LineR  = vec3(   _x,      _y,      _z    );
	LVector3 dRdt   = vec3(  _dxdt,   _dydt,   _dzdt  );
	LVector3 d2Rdt2 = vec3( _d2xdt2, _d2ydt2, _d2zdt2 );

	LVector3 V1 = LineR.Cross( dRdt );
	LVector3 V2 = V1.Cross( dRdt );

	V1.Normalize();
	V2.Normalize();

	// в базисе (V1,V2) строим окружность
	R = LineR + TubeRadius * ( V1 * sv + V2 * cv );

	dRdu = dRdt;
	dRdv = LineR + TubeRadius * ( V1 * cv - V2 * sv );
}

void clTorusKnot::AfterLoad()
{
	// custom setup : recalculate MaxU
	MaxU = 2.0f * static_cast<float>( p ) * Math::PI;
}

void clTorusKnot::BeforeSave()
{
}

clMoebiusBand::clMoebiusBand()
{
	MinV = -1.0f;
	MaxV = +1.0f;
	MinU =  0.0f;
	MaxU =  2.0f * Math::PI;

// MaxU =  4.0f * Math::PI;

	NumU = 50;
	NumV = 50;

	GlueUEdges = false; //true;
	GlueVEdges = false;
}

clMoebiusBand::~clMoebiusBand()
{
}

void clMoebiusBand::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float s2u = sin( 0.5f * u );
	float c2u = cos( 0.5f * u );
	float cu = 2.0f * c2u * c2u - 1.0f;
	float su = 2.0f * s2u * c2u;

	R = vec3(
	       ( 1.0f + 0.5f * v * c2u ) * cu,
	       ( 1.0f + 0.5f * v * c2u ) * su,
	       0.5f * v * s2u
	    );

	dRdu = vec3(
	          +0.25f * v * s2u * cu - ( 1.0f + 0.5f * v * c2u ) * su,
	          -0.25f * v * s2u * su + ( 1.0f + 0.5f * v * c2u ) * cu,
	          +0.25f * v * c2u
	       );

	dRdv = vec3(
	          0.5f * c2u * cu,
	          0.5f * c2u * su,
	          0.5f * s2u
	       );
}


clKleinBottle::clKleinBottle()
{
	MinV = 0.0f;
	MaxV = 2.0f * Math::PI;
	MinU = 0.0f;
	MaxU = 2.0f * Math::PI;

	NumU = 100;
	NumV = 50;

	GlueUEdges = true;
	GlueVEdges = true;
}

clKleinBottle::~clKleinBottle()
{
}

void clKleinBottle::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float cu = cos( u );
	float cv = cos( v );

	float su = sin( u );
	float sv = sin( v );

	float c2u = 2.0f * cu * cu - 1.0f;
	float s2u = 2.0f * su * cu;

	float s4u = 2.0f * s2u * c2u;

	float Pi3 = Math::PI * Math::PI * Math::PI;

	float f_u = 20.0f * u * u * u - 65.0f * Math::PI * u * u + 50.0f * Math::PI * Math::PI * u - 16.0f * Math::PI;
	float g_u = sqrt( 8 * c2u * c2u - c2u * ( 24.0f * cu * cu * cu - 8.0f * cu + 15.0f ) + 6.0f * cu * cu * cu * cu * ( 1.0f - 3.0f * su * su ) + 17.0f );

	// производная выражения под корнем
	float g1_du = -16.0f * s4u + 2.0f * s2u * ( 24.0f * cu * cu * cu - 8.0f * cu + 15.0f ) + c2u * ( 3.0f * 24.0f * cu * cu * su - 8.0f * su ) - 4.0f * 6.0f * cu * cu * cu * su * ( 1.0f - 3.0f * su * su ) - 6.0f * cu * cu * cu * cu * 3.0f * s2u;

	// производные f(u) и g(u)
	float f_u_du = 60.0f * u * u - 130.0f * Math::PI * u + 50.0f * Math::PI * Math::PI;
	float g_u_du = 0.5f * g1_du / sqrt( g_u );

	float _x = ( Math::SQRT2 * f_u * cu * cv * ( 3.0f * cu * cu - 1.0f ) - 2.0f * c2u ) / ( 80.0f * Pi3 * g_u ) - 3.0f * ( cu - 1.0f ) / 4.0f;
	float _y = -f_u * sv / ( 60.0f * Pi3 );
	float _z = -( Math::SQRT2 * f_u * su * cv ) / ( 15.0f * Pi3 * g_u ) + ( su * cu * cu + su ) / 4.0f - su * cu / 2.0f;

	float _x_u = 3.0f * sv / 4.0f + ( Math::SQRT2 * cv / ( 80.0f * Pi3 * g_u * g_u ) ) * ( g_u * ( f_u_du * cu * ( 3.0f * cu * cu - 1.0f ) - f_u * su * ( 9.0f * cu * cu - 1.0f ) ) - g_u_du * f_u * cu * ( 3 * cu * cu + 1 ) ) - ( s2u * g_u + c2u * g_u_du ) / ( 20.0f * Pi3 * g_u * g_u );
	float _y_u = f_u_du * sv / ( 60.0f * Pi3 );
	float _z_u = cu * ( cu * cu - 2.0f * su * su + 1.0f ) / 4.0f - c2u / 2.0f + ( ( Math::SQRT2 * cv ) / ( 15.0f * Pi3 * g_u * g_u ) ) * ( ( f_u_du * su + f_u * cu ) * g_u - f_u * su * g_u_du );

	float _x_v = -Math::SQRT2 * f_u * cu * sv / ( 80.0f * Pi3 * g_u );
	float _y_v = -f_u * cv / ( 60.0f * Pi3 );
	float _z_v = Math::SQRT2 * f_u * su * sv / ( 15.0f * Pi3 * g_u );

	R = vec3( _x, _y, _z );

	dRdu = vec3( _x_u, _y_u, _z_u );
	dRdv = vec3( _x_v, _y_v, _z_v );
}

clKleinBottle2::clKleinBottle2()
{
	MinV = 0.0f;
	MaxV = 2.0f * Math::PI;
	MinU = 0.0f;
	MaxU = 2.0f * Math::PI;

	NumU = 100;
	NumV = 50;

	GlueUEdges = true;
	GlueVEdges = true;

	GlueUEdges = false;
	GlueVEdges = false;

	a = 20.0f;
	b = 8.0f;
	c = 5.5f;
	d = 0.4f;

	a *= 0.05f;
	b *= 0.05f;
	c *= 0.05f;
	d *= 0.05f;
}

clKleinBottle2::~clKleinBottle2()
{
}

void clKleinBottle2::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float cu = cos( u );
	float su = sin( u );
	float c2u = 2.0f * cu * cu - 1.0f;
	float s2u = 2.0f * su * cu;

	LVector3 LineR  = vec3( a * ( 1.0f - cu ), b * su * ( 1.0f - cu ), 0.0f );
	LVector3 dRdt   = vec3( a * su,  b * ( cu + c2u ), 0.0f );
	LVector3 d2Rdt2 = vec3( a * cu, -b * ( su + 2.0f * s2u ), 0.0f );

	float cv = cos( v );
	float sv = sin( v );

	float TubeRadius = c - d * ( u - Math::PI ) * sqrt( u * ( 2 * Math::PI - u ) );

	LVector3 V1 = LineR.Cross( dRdt );
	LVector3 V2 = V1.Cross( dRdt );

	V1.Normalize();
	V2.Normalize();

	// в базисе (V1,V2) строим окружность
	R = LineR + TubeRadius * ( V1 * sv + V2 * cv );

	dRdu = dRdt;
	dRdv = LineR + TubeRadius * ( V1 * cv - V2 * sv );
}

clMetricScrewGenerator::clMetricScrewGenerator()
{
	MinV = -4.0f;
	MaxV = +4.0f;
	MinU = -1.0f;
	MaxU = +1.0f;

	NumU = 250;
	NumV = 250;

	GlueUEdges = false;
	GlueVEdges = false;

	FR = 10.0f;
	FD = 0.54f;
	FH = 0.4f;
}

clMetricScrewGenerator::~clMetricScrewGenerator()
{
}

void clMetricScrewGenerator::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float w = FD * ( 1.0f + fabs( u ) * FH / FD );
	float arg = FR * v + Math::PI * u;

	float sa = sin( arg );
	float ca = cos( arg );

	R = vec3( w * sa, w * ca, v );

	float sign_u = u > 0.0f ? +1.0f : -1.0f;

	dRdu = vec3( Math::PI * w * ca + FH * sign_u * sa, -Math::PI * w * sa + FH * sign_u * ca, 0.0f );
	dRdv = vec3( FR * w * ca, -FR * w * sa, 1.0f );
}

clRevolutionSurface::clRevolutionSurface(): Axis( 1, 0, 0 ), P( 0, 0, 0 )
{
	GlueVEdges = true;
	GlueUEdges = false;

	MinV = 0.0f;
	MaxV = 6.28f;
	MinU = 0.0f;
	MaxU = 10.0f;
}

void clRevolutionSurface::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	LVector3 V = C->GetPosition( u ) - P;

	LMatrix4 mm = LMatrix4::GetRotateMatrixAxis( v, Axis );

	dRdu = C->GetDerivative( u );
	dRdv = mm[1].ToVector3();

	R = mm * V;
}

void clTubularNeighbourhood::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float cv = cos( v );
	float sv = sin( v );

	vec3 LineR  = C->GetPosition(u);
	vec3 dRdt   = C->GetDerivative(u);

	vec3 V1 = LineR.Cross( dRdt );
	vec3 V2 = V1.Cross( dRdt );

	V1.Normalize();
	V2.Normalize();

	// в базисе (V1,V2) строим окружность
	R = LineR + TubeRadius * ( V1 * sv + V2 * cv );

	dRdu = dRdt;
	dRdv = LineR + TubeRadius * ( V1 * cv - V2 * sv );
}

clSuperShapeGenerator::~clSuperShapeGenerator()
{
}

clSuperShapeGenerator::clSuperShapeGenerator():
	FA_1( 1.0f ), FA_2( 1.0f ), FB_1( 1.0f ), FB_2( 1.0f ),
	FM_1( 1 ), FM_2( 1 ), FN1_1( 1 ), FN1_2( 1 ), FN2_1( 1 ), FN2_2( 1 ), FN3_1( 1 ), FN3_2( 1 )
{
	GlueVEdges = true;
	GlueUEdges = false;

	MinV = -Math::PI / 2.0f;
	MaxV = +Math::PI / 2.0f;

	MinU = -Math::PI;
	MaxU = +Math::PI;
}

void clSuperShapeGenerator::R_dR( float& R, float& dR, float A, float B, int M, int N1, int N2, int N3, float Phi )
{
	float fM  = static_cast<float>( M );
	float fN1 = static_cast<float>( N1 );
	float fN2 = static_cast<float>( N2 );
	float fN3 = static_cast<float>( N3 );

	float Arg = fM * Phi / 4.0f;

	float C = cosf( Arg ) / A;
	float S = sinf( Arg ) / B;

	float CP = powf( Linderdaum::Math::LAbs( C ), fN2 );
	float SP = powf( Linderdaum::Math::LAbs( S ), fN3 );

	float T = CP + SP;

	R = powf( T, -1.0f / fN1 );

	dR = ( -fN2 * Linderdaum::Math::fsign( C ) * ( CP / C ) * fM * S * B / ( 4.0f * A ) ) + ( fN3 * Linderdaum::Math::fsign( S ) * ( SP / S ) * fM * C * A / ( 4.0f * B ) );
}

void clSuperShapeGenerator::R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
{
	float R1, dR1, R2, dR2;

	float cu = cosf( u );
	float su = sinf( u );
	float cv = cosf( v );
	float sv = sinf( v );

	R_dR( R1, dR1, FA_1, FB_1, FM_1, FN1_1, FN2_1, FN3_1, u );
	R_dR( R2, dR2, FA_2, FB_2, FM_2, FN1_2, FN2_2, FN3_2, v );

	R.X = R1 * cu * R2 * cv;
	R.Y = R1 * su * R2 * cv;
	R.Z = R2 * sv;

	dRdu.X = R2 * cv * ( dR1 * cu - R1 * su );
	dRdu.Y = R2 * cv * ( dR1 * su + R1 * cu );
	dRdu.Z = 0;

	dRdv.X = R1 * cu * ( dR2 * cv - R2 * sv );
	dRdv.Y = R1 * su * ( dR2 * cv - R2 * sv );
	dRdv.Z = dR2 * sv + R2 * cv;
}

/*
y2 = x5 - x  // Bolza surface
*/

/*
 * 07/10/2010
     Log section added
*/
