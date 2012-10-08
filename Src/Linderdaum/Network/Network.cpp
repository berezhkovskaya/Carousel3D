/**
 * \file Network.cpp
 * \brief Network and sockets library
 * \version 0.5.95
 * \date 16/06/2010
 * \author Viktor Latypov, 2009-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Network.h"

#include <errno.h>

#include <iostream>
using namespace std;

#ifdef OS_WINDOWS

#include <iphlpapi.h>

HMODULE IPHlpHandle = NULL;

typedef DWORD ( WINAPI* GetAdaptersInfo_func )( PIP_ADAPTER_INFO, PULONG );

GetAdaptersInfo_func pGetAdaptersInfo = NULL;

/// Link to IP helper library and get the ip enumerator function
void Net_ReloadIPHlp()
{
	if ( IPHlpHandle != NULL ) { return; }

	IPHlpHandle = LoadLibrary( "iphlpapi.dll" ); // std, but not very std...

	pGetAdaptersInfo = ( GetAdaptersInfo_func )GetProcAddress( IPHlpHandle, "GetAdaptersInfo" );
}

#endif

void CreateUDPSocketPair( LUDPSocket_Virtual** S1, LUDPSocket_Virtual** S2 )
{
	*S1 = new LUDPSocket_Virtual();
	*S2 = new LUDPSocket_Virtual();

	( *S1 )->FOthers.push_back( *S2 );
	( *S2 )->FOthers.push_back( *S1 );
}

bool LUDPSocket_Virtual::Open() { return true; }
bool LUDPSocket_Virtual::Close() { FOthers.clear(); return true; }

bool LUDPSocket_Virtual::Bind( const LString& sourceAddress, int port )
{
	FLocalAddress = sourceAddress;
	FLocalAddress.port = /*HostToNetPortNumber*/( port );

	return true;
}

bool LUDPSocket_Virtual::WriteTo( const LNetworkAddress& To, LPacket& Message )
{
	LPacket* P = new LPacket();
	P->ClonePacket( Message );

	int idx = FindOtherSocketByAddress( To );

	if ( idx > -1 )
	{
		FOthers[idx]->FAddresses.push( FLocalAddress );
		FOthers[idx]->FPackets.push( P );
	}

	return true;
}

bool LUDPSocket_Virtual::ReadFrom( LNetworkAddress& NetFrom, LPacket& Message )
{
	if ( FPackets.empty() ) { return false; } // "block"

	NetFrom = FAddresses.front();
	FAddresses.pop();

	LPacket* P = FPackets.front();
	Message.ClonePacket( *P );

	delete P;

	FPackets.pop();

	return true;
}

bool LUDPSocket_Virtual::Write( LPacket& Message ) { ( void )Message; return false; }
bool LUDPSocket_Virtual::Read( LPacket& Message ) { ( void )Message; return false; }


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _MSC_VER
#define SAFE_strcpy(dest, src) strcpy_s(dest, 128, src)
#else
#define SAFE_strcpy(dest, src) strcpy(dest, src)
#endif


#ifdef OS_POSIX

#include <memory.h>

#ifndef OS_ANDROID
#include <ifaddrs.h>
#endif

#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#define ULONG unsigned int

#define SOCKET_CLOSE_FUNCTION close
#define SOCKET_IOCTL_FUNCTION ioctl

// define our local names for functions
#define LNet_CLOSESOCKET close
#define LNet_IOCTLSOCKET ioctl
#define LNet_SETSOCKOPT setsockopt

#define LNet_SELECT  select
#define LNet_FDISSET FD_ISSET
#define LNet_BIND bind
#define LNet_SEND send
#define LNet_RECV recv
#define LNet_SENDTO sendto
#define LNet_RECVFROM recvfrom
#define LNet_SOCKET socket
#define LNet_ACCEPT accept
#define LNet_LISTEN listen
#define LNet_CONNECT connect

#define LNet_GETHOSTBYNAME gethostbyname
#define LNet_INET_ADDR inet_addr
#define LNet_HTONL htonl
#define LNet_HTONS htons
#define LNet_NTOHL ntohl
#define LNet_NTOHS ntohs

#endif

#ifdef OS_WINDOWS

// defing function prototypes for dynamic linking
#define INCL_WINSOCK_API_TYPEDEFS 1

#include <winsock2.h>

// declare some stuff

#define hack_IOC_VENDOR 0x18000000
#define hack_IOC_IN 0x80000000
#define hack_WSAIOW(x,y) (hack_IOC_IN|(x)|(y))
#define SIO_UDP_CONNRESET           hack_WSAIOW(hack_IOC_VENDOR,12)

// declare function pointers

LPFN_SELECT LNet_SELECT;

//__WSAFDIsSet

//WINAPI
typedef int ( WINAPI* LPFN__WSAFDIsSet )( SOCKET, fd_set* );
/*
#ifdef _WIN64
typedef int (*LPFN__WSAFDIsSet)(SOCKET, fd_set*);
#else
#endif
*/
#define LNet_FDISSET(fd, set) LNet__WSAFDIsSet((SOCKET)(fd),(fd_set *)(set))

LPFN__WSAFDIsSet LNet__WSAFDIsSet;

LPFN_BIND     LNet_BIND;
LPFN_SEND     LNet_SEND;
LPFN_RECV     LNet_RECV;
LPFN_SENDTO   LNet_SENDTO;
LPFN_RECVFROM LNet_RECVFROM;
LPFN_ACCEPT   LNet_ACCEPT;
LPFN_LISTEN   LNet_LISTEN;
LPFN_CONNECT  LNet_CONNECT;

LPFN_SOCKET   LNet_SOCKET;

LPFN_WSASTARTUP LNet_WSASTARTUP;
LPFN_WSACLEANUP LNet_WSACLEANUP;
LPFN_WSAIOCTL   LNet_WSAIOCTL;
LPFN_WSAGETLASTERROR LNet_WSAGETLASTERROR;

