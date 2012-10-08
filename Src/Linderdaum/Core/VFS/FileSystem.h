/**
 * \file FileSystem.h
 * \brief Virtual file system
 * \version 0.5.91
 * \date 03/07/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author Viktor Latypov, 2007-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _FileSystem_
#define _FileSystem_

#include "Core/iObject.h"
#include "Utils/Utils.h"
#include "Utils/LArray.h"

class iMountPoint;
class iIStream;
class iOStream;
class mlNode;
class mlTreeBuilder;
class clMemRAWFile;
class clMemFileWriter;
class iRAWFile;

class clBlob;

class clDownloadThread;
class clHTTPServerThread;

class clFileWatchThread;

/**
   \brief Object containing information about File watch process

   Contains file name being watched, optional callback routine and associated internal OS handles.
   On Win32 we do not use ReadDirectoryChanges API, so we manually check each file.
*/
class scriptfinal clFileWatchHandle: public iObject
{
public:
	clFileWatchHandle();
	virtual ~clFileWatchHandle() {}

	LString FFileName;

	/// Actual EVENT_CHANGED handler
	iObject* FHandler;

	/// Index
	int FHandleIndex;

	/// Is the OpenTry loop is already running (prevent multiple change events)
	bool FTryLoopRunning;

#ifdef OS_WINDOWS
	/// We need to track each file manually only for the simple WinAPI

	/// Modification time
	Luint64 FFileTime;

	/// Handle to the watch
	HANDLE FDirHandle;
#endif
};

/**
   \brief Virtual File System interface providing I/O facilities

    - Abstract file access using URLs
    - Supports multiple mount points (converts virtual file names to physical)
    - Processes XML streams

   Provides mount points for Win32 FAT file system.
   It supports not only ordinary directories, but also
   Win32/Win64 resources embedded into the executable, .RAR archives and has
   the possibility to hide a download from remote network site.

   Utility functions can enumerate files in a virtual directory and create an XML/XLML
   parser for the file.

   After the initialization a user can call multiple Mount("<directory_or_resource>")
   commands to add new mount points.

   File access is done through unified stream interface (iIStream/iOStream classes)
**/
class scriptfinal netexportable clFileSystem: public iObject
{
public:
	/// declares this class capable of throwing exceptions
	EXCEPTIONABLE;
	EXCEPTION( clExcept_UnableToFindFile,             "Unable to find file" );
	EXCEPTION( clExcept_UnableToReadFileTime,         "Unable to read file time" );
	EXCEPTION( clExcept_UnableToMountNonexistentPath, "Unable to mount nonexistent path" );
	EXCEPTION( clExcept_UnableToParseXLMLScript,      "Unable to parse XLML script" );
public:
	clFileSystem();
	virtual ~clFileSystem();

	virtual void AfterConstruction();

	/**
	    Create FileReader for the specified file name

	      - Converts FileName to physical (OS-dependant) file name and
	      - Supports Win32/Win64 resources embedded into the executable
	        (Use "RESOURCE:<name>" string as a file name to get access to the "<name>" resource)
	      - Supports unpacked .RAR files as the usual directories
	      -
	**/
	scriptmethod iIStream*       CreateFileReader( const LString& FileName ) const;

	/**
	   Create FileWriter for the specified file name

	      - Only physical files are supported !
	        (If the virtual FileName corresponds to .RAR archive or resource an error is produced)
	**/
	scriptmethod iOStream*       CreateFileWriter( const LString& FileName ) const;

	/**
	   Create a FileReader instance using Str parameter as a data source

	      \param FileName - user's file name
	      \param VirtualFileName - virtual file name provided for this "file"
	      \param Str - string with actual "file" contents
	**/
	scriptmethod iIStream*   CreateFileReaderFromString( const LString& FileName, const LString& VirtualFileName, const LString& Str ) const;

