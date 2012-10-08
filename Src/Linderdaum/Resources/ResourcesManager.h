/**
 * \file ResourcesManager.h
 * \brief Resources manager
 * \version 0.6.02
 * \date 22/09/2011
 * \author Sergey Kosarevsky, 2004-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clResourcesManager_
#define _clResourcesManager_

#include "Platform.h"
#include "Core/iObject.h"
#include "Images/Bitmap.h"
#include "Utils/Thread.h"

#include <list>
#include <deque>
#include <map>

class clCVar;
class clBlob;
class iResource;
class iWaveform;
class iShaderProgram;
class clAnimation;
class clImage;
class clMaterial;
class clMesh;
class clGeom;
class clRenderState;
class clLVLibVolume;
class clUVSurfaceGenerator;
class clVertexAttribs;

class scriptfinal clLoaderThread: public iThread
{
public:
	class iLoadOp
	{
	public:
		virtual ~iLoadOp() {};
		/// invoke an appropiate loader to perform actual loading of the resource
		virtual void Load() = 0;
		virtual iResource* GetResource() const = 0;
	};
	class clLoadOp_Image: public iLoadOp
	{
	public:
		clLoadOp_Image( clImage* Resource ) : FResource( Resource ) {};
		virtual void Load();
		virtual iResource* GetResource() const;
	private:
		clImage* FResource;
	};
	class clLoadOp_Geom: public iLoadOp
	{
	public:
		clLoadOp_Geom( clGeom* Resource ) : FResource( Resource ) {};
		virtual void Load();
		virtual iResource* GetResource() const;
	private:
		clGeom* FResource;
	};
public:
	//
	// iThread interface
	//
	virtual void Run();
	//
	// clLoaderThread
	//
	virtual void EnqueueLoading( iLoadOp* LoadOp );
private:
	iLoadOp* ExtractLoadOp();

	FWD_EVENT_HANDLER( Event_STARTLOADING );
	FWD_EVENT_HANDLER( Event_STOPLOADING );
private:
	std::deque<iLoadOp*> FLoadOps;
	clMutex              FMutex;
};

/// Resources manager
class scriptfinal clResourcesManager: public iObject
{
public:
	clResourcesManager();
	virtual ~clResourcesManager();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// clResourcesManager
	//

#pragma region Blob helpers

	/// Create uninitialized blob
	scriptmethod clBlob* CreateEmptyBlob() const;

	/// Create a newly allocated blob
	scriptmethod clBlob* CreateBlob( size_t Size ) const;

	/// Create a blob for the specified memory block
	scriptmethod clBlob* CreateManagedBlob( void* DataBlock, size_t Size ) const;

#pragma endregion

	/// LV: WTF is this ? explain, please
	void    StopLoaderThread();

#pragma region iWaveform
	scriptmethod iWaveform* LoadWaveform( const LString& FileName );
#pragma endregion

#pragma region iShaderProgram

	/// Load shader program with specified list of defines (separated by spaces)
	scriptmethod iShaderProgram* LoadSP( const LString& FileName, const LString& DefinesList );
#pragma endregion

#pragma region clImage

	/// Create empty image
	scriptmethod clImage* CreateImage() const;

	/// Create image for the given bitmap
	scriptmethod clImage* CreateImageFromBitmap( clBitmap* Bitmap ) const;

	/// Load specified texture as an image
	scriptmethod clImage* LoadImg( const LString& FileName, LTextureType TextureType );

	/// Load specified texture as an image, wait until its async-loaded
	scriptmethod clImage* LoadImgSync( const LString& FileName, LTextureType TextureType );
	scriptmethod clImage* LoadImgHints( const LString& FileName, LTextureType TextureType, int HintSizeX, int HintSizeY, int HintSizeZ, bool AutoGradient, bool AutoESL );

#pragma endregion

#pragma region clMaterial
	/// Create "empty" material
	scriptmethod clMaterial* CreateMaterial() const;

	/// Create "flat" colored material
	scriptmethod clMaterial* CreateColoredMaterial( const LVector4& Color, bool CastShadow, bool ReceiveShadow, float Transparency ) const;

	/// Load material from file
	virtual clMaterial* LoadMaterial( const LString& FileName );
#pragma endregion

#pragma region clGeom
	/// Create an empty mesh (VA container)
	scriptmethod clMesh* CreateMesh() const;

	/// Create an empty Geom
	scriptmethod clGeom* CreateGeom() const;

	/// Create the Geom with a single VA inside
	scriptmethod clGeom* CreateGeomForVA( clVertexAttribs* VA ) const { return CreateGeomForNamedVA( "VA", VA ); }

	/// Create the Geom with a single named VA inside
	scriptmethod clGeom* CreateGeomForNamedVA( const LString& VAName, clVertexAttribs* VA ) const;

	/// Create a Box geom
	scriptmethod clGeom* CreateBox( const LVector3& Min, const LVector3& Max ) const;

	/// Create a Sphere geom
	scriptmethod clGeom* CreateSphere( float Radius, const LVector3& Center ) const;

	/// Create a Tetrahedron inscribed in a sphere
	scriptmethod clGeom* CreateTetrahedron( float Radius, LVector3 Center ) const;

	/// Create an Icosahedron inscribed in a sphere
	scriptmethod clGeom* CreateIcosahedron( float Radius, LVector3 Center ) const;

	/// Create a tesselated plane geom
	scriptmethod clGeom* CreatePlane( float X1, float Y1, float X2, float Y2, float Z, int Subdiv ) const;

	/// LV: some kinda gear...
	scriptmethod clGeom* CreateGear( float BaseRadius, float RootRadius, float OuterRadius, float Width, int TotalTeeth, int InvoluteSteps ) const;

	/// LV: another gear, appears nowhere. There's a UVSurfaceGenerator for this kind of stuff
	scriptmethod clGeom* CreateISOGear( float Module, int NumberOfTeeth, float ProfileShift, float Width ) const;

	/// Load (asynchronoulsy) a mesh from file
	scriptmethod clGeom* LoadGeom( const LString& FileName );

	/// Create a surface generator
	scriptmethod clUVSurfaceGenerator* CreateSurfaceGenerator( const LString& GeneratorName ) const;

	/// Merge a number of transformed meshes to a single static mesh
	scriptmethod clMesh* MergeMeshes( const std::vector<clMesh*>& Meshes, const std::vector<LMatrix4>& Transforms ) const;

	/// Merge a number of transformed meshes to a single static mesh
	scriptmethod clGeom* MergeGeoms( const std::vector<clGeom*>& Geoms, const std::vector<LMatrix4>& Transforms ) const;

#pragma endregion

#pragma region clRenderState
	scriptmethod clRenderState* CreateShader() const;
	scriptmethod clRenderState* CreateCustomShader( const LString& ShaderProgramName, const LString& DefinesList ) const;
	scriptmethod clRenderState* LoadShader( const LString& FileName );
#pragma endregion

#pragma region clAnimation
	scriptmethod clAnimation* CreateAnimation() const;
	scriptmethod clAnimation* LoadAnimation( const LString& FileName );
#pragma endregion

#pragma region clLVLibVolume
	scriptmethod clLVLibVolume* CreateVolume() const;
#pragma endregion

#pragma region Resources management
	static LString        ConvertName( const LString& FileName );
	void          PurgeAll();
	template <class T> T* FindResourceFile( const LString& FileName )
	{
		return dynamic_cast<T*>( FindResourceInGraph( FResourcesGraph, FileName ) );
	};
	void EnqueueLoading( clLoaderThread::iLoadOp* LoadOp );
#pragma endregion

	friend class iResource;

	// used in clMaterial
	std::map<LString, iShaderProgram*> FPrecompiledPrograms;
private:
	void    PreCacheC( const LString& Param );
	void    ReCacheC( const LString& Param );
	void    ReCacheNewC( const LString& Param );
private:
	typedef std::list<iResource*>    clResourcesGraph;
	iResource*    FindResourceInGraph( clResourcesGraph& Graph, const LString& FileName );
private:
	clResourcesGraph    FResourcesGraph;
#pragma region Commulative loading time for performance analysis
	double              FTime_Waveforms;
	double              FTime_ShaderPrograms;
	double              FTime_Images;
	double              FTime_Materials;
	double              FTime_Meshes;
	double              FTime_RenderStates;
	double              FTime_Animations;
#pragma endregion

	clLoaderThread* FLoaderThread;
	clCVar*         FLockedLoading;
};

#endif

/*
 * 22/09/2011
     Animations
 * 10/07/2010
     Reimplemented
 * 10/11/2008
     LoadTexture() proto changed
 * 09/07/2007
     iResourceCreator<clMaterial>
 * 07/07/2007
     LoadMaterial()
 * 24/03/2005
     iResourceCreator<iShaderProgram> implemented
 * 12/01/2005
     iResourceCreator<iTextureObject> implemented
 * 31/12/2004
     It's here
*/
