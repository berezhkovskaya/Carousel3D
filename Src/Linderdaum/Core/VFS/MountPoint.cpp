/**
 * \file MountPoint.cpp
 * \brief Virtual file system mount points
 * \version 0.5.99
 * \date 06/07/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author Viktor Latypov, 2011 (.tar support)
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/Files.h"
#include "Core/Logger.h"
#include "MountPoint.h"

#include <memory>

#ifdef OS_WINDOWS
#  include <direct.h>
#  include <windows.h>
#else // any POSIX system would do (MacOS, Linux, Android etc.)
#  include <dirent.h>
#  include <libgen.h>
#endif

#include <time.h>
#include <sys/stat.h>

/// Common function used in virtual-to-physical name mapping
inline LString FixInternalFileName( const LString& VirtualName )
{
	LString Name( VirtualName );
	LStr::ReplaceAll( &Name, '/', PATH_SEPARATOR );
	LStr::ReplaceAll( &Name, '\\', PATH_SEPARATOR );

	LStr::ToUpper( &Name );

	return Name;
}

bool clMountPoint::FileExistsAtThisPoint( const LString& VirtualName ) const
{
	guard( "%s", VirtualName.c_str() );

	LString LocalPath = MapToThisPoint( VirtualName );

	bool Result = Env->FileSystem->FileExistsPhys( LocalPath );

	Env->Logger->LogP( L_PARANOID, "LocalPath = %s [%i]", LocalPath.c_str(), Result );

	return Result;

	unguard();
}

LString clMountPoint::MapToThisPoint( const LString& VirtualName ) const
{
	if ( !Env->FileSystem->GetUseVirtualFileNames() )
	{
		return VirtualName;
	}

	if ( Env->FileSystem->IsFullPath( VirtualName ) )
	{
		return VirtualName;
	}

	return ( FPhysicalName + VirtualName );
}

iRAWFile* clMountPoint::CreateRAWFile( const LString& VirtualName ) const
{
	guard( "%s", VirtualName.c_str() );

	LString PhysName = MapToThisPoint( VirtualName );

	if ( Env->FileSystem->IsFullPath( VirtualName ) )
	{
		PhysName = VirtualName;
	}

	Env->Logger->LogP( L_PARANOID, "PhysName = %s", PhysName.c_str() );

	return Env->FileSystem->CreatePhysicalFileReader( PhysName, VirtualName );

	unguard();
}

Lint64 clMountPoint::GetFileTime( const LString& VirtualName ) const
{
	LString PhysicalName = MapToThisPoint( VirtualName );

#ifdef OS_WINDOWS
	struct _stat Buf;

	int Result = _stat( PhysicalName.c_str(), &Buf );
#else
	struct stat Buf;

	int Result = stat( PhysicalName.c_str(), &Buf );
#endif

	// Check if statistics are valid
	RAISE_MSG_IF( Result != 0, clFileSystem::clExcept_UnableToReadFileTime, LString( "\"" ) + PhysicalName + "\"" );

	return Buf.st_mtime;
}

//// Archive files

#pragma region .RAR files support

#pragma pack(push, 1)
struct GCC_PACK( 1 ) MarkHeader
{
   char Mark[7];
};

struct GCC_PACK( 1 ) BaseBlock
{
   unsigned short BlockCRC;
   char           BlockType;
   unsigned short Flags;
   unsigned short BlockSize;
};

struct GCC_PACK( 1 ) FileHeader
{
   unsigned int   UnpSize;
   char           HostOS;
   unsigned int   FileCRC;
   unsigned int   FileTime;
   char           UnpVer;
   char           Method;
   unsigned short NameSize;
   union
   {
      unsigned int FileAttr;
      unsigned int SubFlags;
   };
};

struct GCC_PACK( 1 ) ArchiveHeader: BaseBlock
{
   char Reserved1[2];
   char Reserved2[4];
};
#pragma pack(pop)

LString ReadName( iIStream* In, int Size )
{
	LString Name = LStr::ToStr( In->MapStreamFromCurrentPos(), Size );

	In->SeekOffset( Size );

	return Name;
}

bool IsValid( char D[] )
{
	return D[0] == 0x52 && D[1] == 0x61 && D[2] == 0x72 && D[3] == 0x21 && D[4] == 0x1a && D[5] == 0x07 && D[6] == 0x00;
}

const char    HEAD_TYPE_MARKER_BLOCK   = 0x72;     //  marker block
const char    HEAD_TYPE_ARCHIVE_HEADER = 0x73;     //  archive header
const char    HEAD_TYPE_FILE_HEADER    = 0x74;     //  file header
const char    HEAD_TYPE_COMMENT_HEADER = 0x75;     //  comment header
const char    HEAD_TYPE_EXTRA_INFO     = 0x76;     //  extra information
const char    HEAD_TYPE_SUBBLOCK       = 0x77;     //  subblock
const char    HEAD_TYPE_RECOVERY_REC   = 0x78;     //  recovery record

#define  LHD_WINDOWMASK     0x00e0
#define  LHD_DIRECTORY      0x00e0

void clArchiveMountPoint::LocateFiles_RAR( iIStream* In )
{
	guard();

	FATAL( In->GetFileSize() < sizeof( ArchiveHeader ), "Corrupted RAR archive: " + In->GetFileName() );

	Luint64 OldPos = In->GetFilePos();

	while ( !In->Eof() )
	{
		Luint64 Pos = In->GetFilePos();

		BaseBlock Block;
		In->BlockRead( &Block, sizeof( Block ) );

		// read optional additional block size
		unsigned int AddBlockSize = 0;

		if ( ( Block.Flags & 0x8000 ) != 0 )
		{
			In->BlockRead( &AddBlockSize, sizeof( AddBlockSize ) );
		}

		// read data block
		if ( Block.BlockType == HEAD_TYPE_FILE_HEADER )
		{
			FileHeader FH;
			In->BlockRead( &FH, sizeof( FH ) );

			LString Name( ReadName( In, FH.NameSize ) );
			LStr::ReplaceAll( &Name, '/', PATH_SEPARATOR );
			LStr::ReplaceAll( &Name, '\\', PATH_SEPARATOR );

			if ( ( Block.Flags & LHD_WINDOWMASK ) != LHD_DIRECTORY )
			{
				FATAL( FH.Method != 0x30 || AddBlockSize != FH.UnpSize,
				       "Compressed RAR archives are not supported: " + In->GetFileName() + "\n File \"" + Name + "\" seems to be compressed. Set 'Store' compression method while adding files to archive" );

				sFileLocation Location;

				Location.FLocation = Pos + Block.BlockSize;
				Location.FSize     = FH.UnpSize;
				Location.FTime     = FH.FileTime;

				FLocations[ LStr::GetUpper( Name ) ] = Location;

				Env->Logger->Log( L_PARANOID, "File: " + Name );
			}
			else
			{
				Env->Logger->Log( L_PARANOID, "Directory: " + Name );
			}
		}
		else
		{
			// unknown block type - skip it
		}

		Luint64 Offset = Block.BlockSize + AddBlockSize;

		if ( Pos + Offset > In->GetFileSize() ) { break; }

		In->Seek( Pos + Offset );
	}

	In->Seek( OldPos );

	unguard();
}

#pragma endregion

#pragma region .TAR files

/**
Link indicator fieldValue  Meaning
'0'   Normal file
(ASCII NUL) Normal file (now obsolete)
'1'   Hard link
'2'   Symbolic link
'3'   Character special
'4'   Block special
'5'   Directory
'6'   FIFO
'7'   Contiguous file

// GNU Tar long file names

'72'    Long file name
'208'   Long dir name start
'28'    Long file name start
*/

