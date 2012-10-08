/**
 * \file HTTP.h
 * \brief Simple HTTP client-server library (based on HappyHTTP http://scumways.com/happyhttp/happyhttp.html and Simple WebServer from http://www.adp-gmbh.ch)
 * \version 0.5.95
 * \date 16/06/2010
 * \author Viktor Latypov, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
*/

/// First we declare the client part

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

/// Modified by Viktor Latypov to fit Linderdaum's coding style

/// Exceptions/Asserts were removed and replaced by error code passing (fLastError fields and bool return codes)

#ifndef HAPPYHTTP_H
#define HAPPYHTTP_H

#include <list>
#include <map>
#include <vector>
#include <deque>

#include "Network.h"
using namespace std;

#include "Utils/Thread.h"
#include "Utils/Mutex.h"

namespace happyhttp
{

	class HttpResponse;
	class HttpConn;

/// HTTP status codes
	enum HTTPStatusCode
	{
		// 1xx informational
		CONTINUE = 100,
		SWITCHING_PROTOCOLS = 101,
		PROCESSING = 102,

		// 2xx successful
		OK = 200,
		CREATED = 201,
		ACCEPTED = 202,
		NON_AUTHORITATIVE_INFORMATION = 203,
		NO_CONTENT = 204,
		RESET_CONTENT = 205,
		PARTIAL_CONTENT = 206,
		MULTI_STATUS = 207,
		IM_USED = 226,

		// 3xx redirection
		MULTIPLE_CHOICES = 300,
		MOVED_PERMANENTLY = 301,
		FOUND = 302,
		SEE_OTHER = 303,
		NOT_MODIFIED = 304,
		USE_PROXY = 305,
		TEMPORARY_REDIRECT = 307,

		// 4xx client error
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		PAYMENT_REQUIRED = 402,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED = 405,
		NOT_ACCEPTABLE = 406,
		PROXY_AUTHENTICATION_REQUIRED = 407,
		REQUEST_TIMEOUT = 408,
		CONFLICT = 409,
		GONE = 410,
		LENGTH_REQUIRED = 411,
		PRECONDITION_FAILED = 412,
		REQUEST_ENTITY_TOO_LARGE = 413,
		REQUEST_URI_TOO_LONG = 414,
		UNSUPPORTED_MEDIA_TYPE = 415,
		REQUESTED_RANGE_NOT_SATISFIABLE = 416,
		EXPECTATION_FAILED = 417,
		UNPROCESSABLE_ENTITY = 422,
		LOCKED = 423,
		FAILED_DEPENDENCY = 424,
		UPGRADE_REQUIRED = 426,

		// 5xx server error
		INTERNAL_SERVER_ERROR = 500,
		NOT_IMPLEMENTED = 501,
		BAD_GATEWAY = 502,
		SERVICE_UNAVAILABLE = 503,
		GATEWAY_TIMEOUT = 504,
		HTTP_VERSION_NOT_SUPPORTED = 505,
		INSUFFICIENT_STORAGE = 507,
		NOT_EXTENDED = 510,
	};


/// HttpResponse class handles parsing of response data
	class HttpResponse
	{
		friend class HttpConn;

	public:
		/// Retrieve a header or return 0 if not present
		const char* GetHeader( const char* name ) const;

		/// Check for operation completion
		inline bool Completed() const { return FState == COMPLETE; }

		/// Get the HTTP status code. Only valid once we've got the statusline
		inline int GetStatus() const { return FStatus; }

		/// Get the HTTP response reason string. Only valid once we've got the statusline
		inline const char* GetReason() const { return FReason.c_str(); }

		/// Last error code
		LString FLastError;

		/// True if connection is expected to close after this response.
		inline bool WillClose() const { return FWillClose; }

#pragma region Interface used by HttpConn

	protected:

		/// Only HttpConn creates HttpResponses.
		HttpResponse( const char* method, HttpConn& conn );

