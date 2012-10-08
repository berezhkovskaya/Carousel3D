/**
 * \file Archive.h
 * \brief Archive reader interface
 * \version 0.6.06
 * \date 04/01/2012
 * \author Viktor Latypov, 2011-2012 (.tar and .zip support)
 * \author viktor@linderdaum.com
 * \author Sergey Kosarevsky, 2005 (.rar support)
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __archive__h__included__
#define __archive__h__included__

#include "Core/iObject.h"

#include <map>

class iIStream;
class iOStream;

/// One of the supported archive types
enum LArchiveType
{
	ARCH_TAR = 0,
	ARCH_ZIP = 1,
	ARCH_RAR = 2,
	ARCH_INVALID = 255
};

/**
   \brief Encapsulation of .tar,.rar,.zip archive management

   Provides file enumeration facilities and access to individual files

   Contains an extracted list of files (and corresponding time stamps).

   clArchive supports:

   - compressed data using zlib/libbzip2 (pkware's .zip files)
   - uncompressed .tar and .rar files

   Access to the file content is done using iIStream interface,
   so it is completely compatible with the engine's IO.

   There are no delegation and subclasses to avoid having a lot of generated metainformations for them.
   The .rar/.tar/.zip parsing is performed in dedicated methods (Enumerate_*)
   File type is determined from the header or from the file extension.

   The user creates an archive reader instance and supplies the Source (file)stream to read the data from.
   If the ownership flag is true, the stream is automatically closed at the end.

   Utility method OpenArchiveFile() reads the data from the specified file.
   Source file ownership is set to True in this case.

   Since Open() can be a lenghty operation, we provide the information
   in FScanProgress variable and FAbortFlag can terminate the scanning before it is finished.
*/
class scriptfinal netexportable clArchiveReader: public iObject
{
private:
	/// Internal file information
	struct sFileInfo
	{
		/// Offset to the file
		Luint64 FOffset;

		/// (Uncompressed) File size
		Luint64 FSize;

		/// Compressed file size
		Luint64 FCompressedSize;

		/// Timestamp
		Luint64 FTimeStamp;

		/// Compressed data
		void* FSourceData;
	};

	/// File information
	std::vector<sFileInfo> FFileInfos;

	/// File names (adapted for our VFS - upper case, slash-separated path)
	std::vector<LString>   FFileNames;

	/// Real (in-archive) file names
	std::vector<LString>   FRealFileNames;

	/// File information indices (name to index map)
	mutable std::map<LString, int> FFileInfoIdx;

	/// Source file
	iIStream* FSourceFile;

	/// Do we manage the source file
	bool FOwnsSourceFile;

	/// Cached archive type
	LArchiveType FArchiveType;

public:
	clArchiveReader(): FCompletionFlag( NULL ), FSourceFile( NULL ), FOwnsSourceFile( false ), FArchiveType( ARCH_INVALID ) {}
	virtual ~clArchiveReader() { CloseArchive(); }

	/// Assign the source stream and set the internal ownership flag
	scriptmethod bool    OpenArchive( iIStream* Source, bool OwnStream )
	{
		if ( FCompletionFlag ) { *FCompletionFlag = false; }

		if ( !CloseArchive() ) { return false; }

		FSourceFile = Source;
		FOwnsSourceFile = OwnStream;

		return EnumerateFiles();
	}

	/// Create a stream reader and read archive contents
	scriptmethod bool    OpenArchiveFile( const LString& FName );

	/// Extract a file from archive
	scriptmethod bool    ExtractSingleFile( const LString& FName, const LString& Password, int* AbortFlag, float* Progress, iOStream* FOut );

	/// Free everything and optionally close the source stream
	scriptmethod bool    CloseArchive();

	iIStream* GetSourceFile() const { return FSourceFile; }

	/// Get opened archive type
	inline LArchiveType GetArchiveType() const { return FArchiveType; }

	/// Check if such a file exists in the archive
	scriptmethod bool    FileExists( const LString& FileName ) const { return ( GetFileIdx( FileName ) > -1 ); }

	/// Get the size of file
	scriptmethod Luint64 GetFileSize( const LString& FileName ) const
	{
		int idx = GetFileIdx( FileName );
		return ( idx > -1 ) ? FFileInfos[idx].FSize : 0;
	}

	/// Get the data for this file
	const void*   GetFileData( const LString& FileName )
	{
		int idx = GetFileIdx( FileName );
		const void* DataPtr = NULL;

		if ( idx > -1 )
		{
			/// Check if we already have this data in cache
			if ( FExtractedFromArchive.count( idx ) > 0 ) { return FExtractedFromArchive[idx]; }

			/// Decompress/extract the data
			switch ( FArchiveType )
			{
				case ARCH_TAR:
				case ARCH_RAR:
					DataPtr = GetFileData_TAR_or_RAR( idx );
					break;
				case ARCH_ZIP:
					DataPtr = GetFileData_ZIP( idx );
					break;
			}

			if ( DataPtr ) { FExtractedFromArchive[idx] = DataPtr; }
		}

		return DataPtr;
	}

	/// Get the extracted timestamp for this file
	scriptmethod Luint64 GetFileTime( const LString& FileName ) const
	{
		int idx = GetFileIdx( FileName );
		return ( idx > -1 ) ? FFileInfos[idx].FTimeStamp : 0;
	}

	/// Convert file name to an internal linear index
	scriptmethod int     GetFileIdx( const LString& FileName ) const { return ( FFileInfoIdx.count( FileName ) > 0 ) ? FFileInfoIdx[FileName] : -1; }

	/// Get the number of files in archive
	scriptmethod size_t  GetNumFiles() const { return FFileInfos.size(); }

	/// Get i-th file name in archive
	scriptmethod LString GetFileName( Luint idx ) { return FFileNames[idx]; }

#pragma region Contents scanning

	/**
	   When OpenArchive is called, the caller can track progress
	*/

public:
	/// If 1, then the current Open() operation terminates
	int FAbortScanningFlag;

	/// Updated inside the Enumerate()
	int FScanProgress;

	/// The number of files in archive, available soon after the Enumerate() is called
	int FTotalFiles;

	/// If not NULL, EnumerateFiles() sets this to true at the end
	volatile bool* FCompletionFlag;

#pragma endregion

private:
	/// Internal function to enumerate the files in archive
	bool EnumerateFiles();

	bool Enumerate_TAR();
	bool Enumerate_RAR();
	bool Enumerate_ZIP();

	const void* GetFileData_TAR_or_RAR( size_t idx );
	const void* GetFileData_ZIP( size_t idx );

	/// Remove each extracted file from tmp container
	void ClearExtracted()
	{
		/// Other archive types do not decompress and allocate additional buffers
		if ( FArchiveType == ARCH_ZIP )
		{
			for ( std::map<int, const void*>::iterator i = FExtractedFromArchive.begin() ; i != FExtractedFromArchive.end() ; i++ )
			{
				this->FreeData( const_cast<void*>( i->second ) );
			}
		}

		FExtractedFromArchive.clear();
	}

	inline void* AllocData( size_t Sz ) { return ::malloc( Sz ); }

	inline void FreeData( void* DataPtr ) { ::free( DataPtr ); }

	/// Temporary cache for the extracted files. Removed on CloseArchive() call
	std::map<int, const void*> FExtractedFromArchive;
};

#endif
