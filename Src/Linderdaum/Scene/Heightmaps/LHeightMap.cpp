/**
 * \file LHeightMap.cpp
 * \brief Heighmap utils
 * \version 0.5.74
 * \date 28/04/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "LHeightMap.h"

LHeightMap::LHeightMap(): FDataX1( 0 ),
	FDataY1( 0 ),
	FDataX2( 0 ),
	FDataY2( 0 ),
	FOrigin(),
	FBoundingBox(),
	FScales( LVector3( 1, 1, 1 ) ),
	FGranularity( LVector3( 1, 1, 1 ) )

{
	FHeightDataProvider = clPtr<iHeightDataProvider>( new iHeightDataProvider );
	FHeightDataProvider->Env = NULL;
}

void LHeightMap::SetHeightDataProvider( const clPtr<iHeightDataProvider>& Provider )
{
	FHeightDataProvider = Provider;

	SetDataBoundry( 0, 0, Provider->GetSizeX(), Provider->GetSizeY() );
}

void LHeightMap::SetDataBoundry( int X1, int Y1, int X2, int Y2 )
{
	FDataX1 = X1;
	FDataX2 = X2;
	FDataY1 = Y1;
	FDataY2 = Y2;
}

void LHeightMap::GetDataBoundry( int& X1, int& Y1, int& X2, int& Y2 ) const
{
	X1 = FDataX1;
	X2 = FDataX2;
	Y1 = FDataY1;
	Y2 = FDataY2;
}

LAABoundingBox LHeightMap::GetBoundingBox() const
{
	return FBoundingBox;
}

float LHeightMap::GetHeight( float X, float Y ) const
{
	int I;
	int J;
	float FracI;
	float FracJ;

	WorldToData( X, Y, I, J, FracI, FracJ );

	// find heights
	float H0 = DataToWorld( I,   J   ).Z;
	float H1 = DataToWorld( I,   J + 1 ).Z;
	float H2 = DataToWorld( I + 1, J   ).Z;
	float H3 = DataToWorld( I + 1, J + 1 ).Z;

	/*
	   H0
	   ----- H2
	   |  / |
	  Edge1 EdgeHypot
	   |/   |
	   H1---|H3
	*/

	float Result;

	// choose a triangle
	if ( FracI + FracJ < 1.0f )
	{
		float HeightEdge1 = H0 + ( H1 - H0 ) * FracJ;
		float HeightHypot = H2 + ( H1 - H2 ) * FracJ;

		Result = HeightEdge1 + ( HeightHypot - HeightEdge1 ) * FracI;
		/*
		      float W = (FracI + FracJ);

		      float Yh = H0 + (H1-H0) * W;
		      float Xh = H0 + (H2-H0) * W;

		      Result = Xh + (Yh - Xh) * FracJ;
		*/
	}
	else
	{
		/*
		      float W = (FracI + FracJ - 1.0f);

		      float Yh = H2 + (H3-H2) * W;
		      float Xh = H1 + (H3-H1) * W;

		      Result = Yh + (Xh - Yh) * FracJ;
		*/
		float HeightHypot = H2 + ( H1 - H2 ) * FracJ;
		float HeightEdge1 = H2 + ( H3 - H2 ) * FracJ;

		Result = HeightHypot + ( HeightEdge1 - HeightHypot ) * FracI;
	}

	return Result;
}

LVector3 LHeightMap::GetNormal( float X, float Y ) const
{
	int I;
	int J;
	float FracI;
	float FracJ;

	WorldToData( X, Y, I, J, FracI, FracJ );

	LVector3 H0 = GetDataNormal( I,   J   );
	LVector3 H1 = GetDataNormal( I,   J + 1 );
	LVector3 H2 = GetDataNormal( I + 1, J   );
	LVector3 H3 = GetDataNormal( I + 1, J + 1 );

	LVector3 Normal;

	if ( FracI + FracJ < 1.0f )
	{
		LVector3 HeightEdge1 = H0 + ( H1 - H0 ) * FracJ;
		LVector3 HeightHypot = H2 + ( H1 - H2 ) * FracJ;

		Normal = HeightEdge1 + ( HeightHypot - HeightEdge1 ) * FracI;
	}
	else
	{
		LVector3 HeightHypot = H2 + ( H1 - H2 ) * FracJ;
		LVector3 HeightEdge1 = H2 + ( H3 - H2 ) * FracJ;

		Normal = HeightHypot + ( HeightEdge1 - HeightHypot ) * FracI;
	}

	return Normal.GetNormalized();
}

