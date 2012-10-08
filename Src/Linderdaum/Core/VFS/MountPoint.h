/**
 * \file MountPoint.h
 * \brief Virtual file system mount points
 * \version 0.6.06
 * \date 07/01/2012
 * \author Sergey Kosarevsky, 2005-2012
 * \author Viktor Latypov, 2011 (.tar support)
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __MountPoints__h__included__
#define __MountPoints__h__included__

#include "Engine.h"
#include "Core/iObject.h"

#include <map>

class iIStream;
class iRAWFile;

/// Mount point interface for virtual file system
class scriptfinal iMountPoint: public iObject
{
public:
	iMountPoint(): FMountPointName( "" ), FLoaded( true ) {};
	virtual ~iMountPoint() {};

	/// Set internal mount point name
	virtual void    SetName( const LString& N ) { FMountPointName = N; }

	/// Get internal mount point name
	virtual LString GetName() const { return FMountPointName; }

	/// Check if virtual file VirtualName exists at this mount point
	virtual bool         FileExistsAtThisPoint( const LString& VirtualName ) const = 0;

	/// Convert local file VirtualName to global name
	virtual LString      MapToThisPoint( const LString& VirtualName ) const = 0;

	/// Create appropriate file reader for the specified VirtualName
	virtual iRAWFile*    CreateRAWFile( const LString& VirtualName ) const = 0;

	/// Retrieve file time
	virtual Lint64       GetFileTime( const LString& VirtualName ) const = 0;

	/// Mount point is usually a folder on OS native file system
	virtual bool         IsPhysical() const { return true; }

	virtual void         WaitLoad() const { while ( !FLoaded ); }

	void SetLoadComplete( bool Loaded ) { FLoaded = Loaded; }

	volatile bool FLoaded;
private:
	LString       FMountPointName;
};

/// Mount point implementation for physical directories access
class scriptfinal clMountPoint: public iMountPoint
{
public:
	clMountPoint( const LString& PhysicalName ): FPhysicalName( PhysicalName )
	{
		LStr::AddTrailingChar( &FPhysicalName, PATH_SEPARATOR );
	};
	virtual ~clMountPoint() {};
	//
	// iMountPointDriver interface
	//
	virtual bool         FileExistsAtThisPoint( const LString& VirtualName ) const;
	virtual LString      MapToThisPoint( const LString& VirtualName ) const;
	virtual iRAWFile*    CreateRAWFile( const LString& VirtualName ) const;
	virtual Lint64       GetFileTime( const LString& VirtualName ) const;
private:
	LString FPhysicalName;
};

/// File entry attributes
struct sFileLocation
{
	/// Offset in the memory block or in the containing file
	Luint64 FLocation;

	/// Size of the file
	Luint64 FSize;

	/// Timestamp
	Luint64 FTime;

#pragma region Support for in-memory files

	/// Pointer to the source memory block (mmap-ed file or just some
	void*   FSourceBlock;

	/// Absolute pointer to this file data: FSourceBlock + FLocation
	void*   FFileData;

#pragma endregion
};

/// Implementation of a mount point for the uncompressed .RAR/.TAR files
class scriptfinal clArchiveMountPoint: public iMountPoint
{
public:
	clArchiveMountPoint( sEnvironment* Env, const LString& PhysicalName );
	virtual ~clArchiveMountPoint();
	//
	// clMountPoint
	//
	virtual bool         FileExistsAtThisPoint( const LString& VirtualName ) const;
	virtual LString      MapToThisPoint( const LString& VirtualName ) const;
	virtual iRAWFile*    CreateRAWFile( const LString& VirtualName ) const;
	virtual Lint64       GetFileTime( const LString& VirtualName ) const;
	virtual bool         IsPhysical() const { return false; };
private:
	void    LocateFiles_RAR( iIStream* In );
	void    LocateFiles_TAR( iIStream* In );
private:
	typedef std::map<LString, sFileLocation>    clLocationsList;
private:
	mutable clLocationsList    FLocations;
	iIStream*                  FArchiveFile;
};


/**
   \brief Memory block containing a number of virtual files

   MemoryMountPoint is the base class for any preprocessed bunch of files:
      packed/compressed/encrypted/remote(downloadable)

   List of locations is just a collection of pointers.

   All of the files here can be read from some archive or manually added one by one.
*/
class scriptfinal clMemoryMountPoint: public iMountPoint
{
public:
	clMemoryMountPoint() {}

	/// Add a new virtual file
	scriptmethod void RegisterFile( const LString& Name, void* Data, size_t Size, Luint64 Time );

	/// Remove file from the list
	scriptmethod void UnregisterFile( const LString& VirtualName );

	/// Check for file existence
	virtual bool FileExistsAtThisPoint( const LString& VirtualName ) const;

	/// Get file's timestamp
	virtual Lint64 GetFileTime( const LString& VirtualName ) const;

	/// Internals: Create actual file reader
	virtual iRAWFile* CreateRAWFile( const LString& VirtualName ) const;

	/// No name conversions
	virtual LString      MapToThisPoint( const LString& VirtualName ) const { return VirtualName; }

	/// This mount-point is not physical
	virtual bool         IsPhysical() const { return false; }
private:
	typedef std::map<LString, sFileLocation>    clLocationsList;

	mutable clLocationsList    FLocations;
};

/// The decorator to allow file name dereferencing
class scriptfinal clAliasMountPoint: public iMountPoint
{
public:
	clAliasMountPoint( iMountPoint* Src ): FMountPoint( Src ) { Env = Src->Env; };
	virtual ~clAliasMountPoint() {};

	/// Set alias directory
	scriptmethod void    SetAlias( const LString& N )
	{
		FAlias = N;
		LStr::AddTrailingChar( &FAlias, PATH_SEPARATOR );
	}

	/// Get internal mount point name
	scriptmethod LString GetAlias() const { return FAlias; }

	/// Check if virtual file VirtualName exists at this mount point
	virtual bool         FileExistsAtThisPoint( const LString& VirtualName ) const { return FMountPoint->FileExistsAtThisPoint( FAlias + VirtualName ); }

	/// Convert local file VirtualName to global name
	virtual LString      MapToThisPoint( const LString& VirtualName ) const { return FMountPoint->MapToThisPoint( FAlias + VirtualName ); }

	/// Create appropriate file reader for the specified VirtualName
	virtual iRAWFile*    CreateRAWFile( const LString& VirtualName ) const { return FMountPoint->CreateRAWFile( FAlias + VirtualName ); }

	/// Retrieve file time
	virtual Lint64       GetFileTime( const LString& VirtualName ) const { return FMountPoint->GetFileTime( FAlias + VirtualName ); }

	/// Mount point is usually a folder on OS native file system
	virtual bool         IsPhysical() const { return false; };
private:
	/// Name to append to each file in this mount point
	LString FAlias;

	/// The actual file container
	iMountPoint* FMountPoint;
};

#endif

/*
 * 07/01/2012
     clAliasMountPoint
 * 25/10/2011
     IsPhysical()
 * 05/06/2011
     .TAR support
 * 11/11/2005
     .RAR support
 * 03/12/2004
     It's here
*/