		/// Pump some data in for processing. Returns the number of bytes used or negative value on error. Will always return 0 when response is complete.
		int Pump( const unsigned char* data, int datasize );

		/// Tell response that connection has closed
		inline bool NotifyConnectionClosed()
		{
			if ( FState == COMPLETE ) { return true; }

			/// EOF can be valid
			if ( FState == BODY && !FChunked && FLength == -1 )
			{
				Finish();   // we're all done!
			}
			else
			{
				FLastError = LString( "HttpConn closed unexpectedly" );
				return false;
			}

			return true;
		}

#pragma endregion

	private:
		enum
		{
			STATUSLINE,    // start here. status line is first line of response.
			HEADERS,    // reading in header lines
			BODY,       // waiting for some body data (all or a chunk)
			CHUNKLEN,      // expecting a chunk length indicator (in hex)
			CHUNKEND,      // got the chunk, now expecting a trailing blank line
			TRAILERS,      // reading trailers after body.
			COMPLETE,      // response is complete
		} FState;

		/// Link to owner
		HttpConn& FConnection;

		/// Request method name: "GET", "POST", etc.
		LString FMethod;

#pragma region Status line

		/// HTTP-Version
		LString FVersionString;

		/// 10: HTTP/1.0    11: HTTP/1.x (where x>=1)
		int     FVersion;

		/// Status-Code
		int     FStatus;

		/// Reason-Phrase
		LString FReason;

#pragma endregion

#pragma region Response contents

		/// header/value pairs
		std::map<LString, LString> FHeaders;

		/// body bytes read so far
		int   FBytesRead;

		/// response is chunked?
		bool  FChunked;

		/// bytes left in current chunk
		int   FChunkLeft;

		/// -1 if unknown
		int   FLength;

		/// connection will close at response end?
		bool  FWillClose;

#pragma endregion

		/// Line accumulation for states that want it
		LString FLineBuf;

		/// Accumulation buffer for headers
		LString FHeaderAccum;

		/// Process accumulated header data
		void FlushHeader();

		bool ProcessStatusLine( const LString& line );
		void ProcessHeaderLine( const LString& line );
		void ProcessChunkLenLine( const LString& line );

		/// Handle some body data in (non)chunked mode. Returns number of bytes used
		int ProcessData( const unsigned char* data, int count );

		void BeginBody();

		/// Return true if we think server will automatically close connection
		bool CheckClose();

		/// Finish connection
		void Finish();
		/*
		   {
		      FState = COMPLETE;
		      FConnection.OnComplete(this);
		   }
		*/
	};

/// HttpConn handles the socket connection, issuing of requests and managing responses
	class HttpConn
	{
		friend class HttpResponse;

		enum { IDLE, REQ_STARTED, REQ_SENT } FState;

	public:
		/// Store host and port. HttpConn doesn't connect immediately
		HttpConn( const char* host, int port ) :
			FState( IDLE ),
			FHost( host ),
			FPort( port ),
			FSock( NULL )
		{
		}

		~HttpConn() { Close(); }

		/// Last error string
		LString FLastError;

		/// Don't need to call connect() explicitly as issuing a request will call it automatically if needed.
		/// But it could block (for name lookup etc), so you might prefer to call it in advance.
		bool Connect();

		/// Close connection, discarding any pending requests.
		void Close();

		/// Update the connection (non-blocking). Just keep calling this regularly to service outstanding requests.
		bool Pump();

		/// Any requests still outstanding?
		inline bool Outstanding() const { return !FOutstanding.empty(); }

		/// Get the size of the file being downloaded
		inline int GetLength() const { return FDataLen; }

		/**
		   \brief High-level request interface

		   method is "GET", "POST" etc...
		   url is only path part: eg  "/index.html"
		   headers is array of name/value pairs, terminated by a null-ptr
		   body & bodysize specify body data of request (eg values for a form)
		*/
		bool Request( const char* method, const char* url, const char* headers[] = 0, const unsigned char* body = 0, int bodysize = 0 );

#pragma region Low-level request interface