LPFN_SETSOCKOPT     LNet_SETSOCKOPT;
LPFN_GETHOSTBYNAME  LNet_GETHOSTBYNAME;

LPFN_IOCTLSOCKET    LNet_IOCTLSOCKET;
LPFN_CLOSESOCKET    LNet_CLOSESOCKET;

LPFN_HTONL LNet_HTONL;
LPFN_HTONS LNet_HTONS;
LPFN_NTOHL LNet_NTOHL;
LPFN_NTOHS LNet_NTOHS;

LPFN_INET_ADDR LNet_INET_ADDR;

// WinSock2 Library handle
HMODULE hWinSockDLL;

#endif

// address wrapper (LNetworkAddress)

int HostToNetPortNumber( int Port )
{
	return LNet_HTONS( ( u_short )Port );
}

int NetToHostPortNumber( int Port )
{
	return LNet_NTOHS( ( u_short )Port );
}

// identical for win32/linux
/*
localhost
192.246.40.70
192.246.40.70:28000
*/
bool LStringToSockaddr ( const char* s, struct sockaddr* sadr )
{
	struct hostent* h;
	char*  colon;
	char  copy[128];

	memset ( sadr, 0, sizeof( *sadr ) );
	( ( struct sockaddr_in* )sadr )->sin_family = AF_INET;

	( ( struct sockaddr_in* )sadr )->sin_port = 0;

#ifdef _MSC_VER
	SAFE_strcpy ( copy, s );
#else
	SAFE_strcpy ( copy, s );
#endif

	// strip off a trailing :port if present
	for ( colon = copy ; *colon ; colon++ )
		if ( *colon == ':' )
		{
			*colon = 0;
			( ( struct sockaddr_in* )sadr )->sin_port = LNet_HTONS( ( short )atoi( colon + 1 ) );
		}

	if ( copy[0] >= '0' && copy[0] <= '9' )
	{
		*( int* )&( ( struct sockaddr_in* )sadr )->sin_addr = LNet_INET_ADDR( copy );
	}
	else
	{
		h = LNet_GETHOSTBYNAME( copy );

		if ( ! ( h ) )
		{
			return 0;
		}

		*( int* )&( ( struct sockaddr_in* )sadr )->sin_addr = *( int* )h->h_addr_list[0];
	}

	return true;
}

void IPAndPortToSockAddr( const LString& addrStr, int port, struct sockaddr_in* address )
{
#ifdef OS_WINDOWS

	if ( addrStr.empty() || !_stricmp( addrStr.c_str(), "localhost" ) )
#endif
#ifdef OS_POSIX
		if ( addrStr.empty() || !strcasecmp( addrStr.c_str(), "localhost" ) )
#endif
		{
			address->sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			LStringToSockaddr ( addrStr.c_str(), ( struct sockaddr* )address );
		}

	if ( port == PORT_ANY )
	{
		address->sin_port = 0;
	}
	else
	{
		address->sin_port = LNet_HTONS( ( short )port );
	}

	address->sin_family = AF_INET;
}

bool LNetworkAddress::FromString ( const LString& s )
{
#ifdef OS_WINDOWS
	struct sockaddr sadr;
#endif
#ifdef OS_POSIX
	struct sockaddr_in sadr;
#endif

	if ( !strcmp ( s.c_str(), "localhost" ) )
	{
		memset ( ip, 0, sizeof( ip ) );
		port = 0;

		type = NA_LOOPBACK;
		return true;
	}

	if ( !LStringToSockaddr( s.c_str(), ( sockaddr* )&sadr ) )
	{
		return false;
	}

	FromSockadr( &sadr );

	return true;
}


void LNetworkAddress::ToSockadr( void* s ) const
{
	memset ( s, 0, sizeof( struct sockaddr_in ) );

	if ( type == NA_BROADCAST )
	{
		( ( struct sockaddr_in* )s )->sin_family = AF_INET;
		( ( struct sockaddr_in* )s )->sin_port = static_cast<u_short>( port );
		( ( struct sockaddr_in* )s )->sin_addr.s_addr = INADDR_BROADCAST;
	}
	else if ( type == NA_IP )
	{
		( ( struct sockaddr_in* )s )->sin_family = AF_INET;
		( ( struct sockaddr_in* )s )->sin_addr.s_addr = *( int* )&ip;
		( ( struct sockaddr_in* )s )->sin_port = static_cast<u_short>( port );
	}
}

void LNetworkAddress::FromSockadr( void* s )
{
	*( int* )&ip = ( ( struct sockaddr_in* )s )->sin_addr.s_addr;
	port = ( ( struct sockaddr_in* )s )->sin_port;
	// s->sa_family is always IP
	type = NA_IP;
}

#ifdef _MSC_VER
#define LNET_snprintf _snprintf_s
#else
#define LNET_snprintf snprintf
#endif

// convert to LString
LString LNetworkAddress::ToString() const
{
	// no static vars to avoid problems with DLLs
	char s[64];

	s[0] = '\0';

	if ( type == NA_LOOPBACK )
	{
		LNET_snprintf ( s, sizeof( s ), "loopback" );
	}
	else if ( type == NA_IP )
	{
		LNET_snprintf ( s, sizeof( s ), "%i.%i.%i.%i:%i", ip[0], ip[1], ip[2], ip[3], LNet_NTOHS( static_cast<u_short>( port ) ) );
	}

	return LString( s );
}

// compare base address, without port
bool LNetworkAddress::CompareBase( const LNetworkAddress& other ) const
{
	if ( other.type != type )
	{
		return false;
	}

	return ( ip[0] == other.ip[0] ) && ( ip[1] == other.ip[1] ) && ( ip[2] == other.ip[2] ) && ( ip[3] == other.ip[3] );
}

// compare complete address
bool LNetworkAddress::Compare( const LNetworkAddress& other ) const
{
	return ( CompareBase( other ) && ( other.port == port ) );
}


////////////////

