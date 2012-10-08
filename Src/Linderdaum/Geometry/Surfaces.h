/**
 * \file Surfaces.h
 * \brief Analytic surfaces generator
 * \version 0.5.98
 * \date 17/10/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __clSurfaceGenerator__h_included__
#define __clSurfaceGenerator__h_included__

#include "Core/iObject.h"

#include "Math/LVector.h"

#include "Images/Bitmap.h"

class clGeom;
class clMesh;
class clVertexAttribs;

class LCurve;

#undef LOG_DUMP

/**
   Generates the surface using the [MinU..MaxU] x [MinV..MaxV] as a fundamental polygon

   R(u,v) is the analytical representation of the surface
*/
class scriptfinal clUVSurfaceGenerator : public iObject
{
public:
	clUVSurfaceGenerator();
	virtual ~clUVSurfaceGenerator();

	/// Utility function to create geom object
	virtual clGeom* CreateGeom();

	/// The main surface generator routine
	virtual clVertexAttribs* CreateSurface();

	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()
public:
	/// default minimum V
	float MinV;
	/// default maximum V
	float MaxV;
	/// default minimum U
	float MinU;
	/// default maximum U
	float MaxU;

	/// Are the sides equal for MinU == MaxU ?
	bool GlueUEdges;
	/// Are the sides equal for MinV == MaxV ?
	bool GlueVEdges;

	/// number of subdivisions in U coordinate
	int NumU;
	/// number of subdivisions in V coordinate
	int NumV;

public:
	/// Spatial coordinate and tangent vectors at the (u,v) coordinates
	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv ) = 0;

	/// render vertex array with tangent field
	/// DebugRenderTangentField();

	/**
	   Properties
	*/

	/* Property(Description="Minimum U", Category="Surface parameters", Name="MinU", Type=float, FieldName=MinU) */
	/* Property(Description="Minimum V", Category="Surface parameters", Name="MinV", Type=float, FieldName=MinV) */
	/* Property(Description="Maximum U", Category="Surface parameters", Name="MaxU", Type=float, FieldName=MaxU) */
	/* Property(Description="Maximum V", Category="Surface parameters", Name="MaxV", Type=float, FieldName=MaxV) */
	/* Property(Description="Glue U edges", Category="Surface parameters", Name="GlueUEdges", Type=bool, FieldName=GlueUEdges) */
	/* Property(Description="Glue V edges", Category="Surface parameters", Name="GlueVEdges", Type=bool, FieldName=GlueVEdges) */
	/* Property(Description="Number of U subdivisions", Category="Surface parameters", Name="NumU", Type=int, FieldName=NumU) */
	/* Property(Description="Number of V subdivisions", Category="Surface parameters", Name="NumV", Type=int, FieldName=NumV) */
};

/**
   Plane with the "origin" at P spanned by V1 and V2 vectors
*/
class scriptfinal clSimplePlane : public clUVSurfaceGenerator
{
public:
	clSimplePlane();
	virtual ~clSimplePlane();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	/** Property(Name=V1, Type=vec3, FieldName=V1) */
	/** Property(Name=V2, Type=vec3, FieldName=V2) */

	/** Property(Name=Origin, Type=vec3, FieldName=P) */

	/// plane vectors
	LVector3 V1, V2;

	/// plane "origin"
	LVector3 P;
};

/// Surface defined by displacement of the C curve along Axis
class scriptfinal clLinearSurface : public clUVSurfaceGenerator
{
public:
	clLinearSurface(): Axis( 0, 0, 1 ), P( 0, 0, 0 )
	{
		GlueVEdges = true;
		GlueUEdges = false;

		MinV = 0.0f;
		MaxV = 10.0f;
		MinU = 0.0f;
		MaxU = 10.0f;
	}
	virtual ~clLinearSurface() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	/** Property(Name=Axis, Type=vec3, FieldName=Axis) */
	/** Property(Name=Origin, Type=vec3, FieldName=P) */

public:
	/// Rotation axis
	LVector3 Axis;

	/// "Origin"
	LVector3 P;

	/// The curve itself
	LCurve* C;
};

/// Surface defined by revolution of the C curve around Axis
class scriptfinal clRevolutionSurface : public clUVSurfaceGenerator
{
public:
	clRevolutionSurface();
	virtual ~clRevolutionSurface() {}

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	/** Property(Name=Axis, Type=vec3, FieldName=Axis) */
	/** Property(Name=Origin, Type=vec3, FieldName=P) */

public:
	/// Rotation axis
	LVector3 Axis;

	/// "Origin"
	LVector3 P;

	/// The curve itself
	LCurve* C;
};

/// Neighbourhood of the source curve
class scriptfinal clTubularNeighbourhood: public clUVSurfaceGenerator
{
public:
	clTubularNeighbourhood()
	{
		NumU = 50;
		NumV = 50;

		GlueUEdges = true;
		GlueVEdges = true;

		TubeRadius = 0.1f;

		MinV = 0.0f;
		MaxV = 2.0f * Math::PI;
		MinU = 0.0f;
		MaxU = 2.0f * Math::PI;
	}

