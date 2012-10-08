/**
 * \file Archive.cpp
 * \brief Archive reader
 * \version 0.6.06
 * \date 04/01/2012
 * \author Viktor Latypov, 2011-2012 (.tar and .zip support)
 * \author viktor@linderdaum.com
 * \author Sergey Kosarevsky, 2005 (.rar support)
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Core/Logger.h"

#include "Core/VFS/FileSystem.h"
#include "Core/VFS/Files.h"
#include "Core/VFS/Archive.h"

#include "Utils/LBlob.h"

#include "SysEnv.h"

#include "libcompress.h"

// dummy error handler for the bzip2 library
extern "C" void bz_internal_error( int e_code ) { ( void )e_code; }

bool clArchiveReader::OpenArchiveFile( const LString& FName )
{
	iIStream* TheFile = Env->FileSystem->CreateFileReader( FName );

	return TheFile ? OpenArchive( TheFile, true ) : false;
}

bool clArchiveReader::CloseArchive()
{
	if ( FCompletionFlag ) { *FCompletionFlag = false; }

	FAbortScanningFlag = 0;

	/// Clear info containers
	FFileInfos.clear();
	FFileInfoIdx.clear();
	FFileNames.clear();
	FRealFileNames.clear();

	/// Clear data cache
	ClearExtracted();

	/// Close the source stream
	if ( FOwnsSourceFile ) { if ( FSourceFile ) { FSourceFile->DisposeObject(); } }

	/// Clear the pointers
	FSourceFile = NULL;
	FOwnsSourceFile = false;
	FArchiveType = ARCH_INVALID;

	return true;
}

/// Determine archive type using the file extension
bool clArchiveReader::EnumerateFiles()
{
	if ( FCompletionFlag ) { *FCompletionFlag = false; }

	if ( !FSourceFile ) { return false; }

	FAbortScanningFlag = 0;

	LString Ext, TheName = LStr::GetUpper( FSourceFile->GetFileName() );

	clFileSystem::SplitPath( TheName, NULL, NULL, NULL, &Ext );

	bool Res = false;

	if ( ( Ext == ".RAR" ) || ( TheName.find( "_RAR" ) == TheName.length() - 4 ) )
	{
		if ( Enumerate_RAR() )
		{
			FArchiveType = ARCH_RAR;
			Res = true;
		}
	}
	else if ( ( Ext == ".TAR" ) || ( TheName.find( "_TAR" ) == TheName.length() - 4 ) )
	{
		if ( Enumerate_TAR() )
		{
			FArchiveType = ARCH_TAR;
			Res = true;
		}
	}
	else if ( ( ( Ext == ".ZIP" ) || ( TheName.find( "_ZIP" ) == TheName.length() - 4 ) ) ||
	          ( ( Ext == ".APK" ) || ( TheName.find( "_APK" ) == TheName.length() - 4 ) ) )
	{
		if ( Enumerate_ZIP() )
		{
			FArchiveType = ARCH_ZIP;
			Res = true;
		}
	}

	/// When all else fails
	if ( !Res ) { Env->Logger->LogP( L_WARNING, "Unknown archive type in file %s", TheName.c_str() ); }

	/// Inform our mount point that we are done
	if ( FCompletionFlag ) { *FCompletionFlag = true; }

	return Res;
}

///// .ZIP stuff

/*
   .ZIP reading code is based on the following source:

   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )
   Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )
   Copyright (C) 2007-2008 Even Rouault
   Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

/// First we define some callback routines for zlib/minizip's ioapi.h

/// Here we just use the iIStream as a file handle, thus the open/close routines are just dummies.

#define WRITEBUFFERSIZE (8192)
#define UNZ_ABORTED (-1005)

static voidpf ZCALLBACK l_read_fopen64_file_func ( voidpf opaque, const void* filename, int mode )
{
	( ( iIStream* )opaque )->Seek( 0 );
	( void )filename;
	( void )mode;
	return opaque;
}

static uLong ZCALLBACK l_read_fread_file_func ( voidpf opaque, voidpf stream, void* buf, uLong size )
{
	iIStream* S = ( iIStream* )stream;
	Lint64 CanRead = ( Lint64 )size;
	Lint64 Sz = S->GetFileSize();
	Lint64 Ps = S->GetFilePos();

	if ( CanRead + Ps >= Sz ) { CanRead = Sz - Ps; }

	if ( CanRead > 0 ) {  S->BlockRead( buf, ( Luint64 )CanRead ); }
	else { CanRead = 0; }

	return ( uLong )CanRead;
	/*
	   ((iIStream*)stream)->BlockRead(buf, size);
	   return size;
	*/
}