#ifdef OS_WINDOWS
int Static_NetworkInitialized = -1;
void* Static_WinsockStartData;
#endif

void InitializeNetwork()
{
#ifdef OS_WINDOWS

	if ( Static_NetworkInitialized < 0 )
	{
		Static_NetworkInitialized = 1;
	}
	else
	{
		Static_NetworkInitialized++;
	}

	// 1. load dynamic library
	hWinSockDLL = LoadLibrary( "ws2_32.dll" );

	// 2. get function pointers

#define GET_SOCK_FUNC(Name, FuName) \
   LNet_##Name = (LPFN_##Name)GetProcAddress(hWinSockDLL, FuName);

	LNet__WSAFDIsSet = ( LPFN__WSAFDIsSet )GetProcAddress( hWinSockDLL, "__WSAFDIsSet" );
//	if()

	GET_SOCK_FUNC( HTONL, "htonl" )
	GET_SOCK_FUNC( HTONS, "htons" )
	GET_SOCK_FUNC( INET_ADDR, "inet_addr" )

	GET_SOCK_FUNC( SETSOCKOPT, "setsockopt" )
	GET_SOCK_FUNC( GETHOSTBYNAME, "gethostbyname" )

	GET_SOCK_FUNC( NTOHL, "ntohl" )
	GET_SOCK_FUNC( NTOHS, "ntohs" )

	GET_SOCK_FUNC( IOCTLSOCKET, "ioctlsocket" )
	GET_SOCK_FUNC( CLOSESOCKET, "closesocket" )

	GET_SOCK_FUNC( SOCKET,   "socket" )
	GET_SOCK_FUNC( CONNECT,  "connect" )
	GET_SOCK_FUNC( LISTEN,   "listen" )
	GET_SOCK_FUNC( ACCEPT,   "accept" )

	GET_SOCK_FUNC( BIND,     "bind" )

	GET_SOCK_FUNC( SELECT,   "select" )
	GET_SOCK_FUNC( RECV,     "recv" )
	GET_SOCK_FUNC( SEND,     "send" )
	GET_SOCK_FUNC( RECVFROM, "recvfrom" )
	GET_SOCK_FUNC( SENDTO,   "sendto" )

	GET_SOCK_FUNC( WSASTARTUP, "WSAStartup" )
	GET_SOCK_FUNC( WSACLEANUP, "WSACleanup" )
	GET_SOCK_FUNC( WSAIOCTL,   "WSAIoctl" )
	GET_SOCK_FUNC( WSAGETLASTERROR, "WSAGetLastError" )
#undef GET_SOCK_FUNC

	TODO( "handle invalid winsock function pointers" )

	// 3. initialize WinSock stuff
	Static_WinsockStartData = reinterpret_cast<void*>( new WSADATA() );

	// request version 2.0 - we need a workaround for async sockets in Win2000
	if ( LNet_WSASTARTUP( MAKEWORD( 2, 0 ), reinterpret_cast<WSADATA*>( Static_WinsockStartData ) ) != 0 )
	{
		MessageBoxW( 0, static_cast<LPCWSTR>( L"Can't load WinSock" ), static_cast<LPCWSTR>( L"Error" ), 0 );
		Static_NetworkInitialized++;
	}

#endif
}

void ShutdownNetwork()
{
#ifdef OS_WINDOWS
	Static_NetworkInitialized--;

	// deinitialize only when there are no more 'clients'
	if ( Static_NetworkInitialized == 0 )
	{
		LNet_WSACLEANUP();
//               delete reinterpret_cast<WSADATA*>(Static_WinsockStartData);

		// unload library
	}

#endif
}

LString LNetwork_LastErrorAsString()
{
	char errBuf[128];
#ifdef OS_POSIX
	// standart libc routine
	SAFE_strcpy( errBuf, strerror( errno ) );
#endif

#ifdef OS_WINDOWS
	//  Convert WinSock error code to ASCII LString
	int ErrorCode = LNet_WSAGETLASTERROR();

#define WSA_STRING(Err) case Err :  SAFE_strcpy(errBuf, #Err ); break;

	switch ( ErrorCode )
	{
			WSA_STRING( WSAEINTR )
			WSA_STRING( WSAEBADF )
			WSA_STRING( WSAEACCES )
			WSA_STRING( WSAEDISCON )
			WSA_STRING( WSAEFAULT )
			WSA_STRING( WSAEINVAL )
			WSA_STRING( WSAEMFILE )
			WSA_STRING( WSAEWOULDBLOCK )
			WSA_STRING( WSAEINPROGRESS )
			WSA_STRING( WSAEALREADY )
			WSA_STRING( WSAENOTSOCK )
			WSA_STRING( WSAEDESTADDRREQ )
			WSA_STRING( WSAEMSGSIZE )
			WSA_STRING( WSAEPROTOTYPE )
			WSA_STRING( WSAENOPROTOOPT )
			WSA_STRING( WSAEPROTONOSUPPORT )
			WSA_STRING( WSAESOCKTNOSUPPORT )
			WSA_STRING( WSAEOPNOTSUPP )
			WSA_STRING( WSAEPFNOSUPPORT )
			WSA_STRING( WSAEAFNOSUPPORT )
			WSA_STRING( WSAEADDRINUSE )
			WSA_STRING( WSAEADDRNOTAVAIL )
			WSA_STRING( WSAENETDOWN )
			WSA_STRING( WSAENETUNREACH )
			WSA_STRING( WSAENETRESET )
			WSA_STRING( WSAECONNABORTED )
			WSA_STRING( WSAECONNRESET )
			WSA_STRING( WSAENOBUFS )
			WSA_STRING( WSAEISCONN )
			WSA_STRING( WSAENOTCONN )
			WSA_STRING( WSAESHUTDOWN )
			WSA_STRING( WSAETOOMANYREFS )
			WSA_STRING( WSAETIMEDOUT )
			WSA_STRING( WSAECONNREFUSED )
			WSA_STRING( WSAELOOP )
			WSA_STRING( WSAENAMETOOLONG )
			WSA_STRING( WSAEHOSTDOWN )
			WSA_STRING( WSASYSNOTREADY )
			WSA_STRING( WSAVERNOTSUPPORTED )
			WSA_STRING( WSANOTINITIALISED )
			WSA_STRING( WSAHOST_NOT_FOUND )
			WSA_STRING( WSATRY_AGAIN )
			WSA_STRING( WSANO_RECOVERY )
			WSA_STRING( WSANO_DATA )
		default:
			SAFE_strcpy( errBuf, "NO ERROR" );
			break;
	}

#undef WSA_STRING

#endif

	return LString( errBuf );
}