LVector3 LHeightMap::GetDataNormal( int I, int J ) const
{
	// find heights
	LVector3 H0 = DataToWorld( I,   J   );
	LVector3 H1 = DataToWorld( I,   J + 1 );
	LVector3 H2 = DataToWorld( I + 1, J   );

	LVector3 Edge1 = H0 - H2;
	LVector3 Edge2 = H0 - H1;

	LVector3 Normal = ( Edge2.Cross( Edge1 ) ).GetNormalized();

	if ( Normal.Z < 0 )
	{
		Normal *= -1.0f;
	}

	return Normal;
}
/*
float Interpolate( float a, float b, float x)
{
   float f = 1.0f - cosf( x * PI );

   f *= 0.5f;

   return ( a * ( 1.0f - f ) + b * f );
}
*/
/*

{
   Normal.X = Height[X-1][Y] - Height[X+1][Y]
   Normal.Y = Height[X][Y-1] - Height[X][Y+1]
   Normal.Z = Distance_Between_Grid_Samples * 2.0f

   Normal.Normalise()
}

float GetInterpolatedHeight( float x, float z)
{
   int I;
   int J;
   float FracI;
   float FracJ;

   WorldToData( X, Y, I, J, FracI, FracJ );

   float v1 = DataToWorld( I,   J   ).Z
   float v2 = DataToWorld( I+1, J   ).Z
   float v3 = DataToWorld( I,   J+1 ).Z
   float v4 = DataToWorld( I+1, J+1 ).Z

   float v12 = Interpolate(v1, v2, FracJ);
   float v34 = Interpolate(v2, v3, FracI);

   float v1234 = Interpolate(v12, v34, fz1);

   return v1234;

}
*/

void LHeightMap::SetWorldAABB( LAABoundingBox Box )
{
	UpdateBoundingBox();

	LVector3 NewDim = Box.FMax - Box.FMin;
	LVector3 OldDim = FBoundingBox.FMax - FBoundingBox.FMin;

	if ( NewDim.X == 0 )
	{
		NewDim.X = 1.0f;
	}

	if ( NewDim.Y == 0 )
	{
		NewDim.Y = 1.0f;
	}

	if ( NewDim.Z == 0 )
	{
		NewDim.Z = 1.0f;
	}

	if ( OldDim.X == 0 )
	{
		OldDim.X = 1.0f;
	}

	if ( OldDim.Y == 0 )
	{
		OldDim.Y = 1.0f;
	}

	if ( OldDim.Z == 0 )
	{
		OldDim.Z = 1.0f;
	}

	FOrigin = Box.FMin;

	FScales = OldDim / NewDim;

	FGranularity = LVector3( 1, 1, 1 ) / FScales;

	FBoundingBox = Box;
}

void LHeightMap::SetGranularity( const LVector3& Granularity )
{
	FGranularity = Granularity;

	FScales = LVector3( 1, 1, 1 ) / FGranularity;
}

LVector3 LHeightMap::GetGranularity() const
{
	return FGranularity;
}

void LHeightMap::UpdateBoundingBox()
{
	int MaxHeight = FHeightDataProvider->GetMaximalHeight( FDataX1,
	                                                       FDataY1,
	                                                       FDataX2,
	                                                       FDataY2 );
	int MinHeight = FHeightDataProvider->GetMinimalHeight( FDataX1,
	                                                       FDataY1,
	                                                       FDataX2,
	                                                       FDataY2 );

	FBoundingBox.FMin = LVector3( 0, 0, MinHeight );
	FBoundingBox.FMax = LVector3( FDataX2 - FDataX1, FDataY2 - FDataY1, MaxHeight );
}

int LHeightMap::GetDataSizeX() const
{
	return FDataX2 - FDataX1;
}

