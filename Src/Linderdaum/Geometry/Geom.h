/**
 * \file Geom.h
 * \brief
 * \version 0.5.91
 * \date 24/02/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clGeom_
#define _clGeom_

#include "Platform.h"
#include "Resources/iResource.h"
#include "Math/LAABB.h"
#include "Mesh.h"

class clMesh;
class iShaderProgram;
class LVector3;
class clSceneNode;

class scriptfinal clGeom: public iResource
{
public:
	clGeom();
	virtual ~clGeom();

	NET_EXPORTABLE()

	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// iResource interface
	//
	virtual LString GetCachingDir() const;
	virtual void    CacheTo( const LString& FileName );
	virtual bool    CacheFrom( const LString& FileName );
	//
	// clGeom
	//
	virtual clMesh* GetCurrentMesh() const;
	virtual clMesh* GetDefaultMesh() const;

	// mesh assignment : be careful with ownership
	virtual void SetMesh( clMesh* Mesh );

	virtual clGeom* CloneGeom() const;

protected:
	virtual bool    LoadFromFile( const LString& FileName );
private:
	virtual void SetMeshInternal( clMesh* Mesh, bool Notify );
private:
	clMesh*    FDefaultMesh;
	clMesh*    FCurrentMesh;
	clMutex    FMutex;
};

#endif

/*
 * 24/02/2010
     Log added
 * 07/02/2010
     It is here
*/