//
// Socket wrapper
//

LSocket::LSocket()
{
	FSocket      = INVALID_SOCKET;
	FError       = false;
	FLastError   = "";
}

LSocket::~LSocket()
{
}

bool LSocket::CheckData( bool CheckRead, bool CheckWrite, bool CheckError, int msec, int musec )
{
	if ( FSocket == INVALID_SOCKET ) { return false; }

	fd_set ReadFDS, WriteFDS, ErrorFDS;

#ifdef _MSC_VER
// We cannot rewrite strange WinSock headers so we disable this
#pragma warning(disable:4127)
#endif

	if ( CheckRead ) { FD_ZERO( &ReadFDS ); FD_SET( FSocket, &ReadFDS ); }

	if ( CheckWrite ) { FD_ZERO( &WriteFDS ); FD_SET( FSocket, &WriteFDS ); }

	if ( CheckError ) { FD_ZERO( &ErrorFDS ); FD_SET( FSocket, &ErrorFDS ); }

	struct timeval Timeout;

	// sleep for 10ms (10000us)
	Timeout.tv_sec  = msec / 1000;

	Timeout.tv_usec = musec + ( msec % 1000 ) * 1000;

	int Res = LNet_SELECT( 1, CheckRead ? &ReadFDS : NULL, CheckWrite ? &WriteFDS : NULL, CheckError ? &ErrorFDS : NULL, &Timeout );

	if ( Res == -1 )
	{
		FError = true;
		FLastError = LNetwork_LastErrorAsString();
		return false;
	}

	bool Result = false;

	if ( CheckRead ) { Result |= ( LNet_FDISSET( FSocket, &ReadFDS )  != 0 ); }

	if ( CheckWrite ) { Result |= ( LNet_FDISSET( FSocket, &WriteFDS ) != 0 ); }

	if ( CheckError ) { Result |= ( LNet_FDISSET( FSocket, &ErrorFDS ) != 0 ); }

	return Result;
}

const char* LSocket::GetAddress() const
{
	return FAddress.c_str();
}

int LSocket::GetPort() const
{
	return FPort;
}

// returns NULL if no incoming connections and the socket is non-blocking
LTCPSocket* LTCPSocket::Accept()
{
	// accept connection

	struct  sockaddr_in sad;
	memset( ( char* )&sad, 0, sizeof( sad ) );

#ifdef _WIN32
	int alen = sizeof( sad ); //sockaddr_in*);
#else
/// POSIX
	socklen_t alen = static_cast<socklen_t>( sizeof( sad ) ); //sockaddr_in*);
#endif

	SOCKET NewSocket = LNet_ACCEPT( FSocket, ( struct sockaddr* )&sad, &alen );

	if ( NewSocket != INVALID_SOCKET )
	{
		LNetworkAddress RemoteAddr;
		RemoteAddr.FromSockadr( &sad );
		LString S = RemoteAddr.ToString();

		// create new socket
		LTCPSocket* Sock = new LTCPSocket();
		Sock->SetSocketAddress( S, RemoteAddr.port );
		Sock->SetSocketHandle( NewSocket );

		return Sock;
	}

	/*
	   cout << "newsock in accept : " << NewSocket << endl;

	   LString Err;
	   LNetwork_LastErrorAsString(Err);
	   cout << "Err = " << Err << endl;
	*/
	return NULL;
}

int LTCPSocket::Listen( int BackLog )
{
	return ( LNet_LISTEN( FSocket, BackLog ) == 0 );
}

int LTCPSocket::EnableAddressReuse()
{
	int optval = 1;

	int res = LNet_SETSOCKOPT( FSocket, SOL_SOCKET, SO_REUSEADDR, ( const char* )&optval , sizeof( int ) );

	return res;
}

bool LTCPSocket::Connect( const LString& addr, int port )
{
	struct  sockaddr_in sad;
	memset( ( char* )&sad, 0, sizeof( sad ) );

	IPAndPortToSockAddr( addr, port, &sad );

	int NewSocket = LNet_CONNECT( FSocket, ( struct sockaddr* )&sad, sizeof( sad ) );

	return ( NewSocket < 0 ) ? false : true;
}

int LTCPSocket::SetTCPDelay( bool delay )
{
#ifdef _WIN32
	//
	int bVal = delay ? 0 : 1;
	int bValSize = sizeof( int );
	return LNet_SETSOCKOPT( FSocket, IPPROTO_TCP, TCP_NODELAY, ( char* )&bVal, bValSize );
#endif
}

bool LUDPSocket::EnableBroadcast( bool bcast )
{
	int val = bcast ? 1 : 0;

	if ( LNet_SETSOCKOPT( FSocket, SOL_SOCKET, SO_BROADCAST, ( const char* )&val, sizeof( int ) ) == -1 )
	{
		FLastError = "setsockopt SO_BROADCAST: " + LNetwork_LastErrorAsString();
		FError = true;

		return false;
	}

	return true;
}

