/**
 * \file HTTP.cpp
 * \brief Simple HTTP client library (based entirely on HappyHTTP  http://scumways.com/happyhttp/happyhttp.html)
 * \version 0.5.95
 * \date 16/06/2010
 * \author Viktor Latypov, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
*/

/*
 * HappyHTTP - a simple HTTP library
 * Version 0.1
 *
 * Copyright (c) 2006 Ben Campbell
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 * be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

#include "HTTP.h"

// for hex-int conversion. Strange, must be removed
#include <iostream>
//#include <strstream>

#ifdef DO_NOT_USE_ENGINE

#include "TimeUtil.h"

#else

#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iOStream.h"
#include "Core/Logger.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Core/CVars.h"

#endif

#include <cstdio>
#include <cstring>

namespace happyhttp
{

	bool HttpConn::Connect()
	{
		if ( FSock ) { return true; }

		FSock = new LTCPSocket();

		if ( !FSock->Open() || !FSock->Connect( FHost.c_str(), FPort ) )
		{
			printf( "%s\n", LNetwork_LastErrorAsString().c_str() );
			FLastError = FSock->FLastError;
			delete FSock;
			FSock = NULL;
			return false;
		}

		return true;
	}

	void HttpConn::Close()
	{
		if ( FSock ) { FSock->Close(); delete FSock; FSock = NULL; }

		// discard any incomplete responses
		while ( !FOutstanding.empty() ) { delete FOutstanding.front(); FOutstanding.pop_front(); }
	}

	bool HttpConn::Request( const char* method, const char* url, const char* headers[], const unsigned char* body, int bodysize )
	{
		bool gotcontentlength = false;   // already in headers?

		/// Check headers for content-length
		/// TODO: check for "Host" and "Accept-Encoding" too and avoid adding them ourselves in PutRequest()
		if ( headers )
		{
			const char** h = headers;

			while ( *h )
			{
				const char* name = *h++;
				const char* value = *h++;

				if ( !value ) { FLastError = LString( "Name with no value" ); return false; }

				if ( strcasecmp( name, "content-length" ) == 0 ) { gotcontentlength = true; }
			}
		}

		PutRequest( method, url );

		if ( body && !gotcontentlength ) { PutHeaderN( "Content-Length", bodysize ); }

		if ( headers )
		{
			const char** h = headers;

			while ( *h )
			{
				const char* name = *h++;
				const char* value = *h++;
				PutHeader( name, value );
			}
		}

		EndHeaders();

		if ( body ) if ( !Send( body, bodysize ) ) { return false; }

		return true;
	}

	bool HttpConn::PutRequest( const char* method, const char* url )
	{
		if ( FState != IDLE ) { FLastError = LString( "Request already issued" ); return false; }

		FState = REQ_STARTED;

		char req[ 2048 ];
#ifdef _MSC_VER
		sprintf_s( req, 2048, "%s %s HTTP/1.1", method, url );
#else
		sprintf( req, "%s %s HTTP/1.1", method, url );
#endif
		FBuffer.push_back( req );

		if ( !PutHeader( "Host", FHost.c_str() ) ) { return false; } ///< required for HTTP1.1

		/// Don't want any fancy encodings please
		if ( !PutHeader( "Accept-Encoding", "identity" ) ) { return false; }

		/// Push a new response onto the queue
		FOutstanding.push_back( new HttpResponse( method, *this ) );

		return true;
	}

	bool HttpConn::PutHeader( const char* header, const char* value )
	{
		if ( FState != REQ_STARTED ) { FLastError = LString( "PutHeader() failed" ); return false; }

		FBuffer.push_back( LString( header ) + ": " + LString( value ) );

		return true;
	}

	bool HttpConn::PutHeaderN( const char* header, int numericvalue )
	{
		char buf[32];
#ifdef _MSC_VER
		sprintf_s( buf, 32, "%d", numericvalue );
#else
		sprintf( buf, "%d", numericvalue );
#endif
		return PutHeader( header, buf );
	}

	bool HttpConn::EndHeaders()
	{
		if ( FState != REQ_STARTED ) { FLastError = LString( "Cannot send header" ); return false; }

		FState = IDLE;

		FBuffer.push_back( "" );

		LString msg;
		std::vector< LString>::const_iterator it;

		for ( it = FBuffer.begin(); it != FBuffer.end(); ++it ) { msg += ( *it ) + "\r\n"; }

		FBuffer.clear();

		return Send( ( const unsigned char* )msg.c_str(), ( int )msg.size() );
	}

	bool HttpConn::Send( const unsigned char* buf, int numbytes )
	{
		if ( FSock == NULL ) if ( !this->Connect() ) { return false; }

		while ( numbytes > 0 )
		{
			int n = FSock->WriteBytes( buf, numbytes );

			if ( n < 0 )
			{
				FLastError = LString( "connect(): " ) + LNetwork_LastErrorAsString();
				return false;
			}

			numbytes -= n;
			buf += n;
		}

		return true;
	}

	bool HttpConn::Pump()
	{
		if ( FOutstanding.empty() ) { return true; } // no requests outstanding

		if ( FSock == NULL ) { return false; }

		unsigned char buf[ 2048 ];
		int a = FSock->ReadBytes( buf, sizeof( buf ) );

		if ( FSock->IsWouldBlock() ) { return true; }

		if ( a < 0 )
		{
			FLastError = LString( "connect(): " ) + LNetwork_LastErrorAsString();
			return false;
		}

		/// Connection has closed
		if ( a == 0 )
		{
			HttpResponse* r = FOutstanding.front();

			if ( !r->NotifyConnectionClosed() ) { FLastError = r->FLastError; return false; }

			if ( !r->Completed() ) { FLastError = LString( "Req is not completed" ); return false; }

			delete r;
			FOutstanding.pop_front();

			/// Any outstanding requests will be discarded
			Close();

			return true;
		}

		int used = 0;

		while ( used < a && !FOutstanding.empty() )
		{
			HttpResponse* r = FOutstanding.front();
			int u = r->Pump( &buf[used], a - used );

			if ( u < -1 ) { FLastError = r->FLastError; return false; }

			if ( r->Completed() ) { delete r; FOutstanding.pop_front(); }

			used += u;
		}

		// NOTE: will lose bytes if response queue goes empty
		// (but server shouldn't be sending anything if we don't have anything outstanding anyway)
		if ( used != a ) { FLastError = LString( "all bytes should be used up by here" ); return false; }

		return true;
	}

	HttpResponse::HttpResponse( const char* method, HttpConn& conn ) :
		FConnection( conn ),
		FState( STATUSLINE ),
		FMethod( method ),
		FVersion( 0 ),
		FStatus( 0 ),
		FBytesRead( 0 ),
		FChunked( false ),
		FChunkLeft( 0 ),
		FLength( -1 ),
		FWillClose( false )
	{
		FConnection.FDataLen = -1;
	}

	const char* HttpResponse::GetHeader( const char* name ) const
	{
		LString lname( name );

		for ( size_t j = 0 ; j < lname.length() ; j++ ) { lname[j] = ( char )tolower( lname[j] ); }

		std::map< LString, LString >::const_iterator it = FHeaders.find( lname );

		return ( it != FHeaders.end() ) ? it->second.c_str() : 0;
	}

	int HttpResponse::Pump( const unsigned char* data, int datasize )
	{
		if ( datasize <= 0 ) { return -2; }

		int count = datasize;

		while ( count > 0 && FState != COMPLETE )
		{
			if ( FState == BODY )
			{
				int bytesused = ProcessData( data, count );

				if ( bytesused < 0 ) { return -4; }

				data += bytesused;
				count -= bytesused;
				continue;
			}

			// we want to accumulate a line
			while ( count > 0 )
			{
				char c = ( char ) * data++;
				--count;

				if ( c != '\n' )
				{
					if ( c != '\r' ) { FLineBuf += c; } // ignore CR

					continue;
				}

				// now got a whole line!
				switch ( FState )
				{
					case STATUSLINE:

						if ( !ProcessStatusLine( FLineBuf ) ) { return -1; }

						break;
					case HEADERS:
						ProcessHeaderLine( FLineBuf );
						break;
					case TRAILERS:

						// handle trailers? (python httplib doesn't seem to!). Just ignore all the trailers
						if ( FLineBuf.empty() ) { Finish(); }

						break;
					case CHUNKLEN:
						ProcessChunkLenLine( FLineBuf );
						break;
					case CHUNKEND:

						// just soak up the crlf after body and go to next state
						if ( !FChunked ) { FLastError = "non-chunked mode error"; return -3; }

						FState = CHUNKLEN;
				}

				/// Done line accumulation
				FLineBuf.clear();
				break;
			}
		}

		// return number of bytes used
		return datasize - count;
	}

	void HttpResponse::ProcessChunkLenLine( const LString& line )
	{
		/// chunklen in hex at beginning of line
		FChunkLeft = strtol( line.c_str(), NULL, 16 );

		if ( FChunkLeft == 0 )
		{
			/// got the whole body, check for trailing headers
			FState = TRAILERS;
			FHeaderAccum.clear();
		}
		else
		{
			FState = BODY;
		}
	}

	int HttpResponse::ProcessData( const unsigned char* data, int count )
	{
		int n = count;

		if ( FChunked )
		{
			if ( n > FChunkLeft ) { n = FChunkLeft; }
		}
		else
		{
			if ( FLength != -1 )
			{
				// we know how many bytes to expect
				int remaining = FLength - FBytesRead;

				if ( n > remaining ) { n = remaining; }
			}
		}

		// invoke callback to pass out the data
		FConnection.OnData( this, data, n );

		FBytesRead += n;

		if ( FChunked )
		{
			FChunkLeft -= n;

			if ( FChunkLeft < 0 ) { FLastError = LString( "Negative chunk size" ); return -1; }

			/// Chunk completed. Soak up the trailing CRLF before next chunk
			if ( FChunkLeft == 0 ) { FState = CHUNKEND; }
		}
		else
		{
			/// Finish if we know we're done. Else we're waiting for connection close
			if ( FLength != -1 && FBytesRead >= FLength ) { Finish(); }
		}

		return n;
	}

	bool HttpResponse::ProcessStatusLine( const LString& line )
	{
		const char* p = line.c_str();

		/// Skip any leading space
		while ( *p && *p == ' ' ) { ++p; }

		/// Get version
		while ( *p && *p != ' ' ) { FVersionString += *p++; }

		while ( *p && *p == ' ' ) { ++p; }

		/// Get status code
		LString status;

		while ( *p && *p != ' ' ) { status += *p++; }

		while ( *p && *p == ' ' ) { ++p; }

		/// Rest of line is reason
		while ( *p ) { FReason += *p++; }

		FStatus = atoi( status.c_str() );

		if ( FStatus < 100 || FStatus > 999 ) { FLastError = LString( "BadStatusLine (" ) + line + LString( ")" ); return false; }

		if ( FVersionString == "HTTP:/1.0" )
		{
			FVersion = 10;
		}
		else if ( FVersionString.compare( 0, 7, "HTTP/1." ) == 0 )
		{
			FVersion = 11;
		}
		else
		{
			/// TODO: support for HTTP/0.9
			FLastError = LString( "UnknownProtocol (" ) + FVersionString + LString( ")" );
			return false;
		}

		/// OK, now we expect headers!
		FState = HEADERS;
		FHeaderAccum.clear();

		return true;
	}

	void HttpResponse::FlushHeader()
	{
		/// no flushing required
		if ( FHeaderAccum.empty() ) { return; }

		const char* p = FHeaderAccum.c_str();

		LString header, value;

		while ( *p && *p != ':' ) { header += ( char )tolower( *p++ ); }

		/// skip ':'
		if ( *p ) { ++p; }

		/// skip space
		while ( *p && ( *p == ' ' || *p == '\t' ) ) { ++p; }

		/// rest of line is value
		value = p;

		FHeaders[header] = value;

		FHeaderAccum.clear();
	}

	void HttpResponse::ProcessHeaderLine( const LString& line )
	{
		const char* p = line.c_str();

		if ( line.empty() )
		{
			FlushHeader();
			// end of headers

			// HTTP code 100 handling (we ignore 'em)

			// reset parsing and expect new status line or start new body
			if ( FStatus == CONTINUE ) { FState = STATUSLINE; }
			else { BeginBody(); }

			return;
		}

		if ( isspace( *p ) )
		{
			// it's a continuation line - just add it to previous data
			++p;

			while ( *p && isspace( *p ) ) { ++p; }

			FHeaderAccum += ' ';
			FHeaderAccum += p;
		}
		else
		{
			// begin a new header
			FlushHeader();
			FHeaderAccum = p;
		}
	}

/// OK, we've now got all the headers read in, so we're ready to start on the body
/// But we need to see what info we can glean from the headers first
	void HttpResponse::BeginBody()
	{
		FChunked = false;
		FLength = -1;  // unknown
		FWillClose = false;

		/// Using chunked encoding ?
		const char* trenc = GetHeader( "transfer-encoding" );

		if ( trenc && 0 == strcasecmp( trenc, "chunked" ) )
		{
			FChunked = true;

			/// Mark unknown length
			FChunkLeft = -1;
		}

		FWillClose = CheckClose();

		/// Is the length supplied ?
		const char* contentlen = GetHeader( "content-length" );

		/// Store length
		if ( contentlen && !FChunked )
		{
			FConnection.FDataLen = FLength = atoi( contentlen );
		}

		/// check for various cases where we expect zero-length body
		/// 1xx codes have no body
		if ( FStatus == NO_CONTENT || FStatus == NOT_MODIFIED || ( FStatus >= 100 && FStatus < 200 ) || FMethod == "HEAD" )
		{
			FLength = 0;
		}

		/// If we're not using chunked mode, and no length has been specified, assume connection will close at end.
		if ( !FWillClose && !FChunked && FLength == -1 ) { FWillClose = true; }

		FConnection.OnBegin( this );

		/// now start reading body data!
		FState = FChunked ? CHUNKLEN : BODY;
	}

	bool HttpResponse::CheckClose()
	{
		if ( FVersion == 11 )
		{
			/// HTTP1.1 - open unless "connection: close" is specified.
			const char* conn = GetHeader( "connection" );
			return ( conn && ( 0 == strcasecmp( conn, "close" ) ) );
		}

		/// Older HTTP keep-alive header indicates persistant connection
		return ( GetHeader( "keep-alive" ) == NULL );
	}

	void HttpResponse::Finish()
	{
		FState = COMPLETE;
		FConnection.OnComplete( this );
	}

} // end namespace happyhttp


//// Downloading

class clDownloadAsyncResult;

class HttpDownloadConn: public happyhttp::HttpConn
{
public:
	sEnvironment* Env;

	iObject* FCompletionHandler;

	HttpDownloadConn( const LString& addr, int port, const LString& path, const LString& outfile ):
		HttpConn( addr.c_str(), port ), FPath( path ), FOutFileName( outfile ), FFinished( false ), FCount( 0 ), FPaused( false ), FResult( NULL ) {}

	virtual ~HttpDownloadConn() {}

	/// Process incoming packets
	void Poll()
	{
		if ( FFinished ) { return; }

		if ( FPaused ) { return; }

		if ( !Outstanding() )
		{
			FFinished = true;
			return;
		}

		if ( !Pump() )
		{
			LString Err = LNetwork_LastErrorAsString();

//			if(Err != "NO ERROR")
			{
#ifdef DO_NOT_USE_ENGINE
//				printf("Socket error in download: %s\n", LNetwork_LastErrorAsString().c_str());
//				printf("Err: %s\n", FLastError.c_str());
#else
				Env->Logger->LogP( L_DEBUG, "Socket error in download: %s\n", Err.c_str() );
				Env->Logger->LogP( L_DEBUG, "Err: %s\n", FLastError.c_str() );
#endif
			}

			FFinished = true;
		}
	}

	bool FFinished;
	bool FPaused;

	LString FPath;
	LString FOutFileName;

	double FStartTime;

	int BytesDownloaded() const { return FCount; }

	double PercentComplete() const { double l = ( double )GetLength(); if ( l < 0.0001f ) { l = 1.0f; } return ( double )FCount / ( double )( l ); };

	double GetDownloadSpeed() const
	{
#ifdef DO_NOT_USE_ENGINE
		double dt = ( DoGetTime() - FStartTime );
#else
		double dt = ( Env->GetEngineTime() - FStartTime );
#endif

		if ( dt < 0.0001f ) { dt = 1.0f; }

		return ( ( double )FCount ) / dt;
	}

	/// Link to AsyncResult
	clDownloadAsyncResult* FResult;

#ifdef DO_NOT_USE_ENGINE

	virtual void OnBegin( const happyhttp::HttpResponse* r )
	{
		FStartTime = DoGetTime();

		printf( "BEGIN (%d %s)\n", r->GetStatus(), r->GetReason() );
		FCount = 0;

		ff = fopen( FOutFileName.c_str(), "wb" );
	}

	virtual void OnData( const happyhttp::HttpResponse* r, const unsigned char* data, int n )
	{
		fwrite( data, 1, n, ff );
		FCount += n;
		printf( "[%s] count: %d/%d\n", FPath.c_str(), FCount, GetLength() );
	}

	virtual void OnComplete( const happyhttp::HttpResponse* r );

private:
	int FCount;
	FILE* ff;

#else

	virtual void OnBegin( const happyhttp::HttpResponse* r )
	{
		FStartTime = Env->GetEngineTime();

		Env->Logger->LogP( L_DEBUG, "Begin download, status (%d %s)\n", r->GetStatus(), r->GetReason() );
		FCount = 0;

		OutStream = Env->FileSystem->CreateFileWriter( FOutFileName.c_str() );

		if ( !OutStream )
		{
			FFinished = true;
		}
	}

	virtual void OnData( const happyhttp::HttpResponse* r, const unsigned char* data, int n )
	{
		if ( !OutStream ) { return; }

		OutStream->BlockWrite( data, n );
		FCount += n;

		Env->Logger->LogP( L_DEBUG, "[%s] count: %d/%d\n", FPath.c_str(), FCount, GetLength() );
	}

	virtual void OnComplete( const happyhttp::HttpResponse* r )
	{
		if ( !OutStream ) { return; }

		OutStream->DisposeObject();
	}

private:
	int FCount;
	iOStream* OutStream;
#endif
};


class clDownloadAsyncResult: public iAsyncResult
{
public:
	clDownloadAsyncResult(): FConn( NULL ) { FFinished = false; FPaused = false; }
	virtual ~clDownloadAsyncResult() {}

	virtual bool Finished() { return FConn ? FConn->FFinished : FFinished; }
	virtual bool Paused()   { return FConn ? FConn->FPaused : false; }

	virtual void Pause( bool P ) { if ( FConn ) { FConn->FPaused = P; } }

public:
	HttpDownloadConn* FConn;
};

#ifdef DO_NOT_USE_ENGINE
void HttpDownloadConn::OnComplete( const happyhttp::HttpResponse* r )
{
	fclose( ff );

	if ( FResult ) { FResult->FFinished = true; FResult->FConn = NULL; }

	printf( "COMPLETE (%d bytes)\n", FCount );
}
#endif

void clDownloadThread::DumpDownloadState()
{
#ifdef DO_NOT_USE_ENGINE
	int cnt = 0;

	printf( "Total downloads: %d\n", FDownloads.size() );

	for ( std::list<HttpDownloadConn*>::iterator j = FDownloads.begin() ; j != FDownloads.end() ; j++ )
	{
		float dl = ( float )( *j )->GetDownloadSpeed();
		float perc = ( float )( *j )->PercentComplete();

		printf( "Download[%d, %s]: %d / %d, complete = %f, speed = %f bytes/sec\n", ( int )cnt, ( *j )->FPath.c_str(), ( *j )->BytesDownloaded(), ( *j )->GetLength(), perc, dl );

		++cnt;
	}

#endif
}

iAsyncResult* clDownloadThread::AddDownload( const LString& HostName, int Port, const LString& Path, const LString& OutFileName, iObject* CompletionHandler )
{
//	printf("New dnld\n");

	HttpDownloadConn* D = new HttpDownloadConn( HostName, Port, Path, OutFileName );
	D->Env = this->Env;
	D->FCompletionHandler = CompletionHandler;

	if ( !D->Request( "GET", Path.c_str(), 0, 0, 0 ) )
	{
		Env->Logger->LogP( L_DEBUG, "Error adding download: %s", D->FLastError.c_str() );
//		printf("Err: %s\n", D->FLastError.c_str());

		delete D;
		return NULL;
	}

//	printf("Add\n");

	FAddMutex.Lock();
	FDownloads.push_back( D );
	FAddMutex.Unlock();

	clDownloadAsyncResult* Res = new clDownloadAsyncResult();
	Res->FConn = D;

	D->FResult = Res;

	return Res;
}

void clDownloadThread::PollDownloads()
{
	FAddMutex.Lock();

	bool HasFinishedConns = false;

	for ( std::list<HttpDownloadConn*>::iterator j = FDownloads.begin() ; j != FDownloads.end() ; j++ )
	{
		if ( !( *j )->FPaused && !( *j )->FFinished )
		{
			( *j )->Poll();
		}

		HasFinishedConns |= ( *j )->FFinished;
	}

	if ( HasFinishedConns )
	{
		std::list<HttpDownloadConn*> NewDownloads;

		for ( std::list<HttpDownloadConn*>::iterator j = FDownloads.begin() ; j != FDownloads.end() ; j++ )
		{
			if ( ( *j )->FFinished )
			{
				Env->Console->DisplayInfoTip( "Downloaded file " + ( *j )->FOutFileName );

#ifndef DO_NOT_USE_ENGINE

				if ( ( *j )->FCompletionHandler )
				{
					( *j )->FCompletionHandler->SendAsyncNoArgs( L_EVENT_PROCESS_COMPLETE );
				}

#endif

				delete ( *j );
			}
			else
			{
				NewDownloads.push_back( *j );
			}
		}

		FDownloads = NewDownloads;
	}

	FAddMutex.Unlock();
}

void clDownloadThread::Run()
{
	Env->Logger->SetCurrentThreadName( "Downloader" );

	while ( !IsPendingExit() )
	{
		if ( FDownloads.size() == 0 )
		{
			/// Do not waste time while no downloads pending
			Env->ReleaseTimeslice( 100 );
		}
		else
		{
			PollDownloads();
		}
	}
}

///////////// Server part

static const LString base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline bool is_base64( unsigned char c ) { return ( isalnum( c ) || ( c == '+' ) || ( c == '/' ) ); }

inline void base64_encode3to4( unsigned char arr3[3], unsigned char arr4[4] )
{
	arr4[0] = ( arr3[0] & 0xfc ) >> 2;
	arr4[1] = ( ( arr3[0] & 0x03 ) << 4 ) + ( ( arr3[1] & 0xf0 ) >> 4 );
	arr4[2] = ( ( arr3[1] & 0x0f ) << 2 ) + ( ( arr3[2] & 0xc0 ) >> 6 );
	arr4[3] = arr3[2] & 0x3f;
}

inline void base64_decode4to3( unsigned char arr4[4], unsigned char arr3[3] )
{
	for ( int i = 0; i < 4; i++ ) { arr4[i] = ( unsigned char )base64_chars.find( arr4[i] ); }

	arr3[0] = ( arr4[0] << 2 ) + ( ( arr4[1] & 0x30 ) >> 4 );
	arr3[1] = ( ( arr4[1] & 0xf ) << 4 ) + ( ( arr4[2] & 0x3c ) >> 2 );
	arr3[2] = ( ( arr4[2] & 0x3 ) << 6 ) + arr4[3];
}

LString base64_encode( unsigned char const* bytes_to_encode, unsigned int in_len )
{
	LString ret;

	int i = 0;
	int j = 0;

	unsigned char arr3[3];
	unsigned char arr4[4];

	while ( in_len-- )
	{
		arr3[i++] = *( bytes_to_encode++ );

		if ( i == 3 )
		{
			base64_encode3to4( arr3, arr4 );

			for ( i = 0; ( i < 4 ) ; i++ )
			{
				ret += base64_chars[arr4[i]];
				i = 0;
			}
		}

		if ( i )
		{
			for ( j = i; j < 3; j++ ) { arr3[j] = '\0'; }

			base64_encode3to4( arr3, arr4 );

			for ( j = 0; ( j < i + 1 ); j++ ) { ret += base64_chars[arr4[j]]; }

			while ( ( i++ < 3 ) ) { ret += '='; }
		}
	}

	return ret;
}

LString base64_decode( const LString& encoded_string )
{
	int in_len = ( int )encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char arr4[4], arr3[3];

	LString ret;

	while ( in_len-- && ( encoded_string[in_] != '=' ) && is_base64( encoded_string[in_] ) )
	{
		arr4[i++] = encoded_string[in_];
		in_++;

		if ( i == 4 )
		{
			base64_decode4to3( arr4, arr3 );

			for ( i = 0; ( i < 3 ); i++ ) { ret += arr3[i]; }

			i = 0;
		}
	}

	if ( i )
	{
		for ( j = i; j < 4; j++ ) { arr4[j] = 0; }

		base64_decode4to3( arr4, arr3 );

		for ( j = 0; ( j < i - 1 ); j++ ) { ret += arr3[j]; }
	}

	return ret;
}

bool HTTPRemoveProtocolFromUrl( const LString& url, LString& protocol, LString& rest )
{
	LString::size_type pos_colon = url.find( ":" );

	if ( pos_colon == LString::npos )
	{
		rest = url;
		return false;
	}

	if ( url.size() < pos_colon + 2 )
	{
		rest = url;
		return false;
	}

	if ( url[pos_colon + 1] != '/' || url[pos_colon + 2] != '/' )
	{
		rest = url;
		return false;
	}

	protocol = url.substr( 0, pos_colon );
	rest     = url.substr( 3 + pos_colon ); // Skipping three characters ( '://' )

	return true;
}

void HTTPSplitGetReq( LString _req, LString& path, std::map<std::string, std::string>& params )
{
	// Remove trailing newlines
	for ( int i = 0 ; i < 2 ; i++ )
		if ( _req[_req.size()-1] == '\x0d' || _req[_req.size()-1] == '\x0a' ) { _req = _req.substr( 0, _req.size() - 1 ); }

	// Remove potential Trailing HTTP/1.x
	if ( _req.size() > 7 )
	{
		if ( _req.substr( _req.size() - 8, 7 ) == "HTTP/1." )
		{
			_req = _req.substr( 0, _req.size() - 9 );
		}
	}

	LString::size_type qm = _req.find( "?" );

	if ( qm == LString::npos )
	{
		path = _req;
		return;
	}

	LString url_params = _req.substr( qm + 1 );

	path = _req.substr( 0, qm );

	// Appending a '&' so that there are as many '&' as name-value pairs.
	// It makes it easier to split the url for name value pairs, he he he
	url_params += "&";

	LString::size_type next_amp = url_params.find( "&" );

	while ( next_amp != LString::npos )
	{
		LString name_value = url_params.substr( 0, next_amp );
		url_params         = url_params.substr( next_amp + 1 );
		next_amp           = url_params.find( "&" );

		LString::size_type pos_equal = name_value.find( "=" );

		LString nam = name_value.substr( 0, pos_equal );
		LString val = name_value.substr( pos_equal + 1 );

		LString::size_type pos_plus;

		while ( ( pos_plus = val.find( "+" ) ) != LString::npos )
		{
			val.replace( pos_plus, 1, " " );
		}

		// Replacing %xy notation
		LString::size_type pos_hex = 0;

		while ( ( pos_hex = val.find( "%", pos_hex ) ) != LString::npos )
		{
			/*
			         std::strstream h;
			         h << val.substr(pos_hex+1, 2);
			         h << std::hex;
			*/
			int i = ( int )LStr::StrToHex( val.substr( pos_hex + 1, 2 ) );
