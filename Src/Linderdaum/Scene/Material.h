/**
 * \file Material.h
 * \brief
 * \version 0.5.91
 * \date 15/03/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clMaterial_
#define _clMaterial_

#include "Resources/iResource.h"

#include "Math/LVector.h"

#include <map>

class iVertexArray;
class clRenderState;
class iGPUBuffer;
class clSceneNode;
class iShaderProgram;
class iTexture;

// slot 0 is reserved for GPU skinning
enum LMatSysSlot
{
	L_MAT_SLOT_AMBIENT  = 1,
	L_MAT_SLOT_DIFFUSE  = 2,
	L_MAT_SLOT_SPECULAR = 3,
	L_MAT_SLOT_BUMP     = 4,
	L_MAT_SLOT_NOISE    = 5,
	L_MAT_SLOT_REFLECT  = 6
};

/// Type of passes for material and bit mask for pass (used in clScene)
enum LMatSysPass
{
	L_MATSYS_PASS_NORMAL          = 1,
	L_MATSYS_PASS_SHADOW          = 2,
	L_MATSYS_PASS_REFL            = 4,
	L_MATSYS_PASS_DEFERRED_NORMAL = 8,
	L_MATSYS_PASS_DEFERRED_SHADOW = 16
};

/// Bitwise image of material properties layout in "Layout.sp"
struct sMaterialProperties
{
public:
	sMaterialProperties(): FAmbientColor( 0.0 ),
		FDiffuseColor( 0.0 ),
		FSpecularColor( 0.0 ),
		FRoughness( 0.0 ),
		//
		FTransparency( 0.0 ),
		FReflectivity( 0.0 ),
		FShine( 0.0 ),
		FShineStrength( 0.0 ) {};
	//
	LVector4          FAmbientColor;
	LVector4          FDiffuseColor;
	LVector4          FSpecularColor;
	//
	float             FTransparency;
	float             FReflectivity;
	LVector4          FShine;
	float             FShineStrength;
	float             FRoughness;
};

/// Material description for material system
struct sMaterialDesc
{
public:
	sMaterialDesc(): FMaterialName( "unknown" ),
		FMaterialClass( "default" ),
		// properties
		FScaleCoef( 1.0 ),
		FCastShadow( true ),
		FReceiveShadow( true ),
		FTesselation( 0.0 ),
		FRenderingOrder( 0 ),
		FReflectionPlane( 0.0 ),
		FPlanarReflection( false ),
		FAlphaTransparency( false ),
		FUseAmbientOnly( false ),
		FWireframe( false ),
		// GPU properties
		FProperties(),
		// maps
		FAmbientMap( "" ),
		FDiffuseMap( "" ),
		FSpecularMap( "" ),
		FBumpMap( "" ),
		FReflectionMap( "" ),
		// overriden maps
		FAmbientMapOverride( NULL ),
		FDiffuseMapOverride( NULL ),
		FSpecularMapOverride( NULL ),
		FBumpMapOverride( NULL ),
		FReflectionMapOverride( NULL ),
		//
		FUniformsBuffer( NULL ) {}
	// IDs
	LStringBuffer     FMaterialName;
	LStringBuffer     FMaterialClass;
	// properties
	float             FScaleCoef;
	bool              FCastShadow;
	bool              FReceiveShadow;
	float             FTesselation;
	int               FRenderingOrder;
	LVector4          FReflectionPlane;
	bool              FPlanarReflection;
	bool              FAlphaTransparency;
	bool              FUseAmbientOnly;
	bool              FWireframe;
	// properties passed to shader program
	sMaterialProperties FProperties;
	// maps
	LStringBuffer     FAmbientMap;
	LStringBuffer     FDiffuseMap;
	LStringBuffer     FSpecularMap;
	LStringBuffer     FBumpMap;
	LStringBuffer     FReflectionMap;
	// overriden maps
	iTexture*         FAmbientMapOverride;
	iTexture*         FDiffuseMapOverride;
	iTexture*         FSpecularMapOverride;
	iTexture*         FBumpMapOverride;
	iTexture*         FReflectionMapOverride;

	bool    operator==( const sMaterialDesc& Mat ) const;
	bool    operator!=( const sMaterialDesc& Mat ) const;

	void             UpdateUniformsBuffer( sEnvironment* Env );
	clRenderState*   GetRenderStateForPass( sEnvironment* Env, const sMaterialDesc& Material, LMatSysPass Pass ) const;
private:
	iGPUBuffer*      FUniformsBuffer;
};

class scriptfinal clMaterial: public iResource
{
public:
	clMaterial();
	virtual ~clMaterial();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	SERIALIZABLE_CLASS();
	//
	// iCacheable interface
	//
	virtual LString   GetCachingDir() const;
	virtual bool      IsSameResource( iResource* Other );
	//
	// clMaterial
	//
	virtual sMaterialDesc GetMaterialDesc() const { return FMaterialDesc; };
	virtual void          SetMaterialDesc( const sMaterialDesc& Desc ) { FMaterialDesc = Desc; };
	virtual void          SetPlanarReflection( const LVector4& ReflPlane )
	{
		FMaterialDesc.FPlanarReflection = true;
		FMaterialDesc.FReflectionPlane = ReflPlane;
	};
	virtual LVector4      GetPlanarReflection() const { return FMaterialDesc.FReflectionPlane; };
public:
	sMaterialDesc    FMaterialDesc;

#pragma region Properties
	/* Property(Name="MaterialName",     Type=LStringBuffer, FieldName=FMaterialDesc.FMaterialName) */
	/* Property(Name="MaterialClass",    Type=LStringBuffer, FieldName=FMaterialDesc.FMaterialClass) */

	// properties

	/* Property(Description="Ambient color",  Category="Ambient", Name="AmbientColor",     Type=vec4,  FieldName=FMaterialDesc.FProperties.FAmbientColor) */
	/* Property(Description="Ambient map",    Category="Ambient", Name="AmbientMap",       Type=LStringBuffer, Getter=GetAmbientMap, Setter=SetAmbientMap) */

	/* Property(Description="Diffuse color",  Category="Diffuse", Name="DiffuseColor",     Type=vec4,  FieldName=FMaterialDesc.FProperties.FDiffuseColor) */
	/* Property(Description="Diffuse map",    Category="Diffuse", Name="DiffuseMap",       Type=LStringBuffer, Getter=GetDiffuseMap, Setter=SetDiffuseMap) */

	/* Property(Description="Transparency factor",   Category="Transparency", Name="Transparency",     Type=float, FieldName=FMaterialDesc.FProperties.FTransparency) */
	/* Property(Description="Use transparency in Diffuse alpha", Category="Transparency", Name="AlphaTransparency",Type=bool,  FieldName=FMaterialDesc.FAlphaTransparency) */

	/* Property(Description="Specular color", Category="Specular", Name="SpecularColor",   Type=vec4,  FieldName=FMaterialDesc.FProperties.FSpecularColor) */
	/* Property(Description="Specular map",   Category="Specular", Name="SpecularMap",     Type=LStringBuffer, Getter=GetSpecularMap, Setter=SetSpecularMap) */
	/* Property(Description="Shine",          Category="Specular", Name="Shine",            Type=vec4,  FieldName=FMaterialDesc.FProperties.FShine) */
	/* Property(Description="Shine strength", Category="Specular", Name="ShineStrength",    Type=float, FieldName=FMaterialDesc.FProperties.FShineStrength) */
	/* Property(Description="Cook-Torrance roughness", Category="Specular", Name="Roughness",        Type=float, FieldName=FMaterialDesc.FProperties.FRoughness) */

	/* Property(Description="Does this object cast shadows",    Category="Common", Name="CastShadow",       Type=bool,  FieldName=FMaterialDesc.FCastShadow) */
	/* Property(Description="Does this object receive shadows", Category="Common", Name="ReceiveShadow",    Type=bool,  FieldName=FMaterialDesc.FReceiveShadow) */

	/* Property(Description="Scale coefficient",             Category="Common", Name="ScaleCoef",        Type=float, FieldName=FMaterialDesc.FScaleCoef) */
	/* Property(Description="Tesseleation factor",           Category="Common", Name="Tesselation",      Type=float, FieldName=FMaterialDesc.FTesselation) */
	/* Property(Description="Internal rendering priority",   Category="Common", Name="RenderingOrder",   Type=int,   FieldName=FMaterialDesc.FRenderingOrder) */
	/* Property(Description="Use only ambient map or color", Category="Common", Name="UseAmbientOnly",   Type=bool,  FieldName=FMaterialDesc.FUseAmbientOnly ) */
	/* Property(Description="Use wireframe for this object", Category="Common", Name="Wireframe",        Type=bool,  FieldName=FMaterialDesc.FWireframe ) */

	// maps
	/* Property(Description="Bump map", Category="Aux Maps", Name="BumpMap",          Type=LStringBuffer, Getter=GetBumpMap, Setter=SetBumpMap) */

	/* Property(Description="Strength", Category="Reflection", Name="Reflectivity",     Type=float, FieldName=FMaterialDesc.FProperties.FReflectivity) */
	/* Property(Description="Map",      Category="Reflection", Name="ReflectionMap",    Type=LStringBuffer, Getter=GetReflectionMap, Setter=SetReflectionMap) */
	/* Property(Description="Plane",    Category="Reflection", Name="PlanarReflection", Type=vec4,  Setter=SetPlanarReflection, Getter=GetPlanarReflection) */

#pragma endregion
public:
	scriptmethod void          SetAmbientMap( LStringBuffer AmbMap );
	scriptmethod LStringBuffer GetAmbientMap() const { return FMaterialDesc.FAmbientMap; }
	scriptmethod void          SetDiffuseMap( LStringBuffer DifMap );
	scriptmethod LStringBuffer GetDiffuseMap() const { return FMaterialDesc.FDiffuseMap; }
	scriptmethod void          SetSpecularMap( LStringBuffer SpcMap );
	scriptmethod LStringBuffer GetSpecularMap() const { return FMaterialDesc.FSpecularMap; }
	scriptmethod void          SetBumpMap( LStringBuffer BmpMap );
	scriptmethod LStringBuffer GetBumpMap() const { return FMaterialDesc.FBumpMap; }
	scriptmethod void          SetReflectionMap( LStringBuffer RflMap );
	scriptmethod LStringBuffer GetReflectionMap() const { return FMaterialDesc.FReflectionMap; }
};

#endif

/*
 * 15/03/2010
     It's here
*/