bool LSocket::Open()
{
	// create socket
	FSocket = LNet_SOCKET( AF_INET, GetSockType(), GetIPProtocol() );

	if ( FSocket == INVALID_SOCKET )
	{
		FLastError = "OpenSocket error : " + LNetwork_LastErrorAsString();
		FError = true;

		return false;
	}

#ifdef OS_WINDOWS

	if ( GetIPProtocol() != IPPROTO_UDP ) { return true; }

	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	DWORD status;

	// disable  new behavior using
	// IOCTL: SIO_UDP_CONNRESET
	status = LNet_WSAIOCTL( FSocket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof( bNewBehavior ), NULL, 0, &dwBytesReturned, NULL, NULL );

	if ( SOCKET_ERROR == status )
	{
		DWORD dwErr = LNet_WSAGETLASTERROR();

		if ( WSAEWOULDBLOCK != dwErr )
		{
			FLastError = LString( "WSAIoctl(SIO_UDP_CONNRESET) Error: " ) + LNetwork_LastErrorAsString();
			return false;
		}
	}

#endif

	return true;
}

bool LSocket::Bind( const LString& sourceAddress, int port )
{
	// 1. resolve address and port
	FAddress = sourceAddress;
	FPort    = port;

	struct sockaddr_in address;

	IPAndPortToSockAddr( sourceAddress, port, &address );

	// 2. bind socket to address
	if ( LNet_BIND ( FSocket, ( struct sockaddr* )/*(void *)*/&address, sizeof( address ) ) == -1 )
	{
		FLastError = LNetwork_LastErrorAsString();

		if ( FLastError != "NO ERROR" )
		{
			FLastError = "OpenSocket: bind: " + FLastError;
			FError = true;

			LNet_CLOSESOCKET( FSocket );

			return false;
		}
	}

	return true;
}

bool LSocket::SetNonBlocking( bool nonBlocking )
{
	ULONG _flag = nonBlocking ? 1 : 0;

	if ( LNet_IOCTLSOCKET( FSocket, FIONBIO, &_flag ) == -1 )
	{
		FLastError = "ioctl FIONBIO error : " + LNetwork_LastErrorAsString();
		FError = true;

		return false;
	}

	return true;
}

bool LSocket::Close()
{
	int ret = LNet_CLOSESOCKET( FSocket );

	if ( ret != -1 )
	{
		// some error
		return false;
	}

	return true;
}

bool LSocket::IsWouldBlock() const
{
#ifdef _WIN32
	int err = LNet_WSAGETLASTERROR();

	// wouldblock is silent
	if ( err == WSAEWOULDBLOCK ) { return true; }

#else
	int err = errno;

	if ( err == EWOULDBLOCK ) { return true; }
#endif

	return false;
}

bool LSocket::Write( LPacket& msg )
{
	int      ret;

	FError = false;

	if ( FSocket == INVALID_SOCKET )
	{
		return false;
	}

// cout << "Socket::Write, PacketSize = " << msg.FCurSize << endl;
	ret = LNet_SEND ( FSocket, ( char* )( msg.FData ), msg.FCurSize, 0 );

	if ( ret == -1 )
	{
		// wouldblock is silent
		if ( IsWouldBlock() ) { return false; }

		FLastError = "Socket::Write() : " + LNetwork_LastErrorAsString();
		FError = true;
	}

	return true;
}

int  LSocket::WriteBytes( const unsigned char* Buf, int num )
{
	return LNet_SEND ( FSocket, ( char* )Buf, num, 0 );
}

int  LSocket::ReadBytes( unsigned char* Buf, int MaxBytes )
{
	return LNet_RECV ( FSocket, ( char* )Buf, MaxBytes, 0 );
}

LString LSocket::ReceiveBytes( int MaxLen )
{
	LString Ret;

	static const int MAX_STRING_LEN = 1024;

	char buf[MAX_STRING_LEN];

	if ( MaxLen > MAX_STRING_LEN ) { MaxLen = MAX_STRING_LEN; }

	for ( ;; )
	{
		unsigned long NumAvail = 0;

		/// Check data availability
		if ( LNet_IOCTLSOCKET( FSocket, FIONREAD, &NumAvail ) != 0 ) { break; }

		if ( NumAvail == 0 ) { break; }

		if ( ( int )NumAvail > MaxLen ) { NumAvail = ( unsigned long )MaxLen; }

		int NumRead = LNet_RECV( FSocket, buf, NumAvail, 0 );

		if ( NumRead <= 0 ) { break; }

		LString T;

		T.assign ( buf, NumRead );

		Ret += T;
	}

	return Ret;
}

// TODO: timeout would be nice here
LString LSocket::ReceiveLine( int MaxLen )
{
	LString Ret;

	for ( int Len = 0 ; Len < MaxLen ; )
	{
		char R;

		switch ( LNet_RECV( FSocket, &R, 1, 0 ) )
		{
				/// Connection terminated
			case 0:
				return "";

				/// Blocking condition or something critical
			case -1:

				if ( ! IsWouldBlock() ) { return ( errno == EAGAIN ) ? Ret : ""; }
		}

		if ( !IsWouldBlock() )
		{
			Ret += R;

			if ( R == '\n' ) { return Ret; }

			Len++;
		}
	}

	return Ret;
}

bool LSocket::SendBytes( const LString& S )
{
	return ( LNet_SEND( FSocket, S.c_str(), ( int )S.length(), 0 ) != -1 );
}

bool LSocket::Read( LPacket& Message )
{
	int   ret;

	if ( FSocket ==  INVALID_SOCKET )
	{
//    printf("Socket is unassigned\n");
		return false;
	}

// printf("Before recv, msg_size = %d, msg_data = %d\n", Message.FMaxSize, reinterpret_cast<int>(Message.FData));

	ret = LNet_RECV ( FSocket, reinterpret_cast<char*>( Message.FData ), Message.FMaxSize , 0 );

// printf("After recv, ret = %d\n", ret);

	if ( ret == -1 )
	{
		// ECONNREFUSED too ?
		if ( IsWouldBlock() ) { return false; }

		// the error handling is done outside - packet loss is not fatal
		FError = true;
		FLastError = /*"Socket::Read() : " + */LNetwork_LastErrorAsString();

		return false;
	}

	/*
	   else
	   {
	      printf("ret is %d \n", ret);
	   }
	*/

	// non-zero messages only
	if ( ret > -1 )
	{
		if ( ret == Message.FMaxSize )
		{
//			printf ("Oversize packet\n");
			return true; //false;
		}

		Message.FCurSize = ret;
		return true;
	}

	FError = false;
	return false;
}

