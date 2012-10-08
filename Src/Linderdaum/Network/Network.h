/**
 * \file Network.h
 * \brief Network and sockets library
 * \version 0.5.95
 * \date 16/06/2010
 * \author Viktor Latypov, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __LNetwork__h__included__
#define __LNetwork__h__included__

#undef DO_NOT_USE_ENGINE

#ifdef DO_NOT_USE_ENGINE

#include <string>
typedef std::string LString;

#define scriptmethod
#define netexportable

#define OS_WINDOWS
#define PLATFORM_GCC
#define GCC_PACK(X)
#define TODO(X)

class iObject {};

#else

#include "Core/iObject.h"

#endif

#define PORT_ANY -1

class LSocket;
class LPacket;

#define NA_LOOPBACK   0
#define NA_IP         1
#define NA_BROADCAST  2

#if defined(OS_WINDOWS)
#if defined(PLATFORM_GCC)
// defing function prototypes for dynamic linking
#define INCL_WINSOCK_API_TYPEDEFS 1

#include <winsock2.h>
#else
typedef size_t SOCKET;
#endif
#endif

#if defined(OS_POSIX)
typedef int SOCKET;
#endif

#define INVALID_SOCKET (SOCKET)(~0)

#include <queue>

/// Short structure with the information about available network adapters
struct sAdapterInfo
{
	/// Internal name of the adapter
	LStringBuffer FName;

	/// Network address (ipv4 or v6)
	LStringBuffer FIP; // v4 or v6

	/// UID of the adapter (just a number in linux, GUID in win32)
	LStringBuffer FID;
};

/// Standalone info function
bool Net_EnumerateAdapters( std::vector<sAdapterInfo>& Adapters );

/**
   IP Network Address wrapper
   Provides LString conversion and comparison routines
**/
struct GCC_PACK( 1 ) LNetworkAddress
{
   public:
   int type;
   unsigned char ip[4]; // ipv4. should be no problems with ipv6 ?
   int port;  // -1 for PORT_ANY
   public:
   LNetworkAddress()
{
	for ( int i = 0 ; i < 4 ; i++ )
	{
		ip[i] = 0;
	}

	port = -1;
	type = NA_LOOPBACK;
}

LNetworkAddress( const LString& S )
{
	FromString( S );
}

LNetworkAddress( const LNetworkAddress& Other )
{
	Assign( Other );
}

LNetworkAddress& operator = ( const LNetworkAddress& Other )
{
	Assign( Other );
	return *this;
}

void Assign( const LNetworkAddress& Other )
{
	for ( int i = 0 ; i < 4 ; i++ )
	{
		ip[i] = Other.ip[i];
	}

	type = Other.type;
	port = Other.port;
}

// convert from LString
bool FromString( const LString& str );
// convert to LString

LString ToString() const;

// compare base address, without port
bool CompareBase( const LNetworkAddress& other ) const;
// compare complete address
bool Compare( const LNetworkAddress& other ) const;

// from sockaddr_in (wsock/BSDSockets)
void FromSockadr( void* s );
// to sockaddr_in (wsock/BSDSockets)
void ToSockadr( void* s ) const;
};

int HostToNetPortNumber( int Port );
int NetToHostPortNumber( int Port );

/**
   Initialization of WinSock (for Win32)

   Twwo static variables are OK here. Even with multiple environments there will be no errors
**/

#ifdef OS_WINDOWS
extern int Static_NetworkInitialized; // counter of initializations
extern void* Static_WinsockStartData;
#endif

void InitializeNetwork();
void ShutdownNetwork();

LString LNetwork_LastErrorAsString();

class scriptfinal LSocket: public iObject
{
public:
	LSocket();
	virtual ~LSocket();

	virtual bool Open();
	virtual bool Close();

	/// Check for data availability using select() call
	virtual bool CheckData( bool Read, bool Write, bool Error, int msec, int musec );

	virtual bool Bind( const LString& sourceAddress, int port );

	/// Receive a long string with possible internal EOLNs
	LString ReceiveBytes( int MaxLen );

	/// Receive a string terminated by Newline marker
	LString ReceiveLine( int MaxLen );

	/// Send a line terminated by NewLine marker. Used in HTTP-like protocols
	scriptmethod void SendLine( const LString& s ) { SendBytes( s + LString( "\n" ) ); }

	/// Send a long string
	scriptmethod bool SendBytes( const LString& S );

	virtual int  WriteBytes( const unsigned char* Buf, int num );
	virtual int  ReadBytes( unsigned char* Buf, int MaxBytes );

	virtual bool Write( LPacket& Message );
	virtual bool Read( LPacket& Message );

	virtual bool WriteTo( const LNetworkAddress& To, LPacket& Message );
	virtual bool ReadFrom( LNetworkAddress& NetFrom, LPacket& Message );

	virtual const char* GetAddress() const;
	virtual int GetPort() const;

	// were there any fatal errors in Read/Write ?
	virtual bool IsError() const
	{
		return FError;
	}
	virtual LString GetError() const
	{
		return FLastError;
	}

	virtual bool IsOpened() const
	{
		return ( FSocket != INVALID_SOCKET );
	}

	virtual bool SetNonBlocking( bool nonBlocking );

	virtual bool IsWouldBlock() const;

	// for TCP accept() and connect()
	void SetSocketHandle( SOCKET Sock )
	{
		FSocket = Sock;
	}
	void SetSocketAddress( const LString& addr, int port )
	{
		FAddress = addr;
		FPort = port;
	}

public:
	SOCKET    FSocket;
	int       FPort;
	LString   FAddress;
	bool      FError;
	LString   FLastError;
protected:
	virtual int GetIPProtocol() = 0;
	virtual int GetSockType() = 0;
};

