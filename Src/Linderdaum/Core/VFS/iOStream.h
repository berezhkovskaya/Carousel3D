/**
 * \file iOStream.h
 * \brief Output stream interface
 * \version 0.5.67
 * \date 13/06/2006
 * \author Sergey Kosarevsky, 2005-2006
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iOStream_
#define _iOStream_

#include "LString.h"

#include "Core/iObject.h"
#include "Utils/Exceptions.h"

/// Output stream
class scriptfinal iOStream: public iObject
{
public:
	EXCEPTIONABLE;
	EXCEPTION( clExcept_UnableToCreateFile, "Unable to create file" );
public:
	iOStream() {};
	virtual ~iOStream() {};
	//
	// iOStream
	//

	/// Get real file name for this stream (TODO : move to FileStream, some streams must not have any file name)
	virtual LString GetFileName() const = 0;

	/// Change absolute file position to Position
	virtual void    Seek( const Luint64 Position ) = 0;

	/// Get current write position
	virtual Luint64 GetFilePos() const = 0;

	/// Write Size bytes from Buf
	virtual void    BlockWrite( const void* Buf, const Luint64 Size ) = 0;

	/// Write single null-terminated string
	virtual void    WriteLine( const LString& Line )
	{
		LString Buffer = Line;
#ifdef OS_WINDOWS
		Buffer.push_back( '\r' );
		Buffer.push_back( '\n' );
#else
		Buffer.push_back( '\r' ); // or '\n' ?
#endif

		BlockWrite( Buffer.c_str(), Buffer.length() );
	}

	/// Write "pascal-style" string: one DWord with length and then string contents
	virtual void    WriteLineLength( const LString& Line )
	{
		LString Buffer = Line;

		WriteInt_Binary( static_cast<int>( Buffer.length() ) );

		BlockWrite( Buffer.c_str(), Buffer.length() );
	}

	/// Write standart (big-endian) 4-byte integer
	virtual void    WriteInt_Binary( int Int )
	{
		BlockWrite( &Int, sizeof( Int ) );
	}
};

#endif

/*
 * 13/06/2006
     GetFileName()
 * 08/08/2005
     iStream deprecated
 * 26/11/2004
     It's here
*/