		/**
		   \brief Begin request

		   method is "GET", "POST", etc.
		   url is only path part: eg  "/index.html"
		*/
		bool PutRequest( const char* method, const char* url );

		/// Add a header to the request (call after putrequest() )
		bool PutHeader( const char* header, const char* value );
		/// alternate version
		bool PutHeaderN( const char* header, int numericvalue );

		/// Finished adding headers, issue the request.
		bool EndHeaders();

#pragma endregion

		// Send body data if any. To be called after endheaders()
		bool Send( const unsigned char* buf, int numbytes );

		virtual void OnBegin( const HttpResponse* r ) { ( void )r; }
		virtual void OnData( const HttpResponse* r, const unsigned char* data, int numbytes ) { ( void )r; ( void )data; ( void )numbytes; }
		virtual void OnComplete( const HttpResponse* r ) { ( void )r; }

	protected:
		// some bits of implementation exposed to HttpResponse class

		/// Size of the file being downloaded
		int FDataLen;

	private:
		/// Remote host name
		LString FHost;

		/// Remote port
		int FPort;

		/// Socket handle
		LTCPSocket* FSock;

		/// Lines of request
		std::vector< LString > FBuffer;

		/// HttpResponses for outstanding requests
		std::deque< HttpResponse* > FOutstanding;
	};

} // end namespace happyhttp

/**

// Smallest sample to download a file

#include <stdio.h>

c_lass MyConn: public happyhttp::HttpConn
{
   int count;
   FILE* ff;
public:
   MyConn(const char* address, int port): HttpConn(address, port) {}

   virtual void OnBegin(const happyhttp::HttpResponse* r)
   {
      printf( "BEGIN (%d %s)\n", r->GetStatus(), r->GetReason() );
      count = 0;

      ff = fopen("out.png", "wb");
   }

   virtual void OnData(const happyhttp::HttpResponse* r, const unsigned char* data, int n)
   {
      fwrite(data, 1, n, ff);
      count += n;
      printf("count: %d/%d\n", count, GetLength());
   }

   virtual void OnComplete(const happyhttp::HttpResponse* r)
   {
      fclose(ff);

      printf( "COMPLETE (%d bytes)\n", count );
   }
};

int main()
{
   InitializeNetwork();

   happyhttp::HttpConn* conn = new MyConn( "ftp.linderdaum.com", 80 );

   if (!conn->Request( "GET", "/AnimatedVolume.avi", 0, 0, 0))
   {
      printf("Err: %s\n", conn->FLastError.c_str());
      return 1;
   }

   while( conn->Outstanding() )
      if (!conn->Pump())
      {
         printf("Err: %s\n", conn->FLastError.c_str());
         break;
      }

   delete conn;

   ShutdownNetwork();

   return 0;
}
*/

//// Now the Server part of the library


/*
   Simple HTTP server derived from Rene Nyffenegger's source code.
   - Removed any OS dependencies
   - Reformatted source code
   - Binary file uploading in GET method
   - Added support for POST method to upload files

   Original source code notice:
   ============================

   Copyright (C) 2003-2004 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

   http://www.adp-gmbh.ch/
*/

/**
   \brief HTTP server request/response structure

   This class has much in common with happyhttp::HttpResponse,
   but it is just not the point here to achieve maximum code reuse.

   Both classes are compatible and this server can be accessed with the previous Client Download class

   Since we are not reinventing ACE here,
   we do not bother about a little code duplication.
*/
struct sHTTPServerRequest
{
	/// Empty constructor
	sHTTPServerRequest() {}

#pragma region Binary responce buffer

	/// Is the answer binary. If yes, the FData is used. FAnswer string is used otherwise.
	bool FBinaryAnswer;