//			h>>i;

			val.replace( pos_hex, 3, LStr::ToStr( static_cast<char>( i ) ) );
			pos_hex ++;
		}

		params.insert( std::map<LString, LString>::value_type( nam, val ) );
	}
}

void HTTPSplitUrl( LString const& url, LString& protocol, LString& server, LString& path )
{
	HTTPRemoveProtocolFromUrl( url, protocol, server );

	if ( protocol == "http" )
	{
		LString::size_type pos_slash = server.find( "/" );

		if ( pos_slash != LString::npos )
		{
			path   = server.substr( pos_slash );
			server = server.substr( 0, pos_slash );
		}
		else
		{
			path = "/";
		}
	}
	else
	{
		// std::cerr << "unknown protocol in SplitUrl: '" << protocol << "'" << std::endl;
	}
}

/////////////////////////// Client thread

class clHTTPRequestThread: public iThread
{
public:
	clHTTPRequestThread() {}
	virtual ~clHTTPRequestThread() {}

	virtual void Run();

	void HandleRequest( sHTTPServerRequest* r );

	/// Link to parent server
	clHTTPServerThread* FServer;

	/// Link to the incoming socket
	LTCPSocket* FSocket;
};

string CompressSpaces( const string& s )
{
	string out = "";

	for ( size_t j = 0 ; j < s.length() ; j++ )
	{
		char cc = s[j];

		if ( cc == ' ' || cc == '\n' || cc == '\r' ) { continue; }

		out += cc;
	}

	return out;
}

