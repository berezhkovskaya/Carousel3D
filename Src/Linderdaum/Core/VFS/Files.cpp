/**
 * \file Files.cpp
 * \brief Files access implemenations (memory-mapped read/write)
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Core/Logger.h"

#include "Files.h"

#include "Utils/LBlob.h"

#if !defined(OS_WINDOWS)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/mman.h>
#  include <fcntl.h>
#  include <errno.h>

// error/strerror
#  include <stdlib.h>
#  include <string.h>
#  include <errno.h>

#endif

bool clRAWFile::Open( const LString& FileName, const LString& VirtualFileName )
{
	SetFileName( FileName );
	SetVirtualFileName( VirtualFileName );

#ifdef OS_WINDOWS
	FMapFile = CreateFileA( FFileName.c_str(),
	                        GENERIC_READ,
	                        FILE_SHARE_READ,
	                        NULL,
	                        OPEN_EXISTING,
	                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
	                        NULL );
	RAISE_MSG_IF( FMapFile == INVALID_HANDLE_VALUE, clExcept_FileNotFound, LString( "\"" ) + FileName + "\"" );
#else
	FFileHandle = open( FileName.c_str(), O_RDONLY );

	if ( FFileHandle == -1 )
	{
		int ErrorCode = 0;

		volatile int* ErrorPtr = __errno();

		if ( ErrorPtr ) { ErrorCode = *ErrorPtr; }

		RAISE_MSG( clExcept_FileNotFound, LString( "\"" ) + FileName + "\" ErrorCode:" + LStr::ToStr( ErrorCode ) );
	}

#endif

#ifdef OS_WINDOWS
	FMapHandle = CreateFileMapping( ( HANDLE )FMapFile,
	                                NULL,
	                                PAGE_READONLY,
	                                0,
	                                0,
	                                NULL );

	FFileData = static_cast<Lubyte*>( MapViewOfFile( FMapHandle,
	                                                 FILE_MAP_READ,
	                                                 0,
	                                                 0,
	                                                 0 ) );

	DWORD dwSizeLow = 0;
	DWORD dwSizeHigh = 0;
	dwSizeLow = ::GetFileSize( FMapFile, &dwSizeHigh );
	FSize = ( ( Luint64 )dwSizeHigh << 32 ) | ( Luint64 )dwSizeLow;

#else
	struct stat FileInfo;

	if ( fstat( FFileHandle, &FileInfo ) == -1 )
//	if(fstat64( FFileHandle, &FileInfo ) == -1)  // try 64 if fstat() does not work
	{
		Env->Logger->LogP( L_WARNING, "Error in fstat() call: %s\n", strerror( errno ) );
	}

	FSize = static_cast<Luint64>( FileInfo.st_size );
//	fseek( FFileHandle, 0, SEEK_END );
//	FSize = ftell( FFileHandle );
//	rewind( FFileHandle );

	// create share/read-only file mapping
	// using FFfileHandle and zero offset
	FFileData = reinterpret_cast<Lubyte*>( mmap( NULL, FSize, PROT_READ, MAP_SHARED, FFileHandle, 0 ) );

	close( FFileHandle );
#endif

	return true;
}

/// Try to close physical file (TODO : store error message)
bool clRAWFile::Close()
{
	guard();

#ifdef OS_WINDOWS
	UnmapViewOfFile( FFileData );
	CloseHandle( FMapHandle );
	CloseHandle( FMapFile );
#else
	munmap( reinterpret_cast<void*>( FFileData ), FSize );

	// already closed in Open()
	// close( FFileHandle );
#endif

	return true;

	unguard();
}


bool clRAWFileNonMapped::Open( const LString& FileName, const LString& VirtualFileName )
{
	SetFileName( FileName );
	SetVirtualFileName( VirtualFileName );

	FILE* File = fopen( FFileName.c_str() , "rb" );

	RAISE_MSG_IF( File == NULL, clExcept_FileNotFound, LString( "\"" ) + FileName + "\"" );

	// obtain file size
	fseek( File, 0, SEEK_END );
	FSize = ftell( File );
	rewind( File );

	FFileData = static_cast<Lubyte*>( malloc( static_cast<size_t>( FSize ) ) );

	size_t BytesRead = fread( FFileData, 1, static_cast<size_t>( FSize ), File );

	RAISE_MSG_IF( BytesRead != FSize, clExcept_FileNotFound, LString( "\"" ) + FileName + "\"" );

	fclose( File );

	return true;
}

/// Try to close physical file (TODO : store error message)
bool clRAWFileNonMapped::Close()
{
	free( FFileData );

	return true;
}

clMemRAWFile::clMemRAWFile()
{
	FBuffer     = NULL;
	FBufferSize = 0;

	FOwnBuffer  = false;
}

clMemRAWFile::~clMemRAWFile()
{
	DeleteBuffer();
}

void clMemRAWFile::DeleteBuffer()
{
	if ( FBuffer )
	{
		if ( FOwnBuffer )
		{
			delete [] reinterpret_cast<const char*>( FBuffer );
		}

		FBuffer = NULL;
	}
}

void clMemRAWFile::CreateFromBuffer( const void* BufPtr, Luint64 BufSize )
{
	DeleteBuffer();

	FBuffer     = BufPtr;
	FBufferSize = BufSize;

	FOwnBuffer = true;
}

void clMemRAWFile::CreateFromManagedBuffer( const void* BufPtr, Luint64 BufSize )
{
	DeleteBuffer();

	FBuffer     = BufPtr;
	FBufferSize = BufSize;

	FOwnBuffer = false;
}

void clMemRAWFile::CreateFromString( const LString& InString )
{
	DeleteBuffer();

	FBufferSize = InString.length();

	if ( !InString.empty() )
	{
		char* LocalBuffer = new char[ InString.length() ];

		memcpy( LocalBuffer, InString.c_str(), static_cast<size_t>( FBufferSize ) );

		FBuffer = LocalBuffer;
	}
}

clFileWriter::clFileWriter(): FPosition( 0 )
{
}

clFileWriter::~clFileWriter()
{
	Close();
}

/// (Re)Open file (FIXME: return true or fale, avoid FATAL_MSG)
bool clFileWriter::Open( const LString& FileName )
{
	FFileName = FileName;

#ifdef OS_WINDOWS
	FMapFile = CreateFile( FFileName.c_str(),
	                       GENERIC_WRITE,
	                       FILE_SHARE_READ,
	                       NULL,
	                       CREATE_ALWAYS,    // OPEN_ALWAYS - for append
	                       FILE_ATTRIBUTE_NORMAL,
	                       NULL );

	RAISE_MSG_IF( FMapFile == INVALID_HANDLE_VALUE,
	              clExcept_UnableToCreateFile,
	              LString( "\"" ) + FileName + "\"" );
#else
	FMapFile = open( FFileName.c_str(), O_WRONLY | O_CREAT
#if defined(OS_POSIX) && !defined(OS_ANDROID)
	                 // on a unix system we must set permissions for the created files. later these must be supplied as a parameter
	                 , S_IRUSR | S_IWUSR // S_IRWXU
#endif
	               );

	RAISE_MSG_IF( FMapFile == -1,
	              clExcept_UnableToCreateFile,
	              LString( "\"" ) + FileName + "\"" );
#endif
	FPosition = 0;

	return true;
}

/// Close file (TODO : get error code)
bool clFileWriter::Close()
{
#ifdef OS_WINDOWS
	CloseHandle( FMapFile );
#else
	close( FMapFile );
#endif

	return true;
}

LString clFileWriter::GetFileName() const
{
	return FFileName;
}

void clFileWriter::BlockWrite( const void* Buf, const Luint64 Size )
{
#ifdef OS_WINDOWS
	DWORD written;
	// this will fail is Size is more than 32Mb on Win32 and more than 64Mb on Win64
	WriteFile( FMapFile, Buf, DWORD( Size ), &written, NULL );
#else
	write( FMapFile, Buf, Size );
#endif
	FPosition += Size;
}

void clFileWriter::Seek( const Luint64 Position )
{
#ifdef OS_WINDOWS
	SetFilePointerEx( FMapFile, *reinterpret_cast<const LARGE_INTEGER*>( &Position ), NULL, FILE_BEGIN );
#else
	lseek( FMapFile, Position, SEEK_SET );
#endif
	FPosition = Position;
}


const size_t MAX_LINE_WIDTH = 2 * 65535;

clFileMapper::clFileMapper(): FRAWFile( NULL ), FPosition( 0 )
{
}

void clFileMapper::SetRawFile( iRAWFile* RAWFile )
{
	FRAWFile  = RAWFile;

	FFileData = FRAWFile->GetFileData();
	FSize     = FRAWFile->GetFileSize();
}

clFileMapper::~clFileMapper()
{
	delete( FRAWFile );
}

LString clFileMapper::GetVirtualFileName() const
{
	return FRAWFile->GetVirtualFileName();
}

LString clFileMapper::GetFileName() const
{
	return FRAWFile->GetFileName();
}

void clFileMapper::BlockRead( void* Buf, const Luint64 Size )
{
	RAISE_MSG_IF( Size + FPosition > FSize,
	              clExcept_ReadingPastEndOfFile,
	              LString( "\"" ) + FRAWFile->GetFileName() + "\" (FSize = " + LStr::ToStr( FSize ) + ", ReadPos = " + LStr::ToStr( FPosition ) + ", ReadSize = " + LStr::ToStr( Size ) + ")" );

	memcpy( Buf, ( FFileData + FPosition ), static_cast<size_t>( Size ) );

	FPosition += Size;
}

LString clFileMapper::ReadLine()
{
#ifdef MSC_VER
//   __mm_prefetch( &FFileData[FPosition], 2 );
#endif
	char LineBuffer[ MAX_LINE_WIDTH+1 ];

	const Lubyte* C = &FFileData[FPosition];
	char* Out       = &LineBuffer[0];
	char* End       = &LineBuffer[ MAX_LINE_WIDTH ];

	while ( FPosition < FSize && Out != End )
	{
		FPosition++;

		char Ch = ( *C++ );

		if ( Ch == 13   ) { continue; }   // kill char

		if ( Ch == 10   ) { break; }

		*Out++ = Ch;
	}

	( *Out ) = 0;

	return LString( LineBuffer );
}

LString clFileMapper::ReadLineTrimLeadSpaces()
{
	char LineBuffer[ MAX_LINE_WIDTH+1 ];

	const Lubyte* C = &FFileData[FPosition];
	char* Out       = &LineBuffer[0];
	char* End       = &LineBuffer[ MAX_LINE_WIDTH ];

	// scan leading spaces
	while ( FPosition < FSize )
	{
		if ( !LStr::IsSeparator( *C ) )
		{
			break;
		}

		FPosition++;
		C++;
	}

	// scan to the end of the string
	while ( FPosition < FSize && Out != End )
	{
		FPosition++;

		char Ch = ( *C++ );

		if ( Ch == 13   ) { continue; }   // kill char

		if ( Ch == 10   ) { break; }

		*Out++ = Ch;
	}

	( *Out ) = 0;

	return LString( LineBuffer );
}

LString clFileMapper::ReadLineLength()
{
	LString String = "";

	Luint64 Length = ReadInt_Binary();

	/// don't read anything beyond EOF
	if ( FPosition + Length > FSize ) { Length = FSize - FPosition; }

	const char* Pos = reinterpret_cast<const char*>( FFileData ) + FPosition;

	FPosition += Length;

	return LString( Pos, static_cast<size_t>( Length ) );
}

int clFileMapper::ReadInt_Binary()
{
	int Int;

	RAISE_MSG_IF( sizeof( Int ) + FPosition > FSize,
	              clExcept_ReadingPastEndOfFile,
	              LString( "\"" ) + FRAWFile->GetFileName() + "\"" );

	BlockRead( &Int, sizeof( Int ) );

	return Int;
}

clMemFileWriter::~clMemFileWriter()
{
	if ( FContainer ) { FContainer->DisposeObject(); }
}

void clMemFileWriter::BlockWrite( const void* Buf, const Luint64 Size )
{
	if ( !FContainer ) { return; }

	void* DestPtr = ( void* )( &( ( ( Lubyte* )( FContainer->GetData() ) )[FPosition] ) );

	Luint64 ThisPos = FPosition;

	/// Ensure there is enough space
	Seek( ThisPos + Size );

	if ( FPosition + Size > FMaxSize ) { return; }

	/// Write the data
	memcpy( DestPtr, Buf, static_cast<size_t>( Size ) );

//	return true;
}

void clMemFileWriter::Seek( const Luint64 Position )
{
	/// Sanity check
	if ( !FContainer ) { return; }

	if ( Position > FContainer->GetSize() )
	{
		/// Check for oversize
		if ( Position > FMaxSize - 1 ) { return; }

		/// Resize the Blob
		if ( !FContainer->SafeResize( static_cast<size_t>( Position ) + 1 ) ) { return; }
	}

	FPosition = Position;
}

/*
 * 06/01/2005
     It's here
*/