	/// Data for binary response. Managed externally. Mus be freed with delete[] call
	unsigned char* FData;

	/// Size of binary response buffer
	int FDataLen;

#pragma endregion

	/// Internal TCP socket handle
	LTCPSocket* FSocket;

	/// GET or POST method signature
	LString FMethod;

	/// Path part of the URI
	LString FPath;

	/// Dictionary with all HTTP request parameters (URI part after '?' symbol)
	std::map<LString, LString> FParams;

	/// Information about referer
	LString FReferer;

#pragma region

	LString FAccept;
	LString FAcceptLanguage;
	LString FAcceptEncoding;
	LString FUserAgent;

#pragma endregion

	/// Server error status (202, 404 etc) stored as string
	LString FStatus;

	/// The actual response body
	LString FAnswer;

#pragma region POST method support

	LString FContent;
	LString FContentType;
	LString FContentLength;
	LString FContentDisposition;

#pragma endregion
};

/// Now the internal engine stuff

/// Handle to some lengthy operation
class iAsyncResult
{
public:
	iAsyncResult(): FFinished( false ), FPaused( false ) {}
	virtual ~iAsyncResult() {}

	virtual bool Finished() { return FFinished; }
	virtual bool Paused()   { return FPaused;   }

	virtual void Pause( bool P ) { FPaused = P; }

public:
	bool FFinished;
	bool FPaused;
};

class HttpDownloadConn;

class sEnvironment;

/**
   \brief High-level interface for network operations - http file downloads

   Handles http download streams
*/
class scriptfinal clDownloadThread: public iThread
{
public:
	clDownloadThread() {}
	virtual ~clDownloadThread() {}

	scriptmethod size_t GetNumActiveDownloads() const { return FDownloads.size(); }

	/// Add download to process list and return its internal handle. Optional completion handler receives events about download.
	iAsyncResult* AddDownload( const LString& HostName, int Port, const LString& Path, const LString& OutFileName, iObject* CompletionHandler );

	/// Might be blocking, if the connection was not yet established
	scriptmethod void PollDownloads();

	/// Debug print download state
	scriptmethod void DumpDownloadState();

	scriptmethod void StopDownloads();

	scriptmethod void PauseDownloads();

	scriptmethod double GetCombinedDownloadSpeed() const;

	virtual void Init() {}
	virtual void Run();
	virtual void Finish() {}

private:
	std::list<HttpDownloadConn*> FDownloads;

	clMutex FAddMutex;
};

///////// Server part

/**
   \brief HTTP server thread

   Supports Remote console interface, Screenshot captures and access to shared files

   We do not solve the C10K problem here, so there is no
   event dispatching or request queues.

   We spawn a new thread for each incoming connection
*/
class scriptfinal clHTTPServerThread: public iThread
{
public:
	clHTTPServerThread(): FBindAddress( "127.0.0.1" ), FPort( 8080 ), FMaxConnections( 5 ) {}
	virtual ~clHTTPServerThread() {}

	virtual void Init() {}
	virtual void Run();
	virtual void Finish() {}

	/// Add an alias to the file in VFS
	scriptmethod void AddSharedFile( const LString& FileName, const LString& ServerPath );

	/// Remove all aliases for the FileName
	scriptmethod void RemoveSharedFile( const LString& FileName );

	scriptmethod LString ServerPathToFileName( const LString& ServerPath );

	scriptmethod LString FileNameToServerPath( const LString& FileName );

	scriptmethod bool IsSharedFile( const LString& ServerPath );

public:
	/// Maximum number of simultaneous connections
	int FMaxConnections;

	/// Address which the server is bound to
	LString FBindAddress;

	/// Port on which the server resides
	int FPort;

	/// Dictionary (ServerPath -> FileName)
	std::map<LString, LString> FSharedFiles;

	/// Socket accepting incoming connections
	LTCPSocket* FSocket;
};

#endif // HAPPYHTTP_H
