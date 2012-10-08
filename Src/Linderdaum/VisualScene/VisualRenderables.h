/**
 * \file VisualRenderables.h
 * \brief Visual renderables
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualRenderables__h__included__
#define __VisualRenderables__h__included__

#include "VisualObject.h"

#include "Math/LAABB.h"

#include "Geometry/Geom.h"
#include "Scene/Material.h"

class clUVSurfaceGenerator;
class clMesh;

class netexportable scriptfinal clVisualRenderable : public clVisualObject
{
public:
	clVisualRenderable() {}
	virtual ~clVisualRenderable() {}

	SERIALIZABLE_CLASS()

	virtual void SetTransform( const LMatrix4& Mtx ) { clVisualObject::SetTransform( Mtx ); }
	virtual void UpdateTransform() { clVisualObject::UpdateTransform(); }
};

class netexportable scriptfinal clVisualGeom : public clVisualRenderable
{
public:
	clVisualGeom();
	virtual ~clVisualGeom();

	SERIALIZABLE_CLASS()

	/// Initial setup
	virtual void SetGeom( clGeom* G );

	/// Update the material for the geom
	virtual void UpdateMaterial( clMaterial* M );

	/// Update the current geometry
	virtual void UpdateGeom( clGeom* G );

	virtual void SetMaterial( clMaterial* M ) { FMaterial = M; }
	virtual clMaterial* GetMaterial() { return FMaterial; }

	virtual void     SetTransform( const LMatrix4& Mtx );
	virtual LMatrix4 GetTransform() const;

	/// Deleagates for the Scene::SetAnimation
	virtual void     SetAnimation( const LString& AnimName );
	virtual void     SetAnimationSpeed( float Speed );

	/* Property(Name="Material", Type=string, Getter=GetMaterialName, Setter=SetMaterialName) */

	virtual LString GetMaterialName() const { return FMaterialName; }
	virtual void SetMaterialName( const LString& MaterialName );

	virtual void AddGeometry( clScene* S );
	virtual void RemoveGeometry( clScene* S );

	virtual bool IntersectsWithRay( const LMatrix4& Proj, const LMatrix4& View, const LVector3& P, const LVector3& Dir, LVector3& isect );

	virtual void UpdateTransform();

protected:
	LString FMaterialName;

	clGeom* FGeom;
	clMaterial* FMaterial;

	int FGeomID;
	int FMatID;
};

class scriptfinal netexportable clVisualMesh : public clVisualGeom
{
public:
	clVisualMesh();
	virtual ~clVisualMesh();

	virtual LString GetMesh() const { return FMeshName; }
	virtual void SetMesh( const LString& MeshName );

	/** Property(Name="Mesh", Type=string, Getter=GetMesh, Setter=SetMesh) */

	SERIALIZABLE_CLASS()
public:
	LString FMeshName;
};

/**
   Serializable Kochanek-Bartels spline curve with unifom tension/bias/continuity
*/
class scriptfinal netexportable clSplineKB : public iObject
{
public:
	clSplineKB();
	virtual ~clSplineKB();

	scriptmethod LVector3 GetPosition( float t );

	scriptmethod LVector3 GetDerivative( float t );

	scriptmethod LVector3 GetSecondDerivative( float t );

	scriptmethod LMatrix3 GetCanonicalFrame( float t );

	scriptmethod LVector3 GetKnot( int i ) { return R[i]; }

	scriptmethod void SetKnot( int i, const LVector3& P ) { R[i] = P; }

	/** Property(Name=Tension,    Type=float, FieldName=FTension) */
	/** Property(Name=Bias,       Type=float, FieldName=FBias) */
	/** Property(Name=Continuity, Type=float, FieldName=FContinuity) */

public:
	/// Curve tension
	float FTension;

	/// Curve bias
	float FBias;

	/// Curve continuity
	float FContinuity;

	/// List of control points. Indexing is from -1 to N, but in the array everything is in [ 0..M-1 ]
	LArray<LVector3>  R;

	/// List of timestamps
	LArray<float> T;

private:
	int FindSegment( float t );

	LVector3 ValueForSegment( int Set, float t );

	LVector3 DerivativeForSegment( int Set, float t );

	LVector3 SecondDerivativeForSegment( int Seg, float t );
};


/// Kochanek-Bartels spline visualizer
class scriptfinal netexportable clVisualCurve : public clVisualRenderable
{
public:
	clVisualCurve();
	virtual ~clVisualCurve();

	virtual void AddGeometry( clScene* S );
	virtual void RemoveGeometry( clScene* S );

	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );
};


class scriptfinal netexportable clVisualGenerator : public clVisualGeom
{
public:
	clVisualGenerator();
	virtual ~clVisualGenerator();

	virtual void RegenerateSurface();

	virtual void AddGeometry( clScene* S );
	virtual void RemoveGeometry( clScene* S );

	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );

	//vi_rtual void    SetGeneratorType( const LString& GeneratorType );
	//vi_rtual LString GetGeneratorType() const;

	//vi_rtual clUVSurfaceGenerator* GetGenerator();

	/* Pro_perty(Name="GeneratorType", Type=string, Getter=GetGeneratorType, Setter=SetGeneratorType) */

	/* Property(Name=Generator, Type=clUVSurfaceGenerator, FieldName=FGenerator) */

	SERIALIZABLE_CLASS()

public:
	clUVSurfaceGenerator* FGenerator;
//	LString FGeneratorType;
};

class scriptfinal netexportable clVisualDebugItem : public clVisualRenderable
{
public:
	clVisualDebugItem() {}
	virtual ~clVisualDebugItem() {}

	SERIALIZABLE_CLASS()
public:
	/* Property(Name=Color, Type=LVector4, FieldName=FColor) */
	LVector4 FColor;
};

class scriptfinal netexportable clVisualDebugBox : public clVisualDebugItem
{
public:
	clVisualDebugBox() : FBox( vec3( -1, -1, -1 ), vec3( 1, 1, 1 ) ) {}
	virtual ~clVisualDebugBox() {}

	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );

public:
	LAABoundingBox FBox;
};

class scriptfinal netexportable clVisualDebugPlane : public clVisualDebugItem
{
public:
	clVisualDebugPlane() {}
	virtual ~clVisualDebugPlane() {}

	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );
};

/**
   \brief The new-scene-wrapper for the VolumeRenderer

   Supports autocomposition

   (i.e., multiple volumes are sorted, properly rendered to z-buffer in iso-surface mode and finally composed to the scene)
*/
class scriptfinal netexportable clVisualVolume : public clVisualRenderable
{
public:
	clVisualVolume() {}
	virtual ~clVisualVolume() {}

	SERIALIZABLE_CLASS()
};

/// Some particle system, hopefully, GPU-based
class scriptfinal netexportable clVisualParticles : public clVisualRenderable
{
public:
	clVisualParticles() {}
	virtual ~clVisualParticles() {}

	SERIALIZABLE_CLASS()
};

#endif


/*
 * 12/06/2010
     Log section added
*/