	/**
	   Create a FileReader instance using BufPtr and BufSize parameters as a data source

	      \param FileName - user's file name
	      \param VirtualFileName - virtual file name provided for this "file"
	      \param BufPtr - data buffer with actual "file" contents
	      \param BufSize - size of data buffer
	**/
	scriptmethod iIStream*        CreateFileReaderFromMemory( const LString& FileName, const LString& VirtualFileName, const void* BufPtr, Luint64 BufSize ) const;

	/// Create the output stream which writes text lines to engine.log
	scriptmethod iOStream*        CreateLogFileWriter() const;

	/// Create the in-mem output stream (used for decompression/downloading)
	scriptmethod clMemFileWriter* CreateMemFileWriter( const LString& FileName, Luint64 InitialSize ) const;

	/// Create every non-existent (physical) directory in the DirectoryName path
	static       void             CreateDirsPhys( const LString& DirectoryName );

	/// Delete physical file
	scriptmethod bool             DeleteFilePhys( const LString& FileName );

	/// Create unique temp file name using system API. RequireUnique creates new file and ensures ownership
	scriptmethod LString          CreateTempFileName( const LString& BaseDirectory, const LString& Prefix, bool RequireUnique ) const;

	/// Create every non-existent (virtual or physical) directory in the DirectoryName path
	scriptmethod void             CreateDirs( const LString& DirectoryName ) const;

	/**
	   \brief Add new mount point

	   \param PhysicalName - the name of physical directory, uncompressed .RAR archive
	                            or "RESOURCE:<name>" string, telling the file system
	                            to use the resource embedded into the executable
	**/
	scriptmethod void             Mount( const LString& PhysicalPath );

	/**
	   \brief Remove the mount point

	   This can be dangerous, because no information about opened file streams is preserved

	   \param ThePath - name/path of the mount point to remove
	*/
	scriptmethod void             Unmount( const LString& ThePath );

	/** \brief Add virtual mount point
	     To allow mounting "App.apk:assets/Data" we provide alias mount points

	    Usage:
	      VFS->Mount("App.apk")
	      VFS->AddAliasMountPoint("App.apk", "assets/Data");
	*/
	scriptmethod void             AddAliasMountPoint( const LString& SrcPath, const LString& AliasPrefix );

	/// Manually add the mount point
	scriptmethod void             AddMountPoint( iMountPoint* MP );

	/// Manually remove the mount point
	scriptmethod void             RemoveMountPoint( iMountPoint* MP );

	/// Get total number of used mount points
	scriptmethod size_t           GetNumMountPoints() const { return FMountPoints.size(); }

	/// Get mount point by index
	scriptmethod iMountPoint*     GetMountPoint( size_t Idx ) const { return FMountPoints[Idx]; }

	/// Get mount point by its name (dir/archive)
	scriptmethod iMountPoint*     FindMountPointByName( const LString& ThePath );

	/**
	   \brief Set the root directory name for the virtual system.

	        This directory name is added to any virtual file name passed to the virtual file system.
	   E.g., if RootDir == "Data", then the virtual file name "test.file" will be converted to "Data/test.file"
	      \param RootDir - the name of root directory
	**/
	scriptmethod void            SetRootDir( const LString& RootDir );

	/// Convert internal file name to system path
	scriptmethod LString         VirtualNameToPhysical( const LString& Path ) const;

	/// Convert internal file name to full system path
	scriptmethod LString         VirtualNameToPhysicalFull( const LString& Path ) const;

	/// Check if the physical file named "PhysicalName" exists
	static       bool            FileExistsPhys( const LString& PhysicalName );

	/// Check if the virtual file name corresponds to some physical file in one of the mount points
	scriptmethod bool            FileExists( const LString& Name ) const;

	/// Check if the virtual file name RESOURCE:filename corresponds to the actual resource
	scriptmethod bool            FileExistsInResource( const LString& Name ) const;

	/// Check if virtual file named NewFile exists and is newer than the virtual file named OldFile
	scriptmethod bool            FileExistsAndNewer( const LString& OldFile, const LString& NewFile ) const;