bool LSocket::WriteTo( const LNetworkAddress& to, LPacket& msg )
{
	int      ret;
#ifdef _WIN32
	struct sockaddr   addr;
#else
	// posix
	struct sockaddr_in   addr;
#endif
	FError = false;

	if ( FSocket ==  INVALID_SOCKET )
	{
		return false;
	}

	if ( to.type != NA_IP && to.type != NA_BROADCAST )
	{
		FLastError = "Socket::Write() : bad address type";
		FError = true;
		return false;
	}

	to.ToSockadr( &addr );

	ret = LNet_SENDTO ( FSocket, ( char* )( msg.FData ), msg.FCurSize, 0, ( struct sockaddr* )&addr, sizeof( addr ) );

	if ( ret == -1 )
	{
#ifdef _WIN32
		int err = LNet_WSAGETLASTERROR();

		// wouldblock is silent
		if ( err == WSAEWOULDBLOCK )
		{
			return false;
		}

		// some PPP links dont allow broadcasts
		if ( ( err == WSAEADDRNOTAVAIL ) && ( ( to.type == NA_BROADCAST ) ) )
		{
			return false;
		}

#endif
		FLastError = /*"Socket::Write() : " + */LNetwork_LastErrorAsString();
		FError = true;
	}
	else
	{
//    cout << "Bytes sent: " << ret << endl;
	}

	return true;
}

bool LSocket::ReadFrom( LNetworkAddress& NetFrom, LPacket& Message )
{
	int   ret;
#ifdef _WIN32
	struct sockaddr from;
#else
	// posix
	struct sockaddr_in   from;
#endif
	int      fromlen;

	if ( FSocket ==  INVALID_SOCKET )
	{
//    printf("Socket is unassigned\n");
		return false;
	}

	fromlen = sizeof( from );

	ret = LNet_RECVFROM ( FSocket, reinterpret_cast<char*>( Message.FData ), Message.FMaxSize , 0, ( struct sockaddr* )&from,
#ifdef _WIN32
	                      & fromlen );
#else
	reinterpret_cast<socklen_t*>( &fromlen ) );
#endif

	if ( ret == -1 )
{
	if ( IsWouldBlock() ) { return false; }

		// the error handling is done outside - packet loss is not fatal
		FError = true;
		FLastError = /*"Socket::Read() : " + */LNetwork_LastErrorAsString();

		return false;
	}
	/*
	   else
	   {
	      printf("ret is %d \n", ret);
	   }
	*/

	// non-zero messages only
	if ( ret > -1 )
{
	// fill the NetFrom address
	NetFrom.FromSockadr( &from );

		if ( ret == Message.FMaxSize )
		{
//			printf ("Oversize packet from %s\n", NetFrom.ToString().c_str());
			return false;
		}

		Message.FCurSize = ret;
		return true;
	}

	FError = false;
	return false;
}

int LUDPSocket::GetIPProtocol() { return IPPROTO_UDP; }
int LTCPSocket::GetIPProtocol() { return IPPROTO_TCP; }

int LUDPSocket::GetSockType() { return SOCK_DGRAM; }
int LTCPSocket::GetSockType() { return SOCK_STREAM;}

// Message interface : binary data manipulation

unsigned char LPacket::ReadByte()
{
	/*
	   assert(FReadCount < FMaxSize && FReadCount >= 0);
	   assert(FData != NULL);
	*/
	return FData[FReadCount++];
}

void LPacket::WriteByte( unsigned char b )
{
	/*
	   assert(FCurSize < FMaxSize && FCurSize >= 0);
	   assert(FData != NULL);
	*/
	FData[FCurSize++] = b;
}

void LPacket::WriteInt32( int c )
{
	unsigned char* buf = reinterpret_cast<unsigned char*>( FData ) + FCurSize;

	buf[0] = ( c       ) & 0xff;
	buf[1] = ( c >>  8 ) & 0xff;
	buf[2] = ( c >> 16 ) & 0xff;
	buf[3] = ( c >> 24 );

	FCurSize += 4;
}

int LPacket::ReadInt32()
{
	int   c;

	if ( FReadCount + 4 > FCurSize )
	{
		c = -1;
	}
	else
		c = FData[FReadCount + 0]
		    + ( FData[FReadCount + 1] <<  8 )
		    + ( FData[FReadCount + 2] << 16 )
		    + ( FData[FReadCount + 3] << 24 );

	FReadCount += 4;

	return c;
}

void LPacket::WriteString( const LString& str )
{
	for ( size_t i = 0 ; i != str.length() ; ++i )
	{
		WriteByte( str[i] );
	}

	// trailing zero
	WriteByte( 0 );
}

LString LPacket::ReadString()
{
	char buf[256];
	int l = 0;
	char c;

	do
	{
		c = ReadByte();

		if ( c == -1 || c == 0 )
		{
			buf[l] = 0;
			break;
		}

		buf[l] = c;
		l++;
	}
	while ( l < 255 );

	return LString( buf );
}

void LPacket::ReserveSpace( int size )
{
	FData = new unsigned char[size];
	FMaxSize = size;

	BeginWrite();
}

void LPacket::ReadBytes( unsigned char* Data, int Num )
{
	for ( int i = 0 ; i < Num ; i++ )
	{
		Data[i] = ReadByte();
	}
}

void LPacket::WriteBytes( unsigned char* Data, int Num )
{
	for ( int i = 0 ; i < Num ; i++ )
	{
		WriteByte( Data[i] );
	}
}