/// TODO: Handle %0A etc.
std::string Htmlize( const std::string _in )
{
	std::string _out = "<pre>";

	for ( size_t j = 0 ; j < _in.length() ; j++ )
	{
		if ( _in[j] == 10 || _in[j] == 13 ) { _out += "</pre><br><pre>"; }
		else { _out += _in[j]; }
	}

	return _out + "</pre>";
}

LString PercentEncode( const std::string& s )
{
	return s;
}

LString DecodePercents( const LString& s )
{
	LString val = s;
	LString::size_type pos_plus;

	while ( ( pos_plus = val.find( "+" ) ) != LString::npos )
	{
		val.replace( pos_plus, 1, " " );
	}

	// Replacing %xy notation
	LString::size_type pos_hex = 0;

	while ( ( pos_hex = val.find( "%", pos_hex ) ) != LString::npos )
	{
		/*
		      std::strstream h;
		      h << val.substr(pos_hex+1, 2);
		      h << std::hex;

		      int i;
		      h>>i;
		*/
		int i = ( int )LStr::StrToHex( val.substr( pos_hex + 1, 2 ) );

		val.replace( pos_hex, 3, LStr::ToStr( static_cast<char>( i ) ) );
		pos_hex ++;
	}

	return val;
}

void clHTTPRequestThread::HandleRequest( sHTTPServerRequest* r )
{
	/*if(r->FMethod == "POST")
	{
	   printf("Path for post is %s\n", r->FPath.c_str());
	}*/

	LString title;
	LString body;
	LString bgcolor = "#ffffff";
	LString links =
	   "<br><a href='/RemoteConsole'>Remote Console</a> "
	   "<br><a href='/Variables'>Change and inspect variables</a> "
	   "<br><a href='/Properties'>Change and inspect object properties</a> "
	   "<br><a href='/Statistics'>Runtime statistics</a> "
	   "<br><a href='/MakeScreenshot'>Make a screenshot</a> "
	   "<br><a href='/UploadFile'>Upload a file to the engine</a> ";

	LString connection_info =
	   LString( "<br><hr>" ) +
	   LString( "Connection information" ) +
	   LString( "<table>" ) +
	   "<hr>"
	   "<tr><td>Referer:</td><td>"         + r->FReferer        + "</td></tr>" +
	   "<tr><td>Accept:</td><td>"          + r->FAccept         + "</td></tr>" +
	   "<tr><td>Accept-Encoding:</td><td>" + r->FAcceptEncoding + "</td></tr>" +
	   "<tr><td>Accept-Language:</td><td>" + r->FAcceptLanguage + "</td></tr>" +
	   "<tr><td>User-Agent:</td><td>"      + r->FUserAgent      + "</td></tr>" +
	   "</table>";

	r->FBinaryAnswer = false;

	if ( r->FPath == "/" )
	{
		title = "Linderdaum Web Server";

		body = "<img src=\"/banner.jpg\"/><br>";

		body  += LString( "<h1>Linderdaum Web Server</h1>" ) + links + "<br>" + connection_info;
	}

	else if ( r->FPath == "/upload.php" )
	{
		Env->Logger->LogP( L_NOTICE, "Got method[%s] for upload.php\n", r->FMethod.c_str() );

		Env->Logger->LogP( L_NOTICE, "Content disposition: %s\n", r->FContentDisposition.c_str() );
		Env->Logger->LogP( L_NOTICE, "Content type: %s\n", r->FContentType.c_str() );
		Env->Logger->LogP( L_NOTICE, "Content length: %s\n", r->FContentLength.c_str() );

		Env->Logger->LogP( L_NOTICE, "Uploaded data:\n%s\n", r->FContent.c_str() );

		Env->Logger->LogP( L_NOTICE, "Decode64:\n%s\n", base64_decode( r->FContent ).c_str() );
	}

	else if ( r->FPath == "/banner.jpg" )
	{
		// Sending logo image
		Luint64 Len;
		Env->FileSystem->LoadFileData( "banner.jpg", ( void** )&r->FData, &Len );
		r->FDataLen = ( int )Len;
		r->FBinaryAnswer = true;
	}

	else if ( r->FPath == "/http_screenshot.png" )
	{
		// Sending SCREENSHOT

		Env->Console->QueryCommand( "SaveScreenshot tmp_http_screen.png" );
		LString PathToImg = Env->Console->GetVarValueStr( "Renderer.ScreenshotsDir", "ScreenShots" ) + "/tmp_http_screen.png";

		const double MaxWait = 2.0;

		double Timeout = Env->GetEngineTime();

		// Tricky trick. Poll until the 'tmp_http_screen' file becomes available
		while ( true )
		{
			Env->SleepSeconds( 0.5 );

			if ( Env->FileSystem->FileExists( PathToImg ) )
			{
				Luint64 Len;
				Env->SleepSeconds( 0.5 );
				Env->FileSystem->LoadFileData( PathToImg, ( void** )&r->FData, &Len );
				r->FDataLen = ( int )Len;
				r->FBinaryAnswer = true;

				break;
			}

			if ( Env->GetEngineTime() - Timeout > MaxWait ) { break; }
		}
	}

	else if ( r->FPath == "/MakeScreenshot" )
	{
		title = "Screenshot";

		body = "<img src=\"/http_screenshot.png\"/>"
		       "<br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/Properties" )
	{
		title   = "Manage object properties";

		body =
		   "<b>Set property value</b>"
		   "<hr>"

		   "<form name=\"test\" action='/SetProperty'>"
		   "<p><b>Object ID:</b><br>"
		   "<input name='ObjectID' type='text' size='60'>"
		   "<p><b>Property name:</b><br>"
		   "<input name='PropName' type='text' size='60'>"
		   "<p><b>Property value:</b><br>"
		   "<input name='PropValue' type='text' size='60'>"
		   "<p><input type=\"submit\" value=\"Set\">"
		   "<input type=\"reset\" value=\"Clear\"></p>"
		   "</form>"
		   "</p>"
		   "<hr>"

		   "<b>Get property value</b>"
		   "<hr>"

		   "<form name=\"test\" action='/GetProperty'>"
		   "<p><b>Object ID:</b><br>"
		   "<input name='ObjectID' type='text' size='60'>"
		   "<p><b>Property name:</b><br>"
		   "<input name='PropName' type='text' size='60'>"
		   "<p><input type=\"submit\" value=\"Get\">"
		   "<input type=\"reset\" value=\"Clear\"></p>"
		   "</form>"
		   "</p>"
		   "<hr>"

		   "<hr><br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/Variables" )
	{
		title   = "Manage variables";

		body =
		   "<b>Set variable value</b>"
		   "<hr>"

		   "<form name=\"test\" action='/SetValue'>"
		   "<p><b>Variable:</b><br>"
		   "<input name='VarName' type='text' size='60'>"
		   "<p><b>Value:</b><br>"
		   "<input name='VarValue' type='text' size='60'>"
		   "<p><input type=\"submit\" value=\"Set\">"
		   "<input type=\"reset\" value=\"Clear\"></p>"
		   "</form>"
		   "</p>"
		   "<hr>"

		   "<b>Get variable value</b>"
		   "<hr>"

		   "<form name=\"test\" action='/GetValue'>"
		   "<p><b>Variable:</b><br>"
		   "<input name='VarName' type='text' size='40'>"
		   "<p><input type=\"submit\" value=\"Get\">"
		   "<input type=\"reset\" value=\"Clear\"></p>"
		   " </form>"
		   "<hr>"

		   "<hr><br><a href='/'>Back to main</a> ";
	}

	/// Linderdaum shell
	else if ( r->FPath == "/RemoteConsole" )
	{
		title   = "Linderdaum Remote Console"; // TODO: add IP info

		body =
		   "<b>Execute script</b>"
		   "<hr>"

		   "<form name=\"test\" action='/ExecScript'>"
		   "<p>Script<br>"
		   "<textarea name='script' cols='70' rows='8'></textarea></p>"
		   "<p><input type='submit' value='Execute'>"
		   "<input type='reset' value='Clear'></p>"
		   "</form>"
		   "<hr>"

		   "<hr><br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/SetValue" )
	{
		body += "<br>Var name : " + r->FParams["VarName"];
		body += "<br>Var value: " + r->FParams["VarValue"];

		Env->Console->GetVar( r->FParams["VarName"] )->SetString( r->FParams["VarValue"] );

		body += "<hr><br><a href='/Variables'>Set another variable</a> ";
		body += "<br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/GetValue" )
	{
		body += "<br>Var name : " + r->FParams["VarName"];
		body += "<br>Var value: " + Env->Console->GetVar( r->FParams["VarName"] )->GetString();

		body += "<hr><br><a href='/Variables'>Get another variable</a> ";
		body += "<br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/SetProperty" )
	{
		iObject* Obj = Env->FindObject( r->FParams["ObjectID"] );

		if ( Obj )
		{
			body += "<br>Object ID     : " + r->FParams["ObjectID"];
			body += "<br>Property name : " + r->FParams["PropName"];
			body += "<br>Property value: " + r->FParams["PropValue"];

			Obj->SetPropertyValue( r->FParams["PropName"], r->FParams["PropValue"] );
		}
		else
		{
			body += "<br>Object [" + r->FParams["ObjectID"] + "] not found";
		}

		body += "<hr><br><a href='/Properties'>Set another property</a> ";
		body += "<br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/GetProperty" )
	{
		iObject* Obj = Env->FindObject( r->FParams["ObjectID"] );

		if ( Obj )
		{
			body += "<br>Object ID      : " + r->FParams["ObjectID"];
			body += "<br>Property name  : " + r->FParams["PropName"];
			body += "<br>Property value : " + Obj->GetPropertyValue( r->FParams["PropName"] );
		}
		else
		{
			body += "<br>Object [" + r->FParams["ObjectID"] + "] not found";
		}

		body += "<hr><br><a href='/Properties'>Get another property</a> ";
		body += "<br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/ExecScript" )
	{
		LString Script = DecodePercents( r->FParams["script"] );

		body = LString( "<br>Executing:<br><br>" ) + Script;

		Env->Console->SendCommand( Script );

		body += "<hr><br><a href='/RemoteConsole'>Execute another command</a> ";
		body += "<br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/Statistics" )
	{
		body = LString( "Statistics" );
		// Env->Console->GetVar(r->FParams["VarName"])->GetString();

		body += "<hr><br><a href='/'>Back to main</a> ";
	}

	else if ( r->FPath == "/UploadFile" )
	{
		/// Test large file uploading
		title = "Large file upload";

		body =
		   "<b>Enter file name and click Submit</b>"
		   "<form method=\"POST\" action=\"/upload.php\" enctype=\"multipart/form-data\">"
		   "<input type='file' name='myfile1' size='60'>"
		   "<input type='submit' value='Submit'>"
		   "<input type='reset' value='Reset' name='B2'>"
		   "</form>"

		   "<hr><br><a href='/'>Back to main</a> ";
	}
	else
	{
		/// Check for shared files
		if ( FServer->IsSharedFile( r->FPath ) )
		{
			LString LocalFile = FServer->ServerPathToFileName( r->FPath );

			Luint64 Len;

			if ( Env->FileSystem->LoadFileData( LocalFile, ( void** )&r->FData, &Len ) )
			{
				r->FDataLen = ( int )Len;
				r->FBinaryAnswer = true;
			}
			else
			{
				Env->Logger->LogP( L_WARNING, "Could not read [%s] mapped to HTTP [%s]", LocalFile.c_str(), r->FPath.c_str() );
			}
		}

		/// Were we able to find the file ?
		if ( !r->FBinaryAnswer )
		{
			r->FStatus = "404 Not Found";
			title      = "Unsupported service URL";
			body       = "<h1>Unsupported service URL or invalid shared file name</h1>";
			body      += "Path is : &gt;" + r->FPath + "&lt;";
		}
	}

	if ( !r->FBinaryAnswer )
	{
		r->FAnswer  = "<html><head><title>";
		r->FAnswer += title;
		r->FAnswer += "</title></head><body bgcolor='" + bgcolor + "'>";
		r->FAnswer += body;
		r->FAnswer += "</body></html>";
	}
}