struct GCC_PACK( 1 ) sTARHeader
{
   /// File name
   Lubyte     FFileName[100];

   /// File mode
   Luint64    FFileMode;

   /// Owner's numeric user ID
   Luint64    FOwnerID;

   /// Group's numeric user ID
   Luint64    FGroupID;

   /// 12 bytes for file size
   Lubyte     FFileSize[12];

   /// Last modification time in numeric Unix time format
   Lubyte     FModificationDate[12];

   /// Checksum for header block
   Luint64    FHeaderCheckSum;

   /// Link indicator (file type)
   Lubyte     FType;

   /// Name of linked file
   Lubyte     FLinkedFileName[100];

   /// UStar indicator "ustar"
   Lubyte     FUSTAR[6];

   /// UStar version "00"
   Lubyte     FUSTARVersion[2];

   /// Owner user name
   Lubyte     FOwnerUserName[32];

   /// Owner group name
   Lubyte     FOwnerGroupName[32];

   /// Device major number
   Luint64    FDeviceMajor;

   /// Device minor number
   Luint64    FDeviceMinor;

   /// Filename prefix
   Lubyte     FFileNamePrefix[155];

   /// 512-byte padding
   Lubyte     FPadding[12];
};

Luint64 OctalByteArrayToLongInt( Lubyte* ptr, int ArrayLen )
{
	Luint64 Res = 0;

	for ( int i = 0 ; i < ArrayLen ; i++ )
		if ( ptr[i] > 32 )
		{
			Res *= 8;
			Res += ( Luint64 ) ( ( char )ptr[i] - '0' );
		}

	return Res;
}

