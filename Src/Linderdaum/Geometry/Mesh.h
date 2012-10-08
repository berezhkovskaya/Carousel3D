/**
 * \file Mesh.h
 * \brief
 * \version 0.5.91
 * \date 23/03/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clMesh_
#define _clMesh_

#include "Animation.h"
#include "VertexAttribs.h"
#include "Scene/Material.h"

class iTexture;
class iVertexArray;
class clRenderState;
class clSceneNode;
class iShaderProgram;
class clMesh;

class scriptfinal iMeshLoader: public iObject
{
public:
	virtual bool    LoadMesh( iIStream* IStream, clMesh* Resource ) = 0;
};

class scriptfinal clMesh: public iObject
{
public:
	clMesh();
	virtual ~clMesh();

	NET_EXPORTABLE()

	//
	// clMesh
	//

	/// Internal copy routine. Temporarily implemented using Save-Load pair
	scriptmethod clMesh* CloneMesh() const;

	/// Merge some vertex attribs in a mesh. No animation support yet
	scriptmethod void MergeRigids( int Idx1, int Idx2 );

	/// Merge all similar (determined by rigid name)
	scriptmethod void MergeSimilarRigids();

	/// Add all rigids from another mesh. Won't work (yet) with hierarchical meshes
	scriptmethod void MergeWith( clMesh* M );

	/// Transform every root node of the mesh by the given matrix
	scriptmethod void Transform( const LMatrix4& T );

	/// Transform vertices of each rigid to global coordinates
	scriptmethod void FlattenMesh();

	scriptmethod void CopyMaterialsFrom( const clMesh* M );

#pragma region Load data from file in generic supported formats

	scriptmethod bool    LoadMesh( const LString& FileName );
	scriptmethod bool    SaveMesh( const LString& FileName );

#pragma endregion

#pragma region Save and load in internal binary format

	scriptmethod void    SaveToStream( iOStream* Stream ) const;
	scriptmethod bool    LoadFromStream( iIStream* Stream );

	scriptmethod void    SaveToBinaryFile( const LString& FName ) const;
	scriptmethod void    LoadFromBinaryFile( const LString& FName );

#pragma endregion

#pragma region Named animations
	scriptmethod sAnimSequence    GetAnimSequenceByName( const LString& Name ) const;
	scriptmethod clAnimation*     GetAnimation() const { return FAnimations; }
	scriptmethod LString          GetAnimSequenceName( size_t i ) const;
	scriptmethod size_t           GetNumAnimSequences() const;
	scriptmethod void             LoadAnimStates( const LString& FileName );
#pragma endregion

#pragma region Mesh construction
	/// add new part to the mesh and return its ID and take ownership of Attribs
	scriptmethod int AddVertexAttribs( const LString& Name,
	                                   clVertexAttribs* Attribs,
	                                   const LMatrix4& GlobalTransform,
	                                   int ParentID,
	                                   int MaterialID );

	/// Remove the rigid from mesh. WARNING: no mesh hierarchy checks are done. So if there are child nodes something can crash
	scriptmethod void RemoveVertexAttribs( int RigidID );

	scriptmethod int  FindRigidByName( const LString& Name ) const;

	/// add new material to the mesh and return its ID
	scriptmethod int  AddMaterial( const sMaterialDesc& Material );

	/// fills the FLocalTransforms array based on FGlobalTransforms
	scriptmethod void RecalculateLocalFromGlobal();

	/// recalculate and return AABB for the entire mesh
	scriptmethod LAABoundingBox    GetBoundingBox() const;

	/// return cached AABB for the entire mesh
	scriptmethod LAABoundingBox    GetCachedBoundingBox() const;

	scriptmethod void              PrecalculateBoundingBoxes();

#pragma endregion

	scriptmethod size_t                  GetMaterialsCount() const { return FMaterials.size(); }

	scriptmethod sMaterialDesc           GetMaterialDesc( int i ) const { return FMaterials[i]; }

	scriptmethod size_t                  GetRigidsCount()      const { return FRigids.size(); }

	scriptmethod clVertexAttribs*        GetRigid( size_t i )     const { return FRigids[i]; }

	scriptmethod LString                 GetRigidName( size_t i ) const { return FNames[i]; }

	scriptmethod int                     GetParentRef( size_t i ) const { return FParentRef.FStream[i]; }

	scriptmethod int                     GetRigidMaterial( size_t i ) const { return FMaterialRef[i]; }

	scriptmethod const sMaterialDesc*    GetMaterial( size_t i ) const
	{
		return ( FMaterialRef.FStream[i] > -1 ) ? &FMaterials.FStream[ FMaterialRef.FStream[i] ] : NULL;
	}

	scriptmethod const LMatrix4&         GetLocalTransform( size_t i ) const { return FLocalTransforms.FStream[i]; }

	scriptmethod iVertexArray*           GetVertexArray( size_t i ) const;
	scriptmethod iTexture*               GetPackedSkeleton( size_t i ) const;

	/// Check intersection with ray and calculate intersection point
	scriptmethod bool                    IntersectWithRay( const LVector3& P, const LVector3& A, LVector3& isect ) const;

	/// Check intersection with ray and calculate intersection point and return rigid index
	scriptmethod bool                    IntersectWithRayIdx( const LVector3& P, const LVector3& A, LVector3& isect, int& Rigid ) const;

private:
	/// Collection of rigid parts of the mesh
	std::vector<clVertexAttribs*>  FRigids;

	/// Names of the rigids
	std::vector<LString>           FNames;

	/// Local transformation for every rigid
	sAttribStream<LMatrix4>        FLocalTransforms;

	/// Precached global transformations
	sAttribStream<LMatrix4>        FGlobalTransforms;

	/// Parent index for every rigid. -1 if none
	sAttribStream<int>             FParentRef;

	/// Material index for every rigid. No multimaterials are possible here
	sAttribStream<int>             FMaterialRef;

	/// Collection of mesh materials
	sAttribStream<sMaterialDesc>   FMaterials;

	/// Collection of named animations
	clAnimation*                   FAnimations;

	/// Precached rendering vertex arrays used by RenderDirect()
	mutable std::vector<iVertexArray*>     FVertexArrays;
	mutable std::vector<iTexture*>         FPackedSkeletons;

	/// Precached AABB
	mutable LAABoundingBox    FAABB;
	mutable bool              FAABBValid;
};

#endif

/*
 * 08/01/2011
     IntersectWithRayIdx()
     GetRigidName()
     'scriptmethod' instead of 'virtual' for many methods
 * 23/03/2010
     LoadAnimStates()
 * 18/03/2010
     AddAnimSequence()
 * 09/02/2010
     Data structures defined
 * 07/02/2010
     It's here
*/
