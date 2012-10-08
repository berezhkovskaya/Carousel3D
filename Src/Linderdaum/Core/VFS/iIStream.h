/**
 * \file iIStream.h
 * \brief Input stream interface
 * \version 0.5.89a
 * \date 25/03/2009
 * \author Sergey Kosarevsky, 2005-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iIStream_
#define _iIStream_

#include "Platform.h"
#include "Core/iObject.h"
#include "Utils/Exceptions.h"

/// Input stream (TODO : split to InputStream and InputFileStream )
class scriptfinal iIStream: public iObject
{
public:
	EXCEPTIONABLE;
	EXCEPTION( clExcept_ReadingPastEndOfFile, "Reading past end of file" );
public:
	iIStream() {};
	virtual ~iIStream() {};
	//
	// iIStream
	//
	virtual LString        GetVirtualFileName() const = 0;
	virtual LString        GetFileName() const = 0;
	virtual void           Seek( const Luint64 Position ) = 0;
	virtual void           SeekOffset( const Luint64 Offset ) = 0;
	virtual void           BlockRead( void* Buf, const Luint64 Size ) = 0;
	virtual LString        ReadLine() = 0;

	/// Read "pascal-style" string: DWord with size followed by string contents
	virtual LString        ReadLineLength() = 0;

	virtual LString        ReadLineTrimLeadSpaces() = 0;
	virtual int            ReadInt_Binary() = 0;
	virtual bool           Eof() const = 0;
	virtual Luint64        GetFileSize() const = 0;
	virtual Luint64        GetFilePos() const = 0;
	virtual Luint64        GetBytesLeft() const { return GetFileSize() - GetFilePos(); };

	/// Return pointer to the shared memory corresponding to this file. Can return NULL if memory mapped files are unsupported
	virtual const Lubyte*  MapStream() const = 0;

	/// Return pointer to the shared memory corresponding to the current position in this file. Can return NULL if memory mapped files are unsupported
	virtual const Lubyte*  MapStreamFromCurrentPos() const = 0;
};

#endif

/*
 * 25/03/2009
     Support for large files
 * 22/02/2007
     GetVirtualFileName()
 * 30/11/2005
     SeekOffset()
 * 08/08/2005
     iStream deprecated
 * 11/12/2004
     MapStreamFromCurrentPos()
 * 28/11/2004
     Seek()
 * 25/11/2004
     It's here
*/