	virtual ~clTubularNeighbourhood() {}

public:
	/// the radius of a tube
	float TubeRadius;

	/// The base curve
	LCurve* C;

	/// the base line, around which the circular segments are built
	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/* Property(Description="Tubes radius", Name="TubeRadius", Type=float, FieldName=TubeRadius) */
};

/**
   Whitney's umbrella

   x = uv
   y = u
   z = v^2
*/
class scriptfinal clWhitneyUmbrella : public clUVSurfaceGenerator
{
public:
	clWhitneyUmbrella();
	virtual ~clWhitneyUmbrella();

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS();
};

/**
   Revolution paraboloid

   x = u
   y = v
   z = u^2 + v^2
*/
class scriptfinal clRevolutionParaboloid : public clUVSurfaceGenerator
{
public:
	clRevolutionParaboloid();
	virtual ~clRevolutionParaboloid();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );
};

/**
   Hyperbolic paraboloid

   x = u
   y = v
   z = u^2 - v^2
*/
class scriptfinal clHyperbolicParaboloid : public clUVSurfaceGenerator
{
public:
	clHyperbolicParaboloid();
	virtual ~clHyperbolicParaboloid();

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );
};

/**
   Monkey saddle

   x = u
   y = v
   z = u^3 - 3 * u * v^2
*/
class scriptfinal clMonkeySaddle : public clUVSurfaceGenerator
{
public:
	clMonkeySaddle();
	virtual ~clMonkeySaddle();

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()
};


/**
   Moebius strip

   (1+0.5*v*cos(0.5*u))*cos(u)
   (1+0.5*v*cos(0.5*u))*sin(u)
   0.5*v*sin(0.5*u)

   [0..2*PI] x [-1..+1]
*/
class scriptfinal clMoebiusBand : public clUVSurfaceGenerator
{
public:
	clMoebiusBand();
	virtual ~clMoebiusBand();

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()
};

/**
   'Standard' immersion of Klein bottle
*/
class scriptfinal clKleinBottle : public clUVSurfaceGenerator
{
public:
	clKleinBottle();
	virtual ~clKleinBottle();

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()
};

/**
   Torus knot surface for the pair of cosimple (p,q) integers

   (2,3) is the treefoil
*/
class scriptfinal clTorusKnot : public clUVSurfaceGenerator
{
public:
	clTorusKnot();
	virtual ~clTorusKnot();

	/// radius of the base torus
	float BaseRadius;

	/// smaller radius of the torus
	float SegmentRadius;

	/// the radius of a tube
	float TubeRadius;

	/// torus knot parameters
	int p, q;

	/// the base line, around which the circular segments are built
	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	virtual void AfterLoad();
	virtual void BeforeSave();

	/* Property(Description="Base radius", Category="Torus knot parameters", Name="BaseRadius", Type=float, FieldName=BaseRadius) */
	/* Property(Description="Segment radius", Category="Torus knot parameters", Name="SegmentRadius", Type=float, FieldName=SegmentRadius) */
	/* Property(Description="Tubes radius", Category="Torus knot parameters", Name="TubeRadius", Type=float, FieldName=TubeRadius) */
	/* Property(Description="P coefficient", Category="Torus knot parameters", Name="p", Type=int, FieldName=p) */
	/* Property(Description="Q coefficient", Category="Torus knot parameters", Name="q", Type=int, FieldName=q) */
};

class scriptfinal clKleinBottle2 : public clUVSurfaceGenerator
{
public:
	clKleinBottle2();
	virtual ~clKleinBottle2();

	float a, b, c, d;

	/// the base line, around which the circular segments are built
	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/* Property(Description="a", Category="Bottle parameters", Name="a", Type=float, FieldName=a)          */
	/* Property(Description="b", Category="Bottle parameters", Name="b", Type=float, FieldName=b)          */
	/* Property(Description="c", Category="Bottle parameters", Name="c", Type=float, FieldName=c)          */
	/* Property(Description="d", Category="Bottle parameters", Name="d", Type=float, FieldName=d)          */
};

class scriptfinal clMetricScrewGenerator : public clUVSurfaceGenerator
{
public:
	clMetricScrewGenerator();
	virtual ~clMetricScrewGenerator();

	// "local", non-metric, screw parameters
	float FD;
	float FR;
	float FH;

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS()

	/* Property(Description="Pitch", Category="Screw parameters", Name="R", Type=float, FieldName=FR)          */
	/* Property(Description="Inner diameter", Category="Screw parameters", Name="D", Type=float, FieldName=FD) */
	/* Property(Description="Rake height", Category="Screw parameters", Name="H", Type=float, FieldName=FH)    */
};