int IsZero( void* buf )
{
	for ( int i = 0 ; i < 512 ; i++ ) if ( ( ( unsigned char* )buf )[i] ) { return 0; }

	return 1;
}

LString SafeArrayToString( Lubyte* ArrayPtr, int MaxLen )
{
	if ( MaxLen <= 0 ) { return ""; }

	char* StrPtr = ( char* )ArrayPtr;

	return LString( &StrPtr[0] );
}

void clArchiveMountPoint::LocateFiles_TAR( iIStream* In )
{
	guard();

	struct sTARHeader Head;

	bool LongName = false;
	LString CurrentName = "";

	Luint64 Ofs = 0;

	const int HSize = sizeof( struct sTARHeader );

	while ( !In->Eof() )
	{
		In->BlockRead( &Head, HSize );

		Ofs += HSize;

		if ( IsZero( &Head ) ) { break; }

		Lubyte Type = ( Lubyte )( ( char )Head.FType - '0' );
		Luint64 Size = OctalByteArrayToLongInt( Head.FFileSize, 12 );

		/// Round to 512
		Luint64 Mod = ( Size % 512 );
		Luint64 RoundSize = Size + ( Mod == 0x0 ? 0x0 : ( 512 - Mod ) );

		/// Skip unknown blocks (0 and 72 are supported)
		if ( Type == 0 )
		{
			/// Normal file, skip bytes
			In->SeekOffset( RoundSize );

			LString TheName = LongName ? CurrentName : SafeArrayToString( Head.FFileName, 300 );

			// strip ./ from the file name
			if ( LStr::StartsWith( TheName, "./" ) ) { TheName = LString( TheName.c_str() + 2 ); }

			Env->Logger->LogP( L_PARANOID, "Found file in .tar: %s", TheName.c_str() );

			sFileLocation Location;

			Location.FLocation = Ofs;
			Location.FSize     = Size;
			Location.FTime     = OctalByteArrayToLongInt( Head.FModificationDate, 12 );

			FLocations[ LStr::GetUpper( TheName ) ] = Location;

			Ofs += RoundSize;

			LongName = false;
			CurrentName = "";
		}
		else if ( Type == 72 )
		{
			LongName = true;
			CurrentName =  SafeArrayToString( Head.FFileName, 300 );
		}
	}

	unguard();
}

#pragma endregion

clArchiveMountPoint::clArchiveMountPoint( sEnvironment* Env, const LString& PhysicalName )
{
	this->Env = Env;

	// LV : I think we should use PhysicalName directly and not try to add the "Data\\" prefix, as it causes a lot of confusion
	// i.e., it is _impossible_ to mount the .rar file in the project (not in Data\ folder) , without setting DataDir to "".

	iRAWFile* RawFile = Env->FileSystem->CreatePhysicalFileReader( PhysicalName, PhysicalName );
	FArchiveFile = Env->FileSystem->CreateFileMapperForRawFile( RawFile );

	if ( ( LStr::GetUpper( PhysicalName ).find( ".RAR" ) != -1 ) || ( LStr::GetUpper( PhysicalName ).find( "_RAR" ) != -1 ) )
	{
		MarkHeader Head;
		FArchiveFile->BlockRead( &Head, sizeof( Head ) );

		if ( Head.Mark[0] != 'L' )
		{
			FATAL( !IsValid( Head.Mark ), "Invalid RAR file header: " + FArchiveFile->GetFileName() );
		}

		FArchiveFile->SeekOffset( sizeof( ArchiveHeader ) );

		LocateFiles_RAR( FArchiveFile );
	}
	else if ( ( LStr::GetUpper( PhysicalName ).find( ".TAR" ) != -1 ) || ( LStr::GetUpper( PhysicalName ).find( "_TAR" ) != -1 ) )
	{
		LocateFiles_TAR( FArchiveFile );
	}
	else
	{
		FATAL_MSG( LString( "Cannot determine archive type for " ) + PhysicalName );
	}
};

