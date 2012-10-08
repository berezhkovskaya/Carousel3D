/**
 * \file VisualProjector.h
 * \brief Visual projector
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualProjector__h__included__
#define __VisualProjector__h__included__

// TO DO : handle FPosition/FOrientation changes and recalculate frustum points etc.

class iCameraPositioner;
class clGeom;

#include "VisualObject.h"
#include "Math/LProjection.h"

/**
   \brief Projector + Frustum + Optional positioner

   Superclass for lights and cameras. Holds the information
   about current projection and frustum.

   Optional positioner can be set to control the projector's transform
*/
class scriptfinal netexportable clVisualProjector : public clVisualObject
{
public:
	clVisualProjector();
	virtual ~clVisualProjector();

	/// Add a debug light/camera proxy. Actual mesh is defined in
	virtual void AddDebugGeometry( clScene* S );
	virtual void RemoveDebugGeometry( clScene* S );

	/// Get the name of debug mesh for this object
	virtual LString GetDebugMeshName() { return ""; }

	virtual LMatrix4         GetProjection() const { return FProjection; }
	virtual void             SetProjection( const LMatrix4& Projection );

	virtual void             SetProjectionType( int ProjType ); // LProjectionType
	virtual LProjectionType  GetProjectionType() const { return FProjectionType; }

	virtual void             CalculateTransformFromDirUpTarget();

	virtual void     SetTransform( const LMatrix4& Mtx );
	virtual void     SetAngles( const LVector3& A );
	virtual void     SetPosition( const LVector3& P );

	SERIALIZABLE_CLASS()

	virtual void SplitFrustum( std::vector<float>& Dists );
	virtual void SplitMatrices( std::vector<LMatrix4>* Matrices, int NumSplits, const LVector3& CamPos, const LMatrix4& CamProj ) {};

	virtual LMatrix4 CalculateCropMatrix();

	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );

	void DebugRenderFrustumNormals( const LMatrix4& _proj, const LMatrix4& _view );
	void DebugRenderFrustum( const LMatrix4& _proj, const LMatrix4& _view );

	virtual void Refresh() { UpdateProjection(); }

	/// Set optional positioner for this projector
	scriptmethod void SetPositioner( iCameraPositioner* Positioner ) { FPositioner = Positioner; FUseTarget = false; }

	/// Optionally fetch the transform from campositioner
	virtual void Update( float dt );

public:

	void RecalculatePlanes();

	/// Recalculate projection matrix, corner points and clipping planes
	scriptmethod void UpdateProjection();

	/**
	   camera-like parameters

	   Direction and Target are complementary. If someone sets the target, then direction and distance are calculated and vice versa
	*/
#pragma region Projector properties

	/* Property(Category="Projector properties", Description="Use target", Name=UseTarget, Type=bool, FieldName=FUseTarget) */

	/// "Free" or "Targeted" light
	bool FUseTarget;

	/* Property(Category="Projector properties", Description="Up vector", Name=Up, Type=vec3, FieldName=FUpVector) */
	LVector3 FUpVector;
	/* Property(Category="Projector properties", Description="Side vector", Name=Side, Type=vec3, FieldName=FSideVector) */
	LVector3 FSideVector;
	/* Property(Category="Projector properties", Description="Target point", Name=Target, Type=vec3, Setter=SetTarget, Getter=GetTarget) */
	LVector3 FTarget;
	/* Property(Category="Projector properties", Description="Direction", Name=Direction, Type=vec3, Setter=SetDirection, Getter=GetDirection) */
	LVector3 FDirection;
	/* Property(Category="Projector properties", Description="Distance to target", Name=DistanceToTarget, Type=float, Setter=SetDistanceToTarget, Getter=GetDistanceToTarget) */
	float   FDistanceToTarget;

	void SetDirection( const LVector3& ADirection );
	void SetDistanceToTarget( float ADistance );
	void SetTarget( const LVector3& ATarget );

	inline LVector3 GetDirection() const { return FDirection; }
	inline LVector3 GetTarget() const { return FTarget; }
	inline float   GetDistanceToTarget() const { return FDistanceToTarget; }
#pragma endregion

public:
	/* Property(Category="Projection parameters", Description="Projection matrix", Name=Projection, Type=mtx4, Setter=SetProjection, Getter=GetProjection) */
	LMatrix4 FProjection;
	/// TODO : LProjectionType to .NET export
	/* Property(Category="Projection parameters", Description="Projection type", Name=ProjectionType, Type=LProjectionType, Setter=SetProjectionType, Getter=GetProjectionType, ToStringConverter=LStr::ProjectionTypeToStr, FromStringConverter=LStr::StrToProjectionType) */
	LProjectionType FProjectionType;