	/// Windows-specific check for the appearance of ":" symbol
	scriptmethod bool            IsFullPath( const LString& Path ) const;

	/// Convert relative physical path to absolute physical path
	scriptmethod LString         GetFullPath( const LString& PhysicalPath ) const;

	/// Get file modification date/time
	scriptmethod Lint64          GetFileTime( const LString& FileName ) const;

	///  Determine latest modification date/time for the files in a given directory
	scriptmethod Lint64          GetMaxTimeForDirectory( const LString& DirName ) const;

	noexport bool                CopyFile( const LString& Src, const LString& Dst ) const;
#pragma region Utility functions

	/// Download file from "http://Server:Port/Path"  to OutFile, Timeout is in seconds, 0 timeout means wait forever, return downloadID
	scriptmethod int DownloadFile( const LString& Server, int Port, const LString& Path, const LString& OutFile, float Timeout, iObject* CompletionHandler );

	/// Cancel download in-progress. Return true if download was cancelled and false otherwise or if invalid DownloadID was specified
	scriptmethod bool CancelDownload( int DownloadID );

	/// Start a new instance of web server and attach it to the specified address allowing MaxConn simultaneous connections
	scriptmethod clHTTPServerThread* StartWebServer( const LString& BindAddress, int BindPort, int MaxConn );

	/// Manual control of the download thread
	scriptmethod clDownloadThread* GetDownloadThread() const { return FDownloadThread; }

	/// Load all lines from text file to LArray
	bool LoadFileLinesArray( const LString& FName, LArray<LString>& Lines ) const;

	/// Load all lines from text file to std::vector
	bool LoadFileLinesVec( const LString& FName, std::vector<LString>& Lines ) const;

	/// Save an array of strings to file
	bool SaveFileLinesArray( const LString& FName, const LArray<LString>& Lines ) const;

	/// Save a vector of strings to file
	bool SaveFileLinesVec( const LString& FName, const std::vector<LString>& Lines ) const;

	/// Load binary data to the newly allocated buffer
	bool LoadFileData( const LString& FName, void** Data, Luint64* DataSize ) const;

	/// Save binary data to the file
	bool SaveFileData( const LString& FName, const void* Data, Luint64 DataSize ) const;

	/// Load binary data to the newly allocated buffer (Blob)
	scriptmethod clBlob* LoadFileAsBlob( const LString& FName ) const;

	/// Save blob to file
	scriptmethod bool SaveBlobToFile( const LString& FName, clBlob* Blob ) const;

	/// Calculate zero-based CRC32 checksum (using libcompress)
	scriptmethod Luint32 CRC32( const void* Data, Luint64 DataSize ) const;

	/// Calculate zero-based CRC32 checksum (using libcompress)
	scriptmethod Luint32 CRC32ForBlob( const clBlob* B ) const;

#pragma endregion

#pragma region ML-related functions

	/// Create an hierarchy of nodes representing ASE-like XLML stream
	scriptmethod mlNode* LoadXLMLFromStream_ASE( iIStream* IStream ) const;

	/// Load an hierarchy of nodes representing input ASE-like XLML stream
	scriptmethod mlNode* LoadXLML_ASE( const LString& FileName ) const;

	/// Create an hierarchy of nodes representing input XLML stream (IStream)
	scriptmethod mlNode* LoadXLMLFromStream( iIStream* IStream ) const;

	/// Load an hierarchy of nodes representing input XLML stream (input XLML stream type (XLML, BinaryML, or XML) is detected automatically from file extension)
	scriptmethod mlNode* LoadXLML( const LString& FileName ) const;

	/// Create an hierarchy of nodes representing input XLML stream (IStream)
	scriptmethod mlNode* LoadXMLFromStream( iIStream* IStream ) const;

	/// Load an hierarchy of nodes representing input XLML stream (input XLML stream type (XLML, BinaryML, or XML) is detected automatically from file extension)
	scriptmethod mlNode* LoadXML( const LString& FileName ) const;