int LPacket::SaveToFile( const char* szFileName ) const
{
	( void )szFileName;
	/*
	   FILE* pFile = fopen( szFileName,
	#ifdef OS_WINDOWS
	                        "wb"
	#else
	                        "w"
	#endif
	                      );

	   fwrite( FData, sizeof( FData[0] ), FCurSize, pFile );
	   fclose( pFile );
	*/
	return 1;
}

int LPacket::LoadFromFile( const char* szFileName )
{
	( void )szFileName;
	/*
	   FILE* f = fopen( szFileName,
	#ifdef OS_WINDOWS
	                    "rb"
	#else
	                    "r"
	#endif
	                  );

	   if ( f == NULL )
	   {
	      return 0;
	   }

	   fseek( f, 0, SEEK_END );
	   int _sz = ftell( f );
	   fseek( f, 0, SEEK_SET );

	   if ( _sz <= 0 )
	   {
	      fclose( f );
	      return 0;
	   }

	   if ( FData != NULL )
	   {
	      delete[] FData;
	   }

	   FData = new unsigned char[_sz];

	   if ( fread( FData, 1, _sz, f ) < 0 )
	   {
	      fclose( f );
	      return 0;
	   }

	   FMaxSize = FCurSize = _sz;
	   FReadCount = 0;

	   fclose( f );
	*/
	return 1;
}

void LPacket::ClearPacket()
{
	if ( FData != NULL )
	{
		delete [] FData;
		FData = NULL;
	}

	FMaxSize   = 0;
	FCurSize   = 0;
	FReadCount = 0;
	FProcessed = false;
}

void LPacket::ClonePacket( const LPacket& src )
{
	ClearPacket();

	FReadCount = 0;
	FTime = src.FTime;

	FCloned = true;

	// copy contents
	FProcessed = src.FProcessed;
	FCurSize = src.FCurSize;
	FMaxSize = src.FMaxSize;
	FData = new unsigned char[src.FMaxSize];
	FOwnsData = true;
	memcpy( FData, src.FData, src.FCurSize );
}

LPacket* LPacket::Clone() const
{
	LPacket* New = new LPacket();
	New->ClonePacket( *this );
	return New;
}

///// Service functions

#ifdef OS_ANDROID

/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <new>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <net/if.h>

// Android (bionic) doesn't have getifaddrs(3)/freeifaddrs(3).
// We fake it here, so java_net_NetworkInterface.cpp can use that API
// with all the non-portable code being in this file.

// Source-compatible subset of the BSD struct.
struct ifaddrs
{
	// Pointer to next struct in list, or NULL at end.
	ifaddrs* ifa_next;

	// Interface name.
	char* ifa_name;

	// Interface flags.
	unsigned int ifa_flags;

	// Interface address.
	sockaddr* ifa_addr;

	ifaddrs( ifaddrs* next )
		: ifa_next( next ), ifa_name( NULL ), ifa_flags( 0 ), ifa_addr( NULL )
	{
	}

	~ifaddrs()
	{
		delete ifa_next;
		delete[] ifa_name;
		delete ifa_addr;
	}

	// Sadly, we can't keep the interface index for portability with BSD.
	// We'll have to keep the name instead, and re-query the index when
	// we need it later.
	bool setNameAndFlagsByIndex( int interfaceIndex )
	{
		// Get the name.
		char buf[IFNAMSIZ];
		char* name = if_indextoname( interfaceIndex, buf );

		if ( name == NULL )
		{
			return false;
		}

		ifa_name = new char[strlen( name ) + 1];
		strcpy( ifa_name, name );

		// Get the flags.
		int fd = socket( AF_INET, SOCK_DGRAM, 0 );

		if ( fd == -1 )
		{
			return false;
		}

		ifreq ifr;
		memset( &ifr, 0, sizeof( ifr ) );
		strcpy( ifr.ifr_name, name );
		int rc = ioctl( fd, SIOCGIFFLAGS, &ifr );

		if ( rc == -1 )
		{
			close( fd );
			return false;
		}

		ifa_flags = ifr.ifr_flags;
		close( fd );
		return true;
	}

	// Netlink gives us the address family in the header, and the
	// sockaddr_in or sockaddr_in6 bytes as the payload. We need to
	// stitch the two bits together into the sockaddr that's part of
	// our portable interface.
	void setAddress( int family, void* data, size_t byteCount )
	{
		sockaddr_storage* ss = new sockaddr_storage;
		ss->ss_family = family;

		if ( family == AF_INET )
		{
			void* dst = &reinterpret_cast<sockaddr_in*>( ss )->sin_addr;
			memcpy( dst, data, byteCount );
		}
		else if ( family == AF_INET6 )
		{
			void* dst = &reinterpret_cast<sockaddr_in6*>( ss )->sin6_addr;
			memcpy( dst, data, byteCount );
		}

		ifa_addr = reinterpret_cast<sockaddr*>( ss );
	}
};

// FIXME: use iovec instead.
struct addrReq_struct
{
	nlmsghdr netlinkHeader;
	ifaddrmsg msg;
};

inline bool sendNetlinkMessage( int fd, const void* data, size_t byteCount )
{
	ssize_t sentByteCount = TEMP_FAILURE_RETRY( send( fd, data, byteCount, 0 ) );
	return ( sentByteCount == static_cast<ssize_t>( byteCount ) );
}

inline ssize_t recvNetlinkMessage( int fd, char* buf, size_t byteCount )
{
	return TEMP_FAILURE_RETRY( recv( fd, buf, byteCount, 0 ) );
}