#pragma region Parameters for orthographic projection
	/* Property(Category="Orthographic projection parameters", Description="Left border", Name=Left, Type=float, Setter=SetLeft_Internal, Getter=GetLeft_Internal) */
	float FLeft;
	/* Property(Category="Orthographic projection parameters", Description="Right border", Name=Right, Type=float, Setter=SetRight_Internal, Getter=GetRight_Internal) */
	float FRight;
	/* Property(Category="Orthographic projection parameters", Description="Bottom border", Name=Bottom, Type=float, Setter=SetBottom_Internal, Getter=GetBottom_Internal) */
	float FBottom;
	/* Property(Category="Orthographic projection parameters", Description="Top border", Name=Top, Type=float, Setter=SetTop_Internal, Getter=GetTop_Internal) */
	float FTop;
#pragma endregion

#pragma region Generic projection parameters
	/* Property(Category="Projection parameters", Description="Far clip plane", Name=Far, Type=float, Setter=SetFar_Internal, Getter=GetFar_Internal) */
	float FFarClipPlane;
	/* Property(Category="Projection parameters", Description="Near clip plane", Name=Near, Type=float, Setter=SetNear_Internal, Getter=GetNear_Internal) */
	float FNearClipPlane;
#pragma endregion

#pragma region Parameters for the perspective projection
	/* Property(Category="Perspective projection parameters", Description="Aspect ratio", Name=Aspect, Type=float, Setter=SetAspect_Internal, Getter=GetAspect_Internal) */
	float FAspectRatio;
	/* Property(Category="Perspective projection parameters", Description="Field of view", Name=FOV, Type=float, Setter=SetFOV_Internal, Getter=GetFOV_Internal) */
	float FFieldOfView;
#pragma endregion

public:
	// Property(Category="Debugging", Description="Corner points", Name=CornerPoint, Type=vec3, IndexType=int, Getter=GetCornerPoint)

	/// Corner points for debug rendering
	LVector3    FCornerPoints[8];

	inline LVector3 GetCornerPoint( int PtIdx ) const { return FCornerPoints[PtIdx]; }

	/// Clipping planes
	LVector4    FPlanes[6];
private:

	void CalcOrthographicFrustumPoints( float L, float R, float B, float T, float N, float F, const LVector3& Pos, const LVector3& To, const LVector3& Up );
	void CalcPerspectiveFrustumPoints( float Angle, float Ratio, float NearCP, float FarCP, const LVector3& Pos, const LVector3& To, const LVector3& Up );
	void CalcFrustumInBasis( const LVector3& Pos, const LVector3& To, const LVector3& Up, float fw, float fh, float nw, float nh );
public:
	// The following methods are public only to allow access in C++/CLI
#pragma region Internal getters and setters
	inline void SetLeft_Internal( float ALeft ) { FLeft = ALeft; if ( FProjectionType == PROJECTION_ORTHOGRAPHIC ) { UpdateProjection(); } }
	inline void SetRight_Internal( float ARight ) { FRight = ARight; if ( FProjectionType == PROJECTION_ORTHOGRAPHIC ) { UpdateProjection(); } }
	inline void SetBottom_Internal( float ABottom ) { FBottom = ABottom; if ( FProjectionType == PROJECTION_ORTHOGRAPHIC ) { UpdateProjection(); } }
	inline void SetTop_Internal( float ATop ) { FTop = ATop; if ( FProjectionType == PROJECTION_ORTHOGRAPHIC ) { UpdateProjection(); } }
	inline void SetFar_Internal( float AFar ) { FFarClipPlane = AFar; UpdateProjection(); }
	inline void SetNear_Internal( float ANear ) { FNearClipPlane = ANear; UpdateProjection(); }
	inline void SetAspect_Internal( float AFar ) { FAspectRatio = AFar; if ( FProjectionType == PROJECTION_PERSPECTIVE ) { UpdateProjection(); } }
	inline void SetFOV_Internal( float ANear ) { FFieldOfView = ANear; if ( FProjectionType == PROJECTION_PERSPECTIVE ) { UpdateProjection(); } }

	inline float GetLeft_Internal() const { return FLeft; }
	inline float GetRight_Internal() const { return FRight; }
	inline float GetTop_Internal() const { return FTop; }
	inline float GetBottom_Internal() const { return FBottom; }
	inline float GetAspect_Internal() const { return FAspectRatio; }
	inline float GetFOV_Internal() const { return FFieldOfView; }
	inline float GetFar_Internal() const { return FFarClipPlane; }
	inline float GetNear_Internal() const { return FNearClipPlane; }
#pragma endregion

	/// Optional camera positioner
	iCameraPositioner* FPositioner;

protected:
	/// Internal ID of the proxy Geom in DebugScene
	int FDebugGeomID;

	/// Actual instance of debug geom
	clGeom* FDebugGeom;
};