	/// Save XLML representation of the mlNode tree
	scriptmethod bool    SaveXLML( mlNode* Node, const LString& FileName ) const;

	/// Save XLML representation of the mlNode tree to the output stream
	scriptmethod bool    SaveXLMLToStream( mlNode* Node, iOStream* OStream ) const;

	/// Save XLML representation of the mlNode tree to engine log
	scriptmethod bool    SaveXLMLToLog( mlNode* Node ) const;

	/// Save XML  representation of the mlNode tree to engine log
	scriptmethod bool    SaveXMLToLog( mlNode* Node ) const;

	/// Save XML  representation of the mlNode tree
	scriptmethod bool    SaveXML( mlNode* Node, const LString& FileName ) const;

	/// Save XML  representation of the mlNode tree to the output stream
	scriptmethod bool    SaveXMLToStream( mlNode* Node, iOStream* OStream ) const;

// virtu_al mlNode* LoadBinaryTree(const LString& FileName) const;
// vi_rtual bool    SaveBinaryTree(mlNode* Node) const;

#pragma endregion

	/**
	   Enumerate directories in a given VirtualPath
	      \param VirtualPath - path where the directories are enumerated
	      \param SubDirs     - if true, then the searching recurses to subdirectories
	      \param StrVec      - pointer to vector<> of string with output files list
	**/
	scriptmethod void            EnumerateFiles( const LString& VirtualPath, bool SubDirs, LStr::clStringsVector* StrVec ) const;
	/**
	   Enumerate directories in a given VirtualPath
	      \param VirtualPath - path where the directories are enumerated
	      \param SubDirs     - if true, then the searching recurses to subdirectories
	      \param StrVec      - pointer to vector<> of string with output directories list
	**/
	scriptmethod void            EnumerateDirs( const LString& VirtualPath, bool SubDirs, LStr::clStringsVector* StrVec ) const;
	/**
	   Split path to components
	   If one of the pointers (Drive,Dir,FileName,FileExt) is NULL, then it is not filled
	   then
	      \param Path        - path to be split
	      \param Drive       - pointer to drive container
	      \param Dir         - pointer to dir container
	      \param FileName    - pointer to file name container
	      \param FileExt     - pointer to file extension container
	**/
	noexport static  void   SplitPath( const LString& Path,
	                                   LString* Drive,
	                                   LString* Dir,
	                                   LString* FileName,
	                                   LString* FileExt );

	/**
	  Replace all $ vars in a string.
	  I.e. "C:\$PROJECT\FPC" where $PROJECT will be replaces with
	  the value of PROJECT environment variable
	**/
	noexport static LString      ReplaceEnvVars( const LString& String );

	/**
	   Remove full path from FileName and use full path from ParentFileName if FileName exists there.
	   Otherwise return original FileName.
	**/
	scriptmethod LString      FindFile( const LString& FileName, const LString& ParentFileName );

	/**
	   Check if the specified physical file name is valid for OS
	   (backslashes and other things)
	**/
	static       LString      ValidatePath( LString PathName );

	/// creation of a physical file reader
	scriptmethod iRAWFile*    CreatePhysicalFileReader( const LString& FileName, const LString& VirtualName ) const;

	/// Create file mapper and set its raw file
	scriptmethod iIStream* CreateFileMapperForRawFile( iRAWFile* RF ) const;

	/// Compare two files. Returns 0 if files are equal or difference offset in the first file otherwise
	scriptmethod Luint64      CompareFiles( const LString& FileName1, const LString& FileName2 ) const;

	/// Enable or disable usage of virtual file names
	scriptmethod void         SetUseVirtualFileNames( bool Use ) { FUseVirtualFileNames = Use; };

	/// Disable usage of virtual file names
	scriptmethod bool    GetUseVirtualFileNames() const { return FUseVirtualFileNames; };