class scriptfinal LUDPSocket : public LSocket
{
public:
	LUDPSocket() {}
	virtual ~LUDPSocket() {}

	bool EnableBroadcast( bool bcast );

protected:
	virtual int GetIPProtocol();
	virtual int GetSockType();
};

class LUDPSocket_Virtual;

class scriptfinal LUDPSocket_Virtual : public LUDPSocket
{
public:
	LUDPSocket_Virtual() {}
	virtual ~LUDPSocket_Virtual() {}

	virtual bool Open();
	virtual bool Close();

	virtual bool Bind( const LString& sourceAddress, int port );

	virtual bool Write( LPacket& Message );
	virtual bool Read( LPacket& Message );

	virtual bool WriteTo( const LNetworkAddress& To, LPacket& Message );
	virtual bool ReadFrom( LNetworkAddress& NetFrom, LPacket& Message );

	virtual bool IsOpened() const { return !FOthers.empty(); }

	virtual bool SetNonBlocking( bool nonBlocking ) { ( void )nonBlocking; return true; }

	virtual bool IsWouldBlock() const { return FPackets.empty(); }

	virtual bool IsError() const { return false; }

	/// Link to other virtual sockets
	std::vector<LUDPSocket_Virtual*> FOthers;

	int FindOtherSocketByAddress( const LNetworkAddress& Addr ) const
	{
		for ( size_t j = 0 ; j < FOthers.size() ; j++ )
			if ( FOthers[j]->FLocalAddress.Compare( Addr ) )
			{
				return ( int )j;
			}

		return -1;
	}

	/// This address (set up in Bind)
	LNetworkAddress FLocalAddress;

	/// Packet queue
	std::queue<LPacket*> FPackets;

	/// Address queue
	std::queue<LNetworkAddress> FAddresses;
};

void CreateUDPSocketPair( LUDPSocket_Virtual** S1, LUDPSocket_Virtual** S2 );

// EAGAIN or EWOULDBLOCK

class scriptfinal LTCPSocket: public LSocket
{
public:
	LTCPSocket() : LSocket() {}

	virtual ~LTCPSocket() {}

	// returns NULL if no incoming connections and the socket is non-blocking
	virtual LTCPSocket* Accept();
	virtual int Listen( int BackLog );

	virtual bool Connect( const LString& addr, int port );

	int SetTCPDelay( bool delay );

	int EnableAddressReuse();
protected:
	virtual int GetIPProtocol();
	virtual int GetSockType();
};

/**
   \brief Small class for network buffer management
*/
class LPacket
{
public:
	bool allowoverflow;
	bool overflow;

	/// Internal flag for the packet
	bool FProcessed;

	/// This packet was cloned
	bool FCloned;

	// true, if the packet manages its own data
	bool FOwnsData;

	/// Pointer to actual data. Can be allocated or managed (FOwnsData flag)
	unsigned char* FData;

	/// Maximum size (allocated or supplied data size)
	int FMaxSize;

	/// Current size (num bytes written)
	int FCurSize;

	/// Current number of bytes read
	int FReadCount;

	/// The time that packet was sent or received
	double FTime;

public:
	/// Default constructor assigns NULL data
	LPacket(): FData( NULL ), FMaxSize( 0 ), FCurSize( 0 ), FProcessed( false ), FCloned( false ) {}

	/// Externally managed packet buffers are created here
	LPacket( unsigned char* buf, int maxlen )
	{
		FCloned = false;
		FOwnsData = false;
		FData = buf;
		FMaxSize = maxlen;

		FProcessed = false;
	}

	/// Complete packet cloning
	LPacket( const LPacket& src )
	{
		ClonePacket( src );
	}

	/// Packet assignment (equivalent to Cloning)
	LPacket& operator = ( const LPacket& src )
	{
		ClonePacket( src );
		return *this;
	}

	~LPacket() { ClearPacket(); }

	/// Deallocate all data if the packet manages FData
	void ClearPacket();

	/// Complete copy of the packet and its state
	scriptmethod void ClonePacket( const LPacket& src );

	/// Complete copy of this packet
	scriptmethod LPacket* Clone() const;

	scriptmethod void BeginWrite() { FCurSize   = 0; }
	scriptmethod void BeginRead()  { FReadCount = 0; }

	scriptmethod void SkipReadBytes( int num )  { FReadCount += num; }
	scriptmethod void SkipWriteBytes( int num ) { FCurSize   += num; }

	/// Allocate 'size' bytes for packet and store this size in FMaxSize
	scriptmethod void ReserveSpace( int size );

	// read/write functions
	unsigned char ReadByte();
	void WriteByte( unsigned char b );

	void ReadBytes( unsigned char* Data, int Num );
	void WriteBytes( unsigned char* Data, int Num );

	TODO( "system-independent int types" )
	void WriteInt32( int val );
	int ReadInt32();

	/// Write zero-terminated string to packet
	void WriteString( const LString& str );

	/// Read zero-terminated string from packet
	LString ReadString();

	/// Debug file saving
	int SaveToFile( const char* szFileName ) const;

	/// Debug file loading
	int LoadFromFile( const char* szFileName );
};


#endif // if defined NETWORK_H_INCLUDED

/*
 * 10/09/2010
     SOCKET type is used for correct 64-bit porting
 * 16/06/2010
     Linux port
     UDP/TCP sockets
 * 06/04/2009
     Initial implementation
*/