void clHTTPRequestThread::Run()
{
	Env->Logger->SetCurrentThreadName( "HTTPRequest" );

	int MaxLineLen = 1024;

	LString line = FSocket->ReceiveLine( MaxLineLen );

	if ( line.empty() ) { return; }

	sHTTPServerRequest req;

	if ( line.find( "GET" ) == 0 )
	{
		req.FMethod = "GET";
	}
	else if ( line.find( "POST" ) == 0 )
	{
		req.FMethod = "POST";
	}

	LString path;
	std::map<LString, LString> params;

	// cut POST/GET from the beginning of the string
	size_t posStartPath = line.find_first_not_of( " ", req.FMethod.length() );

	HTTPSplitGetReq( line.substr( posStartPath ), path, params );

	req.FStatus = "202 OK";
	req.FSocket = FSocket;
	req.FPath   = path;
	req.FParams = params;

	/// Ugly ad-hoc parser of request. Sorry, no boost::Spirit or even GoldParser here.

	static const string accept          = "Accept: "             ;
	static const string accept_language = "Accept-Language: "    ;
	static const string accept_encoding = "Accept-Encoding: "    ;
	static const string user_agent      = "User-Agent: "         ;

	static const string referer = "Referer: ";

	static const string content_length      = "Content-Length: ";
	static const string content_type        = "Content-Type: ";
	static const string content_disposition = "Content-Disposition: ";

	string the_boundary;
	req.FBinaryAnswer = false;
	req.FContent.clear();
	req.FContentType.clear();
	req.FContentLength.clear();
	req.FContentDisposition.clear();

	bool reading_data = false;

	/// We also check for timeouts and abnormal terminations here
	while ( !IsPendingExit() )
	{
		line = FSocket->ReceiveLine( MaxLineLen );

		if ( req.FMethod == "GET" )
			if ( line.empty() ) { break; }

		int pos_cr_lf = ( int )line.find_first_of( "\x0a\x0d" );

		if ( req.FMethod == "GET" )
			if ( pos_cr_lf == 0 ) { break; }

		line = line.substr( 0, ( size_t )pos_cr_lf );

		if ( line.substr( 0, 2 ) == "--" )
		{
			if ( the_boundary.empty() )
			{
				the_boundary = line;
//				cout << "STORING Boundary: " << the_boundary << endl;
				reading_data = true;
			}
			else
			{
//				cout << "Got terminator of content" << endl;
				break;
			}
		}
		else if ( line.substr( 0, referer.size() ) == referer )
		{
			req.FReferer = line.substr( referer.size() );
		}
		else if ( line.substr( 0, accept.size() ) == accept )
		{
			req.FAccept = line.substr( accept.size() );
		}
		else if ( line.substr( 0, accept_language.size() ) == accept_language )
		{
			req.FAcceptLanguage = line.substr( accept_language.size() );
		}
		else if ( line.substr( 0, accept_encoding.size() ) == accept_encoding )
		{
			req.FAcceptEncoding = line.substr( accept_encoding.size() );
		}
		else if ( line.substr( 0, user_agent.size() ) == user_agent )
		{
			req.FUserAgent = line.substr( user_agent.size() );
		}
		else if ( line.substr( 0, content_length.size() ) == content_length )
		{
			req.FContentLength = line.substr( content_length.size() );
		}
		else if ( line.substr( 0, content_type.size() ) == content_type )
		{
			req.FContentType = line.substr( content_type.size() );
		}
		else if ( line.substr( 0, content_disposition.size() ) == content_disposition )
		{
			req.FContentDisposition = line.substr( content_disposition.size() );
		}
		else
		{
			if ( !the_boundary.empty() && reading_data )
			{
				// request body for POST
				req.FContent += line + string( "\r\n" );
			}
		}
	}

	/*
	   cout << "=======Data=========" << endl;
	   cout << req.content_ << endl;
	   cout << "==Compressed data===" << endl;
	   req.content_ = CompressSpaces(req.content_);
	   cout << req.content_ << endl;
	   cout << "=======End data=====" << endl;
	*/
	HandleRequest( &req );

	LString SizeString = req.FBinaryAnswer ? LStr::ToStr( req.FDataLen ) : LStr::ToStr( req.FAnswer.size() );
	/*
	   time_t ltime;
	   time(&ltime);
	   tm* gmt= gmtime(&ltime);

	   char* asctime_remove_nl = asctime(gmt);
	   asctime_remove_nl[24] = 0;
	*/
	static string const serverName = "Linderdaum Web server (Platform info goes here)";

	FSocket->SendBytes( "HTTP/1.1 " );

	FSocket->SendLine( req.FStatus );
//	FSocket->SendLine(string("Date: ") + asctime_remove_nl + " GMT");
	FSocket->SendLine( string( "Server: " ) + serverName );
	FSocket->SendLine( "Connection: close" );

	if ( req.FBinaryAnswer )
	{
		FSocket->SendLine( "Content-Type: application/octet-stream" );
	}
	else
	{
		FSocket->SendLine( "Content-Type: text/html; charset=ISO-8859-1" );
	}

	FSocket->SendLine( "Content-Length: " + SizeString );
	FSocket->SendLine( "" );

	if ( req.FBinaryAnswer )
	{
		FSocket->WriteBytes( req.FData, req.FDataLen );
		delete[] req.FData;
	}
	else
	{
		FSocket->SendLine( req.FAnswer );
	}

	FSocket->Close();

	delete FSocket;

	// This must be safe, iThread does not do anything
	delete this;
}