static ZPOS64_T ZCALLBACK l_read_ftell64_file_func ( voidpf opaque, voidpf stream ) { return ( ZPOS64_T )( ( iIStream* )stream )->GetFilePos(); }

static long ZCALLBACK l_read_fseek64_file_func ( voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin )
{
	iIStream* S = ( iIStream* )stream;
	Lint64 NewPos = ( Lint64 )offset, ThisPos = ( Lint64 )S->GetFilePos();
	Lint64 Sz = ( Lint64 )S->GetFileSize();

	switch ( origin )
	{
		case ZLIB_FILEFUNC_SEEK_CUR:
		{ NewPos += ThisPos; break; }
		case ZLIB_FILEFUNC_SEEK_END:
		{ NewPos = Sz - 1 - NewPos; break; }
		case ZLIB_FILEFUNC_SEEK_SET:
		{ break; }
		default:
			return -1;
	}

	if ( NewPos >= 0 && ( NewPos < Sz ) ) { S->Seek( ( Luint64 )NewPos ); }
	else { return -1; }

	return 0;
	/*
	   iIStream* S = ((iIStream*)stream);
	   uLong NewPos = (uLong)offset, ThisPos = (uLong)S->GetFilePos();

	   switch (origin)
	   {
	      case ZLIB_FILEFUNC_SEEK_CUR : { NewPos += ThisPos; break; }
	      case ZLIB_FILEFUNC_SEEK_END : { NewPos = (uLong)S->GetFileSize() - NewPos; break; }
	      case ZLIB_FILEFUNC_SEEK_SET : { break; }
	      default: return -1;
	   }

	   S->Seek(NewPos);
	   return 0;
	*/
}

static int ZCALLBACK l_read_fclose_file_func ( voidpf opaque, voidpf stream ) { return 0; }
// use (((iObject*)stream)->GetError()); ?
static int ZCALLBACK l_read_ferror_file_func ( voidpf opaque, voidpf stream ) { return 0; }

void fill_fopen64_L_readfilefunc ( iIStream* Stream, zlib_filefunc64_def*  pzlib_filefunc_def )
{
	pzlib_filefunc_def->zopen64_file = l_read_fopen64_file_func;
	pzlib_filefunc_def->zread_file = l_read_fread_file_func;
	pzlib_filefunc_def->zwrite_file = NULL;
	pzlib_filefunc_def->ztell64_file = l_read_ftell64_file_func;
	pzlib_filefunc_def->zseek64_file = l_read_fseek64_file_func;
	pzlib_filefunc_def->zclose_file = l_read_fclose_file_func;
	pzlib_filefunc_def->zerror_file = l_read_ferror_file_func;
	pzlib_filefunc_def->opaque = Stream;
}