class scriptfinal netexportable clVisualLight : public clVisualProjector
{
public:
	clVisualLight();
	virtual ~clVisualLight();

	/// Upload new parameters to the scene
	virtual void SyncWithScene( clScene* Scene );

	/** Property(Name=LightEnabled,  Type=bool, Getter=GetEnabled,       Setter=SetEnabled) */
	/** Property(Name=AmbientColor,  Type=vec4, Getter=GetAmbientColor,  Setter=SetAmbientColor)  */
	/** Property(Name=DiffuseColor,  Type=vec4, Getter=GetDiffuseColor,  Setter=SetDiffuseColor)  */
	/** Property(Name=SpecularColor, Type=vec4, Getter=GetSpecularColor, Setter=SetSpecularColor) */
	/** Property(Name=Attenuations,  Type=vec3, Getter=GetAttenuations,  Setter=SetAttenuations)  */

	scriptmethod bool GetEnabled() const { return FEnabled; }
	scriptmethod vec4 GetAmbientColor() const { return FAmbientColor; }
	scriptmethod vec4 GetDiffuseColor() const { return FDiffuseColor; }
	scriptmethod vec4 GetSpecularColor() const { return FSpecularColor; }
	scriptmethod vec3 GetAttenuations() const { return FAttenuations; }

	scriptmethod void SetEnabled( bool flag ) { FEnabled = flag; UpdateLight(); }
	scriptmethod void SetAmbientColor( const vec4& Col ) { FAmbientColor = Col; UpdateLight(); }
	scriptmethod void SetDiffuseColor( const vec4& Col ) { FDiffuseColor = Col; UpdateLight(); }
	scriptmethod void SetSpecularColor( const vec4& Col ) { FSpecularColor = Col; UpdateLight(); }
	scriptmethod void SetAttenuations( const vec3& Att ) { FAttenuations = Att; UpdateLight(); }

	virtual void UpdateTransform();

	virtual void UpdateLight();

public:
	/// Index in the list of clScene. Auto-adjusted while something is added or removed from scene
	int FInternalID;

	/// 'Enable' flag
	bool FEnabled;

	/// Ambient light color
	vec4 FAmbientColor;

	/// Diffuse light color
	vec4 FDiffuseColor;

	/// Specular light color
	vec4 FSpecularColor;

	/// Attenuations (const, linear, quadratic)
	vec3 FAttenuations;
};

class scriptfinal netexportable clVisualSpotLight : public clVisualLight
{
public:
	clVisualSpotLight();
	virtual ~clVisualSpotLight();

	virtual void AddToScene( clVisualScene* VS );
	virtual void RemoveFromScene( clVisualScene* VS );

	virtual void SyncWithScene( clScene* Scene );

	virtual LString GetDebugMeshName() { return "Models/SpotLight.ase"; }

	/** Property(Category="Spot light parameters", Description="Maximum value of NdotL cosine for the spot", Name=SpotCosCutoff, Type=float, Getter=GetSpotCosCutoff, Setter=SetSpotCosCutoff) */
	/** Property(Category="Spot light parameters", Description="Spot exponent", Name=SpotExponent, Type=float, Getter=GetSpotExponent, Setter=SetSpotExponent) */

	scriptmethod float GetSpotCosCutoff() const { return FSpotParams[0]; }
	scriptmethod float GetSpotExponent() const { return FSpotParams[1]; }

	scriptmethod void SetSpotCosCutoff( float CutOff ) { FSpotParams[0] = CutOff; UpdateLight(); }
	scriptmethod void SetSpotExponent( float SpotExp ) { FSpotParams[1] = SpotExp; UpdateLight(); }

public:
	vec3 FSpotParams;
};

class scriptfinal netexportable clVisualPointLight : public clVisualLight
{
public:
	clVisualPointLight();
	virtual ~clVisualPointLight();

	virtual void AddToScene( clVisualScene* VS );
	virtual void RemoveFromScene( clVisualScene* VS );

	virtual void SyncWithScene( clScene* Scene );

	virtual LString GetDebugMeshName() { return "Models/SpotLight.ase"; }
};

class scriptfinal netexportable clVisualDirectionalLight : public clVisualLight
{
public:
	clVisualDirectionalLight();
	virtual ~clVisualDirectionalLight();

	virtual void AddToScene( clVisualScene* VS );
	virtual void RemoveFromScene( clVisualScene* VS );

	virtual void SyncWithScene( clScene* Scene );

	virtual LString GetDebugMeshName() { return "Models/DirLight.ase"; }
};

/// The light which implements CSM shadowing technique
class scriptfinal netexportable clVisualCSMDirectionalLight: public clVisualDirectionalLight
{
public:
	clVisualCSMDirectionalLight();
	virtual ~clVisualCSMDirectionalLight();
};

#endif

/*
 * 12/06/2010
     Log section added
*/