/////////////////////////// End of client thread


void clHTTPServerThread::AddSharedFile( const LString& FileName, const LString& ServerPath )
{
	FSharedFiles[ServerPath] = FileName;
}

void clHTTPServerThread::RemoveSharedFile( const LString& FileName )
{
	LString ServerPath = FileNameToServerPath( FileName );

	/// Remove all aliases
	while ( !ServerPath.empty() )
	{
		FSharedFiles.erase( ServerPath );

		ServerPath = FileNameToServerPath( FileName );
	}
}

LString clHTTPServerThread::FileNameToServerPath( const LString& FileName )
{
	for ( std::map<LString, LString>::iterator i = FSharedFiles.begin() ; i != FSharedFiles.end() ; ++i )
	{
		if ( i->second == FileName )
		{
			return i->first;
		}
	}

	return "";
}

LString clHTTPServerThread::ServerPathToFileName( const LString& ServerPath )
{
	if ( IsSharedFile( ServerPath ) )
	{
		return FSharedFiles[ServerPath];
	}

	return "";
}

bool clHTTPServerThread::IsSharedFile( const LString& ServerPath )
{
	return ( FSharedFiles.count( ServerPath ) > 0 );
}

void clHTTPServerThread::Run()
{
	Env->Logger->SetCurrentThreadName( "HTTPServer" );

	Env->Logger->LogP( L_NOTICE, "Starting HTTP server at %s:%d, MaxConn = %d", FBindAddress.c_str(), FPort, FMaxConnections );

	LTCPSocket* in = new LTCPSocket();

	if ( !in->Open() )
	{
		Env->Logger->Log( L_WARNING, "Cannot open TCP socket" );
		return;
	}

	in->SetNonBlocking( true );

	if ( !in->Bind( FBindAddress, FPort ) )
	{
		Env->Logger->LogP( L_WARNING, "Cannot bind TCP socket to %d port", FPort );
		Env->Logger->LogP( L_WARNING, "Net error: %s", LNetwork_LastErrorAsString().c_str() );
		delete in;
		return;
	}

	if ( !in->Listen( FMaxConnections ) )
	{
		Env->Logger->Log( L_WARNING, "Cannot start listening TCP socket for HTTP server" );

		delete in;
		return;
	}

	// Running threads' list

	while ( !IsPendingExit() )
	{
		// Check data with 100us timeout
		if ( in->CheckData( true, false, false, 0, 100 ) )
		{
			/// Here we cannot block, the Select() returned true
			LTCPSocket* NewSocket = in->Accept();

			if ( NewSocket != 0 )
			{
				// Got socket

				// Add new thread
				clHTTPRequestThread* T = new clHTTPRequestThread();
				T->Env = this->Env;
				T->FServer = this;
				T->FSocket = NewSocket;

				T->Start( this->Env, iThread::Priority_Lowest );
			}
		}
		else
		{
			if ( in->IsError() )
			{
				Env->Logger->LogP( L_WARNING, "Error in socket after Select(): %s", in->GetError().c_str() );
			}
		}

		// Check for finished request threads here
	}

	// Clear runnning threads
	/// RequestThreads.....

	in->Close();

	delete in;
}