/// Internal routine to extract a single file from ZIP archive
int ExtractCurrentFile_ZIP( unzFile uf, const char* password, int* abort_flag, float* progress, iOStream* fout )
{
	sSysEnv* _Env = fout->Env;

	char filename_inzip[256];
	int err = UNZ_OK;
	void* buf;
	uInt size_buf;
	unz_file_info64 file_info;

	err = unzGetCurrentFileInfo64( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );

	if ( err != UNZ_OK ) { _Env->Logger->LogP( L_WARNING, "error %d with zipfile in unzGetCurrentFileInfo\n", err ); return err; }

	Luint64 file_size = ( Luint64 )file_info.uncompressed_size;
	Luint64 total_bytes = 0;

	unsigned char _buf[WRITEBUFFERSIZE];
	size_buf = WRITEBUFFERSIZE;
	buf = ( void* )_buf;

//(void*)malloc(size_buf);
	if ( buf == NULL ) { _Env->Logger->LogP( L_WARNING, "Error allocating memory\n" ); return UNZ_INTERNALERROR; }

	err = unzOpenCurrentFilePassword( uf, password );

	if ( err != UNZ_OK ) { _Env->Logger->LogP( L_WARNING, "error %d with zipfile in unzOpenCurrentFilePassword\n", err ); }

	if ( progress ) { *progress = 0.0f; }

	do
	{
		if ( abort_flag ) { if ( *abort_flag ) { break; } }

		err = unzReadCurrentFile( uf, buf, size_buf );

		if ( err < 0 ) { _Env->Logger->LogP( L_WARNING, "error %d with zipfile in unzReadCurrentFile\n", err ); break; }

		if ( err > 0 ) { total_bytes += err; fout->BlockWrite( buf, err ); /* handle write errors ? */ }

		if ( progress ) { *progress = ( float )( total_bytes ) / ( float )( file_size ); }
	}
	while ( err > 0 );

	int close_err = unzCloseCurrentFile ( uf );

	if ( close_err != UNZ_OK ) { _Env->Logger->LogP( L_WARNING, "error %d with zipfile in unzCloseCurrentFile\n", close_err ); }

//	free(buf);

	if ( abort_flag ) { if ( *abort_flag ) { return UNZ_ABORTED; } }

	return err;
}

bool clArchiveReader::Enumerate_ZIP()
{
	iIStream* TheSource = Env->FileSystem->CreateFileReader( FSourceFile->GetVirtualFileName() );

	zlib_filefunc64_def ffunc;
	fill_fopen64_L_readfilefunc( TheSource/*FSourceFile*/, &ffunc );

	unzFile uf = unzOpen2_64( "", &ffunc );

	unz_global_info64 gi;
	int err = unzGetGlobalInfo64( uf, &gi );
	/* if (err!=UNZ_OK) printf("error %d with zipfile in unzGetGlobalInfo \n",err);*/

	for ( uLong i = 0; i < gi.number_entry; i++ )
	{
		if ( FAbortScanningFlag ) { return false; }

		char filename_inzip[256];
		unz_file_info64 file_info;
		err = unzGetCurrentFileInfo64( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );

		if ( err != UNZ_OK ) { /* printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);*/ break; }

		if ( ( i + 1 ) < gi.number_entry )
		{
			err = unzGoToNextFile( uf );

			if ( err != UNZ_OK ) { Env->Logger->LogP( L_WARNING, "error %d with zipfile in unzGoToNextFile\n", err ); break; }
		}

		sFileInfo Info;
		LString TheName = LStr::GetUpper( LString( filename_inzip ) );
		LStr::ReplaceAll( &TheName, '/', PATH_SEPARATOR );
		LStr::ReplaceAll( &TheName, '\\', PATH_SEPARATOR );

		Luint64 _y = file_info.tmu_date.tm_year;
		Luint64 _m = file_info.tmu_date.tm_mon;
		Luint64 _d = file_info.tmu_date.tm_mday;
		Luint64 _H = file_info.tmu_date.tm_hour;
		Luint64 _M = file_info.tmu_date.tm_min;
//		Luint64 _S = 0;

		Info.FOffset = 0;
		Info.FCompressedSize = file_info.compressed_size;
		Info.FSize = file_info.uncompressed_size;
		Info.FTimeStamp = ( _y << 25 ) | ( _m << 21 ) | ( _d << 16 ) | ( _H << 11 ) | ( _M << 5 );

		FFileInfos.push_back( Info );
		FFileInfoIdx[TheName] = ( int )FFileNames.size();
		FFileNames.push_back( TheName );
		FRealFileNames.push_back( LString( filename_inzip ) );
	}

	unzClose( uf );
	/*
	   Env->Logger->LogP(L_DEBUG, "File list for %s", FSourceFile->GetFileName().c_str());

	   for(size_t j = 0 ; j != FFileNames.size() ; j++)
	   {
	      Env->Logger->LogP(L_WARNING, "File[%d] =  %s (a.k.a '%s'), Size=%i", j, FRealFileNames[j].c_str(), FFileNames[j].c_str(), FFileInfos[j].FSize);
	   }
	*/
	TheSource->DisposeObject();

	return true;
}

