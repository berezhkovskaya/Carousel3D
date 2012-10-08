/**
 * \file Files.h
 * \brief Files access implemenations (memory-mapped read/write)
 * \version 0.5.60
 * \date 17/12/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Files__h__included__
#define __Files__h__included__

#include "Core/iObject.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"

#ifdef OS_WINDOWS
#  include <windows.h>
#  undef min
#  undef max
#endif

class clBlob;
class iRAWFile;

/**
   \brief Basic file access interface

   All I/O is performed through the file mapping
   (mmap on Unix systems and MapViewOfFile on WinAPI)
**/
class scriptfinal clFileMapper: public iIStream
{
public:
	clFileMapper();
	virtual ~clFileMapper();

	scriptmethod void SetRawFile( iRAWFile* RAWFile );

	//
	// iIStream interface
	//
	virtual LString           GetVirtualFileName() const;
	virtual LString           GetFileName() const;
	virtual void              BlockRead( void* Buf, const Luint64 Size );
	virtual int               ReadInt_Binary();
	virtual LString           ReadLine();
	virtual LString           ReadLineLength();
	virtual LString           ReadLineTrimLeadSpaces();

	virtual void              Seek( const Luint64 Position )     { FPosition  = Position; }
	virtual void              SeekOffset( const Luint64 Offset ) { FPosition += Offset;   }

	virtual bool              Eof() const                        { return ( FPosition >= FSize ); }

	virtual Luint64           GetFileSize() const { return FSize;     }
	virtual Luint64           GetFilePos()  const { return FPosition; }
	virtual const Lubyte*     MapStream()   const { return FFileData; }

	virtual const Lubyte*     MapStreamFromCurrentPos() const { return ( FFileData + FPosition ); }

private:
	iRAWFile*          FRAWFile;
	const Lubyte*      FFileData;
	Luint64            FSize;
	Luint64            FPosition;
};

class scriptfinal clFileWriter: public iOStream
{
public:
	clFileWriter();
	virtual ~clFileWriter();

	/// (Re)Open file
	scriptmethod bool Open( const LString& FileName );

	/// Close file
	scriptmethod bool Close();

	// iOStream interface

	virtual LString    GetFileName() const;
	virtual void       Seek( const Luint64 Position );
	virtual Luint64    GetFilePos() const { return FPosition; }
	virtual void       BlockWrite( const void* Buf, const Luint64 Size );
private:
	LString    FFileName;
#ifdef OS_WINDOWS
	HANDLE     FMapFile;
#else
	int        FMapFile;
#endif
	/// Cached write position to avoid ftell() function calls
	Luint64    FPosition;
};

class scriptfinal iRAWFile: public iObject
{
public:
	EXCEPTIONABLE;
	EXCEPTION( clExcept_FileNotFound, "File not found" );
public:
	iRAWFile() {};
	virtual ~iRAWFile() {};
	//
	// iRAWFile
	//
	scriptmethod void        SetVirtualFileName( const LString& VFName ) { FVirtualFileName = VFName; }
	scriptmethod void        SetFileName( const LString& FName )         { FFileName        = FName;  }

	scriptmethod LString     GetVirtualFileName() const { return FVirtualFileName; }
	scriptmethod LString     GetFileName()        const { return FFileName; }

	virtual const Lubyte*    GetFileData() const = 0;
	virtual Luint64          GetFileSize() const = 0;

protected:
	LString    FFileName;
	LString    FVirtualFileName;
};

/// Physical file representation
class scriptfinal clRAWFile: public iRAWFile
{
public:
	clRAWFile() {}
	virtual ~clRAWFile() { Close(); }

	/// Try to open the file for reading
	scriptmethod bool Open( const LString& FileName, const LString& VirtualFileName );

	/// Try to close physical file
	scriptmethod bool Close();

	//
	// iRAWFile interface
	//
	virtual const Lubyte* GetFileData()        const { return FFileData; }
	virtual Luint64       GetFileSize()        const { return FSize;     }

private:
#ifdef OS_WINDOWS
	HANDLE     FMapFile;
	HANDLE     FMapHandle;
#else
	int        FFileHandle;
#endif
	Lubyte*    FFileData;
	Luint64    FSize;
};

/// Physical file representation for those unsopporting memory-mapped files
class scriptfinal clRAWFileNonMapped: public iRAWFile
{
public:
	clRAWFileNonMapped() {}
	virtual ~clRAWFileNonMapped() { Close(); }

	/// Try to open the file for reading
	scriptmethod bool Open( const LString& FileName, const LString& VirtualFileName );

	/// Try to close physical file
	scriptmethod bool Close();

	//
	// iRAWFile interface
	//
	virtual const Lubyte* GetFileData()        const { return FFileData; }
	virtual Luint64       GetFileSize()        const { return FSize;     }

private:
	Lubyte*    FFileData;
	Luint64    FSize;
};

class scriptfinal clMemRAWFile: public iRAWFile
{
public:
	clMemRAWFile();
	virtual ~clMemRAWFile();
	//
	// iRAWFile interface
	//
	virtual const Lubyte* GetFileData() const { return reinterpret_cast<const Lubyte*>( FBuffer ); }
	virtual Luint64       GetFileSize() const { return FBufferSize; }

	//
	// clMemRAWFile
	//
	scriptmethod void CreateFromString( const LString& InString );

	void CreateFromBuffer( const void* BufPtr, Luint64 BufSize );
	void CreateFromManagedBuffer( const void* BufPtr, Luint64 BufSize );

private:
	void DeleteBuffer();

	// do we own the buffer ?
	bool           FOwnBuffer;
	const void*    FBuffer;
	Luint64        FBufferSize;
};

/// File writer for some dynamically-sized Blob
class scriptfinal clMemFileWriter: public iOStream
{
public:
	clMemFileWriter(): FPosition( 0 ), FMaxSize(), FContainer( NULL ), FFileName( "" ) {}
	virtual ~clMemFileWriter();

	/// Set "file" name (just to keep the API consistent for any kind of stream)
	virtual void SetFileName( const LString& FName ) { FFileName = FName; }

	/// Get file name
	virtual LString GetFileName() const { return FFileName; }

	/// Change absolute file position to Position
	virtual void    Seek( const Luint64 Position );

	/// Get current write position
	virtual Luint64 GetFilePos() const { return FPosition; }

	/// Write Size bytes from Buf
	virtual void    BlockWrite( const void* Buf, const Luint64 Size );

	/// Get the actual data. The FContainer can be modified externally
	scriptmethod clBlob* GetContainer() const { return FContainer; }

	/// Assign the data container
	scriptmethod void    SetContainer( clBlob* B ) { FContainer = B; }

	/// Get maximum allowed size for the in-mem file
	scriptmethod Luint64 GetMaxSize() const { return FMaxSize; }

	/// Set maximum allowed size
	scriptmethod void    SetMaxSize( Luint64 MaxSize ) { FMaxSize = MaxSize; }

private:
	/// Resizable blob with actual file contents
	clBlob* FContainer;

	/// Maximum allowable size
	Luint64 FMaxSize;

	/// Actual file name
	LString FFileName;

	/// Current position
	Luint64 FPosition;
};

#endif

/*
 * 06/01/2005
     It's here
*/
