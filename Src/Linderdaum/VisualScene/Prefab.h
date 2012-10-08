#ifndef __game_objects__h__included__
#define __game_objects__h__included__

#include "Core/iObject.h"

#include <map>

class clGeom;

/**
   \brief Prefab for Maze engine

   Imported from ASE, contains collision/logic/mesh.

   FSourceFileName is stored in ImportASE.
*/
class scriptfinal netexportable clPrefab: public iObject
{
public:
	clPrefab();
	virtual ~clPrefab();

	/// Load ASE file and split it to collision geometry and actual rendered geometry
	virtual void ImportASE( const LString& fname );

	virtual clPrefab* CloneTemplate();

	/// Get current collision mesh
	scriptmethod clGeom* GetCollisionGeom() const { return FCollisionMesh; }

	/// Get current mesh
	scriptmethod clGeom* GetGeom() const { return FMesh; }

//	vi_rtual void SaveTo(const LString& OutFileName);
//	vi_rtual void LoadFrom(const LString& InFileName);

public:
	LString FName;

	/// Source .ase file name
	LString FSourceFileName;

	/// Visual representation
	clGeom* FMesh;

	/// Extracted collision mesh
	clGeom* FCollisionMesh;

	LString GetCachedCollisionMeshName() { return FSourceFileName + LString( ".collision_mesh.lcm" ); }
	LString GetCachedMeshName() const { return FSourceFileName + LString( ".mesh.lcm" ); }
};

/// List of objects. Used in high-level management (in editor)
class scriptfinal netexportable clPrefabCollection: public iObject
{
public:
	clPrefabCollection(): FItems() {}
	virtual ~clPrefabCollection() { Clear( true ); }

	scriptmethod clPrefab* GetObjectByName( const LString& Name );

	scriptmethod void LoadDirectory( const LString& DirName );

	scriptmethod size_t GetCount() const { return FItems.size(); }

	scriptmethod clPrefab* GetItem( int i ) { return FItems[i]; }

	scriptmethod void Clear( bool DeleteItems )
	{
		if ( DeleteItems )
			for ( size_t j = 0 ; j < FItems.size() ; j++ )
			{
				FItems[j]->DisposeObject();
			}

		FItems.clear();
	}

public:
	std::vector<clPrefab*> FItems;

	std::map<LString, int> FIndices;
};

#endif