/// end of .ZIP stuff

bool clArchiveReader::ExtractSingleFile( const LString& FName, const LString& Password, int* AbortFlag, float* Progress, iOStream* FOut )
{
	int err = UNZ_OK;

	LString ZipName = FName;
	LStr::ReplaceAll( &ZipName, '\\', '/' );

	if ( FArchiveType == ARCH_ZIP )
	{
		iIStream* TheSource = Env->FileSystem->CreateFileReader( FSourceFile->GetVirtualFileName() );

		/// Decompress the data
		zlib_filefunc64_def ffunc;
		fill_fopen64_L_readfilefunc( TheSource/*FSourceFile*/, &ffunc );

		unzFile uf = unzOpen2_64( "", &ffunc );

		if ( unzLocateFile( uf, ZipName.c_str(), 0/*CASESENSITIVITY - insensitive*/ ) != UNZ_OK )
		{
			Env->Logger->LogP( L_WARNING, "File %s not found in the zipfile\n", FName.c_str() );

			TheSource->DisposeObject();
			return false;
		}

		err = ExtractCurrentFile_ZIP( uf, Password.empty() ? NULL : Password.c_str(), AbortFlag, Progress, FOut );

		if ( err != UNZ_OK ) { Env->Logger->LogP( L_WARNING, "Archive::ExtractSingleFile::Zip - error: %d\n", err ); }

		TheSource->DisposeObject();
		unzClose( uf );
	}
	else
	{
		Luint64 Sz = GetFileSize( FName );
		const void* Ptr = GetFileData( FName );

		if ( !Ptr ) { return false; }

		/// Dump the data to OutStream
		FOut->BlockWrite( Ptr, Sz );
	}

	return ( err == UNZ_OK );
}

const void* clArchiveReader::GetFileData_ZIP( size_t idx )
{
	/// sanity check
	if ( FExtractedFromArchive.count( ( int )idx ) > 0 ) { return FExtractedFromArchive[( int )idx]; }

	clMemFileWriter* FOut = Env->FileSystem->CreateMemFileWriter( "mem_blob", FFileInfos[idx].FSize );

	void* DataPtr = NULL;

	if ( ExtractSingleFile( FRealFileNames[idx], "", NULL, NULL, FOut ) )
	{
		size_t Sz = static_cast<size_t>( FOut->GetFilePos() );

		clBlob* B = FOut->GetContainer();
		B->SafeResize( Sz );

		const void* BlobPtr = B->GetDataConst();
		DataPtr = AllocData( Sz );

		::memcpy( DataPtr, BlobPtr, Sz );
	}

	FOut->DisposeObject();

	return DataPtr;
}

const void* clArchiveReader::GetFileData_TAR_or_RAR( size_t idx )
{
	return ( const void* )( FSourceFile->MapStream() + FFileInfos[idx].FOffset );
}

///////// File enumeration
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

LString Arch_ReadName( iIStream* In, int Size )
{
	LString Name = LStr::ToStr( In->MapStreamFromCurrentPos(), Size );

	In->SeekOffset( Size );

	return Name;
}

bool Arch_IsValid( char D[] )
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

