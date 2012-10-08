/**
 * \file VisualObject.h
 * \brief Visual object
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualObject__h__included__
#define __VisualObject__h__included__

#include "Core/iObject.h"

#include "Math/LMatrix.h"
#include "Math/LTransform.h"

class clVisualScene;

class clScene;

/// The basic element of the VisualScene
class scriptfinal netexportable clVisualObject : public iObject
{
public:
	clVisualObject();
	virtual ~clVisualObject();

	virtual bool BeginLoad();
	virtual bool EndLoad();

	SERIALIZABLE_CLASS()

#pragma region Internal scene management

public:
	virtual void AddToScene( clVisualScene* VS );
	virtual void RemoveFromScene( clVisualScene* VS );

	/// Add some debug geometry when entring the scene "OnSceneEnter"
	virtual void AddDebugGeometry( clScene* S ) {}

	/// Remove added debug geometry when entring the scene "OnSceneEnter"
	virtual void RemoveDebugGeometry( clScene* S ) {}

	/// Add renderables to the scene
	virtual void AddGeometry( clScene* S ) {}

	/// Remove renderables from the scene
	virtual void RemoveGeometry( clScene* S ) {}

	/// Link to parent scene
	virtual void SetScene( clVisualScene* Scene ) { FScene = Scene; }

#pragma endregion

	/// Any kind of debug information rendering
	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View ) {}

	/// Render particle systems to noise buffers, create debug shadow maps etc.
	virtual void RenderAuxBuffers( const LMatrix4& Proj, const LMatrix4& View ) {}

	/// Update geometry somehow using info from camera (?) or just afer parameter change
	virtual void Refresh() {}

	/// Hmmm.... Actor/Entity cloning. Yeah, I know.
	virtual void Update( float dt ) {}

#pragma region Positioning and transformation

	/// Make a local rotation (multiply by the specified matrix)
	virtual void     MakeLocalRotation( const LMatrix4& Mtx );

	/// Multiply by matrix
	virtual void     MakeGlobalTranform( const LMatrix4& Mtx );

	virtual void     SetTransform( const LMatrix4& Mtx );
	virtual LMatrix4 GetTransform() const;

	virtual void     SetAngles( const LVector3& A );
	virtual LVector3 GetAngles() const;

	virtual void     SetPosition( const LVector3& P );
	virtual LVector3 GetPosition() const;

	virtual void               SetAngleSystem( LEulerAngleSystem AS ) { FTransform.SetAngleSystem( AS ); }
	virtual LEulerAngleSystem  GetAngleSystem() const { return FTransform.GetAngleSystem(); }

	/* Property(Category="Transformation", Description="Position", Name=Position, Type=vec3, Getter=GetPosition, Setter=SetPosition) */
	/* Property(Category="Transformation", Description="Orientation angles", Name=Angles, Type=vec3, Getter=GetAngles, Setter=SetAngles) */

	/// TODO : EulerAngleSystem to .NET export
	/* Pro_perty(Category="Transformation", Description="Angular system", Name=AngleSystem, Type=LEulerAngleSystem, Getter=GetAngleSystem, Setter=SetAngleSystem) */

	/// Called after something has changed in transformation
	virtual void UpdateTransform();

	/**
	   To TypeConverters :

	   LEulerAngleSystem  scalar    AngleSystemToStr StrToAngleSystem

	   To NetTypes :

	   LEulerAngleSystem  scalar    AngleSystemToString
	*/

	/// TODO : Heading, Up, Side

#pragma endregion

#pragma region Intersection testing/picking

	virtual bool IntersectsWithRay( const LMatrix4& Proj, const LMatrix4& View, const LVector3& P, const LVector3& Dir, LVector3& isect );

	virtual void   SetBoundingRadius( float R ) { FBoundingRadius = R; }
	virtual float GetBoundingRadius() const { return FBoundingRadius; }
#pragma endregion

#pragma region Event handling

#pragma endregion

protected:
	/// The transformation for this object
	LTransform FTransform;

protected:
	/// Link to the container
	clVisualScene* FScene;

	/// cached bounding sphere radius
	float FBoundingRadius;
};

#endif

/*
 * 09/12/2010
     Log section added
*/