	/// low-level file enumeration (uses FindFirst/FindNext or opendir(), depending on the platform)
	scriptmethod void    Enumerate( const LString& VirtualPath, bool SubDirs, bool Files, bool SaveFullNames, LStr::clStringsVector* StrVec ) const;

	/// Add new file change/delete/rename notification
	scriptmethod void    AddFileWatch( const LString& FileName, iObject* Handler );

	/// Add a single handler for multiple file changes
	scriptmethod void    AddMultipleFileWatches( const LStr::clStringsVector& FileNames, iObject* Handler );

	/// Watch all files in the directory (using FS::Enumerate)
	scriptmethod void    StartDirectoryWatch( const LString& DirName, bool Recursive, iObject* Handler );

	/// Add new dir change notification
	scriptmethod void    AddDirWatch( const LString& DirName, bool Recursive );

	/// Remove file change notification
	scriptmethod void    RemoveFileWatch( const LString& FileName );

	/// Remove dir change notification
	scriptmethod void    RemoveDirWatch( const LString& DirName );

	/// open URL in a default web browser
	scriptmethod void    OpenURL( const LString& URL );

	/// set desktop wallpaper
	scriptmethod void    SetWallpaper( const LString& ImageFileName );

	/// open image dialog
	scriptmethod void    OpenImageDialog();

	FWD_EVENT_HANDLER( Event_POSTINIT );
private:
	/// Search for a mount point for this file
	iMountPoint*  FindMountPoint( const LString& FileName ) const;

	/// Win32/Win64 OS-specific : access to resources embedded into the executable
	clMemRAWFile* GetResourceStream( const LString& ResourceID, const LString& ResourceType ) const;

	/// the list of active mount points
	LArray<iMountPoint*> FMountPoints;

	/// use virtual file names with mount points (true by defaul)
	bool                       FUseVirtualFileNames;

	/// Do we throw exceptions in XLML parsing or not
	bool    IsFatalOnErrors() const;

#pragma region XLML loading internals

	/// Create an hierarchy of nodes representing XLML stream with different reading modes
	scriptmethod mlNode* LoadXLMLFromStream_Internal( iIStream* IStream, bool NonStrictASEMode ) const;

	/// Load an hierarchy of nodes representing input ASE-like XLML stream
	scriptmethod mlNode* LoadXLML_Internal( const LString& FileName, bool NonStrictASEMode ) const;

#pragma endregion

#pragma region Networking/Download stuff
private:
	/// HTTP download thread. I just don't know where to put this
	clDownloadThread* FDownloadThread;

	/// Can be more than one, no time to mess with pointers though
	clHTTPServerThread* FServerThread;

	void DownloadFileC( const LString& Args );
	void StartWebServerC( const LString& Args );
	void StopWebServersC( const LString& Args );

#pragma endregion

#pragma region File change notifications

#ifdef OS_WINDOWS
	// implemented for windows-based content editing and automatic recaching

	/// Our file watching stuff implemented using native file system events
	clFileWatchThread* FFileWatchThread;
#endif

public:
	/**
	yes, I know. They must be private.

	But we bind them in Console and unbind in ~Console, so we must have the access here. friend-class is not a good solution
	*/

	void AddFileWatchC( const LString& Args );
	void AddDirWatchC( const LString& Args );
	void RemoveFileWatchC( const LString& Args );
	void RemoveDirWatchC( const LString& Args );

#pragma endregion
};

#endif

/*
 * 03/07/2010
     FileExistsInResource()
 * 18/05/2010
     ReplaceEnvVars()
 * 08/02/2010
     CompareFiles()
 * 15/05/2009
     GetResourceStream moved here from SystemLayer
     Enumerate() moved from SystemLayer
 * 26/04/2009
     GetMaxTimeForDirectory()
 * 09/04/2009
     Merged with iFileSystem
 * 25/08/2006
     LoadTextFile() removed
 * 21/10/2005
     CreateTokenizer()
     Parametrize()
 * 01/12/2004
     It's here
*/