bool clArchiveReader::Enumerate_RAR()
{
	guard();

	iIStream* In = FSourceFile;

	if ( !In ) { return false; }

	MarkHeader Head;
	In->BlockRead( &Head, sizeof( Head ) );

	if ( Head.Mark[0] != 'L' )
	{
		FATAL( !Arch_IsValid( Head.Mark ), "Invalid RAR file header: " + In->GetFileName() );
	}

	In->SeekOffset( sizeof( ArchiveHeader ) );

	if ( In->GetFileSize() < sizeof( ArchiveHeader ) )
	{
		Env->Logger->Log( L_WARNING, "Corrupted RAR archive: " + In->GetFileName() );
		return false;
	}

	Luint64 OldPos = In->GetFilePos();

	while ( !In->Eof() )
	{
		if ( FAbortScanningFlag ) { return false; }

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

			LString Name( Arch_ReadName( In, FH.NameSize ) );
			LStr::ReplaceAll( &Name, '/', PATH_SEPARATOR );
			LStr::ReplaceAll( &Name, '\\', PATH_SEPARATOR );

			if ( ( Block.Flags & LHD_WINDOWMASK ) != LHD_DIRECTORY )
			{
				if ( FH.Method != 0x30 || AddBlockSize != FH.UnpSize )
				{
					Env->Logger->Log( L_WARNING, "Compressed RAR archives are not supported: " + In->GetFileName() + "\n File \"" + Name + "\" seems to be compressed. Set 'Store' compression method while adding files to archive" );
					return false;
				}

				sFileInfo Info;

				Info.FOffset    = Pos + Block.BlockSize;
				Info.FSize      = Info.FCompressedSize = FH.UnpSize;
				Info.FTimeStamp = FH.FileTime;

				LString TheName = LStr::GetUpper( Name );
				FFileInfos.push_back( Info );
				FFileInfoIdx[TheName] = ( int )FFileNames.size();
				FFileNames.push_back( TheName );

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

	return true;

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

#pragma pack(push, 1)

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

#pragma pack(pop)

Luint64 Arch_OctalByteArrayToLongInt( Lubyte* ptr, int ArrayLen )
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

int Arch_IsZero( void* buf )
{
	for ( int i = 0 ; i < 512 ; i++ ) if ( ( ( unsigned char* )buf )[i] ) { return 0; }

	return 1;
}

LString Arch_SafeArrayToString( Lubyte* ArrayPtr, int MaxLen )
{
	if ( MaxLen <= 0 ) { return ""; }

	char* StrPtr = ( char* )ArrayPtr;

	return LString( &StrPtr[0] );
}

bool clArchiveReader::Enumerate_TAR()
{
	guard();

	iIStream* In = FSourceFile;

	struct sTARHeader Head;

	bool LongName = false;
	LString CurrentName = "";

	Luint64 Ofs = 0;

	const int HSize = sizeof( struct sTARHeader );

	while ( !In->Eof() )
	{
		if ( FAbortScanningFlag ) { return false; }

		In->BlockRead( &Head, HSize );

		Ofs += HSize;

		if ( Arch_IsZero( &Head ) ) { break; }

		Lubyte Type = ( Lubyte )( ( char )Head.FType - '0' );
		Luint64 Size = Arch_OctalByteArrayToLongInt( Head.FFileSize, 12 );

		/// Round to 512
		Luint64 Mod = ( Size % 512 );
		Luint64 RoundSize = Size + ( Mod == 0x0 ? 0x0 : ( 512 - Mod ) );

		/// Skip unknown blocks (0 and 72 are supported)
		if ( Type == 0 )
		{
			/// Normal file, skip bytes
			In->SeekOffset( RoundSize );

			LString TheName = LongName ? CurrentName : Arch_SafeArrayToString( Head.FFileName, 300 );

			// strip ./ from the file name
			if ( LStr::StartsWith( TheName, "./" ) ) { TheName = LString( TheName.c_str() + 2 ); }

			Env->Logger->LogP( L_PARANOID, "Found file in .tar: %s", TheName.c_str() );

			sFileInfo Info;

			Info.FOffset    = Ofs;
			Info.FSize      = Info.FCompressedSize = Size;
			Info.FTimeStamp = Arch_OctalByteArrayToLongInt( Head.FModificationDate, 12 );

			TheName = LStr::GetUpper( TheName );
			FFileInfos.push_back( Info );

			FFileInfoIdx[TheName] = ( int )FFileNames.size();
			FFileNames.push_back( TheName );

			Ofs += RoundSize;

			LongName = false;
			CurrentName = "";
		}
		else if ( Type == 72 )
		{
			LongName = true;
			CurrentName =  Arch_SafeArrayToString( Head.FFileName, 300 );
		}
	}

	return true;

	unguard();
}

#pragma endregion