clArchiveMountPoint::~clArchiveMountPoint()
{
	delete( FArchiveFile );
}

bool clArchiveMountPoint::FileExistsAtThisPoint( const LString& VirtualName ) const
{
	return ( FLocations.find( FixInternalFileName( VirtualName ) ) != FLocations.end() );
}

LString clArchiveMountPoint::MapToThisPoint( const LString& VirtualName ) const
{
	return VirtualName;
}

iRAWFile* clArchiveMountPoint::CreateRAWFile( const LString& VirtualName ) const
{
	clLocationsList::const_iterator i = FLocations.find( FixInternalFileName( VirtualName ) );

	FATAL( i == FLocations.end(), "Unable to access " + VirtualName + " in " + FArchiveFile->GetFileName() );

	sFileLocation Location = i->second;

	clMemRAWFile* File = new clMemRAWFile();
	File->Env = Env;
	File->AfterConstruction();

	File->SetFileName( VirtualName );
	File->SetVirtualFileName( VirtualName );

	File->CreateFromManagedBuffer( FArchiveFile->MapStream() + Location.FLocation, Location.FSize );

	Env->Logger->Log( L_PARANOID, "Accessing " + VirtualName + " in " + FArchiveFile->GetFileName() );

	return File;
}

Lint64 clArchiveMountPoint::GetFileTime( const LString& VirtualName ) const
{
	clLocationsList::const_iterator i = FLocations.find( FixInternalFileName( VirtualName ) );

	RAISE_MSG_IF( i == FLocations.end(), clFileSystem::clExcept_UnableToReadFileTime, FArchiveFile->GetFileName() + LString( ":\"" ) + VirtualName + "\"" );

	sFileLocation Location = i->second;

	return Location.FTime;
}


//////// New implementation of memory mount-points (archived/compressed stuff)

Lint64 clMemoryMountPoint::GetFileTime( const LString& VirtualName ) const
{
	clLocationsList::const_iterator i = FLocations.find( FixInternalFileName( VirtualName ) );

	if ( i == FLocations.end() ) { Env->Logger->Log( L_WARNING, GetName() + LString( ": \"" ) + VirtualName + LString( "\"" ) ); return 0; }

	return i->second.FTime;
}

bool clMemoryMountPoint::FileExistsAtThisPoint( const LString& VirtualName ) const
{
	return ( FLocations.find( FixInternalFileName( VirtualName ) ) != FLocations.end() );
}

iRAWFile* clMemoryMountPoint::CreateRAWFile( const LString& VirtualName ) const
{
	clLocationsList::const_iterator i = FLocations.find( FixInternalFileName( VirtualName ) );

	FATAL( i == FLocations.end(), "Unable to access " + VirtualName + " in " + GetName() );

	sFileLocation Location = i->second;

	clMemRAWFile* File = Construct<clMemRAWFile>( Env );
	File->SetFileName( VirtualName );
	File->SetVirtualFileName( VirtualName );
	File->CreateFromManagedBuffer( Location.FFileData, Location.FSize );

	return File;
}

void clMemoryMountPoint::RegisterFile( const LString& Name, void* Data, size_t Size, Luint64 Time )
{
	/// Add something from the blob to FLocations
	if ( FileExistsAtThisPoint( Name ) )
	{
		Env->Logger->LogP( L_WARNING, "File already exists at this point (%s)", Name.c_str() );
		return;
	}

	sFileLocation F;
	F.FLocation    = 0;
	F.FSize        = Size;
	F.FTime        = Time;
	F.FSourceBlock = Data;
	F.FFileData    = Data;

	FLocations.insert( std::pair<LString, sFileLocation>( Name, F ) );
}

void clMemoryMountPoint::UnregisterFile( const LString& VirtualName )
{
	clLocationsList::iterator i = FLocations.find( FixInternalFileName( VirtualName ) );

	if ( i == FLocations.end() )
	{
		Env->Logger->LogP( L_WARNING, "Unable to remove %s", VirtualName.c_str(), GetName().c_str() );
		return;
	}

	/// Just remove the pointer from FLocations
	FLocations.erase( i );
}
