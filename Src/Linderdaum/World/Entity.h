/**
 * \file Entity.h
 * \brief Game entity
 * \version 0.5.91
 * \date 04/07/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clEntity_
#define _clEntity_

#include "iActor.h"
#include "Math/LTransform.h"

class clGeom;
class clScene;

/** Base class for all visible objects.
    These objects typicaly have geometry, sounds, respond to collisions, can be picked, etc.
**/
class clEntity: public iActor
{
public:
	clEntity();
	virtual ~clEntity();
	//
	// iObject interface
	//
	SERIALIZABLE_CLASS();
	//
	// iObject interface
	//
	virtual bool    EndLoad();
	//
	// iActor interface
	//
	virtual void    UpdateActor( float DeltaSeconds );
	//
	// clEntity
	//
	virtual int        GetGeomHandle() const
	{
		return FGeomHandle;
	};
	virtual int        GetParentGeomHandle() const
	{
		return -1;
	};
	virtual iActor*    GetParentActor() const
	{
		return NULL;
	};
	virtual LString    GetMeshName() const
	{
		return FMeshName;
	};
	virtual void       SetMeshName( const LString& MeshName )
	{
		FMeshName = MeshName;
	};
	virtual float      GetMeshHeight() const
	{
		return FMeshHeight;
	};
	virtual void       SetMeshHeight( float MeshHeight )
	{
		FMeshHeight = MeshHeight;
	};
	virtual clGeom*    GetGeom() const
	{
		return FGeom;
	};
	virtual void       SetGeom( clGeom* Geom )
	{
		FGeom = Geom;
	};

#pragma region Position and orientation
	virtual LVector3          GetPosition() const
	{
		return FTransform.GetPosition();
	};
	virtual void              SetPosition( const LVector3& Pos )
	{
		FTransform.SetPosition( Pos );
	};
	virtual LTransform&       GetTransform()
	{
		return FTransform;
	};
	virtual const LTransform& GetTransformConst() const
	{
		return FTransform;
	};
	virtual bool GetCenterMesh() const
	{
		return FCenterMesh;
	};
	virtual void SetCenterMesh( bool CenterMesh )
	{
		FCenterMesh = CenterMesh;
	};
#pragma endregion

#pragma region Actor events
	virtual void    Actor_EnteredWorld( clWorld* World );
	virtual void    Actor_LeavedWorld();
#pragma endregion

#pragma region Entity events
	virtual void    Entity_BecomeVisible() {};
	virtual void    Entity_BecomeInvisible() {};
#pragma endregion
private:

	/// geometry of this entity
	clGeom*    FGeom;

	/// scene this entity belongs to (only one scene is possible)
	clScene*   FScene;

	/// instance of FGeom added to FScene
	int        FGeomHandle;

	/// position and orientation
	LTransform    FTransform;

#pragma region Properties
	LString     FMeshName;
	float       FMeshHeight;
	bool        FCenterMesh;
	/* Property(Name="MeshName",            Type=string,  Getter=GetMeshName,   Setter=SetMeshName   ) */
	/* Property(Name="MeshHeight",          Type=float,   Getter=GetMeshHeight, Setter=SetMeshHeight ) */
	/* Property(Name="Position",            Type=vec3,    Getter=GetPosition,   Setter=SetPosition   ) */
	/* Property(Name="CenterMesh",          Type=bool,    Getter=GetCenterMesh, Setter=SetCenterMesh ) */
#pragma endregion

	bool        FLoadedFromFile;
};

#endif

/*
 * 04/07/2010
     It's here
*/