int LHeightMap::GetDataSizeY() const
{
	return FDataY2 - FDataY1;
}

void LHeightMap::WorldToLocal( float WorldX, float WorldY,
                               float& X, float& Y ) const
{
	X = ( WorldX - FOrigin.X ) * FScales.X;
	Y = ( WorldY - FOrigin.Y ) * FScales.Y;
}

void LHeightMap::WorldToData( float WorldX, float WorldY,
                              int& I, int& J,
                              float& FracI, float& FracJ ) const
{
	float LocalX = ( WorldX - FOrigin.X ) * FScales.X;
	float LocalY = ( WorldY - FOrigin.Y ) * FScales.Y;

	I = static_cast<int>( floor( LocalX ) );
	J = static_cast<int>( floor( LocalY ) );

	FracI = LocalX - I;
	FracJ = LocalY - J;
}

void LHeightMap::LocalToData( float LocalX, float LocalY,
                              int& I, int& J,
                              float& FracI, float& FracJ ) const
{
	I = static_cast<int>( floor( LocalX ) );
	J = static_cast<int>( floor( LocalY ) );

	FracI = LocalX - I;
	FracJ = LocalY - J;
}

void LHeightMap::LocalToWorld( float LocalX, float LocalY,
                               float& X, float& Y ) const
{
	X = LocalX * FGranularity.X + FOrigin.X;
	Y = LocalY * FGranularity.Y + FOrigin.Y;
}

LVector3 LHeightMap::DataToWorld( int I, int J ) const
{
	LVector3 World( FGranularity.X * I + FOrigin.X,
	                FGranularity.Y * J + FOrigin.Y,
	                FGranularity.Z * FHeightDataProvider->GetHeight( I, J ) + FOrigin.Z
	              );

	return World;
}

void LHeightMap::Level( float X, float Y,
                        float DesiredHeight,
                        float BrushRadius )
{
}

void LHeightMap::Deform( float X, float Y,
                         float DeltaHeight,
                         float BrushSize,
                         iKernel2D* Kernel2D )
{
//   int I, J;
//   float FracI, FracJ;

//   WorldToData( X, Y, I, J, FracI, FracJ );

	// number of x and y points
	int NumX, NumY;
	NumX = static_cast<int>( BrushSize / FGranularity.X );
	NumY = static_cast<int>( BrushSize / FGranularity.Y );

	for ( int i = -NumX ; i <= NumX ; i++ )
	{
		for ( int j = -NumY ; j <= NumY ; j++ )
		{
			float PtX = X + i * FGranularity.X;
			float PtY = Y + j * FGranularity.Y;

			float PtOfs = Kernel2D->GetValue( PtX - X, PtY - Y );

			int PtI, PtJ;
			float PtFracI, PtFracJ;

			WorldToData( PtX, PtY, PtI, PtJ, PtFracI, PtFracJ );

			float Height = FGranularity.Z * FHeightDataProvider->GetHeight( PtI, PtJ ) + DeltaHeight * PtOfs;

			FHeightDataProvider->SetHeight( PtI, PtJ, static_cast<int>( Height * FScales.Z ) );
		}
	}

//   float Height = FGranularity.Z * FHeightDataProvider->GetHeight( I, J ) + DeltaHeight;

//   FHeightDataProvider->SetHeight( I, J, static_cast<int>( Height * FScales.Z ) );
}

bool LHeightMap::IntersectRay( const LVector3& Origin,
                               const LVector3& Ray,
                               LVector3& Intersection ) const
{
	LVector3 Pos = Origin;
	LVector3 Dir = 0.2f * Ray;

	FIXME( "optimize" )

	for ( int i = 0 ; i < 750; i++ )
	{
		float Z = GetHeight( Pos.X, Pos.Y );

		if ( Pos.Z < Z )
		{
			Intersection = Pos;

			return true;
		}

		Pos += Dir;
	}

	return false;
}

/*
 * 28/04/2007
     Heights and normals interpolation improved
 * 24/02/2007
     Bug fixed in GetHeight()
 * 13/02/2007
     IntersectRay()
 * 10/02/2007
     Normals interpolation
 * 28/01/2007
     World in progress
 * 24/01/2007
     It's here
*/
