/**
 * \file ASE.h
 * \brief Probably one of the most complete .ASE parsers on the Net
 * \version 0.5.93
 * \date 17/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _ASELoader_
#define _ASELoader_

#include "Mesh.h"
#include "Geometry/VAMender.h"
#include "Core/iObject.h"

/// ASE texture map description
struct sASEMap
{
	sASEMap(): FName( "unknown" ),
		FClass( "unknown" ),
		FSubNo( 1 ),
		FAmount( 1.0 ),
		FBitmap( "" ),
		FMapType( "unknown" ),
		FUOffset( 0.0 ),
		FVOffset( 0.0 ),
		FUTiling( 1.0 ),
		FVTiling( 1.0 ),
		FUVWAngle( 0.0 ),
		FUVWBlur( 1.0 ),
		FUVWBlurOffset( 0.0 ),
		FUVWNoiseAmt( 1.0 ),
		FUVWNoiseSize( 1.0 ),
		FUVWNoiseLevel( 1 ),
		FUVWNoisePhase( 0.0 ),
		FBitmapFilter( "unknown" ) {};
	//
	LString    FName;
	LString    FClass;
	int        FSubNo;
	float      FAmount;
	LString    FBitmap;
	LString    FMapType;
	float      FUOffset;
	float      FVOffset;
	float      FUTiling;
	float      FVTiling;
	float      FUVWAngle;
	float      FUVWBlur;
	float      FUVWBlurOffset;
	float      FUVWNoiseAmt;
	float      FUVWNoiseSize;
	int        FUVWNoiseLevel;
	float      FUVWNoisePhase;
	LString    FBitmapFilter;
};

/// Internal representation of ASE material entry
struct sASEMaterial
{
	sASEMaterial(): FName( "unknown" ),
		FClass( "unknown" ),
		FAmbient( 0, 0, 0 ),
		FDiffuse( 1, 1, 1 ),
		FSpecular( 1, 1, 1 ),
		FShine( 1.0 ),
		FShineStrength( 0.0 ),
		FTransparency( 0.0 ),
		FWireSize( 1.0 ),
		FShading( "unknown" ),
		FXPFallOff( 1.0 ),
		FSelfIllum( 0.0 ),
		FFallOff( "unknown" ),
		FXPType( "unknown" ),
		FAmbientMap(),
		FDiffuseMap(),
		FSpecularMap(),
		FSubMaterials() {};
	//
	LString     FName;
	LString     FClass;
	LVector3    FAmbient;
	LVector3    FDiffuse;
	LVector3    FSpecular;
	float       FShine;
	float       FShineStrength;
	float       FTransparency;
	float       FWireSize;
	LString     FShading;
	float       FXPFallOff;
	float       FSelfIllum;
	LString     FFallOff;
	LString     FXPType;
	sASEMap     FAmbientMap;
	sASEMap     FDiffuseMap;
	sASEMap     FSpecularMap;
	sASEMap     FBumpMap;
	std::vector<sASEMaterial> FSubMaterials;
	/*
	        map_array       map_ambient;
	        map_array       map_generic;
	        map_array       map_diffuse;
	        map_array       map_specular;
	        map_array       map_bump;
	        map_array       map_selfillum;
	        map_array       map_reflect;
	        map_array       map_shine;
	        map_array       map_shinestrength;
	        map_array       map_opacity;
	        map_array       map_refract;
	        map_array       map_filtercolor;
	*/
};

/// Loader of ASE meshes
class scriptfinal clASELoader: public iMeshLoader
{
public:
	clASELoader(): FMesh( NULL ),
		FSceneFrameSpeed( 0.0f ),
		FSceneTicksPerFrame( 0.0f ) {};
	//
	// iMeshLoader interface
	//
	virtual bool    LoadMesh( iIStream* IStream, clMesh* Resource );
private:
	std::vector<sASEMaterial> FMaterialList;
	clMesh*        FMesh;
	iIStream*      FStream;
	float          FSceneFrameSpeed;
	float          FSceneTicksPerFrame;
private:
	void               AddNode( int ParentIdx,
	                            clVertexAttribs* VA,
	                            const sASEMaterial& MaterialName,
	                            const LString& ObjName,
	                            const LMatrix4& GlobalTransform );
	sMaterialDesc      CreateMaterialFromASE( const sASEMaterial& ASEMaterial );
private:
	void               ASE_SkipBlock( iIStream* FStream, const LString& Block );
	void               ASE_ReadScene( iIStream* FStream );
	void               ASE_ReadGeomObject( iIStream* FStream, int Depth, int ParentIdx );
	void               ASE_ReadShapeObject( iIStream* FStream );
	clVAMender*        ASE_ReadMesh( iIStream* FStream );
	void               ASE_ReadVertexList( iIStream* FStream, clVAMender* Mender );
	void               ASE_ReadTVertexList( iIStream* FStream, clVAMender* Mender );
	void               ASE_ReadFaceList( iIStream* FStream, clVAMender* Mender );
	void               ASE_ReadTFaceList( iIStream* FStream, clVAMender* Mender );
	void               ASE_ReadNormals( iIStream* FStream, clVAMender* Mender );
	void               ASE_ReadMaterialList( iIStream* FStream );
	sASEMaterial       ASE_ReadMaterial( iIStream* FStream, int SubMaterialNesting );
	LMatrix4           ASE_ReadNodeTM( iIStream* FStream );
	void               ASE_ReadTMAnimation( iIStream* FStream );
	void               ASE_ReadControlPosTrack( iIStream* FStream );
	void               ASE_ReadControlRotTrack( iIStream* FStream );
	void               ASE_ReadControlScaleTrack( iIStream* FStream );
	void               ASE_ReadMap( iIStream* FStream, sASEMap* ExtMap );
};


#endif

/*
 * 17/11/2010
     ASE_ReadScene()
     ASE_ReadShapeObject()
     ASE_ReadTMAnimation()
     ASE_ReadControlPosTrack()
     ASE_ReadControlRotTrack()
     ASE_ReadControlScaleTrack()
 * 12/02/2010
     No dependancy on scenegraph
 * 09/02/2010
     Reimplemented
 * 06/04/2009
     sASEMap
 * 17/04/2008
     sASEMaterial
 * 31/10/2007
     sASEInternalMaterial
 * 12/03/2005
     Materials list
 * 04/03/2005
     It's here
*/