/// http://en.wikipedia.org/wiki/Superformula
class scriptfinal netexportable clSuperShapeGenerator : public clUVSurfaceGenerator
{
public:
	clSuperShapeGenerator();
	virtual ~clSuperShapeGenerator();

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv );

	/** Property(Description="A1", Category="Shape parameters", Name="A1", Type=float, FieldName=FA_1) */
	/** Property(Description="A2", Category="Shape parameters", Name="A2", Type=float, FieldName=FA_2) */
	/** Property(Description="B1", Category="Shape parameters", Name="B1", Type=float, FieldName=FB_1) */
	/** Property(Description="B2", Category="Shape parameters", Name="B2", Type=float, FieldName=FB_2) */

	/** Property(Description="M_1",  Category="Shape parameters", Name="M_1",  Type=int, FieldName=FM_1)  */
	/** Property(Description="N1_1", Category="Shape parameters", Name="N1_1", Type=int, FieldName=FN1_1) */
	/** Property(Description="N2_1", Category="Shape parameters", Name="N2_1", Type=int, FieldName=FN2_1) */
	/** Property(Description="N3_1", Category="Shape parameters", Name="N3_1", Type=int, FieldName=FN3_1) */

	/** Property(Description="M_2",  Category="Shape parameters", Name="M_2",  Type=int, FieldName=FM_2)  */
	/** Property(Description="N1_2", Category="Shape parameters", Name="N1_2", Type=int, FieldName=FN1_2) */
	/** Property(Description="N2_2", Category="Shape parameters", Name="N2_2", Type=int, FieldName=FN2_2) */
	/** Property(Description="N3_2", Category="Shape parameters", Name="N3_2", Type=int, FieldName=FN3_2) */

	/** Property(Description="Toroidal or spherical switch", Category="Shape parameters", Name="IsToroidal",  Type=bool,  FieldName=FToroidal) */

public:
	/// Integer parameters
	int  FM_1, FN1_1, FN2_1, FN3_1;

	int  FM_2, FN1_2, FN2_2, FN3_2;

	/// Radii
	float FA_1, FB_1, FA_2, FB_2;

	/// False is spherical, True is toroidal
	bool FToroidal;

	static void R_dR( float& R, float& dR, float A, float B, int M, int N1, int N2, int N3, float Phi );
};

/**
   \brief Heightmap geometry generator

   Takes a greyscale bitmap as an input
   Calculates dR/du and dR/dv for each point.

   (u,v) pairs in [MinU..MaxU]x[MinV..MaxV] range are mapped to [0..W-1]x[0..H-1]
*/
class scriptfinal clHeightmapGenerator : public clUVSurfaceGenerator
{
public:
	clHeightmapGenerator(): FHeightMap( NULL )
	{
		NumU = NumV = 128;
		MinU = MinV = 0.0f;
		MaxU = MaxV = 128.0f;

		FHeightMin = 0.0f;
		FHeightMax = 1.0f;
	}

	scriptmethod void SetHeightmap( clBitmap* Bmp ) { FHeightMap = Bmp; }

	// Precalculate some stuff before actual geometry generation
	virtual clVertexAttribs* CreateSurface()
	{
		if ( NumU == 0 || NumV == 0 ) { return NULL; }

		float URange = ( MaxU - MinU );
		float VRange = ( MaxV - MinV );

		du = URange / static_cast<float>( NumU );
		dv = VRange / static_cast<float>( NumV );

		return clUVSurfaceGenerator::CreateSurface();
	}

	inline float GetHeightFor( float u, float v )
	{
		float U = ( u - MinU ) / ( MaxU - MinU );
		float V = ( v - MinV ) / ( MaxV - MinV );

		int i = Math::Clamp( ( int )( U * ( float )FHeightMap->FBitmapParams.FWidth ), 0, FHeightMap->FBitmapParams.FWidth );
		int j = Math::Clamp( ( int )( V * ( float )FHeightMap->FBitmapParams.FHeight ), 0, FHeightMap->FBitmapParams.FHeight );

		vec4 PP = FHeightMap->GetPixel( i, j, 0 );

		return FHeightMin + PP.X * ( FHeightMax - FHeightMin );
	}

	virtual void R_and_dR( float u, float v, LVector3& R, LVector3& dRdu, LVector3& dRdv )
	{
		float h = GetHeightFor( u, v );

		R = vec3( u, v, h );

		float hl = GetHeightFor( u - du, v );
		float hr = GetHeightFor( u + du, v );
		float hu = GetHeightFor( u, v - dv );
		float hd = GetHeightFor( u, v + dv );

		dRdu = vec3( 1.0f, 0.0f, ( hr - hl ) / ( 2.0f * du ) );
		dRdv = vec3( 0.0f, 1.0f, ( hu - hd ) / ( 2.0f * dv ) );
	}

	float FHeightMin;
	float FHeightMax;

private:
	float du, dv;

	clBitmap* FHeightMap;
};

#endif

/*
 * 07/10/2010
     Log section added
*/