// Source-compatible with the BSD function.
inline int getifaddrs( ifaddrs** result )
{
	// Simplify cleanup for callers.
	*result = NULL;

	// Create a netlink socket.
	int fd = socket( PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE );

	if ( fd < 0 )
	{
		return -1;
	}

	// Ask for the address information.
	addrReq_struct addrRequest;
	memset( &addrRequest, 0, sizeof( addrRequest ) );
	addrRequest.netlinkHeader.nlmsg_flags = NLM_F_REQUEST | NLM_F_MATCH;
	addrRequest.netlinkHeader.nlmsg_type = RTM_GETADDR;
	addrRequest.netlinkHeader.nlmsg_len = NLMSG_ALIGN( NLMSG_LENGTH( sizeof( addrRequest ) ) );
	addrRequest.msg.ifa_family = AF_UNSPEC; // All families.
	addrRequest.msg.ifa_index = 0; // All interfaces.

	if ( !sendNetlinkMessage( fd, &addrRequest, addrRequest.netlinkHeader.nlmsg_len ) )
	{
		close( fd );
		return -1;
	}

	// Read the responses.
//    LocalArray<0> buf(65536); // We don't necessarily have std::vector.
	const int buf_size = 65536;

	char* buf = ( char* )alloca( buf_size );

	ssize_t bytesRead;

	while ( ( bytesRead  = recvNetlinkMessage( fd, &buf[0], buf_size ) ) > 0 )
	{
		nlmsghdr* hdr = reinterpret_cast<nlmsghdr*>( &buf[0] );

		for ( ; NLMSG_OK( hdr, bytesRead ); hdr = NLMSG_NEXT( hdr, bytesRead ) )
		{
			switch ( hdr->nlmsg_type )
			{
				case NLMSG_DONE:
					close( fd );
					return 0;
				case NLMSG_ERROR:
					close( fd );
					return -1;
				case RTM_NEWADDR:
				{
					ifaddrmsg* address = reinterpret_cast<ifaddrmsg*>( NLMSG_DATA( hdr ) );
					rtattr* rta = IFA_RTA( address );
					size_t ifaPayloadLength = IFA_PAYLOAD( hdr );

					while ( RTA_OK( rta, ifaPayloadLength ) )
					{
						if ( rta->rta_type == IFA_LOCAL )
						{
							int family = address->ifa_family;

							if ( family == AF_INET || family == AF_INET6 )
							{
								*result = new ifaddrs( *result );

								if ( !( *result )->setNameAndFlagsByIndex( address->ifa_index ) )
								{
									close( fd );
									return -1;
								}

								( *result )->setAddress( family, RTA_DATA( rta ), RTA_PAYLOAD( rta ) );
							}
						}

						rta = RTA_NEXT( rta, ifaPayloadLength );
					}
				}
				break;
			}
		}
	}

	close( fd );
	// We only get here if recv fails before we see a NLMSG_DONE.
	return -1;
}

// Source-compatible with the BSD function.
inline void freeifaddrs( ifaddrs* addresses ) { delete addresses; }

#endif  // end of crazy android stuff


bool Net_EnumerateAdapters( std::vector<sAdapterInfo>& Adapters )
{
// use GetAdapterInfo()  - iphlpapi.dll
#ifdef OS_WINDOWS
	Net_ReloadIPHlp();

	/* Declare and initialize variables */
	PIP_ADAPTER_INFO pAdapterInfo, pAdapter = NULL;
	DWORD dwRetVal = 0;

	ULONG ulOutBufLen = sizeof( IP_ADAPTER_INFO );
	pAdapterInfo = ( IP_ADAPTER_INFO* ) malloc( ulOutBufLen );

	if ( pAdapterInfo == NULL ) { printf( "Error in malloc/GetAdaptersinfo\n" ); return 1; }

	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if ( pGetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW )
	{
		free( pAdapterInfo );
		pAdapterInfo = ( IP_ADAPTER_INFO* ) malloc( ulOutBufLen );

		if ( pAdapterInfo == NULL ) { printf( "Error in malloc/GetAdaptersinfo(2)\n" ); return 1; }
	}

	dwRetVal = pGetAdaptersInfo( pAdapterInfo, &ulOutBufLen );

	if ( dwRetVal == NO_ERROR )
	{
		pAdapter = pAdapterInfo;

		while ( pAdapter )
		{
			sAdapterInfo Info;
			Info.FName = LStringBuffer( pAdapter->Description );
			Info.FID   = LStringBuffer( pAdapter->AdapterName );
			Info.FIP   = LStringBuffer( pAdapter->IpAddressList.IpAddress.String );

			Adapters.push_back( Info );

			pAdapter = pAdapter->Next;
		}
	}
	else
	{
		/* FLastError = "GetAdaptersInfo failed with error: %d\n", dwRetVal */;
		return false;
	}

	if ( pAdapterInfo ) { free( pAdapterInfo ); }

#else
	// universal POSIX getifaddrs() - linux/android, macos
	struct ifaddrs* MyAddrs, *ifa;
	void* in_addr;
	char buf[64];

	if ( getifaddrs( &MyAddrs ) != 0 ) { /* FLastError = "getifaddrs() failed"; */ return false; }

	int Idx = 0;

	for ( ifa = MyAddrs; ifa != NULL; ifa = ifa->ifa_next )
	{
		if ( ( ifa->ifa_addr == NULL ) || !( ifa->ifa_flags & IFF_UP ) ) { continue; }

		switch ( ifa->ifa_addr->sa_family )
		{
			case AF_INET:
			{ in_addr = &( ( struct sockaddr_in* )ifa->ifa_addr )->sin_addr;   break; }
			case AF_INET6:
			{ in_addr = &( ( struct sockaddr_in6* )ifa->ifa_addr )->sin6_addr; break; }
			default:
				continue;
		}

		if ( inet_ntop( ifa->ifa_addr->sa_family, in_addr, buf, sizeof( buf ) ) )
		{
			sAdapterInfo Info;
			Info.FName = LStringBuffer( ifa->ifa_name );
			Info.FIP   = LStringBuffer( buf );
			Info.FID   = LStringBuffer( LStr::ToStr( Idx ) );
			Adapters.push_back( Info );
			Idx++;
		}
	}

	freeifaddrs( MyAddrs );
#endif

	return true;
}


/*
 * 11/08/2010
     Dynamic linking with WinSock dll
 * 16/06/2010
     Linux port
     UDP/TCP sockets
*/
