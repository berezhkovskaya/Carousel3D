/**
 * \file FileSystem.cpp
 * \brief Virtual file system
 * \version 0.6.06
 * \date 04/12/2012
 * \author Sergey Kosarevsky, 2004-2012
 * \author Latypov Viktor, 2007-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Platform.h"
#include "Environment.h"
#include "FileSystem.h"

#include "Files.h"

#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/VFS/MountPoint.h"

#include "Core/VFS/Archive.h"

#include "Network/HTTP.h"

#include "Utils/LJNI.h"

#include "Utils/LBlob.h"

#ifdef OS_WINDOWS

#  include <direct.h>
#  include <windows.h>
#  include <Shellapi.h>

#else  // any POSIX

#  include <time.h>
#  include <dirent.h>
#  include <libgen.h>
#  include <stdlib.h>

// for file watching
#  include <errno.h>
#  include <sys/types.h>
#  include <sys/inotify.h>

#endif

#include <sys/stat.h>

#include "Utils/Thread.h"
#include "Utils/Mutex.h"

#ifdef L_VFS_USE_PUGIXML
#  include "pugixml.hpp"
#endif

#include "ML.h"

inline LString Arch_FixFileName( const LString& VirtualName )
{
	LString Name( VirtualName );
	LStr::ReplaceAll( &Name, '/', PATH_SEPARATOR );
	LStr::ReplaceAll( &Name, '\\', PATH_SEPARATOR );

	LStr::ToUpper( &Name );

	return Name;
}

/// Implementation of a mount point for the uncompressed .RAR/.TAR files
class scriptfinal clArchiveMP: public iMountPoint
{
public:
	clArchiveMP( clArchiveReader* R ): FArchiveReader( R )
	{
		this->Env = R->Env;
		R->FCompletionFlag = &FLoaded;
		FLoaded = false;
	}

	virtual ~clArchiveMP() { delete( FArchiveReader ); }
	//
	// clMountPoint
	//
	virtual iRAWFile*    CreateRAWFile( const LString& VirtualName ) const
	{
//		WaitLoad();

		LString FName = Arch_FixFileName( VirtualName );

		clMemRAWFile* File = new clMemRAWFile();
		File->Env = Env;
		File->AfterConstruction();

		File->SetFileName( VirtualName );
		File->SetVirtualFileName( VirtualName );

		const void* DataPtr = FArchiveReader->GetFileData( FName );
		Luint64 FileSize = FArchiveReader->GetFileSize( FName );

		File->CreateFromManagedBuffer( DataPtr, FileSize );

		Env->Logger->Log( L_PARANOID, "Accessing " + VirtualName + " in " + FArchiveReader->GetSourceFile()->GetFileName() );

		return File;
	}

	virtual bool         FileExistsAtThisPoint( const LString& VirtualName ) const
	{
//		WaitLoad();
		return FArchiveReader->FileExists( Arch_FixFileName( VirtualName ) );
	}

	virtual Lint64       GetFileTime( const LString& VirtualName ) const
	{
//		WaitLoad();
		return FArchiveReader->GetFileTime( Arch_FixFileName( VirtualName ) );
	}

	virtual LString      MapToThisPoint( const LString& VirtualName ) const { return VirtualName; }
	virtual bool         IsPhysical() const { return false; };
private:
	clArchiveReader* FArchiveReader;
};

/**
   \brief Log output stream (text only)

   If someone wants to dump something into the engine.log this is the way to go
*/
class clLogStream : public iOStream
{
public:
	clLogStream() {};
	virtual ~clLogStream() {};

	// iOStream
	virtual LString GetFileName() const { return "ENGINE_LOG"; }
	virtual void    Seek( const Luint64 Position ) {}
	virtual void    BlockWrite( const void* Buf, const Luint64 Size ) {}
	virtual Luint64 GetFilePos() const { return 0; }

	/// Write a single line to log
	virtual void    WriteLine( const LString& Line )
	{
		Env->Logger->Log( L_LOG, Line );
	}

	virtual void    WriteLineLength( const LString& Line ) {}
	virtual void    WriteInt_Binary( int Int ) {}
};

clFileWatchHandle::clFileWatchHandle()
	: FFileName( "" ),
	  FTryLoopRunning( false )
#ifdef OS_WINDOWS
	  ,
	  FFileTime( 0 ),
	  FDirHandle( 0 )
#endif
{
}

#ifdef OS_WINDOWS

Luint64 _FileTime( const LString& Name )
{
	struct _stat Buf;
	/* int Result = */
	_stat( Name.c_str(), &Buf );
	return Buf.st_mtime;
}

class clFileWatchThread: public iThread
{
public:
	clFileWatchThread(): FSystem( NULL ), FWaitInterval( 100 ) {}
	virtual ~clFileWatchThread() {}

	void AddDirWatch( const LString& DirName, bool Recursive );
	void RemoveDirWatch( const LString& DirName );
	void AddFileWatch( const LString& fname, iObject* Handler );
	void RemoveFileWatch( const LString& fname );

	clFileSystem* FSystem;

public:
	virtual void Init() {}
	virtual void Run();
	virtual void Finish() {}

public:
	/// List of observed items
	std::vector<HANDLE>  FDirHandles;
	std::vector<LString> FDirNames;

	/// List of watched item names
	std::vector<clFileWatchHandle*> FWatchHandles;

	/// Number of milliseconds to wait for changes in each cycle
	DWORD FWaitInterval;

private:
	clMutex FAddMutex;
	clMutex FAccessMutex;

	/// Inbound list of added handles. Used internally. Protected by FAddMutex
	std::vector<clFileWatchHandle*> FFileHandlesToAdd;
	std::vector<HANDLE> FDirHandlesToAdd;
	std::vector<LString> FDirNamesToAdd;

	int DirHandleForFile( const LString& fname );

	void AddNewWatches();
	void HandleChanges( DWORD WaitStatus );
	void CloseHandles();

	/// List of active running file availability checkers
	std::map<clFileWatchHandle*, HANDLE> FInformThreads;
};

int clFileWatchThread::DirHandleForFile( const LString& fname )
{
//	AddNewWatches();

	LString Drive, Dir, File, Ext;

	clFileSystem::SplitPath( fname, &Drive, &Dir, &File, &Ext );

	/// Add drive letter and string the last slash in DirName
	// This is Win32, so the Drive is present
	Dir = Drive + Dir.substr( 0, Dir.length() - 1 );

//	printf("Directory for file [%s] is [%s]\n", fname.c_str(), DirName.c_str());

	const int num_tries = 3;

	for ( int i = 0 ; i < num_tries ; i++ )
	{
		{
			LMutex Lock( &FAddMutex );

			for ( size_t j = 0 ; j < FDirNames.size() ; j++ )
			{

				if ( Dir.length() > FDirNames[j].length() )
				{
					/// Check if the beginning of DirWatch path coincides with the Dir
					if ( Dir.substr( 0, FDirNames[j].length() ) == FDirNames[j] )
					{
						return ( int )j;
					}
				}
				else
				{
					// exact match
					if ( FDirNames[j] == Dir ) { return ( int )j; }
				}
			}
		}

		AddDirWatch( Dir, false );
		AddNewWatches();
	}

	// no such directory
	return -1;
}

void clFileWatchThread::AddFileWatch( const LString& fname, iObject* Handler )
{
	AddNewWatches();

	int idx = DirHandleForFile( fname );

	LMutex Lock( &FAddMutex );

	if ( idx > -1 )
	{
		clFileWatchHandle* Watch = new clFileWatchHandle();
		Watch->Env = Env;
		Watch->AfterConstruction();
		Watch->FFileName = fname;
		Watch->FHandler = Handler;

		Watch->FDirHandle = FDirHandles[idx];
		Watch->FFileTime = _FileTime( fname );
		FWatchHandles.push_back( Watch );
	}
}

void clFileWatchThread::RemoveFileWatch( const LString& fname )
{
	LMutex Lock( &FAddMutex );

	int idx = -1;

	for ( size_t i = 0 ; i < FWatchHandles.size() ; i++ )
	{
		if ( FWatchHandles[i]->FFileName == fname ) { idx = ( int )i; break; }
	}

	if ( idx > -1 )
	{
		delete FWatchHandles[idx];
		FWatchHandles.erase( FWatchHandles.begin() + idx );
	}
}

/// Add a watch handle to the list. Thread-safe
void clFileWatchThread::AddDirWatch( const LString& DirName, bool Recursive )
{
	// Add them to list. Lock here to ensure that we have the time to add DirHandle
	LMutex Lock( &FAddMutex );

	// Create OS handle
	DWORD NotifyFlags = FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE;

	HANDLE NewH = FindFirstChangeNotification ( DirName.c_str(), Recursive, NotifyFlags );

	if ( NewH == INVALID_HANDLE_VALUE )
	{
		Env->Logger->LogP( L_WARNING, "Error adding watch for %s\n", DirName.c_str() );

		return;
	}

	Env->Logger->LogP( L_DEBUG, "Adding watch for directory %s\n", DirName.c_str() );

	FDirHandlesToAdd.push_back( NewH );
	FDirNamesToAdd.push_back( DirName );
}

void clFileWatchThread::RemoveDirWatch( const LString& DirName )
{
	LMutex Lock( &FAddMutex );

	int idx = -1;

	for ( size_t i = 0 ; i < FDirNames.size() ; i++ )
	{
		if ( FDirNames[i] == DirName )
			{ idx = ( int )i; break;}
	}

	if ( idx > -1 )
	{
		HANDLE H = FDirHandles[idx];

		FindCloseChangeNotification( H );

		FDirHandles.erase( FDirHandles.begin() + idx );
		FDirNames.erase( FDirNames.begin() + idx );

		Env->Logger->LogP( L_DEBUG, "Removed notification for Dir [%s]\n", DirName.c_str() );
	}
}

void clFileWatchThread::AddNewWatches()
{
	LMutex Lock( &FAddMutex );

	// dirs
	for ( size_t j = 0 ; j < FDirNamesToAdd.size(); j++ )
	{
		FDirNames.push_back( FDirNamesToAdd[j] );
		FDirHandles.push_back( FDirHandlesToAdd[j] );
	}

	FDirNamesToAdd.resize( 0 );
	FDirHandlesToAdd.resize( 0 );

	// files
	for ( size_t j = 0 ; j < FFileHandlesToAdd.size() ; j++ )
	{
		FWatchHandles.push_back( FFileHandlesToAdd[j] );
	}

	FFileHandlesToAdd.resize( 0 );
}

/// Static procedure which tries to get access to the file
DWORD WINAPI WatchOpenerProcedure( LPVOID Param )
{
	// number of tries to wait for ERROR_SHARING_VIOLATION
	const int MaxFileAccessNumTries = 3;

	/// Get actual file watch handle
	clFileWatchHandle* FileWatch = ( clFileWatchHandle* )Param;

	// a little unsafe, but we'll make it without critical sections...
	FileWatch->FTryLoopRunning = true;

	/// Try to open the file
	for ( int i = 0 ; i < MaxFileAccessNumTries ; i++ )
	{
		// just open the file
		HANDLE hFile = CreateFileA ( FileWatch->FFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

		if ( hFile != INVALID_HANDLE_VALUE )
		{
			// done opening - just close, inform and exit
			CloseHandle( hFile );

			// send the watch handle in arguments
			LEventArgs TheArgs;
			TheArgs.FObjArg = FileWatch;

			FileWatch->Env->Logger->LogP( L_DEBUG, "Sending change notification (%s)", FileWatch->FFileName.c_str() );

			FileWatch->FHandler->SendAsync( L_EVENT_FILE_NOTIFICATION, TheArgs, false );

			break;
		}

		/// Investigate the error
		DWORD Err = GetLastError();

		if ( Err != ERROR_SHARING_VIOLATION )
		{
			// Couldn't open and it is not a sharing violation.
			// Abort any further attempts
			FileWatch->Env->Logger->LogP( L_WARNING, "Cannot open changed file %s, error code = %x", FileWatch->FFileName.c_str(), Err );
			break;
		}

		// 0.05 seconds to wait between sequential OPEN_READ tries
		Sleep( 50 );
	}

	// The FileWatch is ready for another change information
	FileWatch->FTryLoopRunning = false;

	return 0;
}

void clFileWatchThread::HandleChanges( DWORD WaitStatus )
{
	// find appropriate handle
	for ( size_t i = 0 ; i < FDirHandles.size() ; i++ )
	{
		if ( ( WAIT_OBJECT_0 + ( int )i ) == WaitStatus )
		{
			// Yes, we have one
//			Env->Logger->LogP(L_DEBUG, "Something changed in dir %s\n", FDirNames[i].c_str());

			// Check each file watch
			for ( size_t j = 0 ; j < FWatchHandles.size() ; j++ )
			{
				clFileWatchHandle* H = FWatchHandles[j];

				if ( H->FDirHandle == FDirHandles[i] )
				{
					Luint64 ThisFileTime = _FileTime( H->FFileName );

					// check last file time and last file size
					if ( H->FFileTime < ThisFileTime )
					{
						H->FFileTime = ThisFileTime;

//						Env->Logger->LogP(L_DEBUG, "Possibly, %s changed (in dir %s)\n", H->FFileName.c_str(), FDirNames[i].c_str());
						if ( H->FHandler && !H->FTryLoopRunning )
						{
							/// Start new thread which tries to open the file and check its availability
							HANDLE NewThread = CreateThread( NULL, 4096, WatchOpenerProcedure, H, 0, NULL );

							if ( NewThread == INVALID_HANDLE_VALUE )
							{
								Env->Logger->LogP( L_WARNING, "Could not start info thread for FileWatchNofication (%s)", H->FFileName.c_str() );
							}
							else
							{
								// store the handle to wait for its completion later
								FInformThreads[H] = NewThread;
							}
						}
					}
				}
			}

			if ( !FindNextChangeNotification( FDirHandles[i] ) )
			{
//				printf("Removing notification\n");
				// the notification is obsolete now
				RemoveDirWatch( FDirNames[i] );
			}

			break;
		}
	}
}

void clFileWatchThread::CloseHandles()
{
	std::vector<LString> FileWatches;
	std::vector<LString> DirWatches;

	{
		LMutex Lock( &FAddMutex );

		for ( int i = 0 ; i < ( int )FWatchHandles.size() ; i++ ) { FileWatches.push_back( FWatchHandles[i]->FFileName ); }

		for ( int i = 0 ; i < ( int )FDirNames.size() ; i++ ) { DirWatches.push_back( FDirNames[0] ); }
	}

	for ( int i = 0 ; i < ( int )FileWatches.size() ; i++ ) { RemoveFileWatch( FileWatches[i] ); }

	for ( int i = 0 ; i < ( int )DirWatches.size() ; i++ ) { RemoveDirWatch( DirWatches[i] ); }
}

void clFileWatchThread::Run()
{
	while ( !IsPendingExit() )
	{
		if ( FDirHandles.empty() )
		{
			if ( FDirNamesToAdd.empty() && FFileHandlesToAdd.empty() )
			{
				Sleep( 100 );
			}
		}
		else
		{
			DWORD dwWaitStatus = WaitForMultipleObjects( ( DWORD )FDirHandles.size(), &FDirHandles[0], FALSE, FWaitInterval );

			if ( dwWaitStatus == WAIT_TIMEOUT )
			{
				// not fatal
			}
			else if ( dwWaitStatus == WAIT_FAILED )
			{
				DWORD ErrCode = GetLastError();

				Env->Logger->LogP( L_DEBUG, "WaitErr: %x", ErrCode );

				{
					LMutex Lock( &FAddMutex );

					// check each handle
					std::vector<HANDLE> NewHandles;

					for ( size_t jj = 0 ; jj < FDirHandles.size() ; jj++ )
					{
						DWORD Ret;

						if ( GetHandleInformation( FDirHandles[jj], &Ret ) == 0 )
						{
							Env->Logger->LogP( L_DEBUG, "The watch %d is invalid (code = %d)", jj, Ret );

							// remove this dir handle
						}
						else
						{
							NewHandles.push_back( FDirHandles[jj] );
						}
					}

					// remove file handles associated with this stuff

					// add only the remaining handles
					FDirHandles.resize( NewHandles.size() );

					for ( size_t kk = 0 ; kk < NewHandles.size() ; kk++ )
					{
						FDirHandles[kk] = NewHandles[kk];
					}
				}
			}
			else
			{
				HandleChanges( dwWaitStatus );
			}
		}

		AddNewWatches();

		if ( !FInformThreads.empty() )
		{
			// Check the completion of inform threads
			// Check one at a time to avoid messing around with WaitForMultipleObject return codes
			for ( std::map<clFileWatchHandle*, HANDLE>::iterator t = FInformThreads.begin() ; t != FInformThreads.end() ; )
			{
				DWORD dwWaitStatus = WaitForSingleObject( t->second, 0 );

				// ignore timeout, they are OK
				if ( dwWaitStatus == WAIT_FAILED || dwWaitStatus == WAIT_OBJECT_0 )
				{
					clFileWatchHandle* H = t->first;

					// remove this handle and clear the flag
					t++;

					FInformThreads.erase( H );

					H->FTryLoopRunning = false;
				}
				else
				{
					++t;
				}
			}
		}
	}

	// Close all handles
	CloseHandles();
}


#endif // OS_WINDOWS

clFileSystem::clFileSystem(): FUseVirtualFileNames( true )
{
	FServerThread = NULL;
}

clFileSystem::~clFileSystem()
{
	FDownloadThread->Exit( true );
	delete( FDownloadThread );

#ifdef OS_WINDOWS
	FFileWatchThread->Exit( true );
	delete( FFileWatchThread );
#endif

	Utils::DeallocateAll( FMountPoints );
}

void clFileSystem::AddFileWatchC( const LString& Args )
{
	AddFileWatch( LStr::GetToken( Args, 1 ), Env->FindObject( LStr::GetToken( Args, 2 ) ) );
}

void clFileSystem::AddDirWatchC( const LString& Args )
{
	AddDirWatch( LStr::GetToken( Args, 1 ), LStr::ToBool( LStr::GetToken( Args, 2 ) ) );
}

void clFileSystem::RemoveFileWatchC( const LString& Args )
{
	RemoveFileWatch( LStr::GetToken( Args, 1 ) );
}

void clFileSystem::RemoveDirWatchC( const LString& Args )
{
	RemoveDirWatch( LStr::GetToken( Args, 1 ) );
}

void clFileSystem::AddFileWatch( const LString& FileName, iObject* Handler )
{
	if ( FileName.empty() ) { return; }

#ifdef OS_WINDOWS
	iMountPoint* MPD = FindMountPoint( FileName );

	if ( !MPD->FileExistsAtThisPoint( FileName ) ) { return; }

	if ( !MPD->IsPhysical() ) { return; }

	FFileWatchThread->AddFileWatch( VirtualNameToPhysicalFull( FileName ), Handler );
#endif
}

void clFileSystem::StartDirectoryWatch( const LString& DirName, bool Recursive, iObject* Handler )
{
	// TODO: move all this to the WatchThread. All we have to do - store the request

	AddDirWatch( DirName, Recursive );

	LStr::clStringsVector TheList;
	EnumerateFiles( DirName, Recursive, &TheList );

	// dirty hack: wait until the DirWatch is actually added
//	Env->SleepSeconds( 0.1f );

	AddMultipleFileWatches( TheList, Handler );
}

void clFileSystem::AddMultipleFileWatches( const LStr::clStringsVector& FileNames, iObject* Handler )
{
	for ( size_t i = 0 ; i < FileNames.size() ; i++ ) { AddFileWatch( FileNames[i], Handler ); }
}

void clFileSystem::AddDirWatch( const LString& DirName, bool Recursive )
{
	if ( DirName.empty() ) { return; }

#ifdef OS_WINDOWS
	FFileWatchThread->AddDirWatch( VirtualNameToPhysicalFull( DirName ), Recursive );
#endif
}

void clFileSystem::RemoveFileWatch( const LString& FileName )
{
	if ( FileName.empty() ) { return; }

#ifdef OS_WINDOWS
	FFileWatchThread->RemoveFileWatch( VirtualNameToPhysicalFull( FileName ) );
#endif
}

void clFileSystem::RemoveDirWatch( const LString& DirName )
{
	if ( DirName.empty() ) { return; }

#ifdef OS_WINDOWS
	FFileWatchThread->RemoveDirWatch( VirtualNameToPhysicalFull( DirName ) );
#endif
}

void clFileSystem::StopWebServersC( const LString& Args )
{
	if ( FServerThread )
	{
		FServerThread->Exit( true );
		delete( FServerThread );
		FServerThread = NULL;
	}
}

void clFileSystem::AfterConstruction()
{
	FDownloadThread = new clDownloadThread();
	FDownloadThread->Start( this->Env, iThread::Priority_Low );

#ifdef OS_WINDOWS
	FFileWatchThread = new clFileWatchThread();

	FFileWatchThread->FSystem = this;
	FFileWatchThread->Start( this->Env, iThread::Priority_Lowest );
#endif

	Env->Connect( L_EVENT_POSTINIT, BIND( &clFileSystem::Event_POSTINIT ) );
}

void clFileSystem::Event_POSTINIT( LEvent Event, const LEventArgs& Args )
{
	guard();

	// service: net downloading
	Env->Console->RegisterCommand( "DownloadFile", BIND( &clFileSystem::DownloadFileC ) );
	// server: web server
	Env->Console->RegisterCommand( "StartWebServer", BIND( &clFileSystem::StartWebServerC ) );
	Env->Console->RegisterCommand( "StopWebServers", BIND( &clFileSystem::StopWebServersC ) );

	unguard();
}

int clFileSystem::DownloadFile( const LString& Server, int Port, const LString& Path, const LString& OutFile, float Timeout, iObject* CompletionHandler )
{
	LString ThePath = Path;
	LStr::ReplaceAll( &ThePath, '\\', '/' );
	FDownloadThread->AddDownload( Server, Port, ThePath, OutFile, CompletionHandler );

	// TODO: implement
	return 0;
}

/// Cancel download in-progress. Return true if download was cancelled and false otherwise or if invalid DownloadID was specified
bool clFileSystem::CancelDownload( int DownloadID )
{
	// TODO: implement
	return false;
}

clHTTPServerThread* clFileSystem::StartWebServer( const LString& BindAddress, int BindPort, int MaxConn )
{
	FServerThread = new clHTTPServerThread();

	FServerThread->FBindAddress = BindAddress;
	FServerThread->FPort = BindPort;
	FServerThread->FMaxConnections = MaxConn < 0 ? 5 : MaxConn;

	FServerThread->Start( this->Env, iThread::Priority_Lowest );

	return FServerThread;
}

void clFileSystem::StartWebServerC( const LString& Args )
{
	LString BindAddress = LStr::GetToken( Args, 1 );
	int     Port    = LStr::ToInt( LStr::GetToken( Args, 2 ) );
	LString _Max    = LStr::GetToken( Args, 3 );
	int MaxConn = -1;

	if ( !_Max.empty() ) { MaxConn = LStr::ToInt( _Max ); }

	( void )StartWebServer( BindAddress, Port, MaxConn );
}

void clFileSystem::DownloadFileC( const LString& Args )
{
	LString Server  = LStr::GetToken( Args, 1 );
	LString Port    = LStr::GetToken( Args, 2 );
	LString Path    = LStr::GetToken( Args, 3 );
	LString OutFile = LStr::GetToken( Args, 4 );
	LString Timeout = LStr::GetToken( Args, 5 );

	if ( Args.empty() || Server.empty() || !LStr::IsCorrectInt( Port ) || Path.empty() || OutFile.empty() )
	{
		Env->Console->DisplayInfoTip( "Usage: DOWNLOADFILE <ServerAddress> <ServerPort> <RemoteFilePath> <LocalFilePath> [Timeout]" );
		return;
	}

	float TimeoutValue = LStr::IsCorrectDouble( Timeout ) ? LStr::ToFloat( Timeout ) : 0.0f;

	DownloadFile( Server, LStr::ToInt( Port ), Path, OutFile, TimeoutValue, NULL );
}

bool clFileSystem::LoadFileLinesArray( const LString& FName, LArray<LString>& Lines ) const
{
	Lines.resize( 0 );

	iIStream* In = CreateFileReader( FName );

	if ( !In ) { return false; }

	while ( !In->Eof() )
	{
		Lines.push_back( In->ReadLine() );
	}

	delete In;

	return true;
}

bool clFileSystem::LoadFileLinesVec( const LString& FName, std::vector<LString>& Lines ) const
{
	Lines.resize( 0 );

	try
	{
		if ( !FileExists( FName ) ) { return false; }

		iIStream* In = CreateFileReader( FName );

		if ( !In ) { return false; }

		while ( !In->Eof() ) { Lines.push_back( In->ReadLine() ); }

		delete In;
	}
	catch ( ... )
	{
		return false;
	}

	return true;
}

bool clFileSystem::SaveFileLinesArray( const LString& FName, const LArray<LString>& Lines ) const
{
	try
	{
		iOStream* Out = CreateFileWriter( FName );

		if ( !Out ) { return false; }

		for ( size_t j = 0 ; j < Lines.size() ; j++ ) { Out->WriteLine( Lines[j] ); }

		delete Out;
	}
	catch ( ... )
	{
		return false;
	}

	return true;
}

bool clFileSystem::SaveFileLinesVec( const LString& FName, const std::vector<LString>& Lines ) const
{
	iOStream* Out = CreateFileWriter( FName );

	if ( !Out ) { return false; }

	for ( size_t j = 0 ; j < Lines.size() ; j++ )
	{
		Out->WriteLine( Lines[j] );
	}

	delete Out;

	return true;
}

bool clFileSystem::LoadFileData( const LString& FName, void** Data, Luint64* DataSize ) const
{
	if ( !Data ) { return false; }

	if ( !DataSize ) { return false; }

	iIStream* InFile = CreateFileReader( FName );

	if ( !InFile ) { return false; }

	*DataSize = InFile->GetFileSize();

	// Reallocate data, if it is needed (multiple file loads do not require reallocation)
	if ( !( *Data ) ) { *Data = new char[ size_t( *DataSize ) ]; }

	/// It is an error, if the buffer is still zero
	if ( !( *Data ) ) { InFile->DisposeObject(); return false; }

	InFile->BlockRead( *Data, *DataSize );
	InFile->DisposeObject();

	return true;
}

bool clFileSystem::SaveFileData( const LString& FName, const void* Data, Luint64 DataSize ) const
{
	if ( !Data ) { return false; }

	iOStream* OutFile = CreateFileWriter( FName );

	if ( !OutFile ) { return false; }

	OutFile->BlockWrite( Data, DataSize );

	OutFile->DisposeObject();

	return true;
}

clBlob* clFileSystem::LoadFileAsBlob( const LString& FName ) const
{
	void* Data = NULL;
	Luint64 Size;

	if ( LoadFileData( FName, &Data, &Size ) )
	{
		clBlob* Res = Construct<clBlob>( Env );
		Res->SetExternalData( Data, static_cast<size_t>( Size ) );
		return Res;
	}
	else
	{
		Env->Logger->LogP( L_WARNING, "Error loading file: %s", FName.c_str() );
	}

	return NULL;
}

bool clFileSystem::SaveBlobToFile( const LString& FName, clBlob* Blob ) const
{
	if ( !Blob ) { return false; }

	return SaveFileData( FName, Blob->GetDataConst(), Blob->GetSize() );
}

// from libcompress (static link by now)
extern "C" Luint32 crc32( Luint32 crc, const void* buf, Luint32 len );

Luint32 clFileSystem::CRC32( const void* Data, Luint64 DataSize ) const
{
	return ( Luint32 )crc32( 0x0, Data, ( Luint32 )DataSize );
}

Luint32 clFileSystem::CRC32ForBlob( const clBlob* B ) const
{
	return this->CRC32( B->GetDataConst(), B->GetSize() );
}

bool clFileSystem::DeleteFilePhys( const LString& FileName )
{
#ifdef OS_WINDOWS
	// todo: convert from utf8 to WideChar and use DeleteFileW ?
	return ::DeleteFileA( FileName.c_str() );

#else // any posix

	return ( unlink( FileName.c_str() ) == 0 );
#endif
}

LString clFileSystem::CreateTempFileName( const LString& BaseDirectory, const LString& Prefix, bool RequireUnique ) const
{
	char OutName[1024];

#ifdef OS_WINDOWS

	GetTempFileNameA( BaseDirectory.c_str(), Prefix.c_str(), RequireUnique ? 1 : 0, &OutName[0] );

#else // any posix

	( void )RequireUnique;

	sprintf( OutName, "%s/%sXXXXXXXX", BaseDirectory.c_str(), Prefix.c_str() );

	mktemp( &OutName[0] );
#endif
	return LString( &OutName[0] );
}

iIStream* clFileSystem::CreateFileMapperForRawFile( iRAWFile* RF ) const
{
	clFileMapper* Mapper = new clFileMapper();

	Mapper->SetRawFile( RF );

	Mapper->Env = Env;
	Mapper->AfterConstruction();

	return Mapper;
}

iIStream* clFileSystem::CreateFileReader( const LString& FileName ) const
{
	guard( "%s", FileName.c_str() );

	const LString Signature = "RESOURCE:";

	bool FromResource = ( FileName.find( Signature ) == 0 );

	iRAWFile* RAWFile = NULL;

	LString Name = FileName;

	// replace environment variables with values
	LStr::ReplaceAll( &Name, '/', PATH_SEPARATOR );
	LStr::ReplaceAll( &Name, '\\', PATH_SEPARATOR );
	LString ReplFileName = clFileSystem::ReplaceEnvVars( Name );

	if ( FromResource )
	{
		const LString StreamID = "32167";

		LString ResName = FileName.substr( Signature.length(), ReplFileName.size() - Signature.length() );

		RAWFile = GetResourceStream( StreamID, ResName );

		RAISE_MSG_IF( !RAWFile, clExcept_UnableToFindFile, LString( "\"" ) + ResName + "\"" );
	}
	else
	{
		iMountPoint* MountPoint = FindMountPoint( ReplFileName );

		RAISE_MSG_IF( !MountPoint || Name.empty(), clExcept_UnableToFindFile, LString( "\"" ) + ReplFileName + "\"" );

		RAWFile = MountPoint->CreateRAWFile( ReplFileName );
	}

	return CreateFileMapperForRawFile( RAWFile );

	unguard();
}

iIStream* clFileSystem::CreateFileReaderFromString( const LString& FileName, const LString& VirtualFileName, const LString& Str ) const
{
	clMemRAWFile* RAWFile = new clMemRAWFile();
	RAWFile->Env = Env;
	RAWFile->AfterConstruction();
	RAWFile->CreateFromString( Str );

	RAWFile->SetFileName( FileName );
	RAWFile->SetVirtualFileName( VirtualFileName );

	return CreateFileMapperForRawFile( RAWFile );
}

iIStream* clFileSystem::CreateFileReaderFromMemory( const LString& FileName, const LString& VirtualFileName, const void* BufPtr, Luint64 BufSize ) const
{
	clMemRAWFile* RAWFile = new clMemRAWFile();
	RAWFile->Env = Env;
	RAWFile->AfterConstruction();
	RAWFile->CreateFromManagedBuffer( BufPtr, BufSize );

	RAWFile->SetFileName( FileName );
	RAWFile->SetVirtualFileName( VirtualFileName );

	return CreateFileMapperForRawFile( RAWFile );
}

clMemFileWriter* clFileSystem::CreateMemFileWriter( const LString& FileName, Luint64 InitialSize ) const
{
	clMemFileWriter* Stream = Construct<clMemFileWriter>( Env );

	clBlob* B = Construct<clBlob>( Env );
	B->SetSize( static_cast<size_t>( InitialSize ) );

	Stream->SetContainer( B );
	Stream->SetMaxSize( InitialSize * 2 ); // default value

	Stream->SetFileName( FileName );

	return Stream;
}

iOStream* clFileSystem::CreateFileWriter( const LString& FileName ) const
{
	LString Name = VirtualNameToPhysical( ValidatePath( FileName ) );
	LString Dir;

	SplitPath( Name, NULL, &Dir, NULL, NULL );

	CreateDirsPhys( Dir );

	clFileWriter* Writer = new clFileWriter();
	Writer->Env = Env;
	Writer->AfterConstruction();

	if ( !Writer->Open( Name ) )
	{
		// store error
		// FLastError = Writer->GetLastError();
		delete Writer;

		Writer = NULL;
	}

	return Writer;
}

iOStream* clFileSystem::CreateLogFileWriter() const
{
	clLogStream* Log = new clLogStream();
	Log->Env = Env;
	Log->AfterConstruction();

	return Log;
}

void clFileSystem::SplitPath( const LString& Path, LString* Drive, LString* Dir, LString* FileName, LString* FileExt )
{
#ifdef OS_WINDOWS
	char TmpDrive[ _MAX_DRIVE+1 ];
	char TmpDir[ _MAX_DIR+1 ];
	char TmpFileName[ _MAX_FNAME+1 ];
	char TmpFileExt[ _MAX_EXT+1    ];

	_splitpath( Path.c_str(), TmpDrive, TmpDir, TmpFileName, TmpFileExt );

	if ( Drive    )
	{
		*Drive    = LString( TmpDrive );
	}

#else // posix

	char* TmpDir      = dirname( Path.c_str() );
	char* TmpFileName = basename( Path.c_str() );
	char* TmpFileExt  = strrchr( TmpFileName, '.' );

	if ( TmpFileExt ) { *( TmpFileExt++ ) = '\0'; }

#endif

	if ( Dir      )
	{
		*Dir      = LString( TmpDir );
		size_t L = Dir->length();

		if ( L > 0 && Dir->at( L - 1 ) != PATH_SEPARATOR ) { ( *Dir ) += PATH_SEPARATOR; }
	}

	if ( FileName )
	{
		*FileName = LString( TmpFileName );
	}

	if ( FileExt )
	{
		*FileExt = LString();

		if ( TmpFileExt )
		{
#if !defined(OS_WINDOWS) // posix
			// add missing dot before extension
			*FileExt  = "." + LString( TmpFileExt );
#else
			*FileExt  = LString( TmpFileExt );
#endif
		}
	}
}

LString clFileSystem::FindFile( const LString& FileName, const LString& ParentFileName )
{
	LString Drive;
	LString DirName;
	LString File_Name;
	LString File_Ext;
	Env->FileSystem->SplitPath( FileName, &Drive, &DirName, &File_Name, &File_Ext );

	if ( !( Drive + DirName ).empty() && FileExistsPhys( FileName ) )
	{
		return FileName;
	}

	LString ParentDirName;
	Env->FileSystem->SplitPath( ParentFileName, &Drive, &ParentDirName, NULL, NULL );

	ParentDirName = LStr::ReplaceAll( ParentDirName, '/', PATH_SEPARATOR );
	ParentDirName = LStr::ReplaceAll( ParentDirName, '\\', PATH_SEPARATOR );

	LStr::AddTrailingChar( &ParentDirName, PATH_SEPARATOR );

	LString LocalFile = Drive + ParentDirName + FileName;

	if ( FileExists( LocalFile ) ) { return LocalFile; }

	LocalFile = Drive + ParentDirName + File_Name + File_Ext;

	if ( FileExists( LocalFile ) ) { return LocalFile; }

	return FileName;
}

LString clFileSystem::ReplaceEnvVars( const LString& String )
{
	LString InStr  = String;
	LString OutStr( "" );

	for ( size_t i = 0; i != InStr.length(); ++i )
	{
		// get var value
		if ( InStr[i] == '$' )
		{
			LString VarName = LStr::CopyFromPosToChar( InStr, i + 1, PATH_SEPARATOR, NULL );

			char* Value = getenv( VarName.c_str() );

			OutStr += ( Value ) ? LString( Value ) : VarName;

			i += VarName.length();
		}
		else
		{
			OutStr.push_back( InStr[i] );
		}
	}

	return OutStr;
}

void clFileSystem::CreateDirsPhys( const LString& DirectoryName )
{
	LString Path = DirectoryName;

	LStr::AddTrailingChar( &Path, PATH_SEPARATOR );

	LStr::ReplaceAll( &Path, '/', PATH_SEPARATOR );
	LStr::ReplaceAll( &Path, '\\', PATH_SEPARATOR );

	for ( size_t i = 0; i != Path.length(); ++i )
	{
		if ( Path[i] == PATH_SEPARATOR )
		{
			Path[i] = '\0';
#ifdef OS_WINDOWS
			_mkdir( Path.c_str() );
#else
			mkdir( Path.c_str(), 0 ); // second ARG ?
#endif
			Path[i] = PATH_SEPARATOR;
		}
	}
}

void clFileSystem::CreateDirs( const LString& DirectoryName ) const
{
	LString PhysPath = VirtualNameToPhysical( DirectoryName );

	CreateDirsPhys( PhysPath );
}

void clFileSystem::SetRootDir( const LString& RootDir )
{
	Mount( RootDir );
}

bool clFileSystem::FileExistsInResource( const LString& Name ) const
{
	const LString Signature = "RESOURCE:";

	bool FromResource = ( Name.find( Signature ) == 0 );

	if ( !FromResource ) { return false; }

	LString FileName = Name;

	// replace environment variables with values
	LStr::ReplaceAll( &FileName, '/', PATH_SEPARATOR );
	LStr::ReplaceAll( &FileName, '\\', PATH_SEPARATOR );
	LString ReplFileName = clFileSystem::ReplaceEnvVars( FileName );

#ifdef OS_WINDOWS
	HMODULE Handle = GetModuleHandle( NULL );

	HRSRC hResInfo;

	const LString ResourceID = "32167";

	LString ResourceType = Name.substr( Signature.length(), ReplFileName.size() - Signature.length() );

	// first find the resource info block
	if ( ( hResInfo = ::FindResource( Handle, MAKEINTRESOURCE( LStr::ToInt( ResourceID ) ), ResourceType.c_str() ) ) == NULL )
	{
		return false;
	}

	return true;
#else
	return false;
#endif
}

bool clFileSystem::FileExistsPhys( const LString& PhysicalName )
{
#ifdef OS_WINDOWS
	struct _stat buf;

	int Result = _stat( ValidatePath( PhysicalName ).c_str(), &buf );
#else
	struct stat buf;

	int Result = stat( ValidatePath( PhysicalName ).c_str(), &buf );
#endif

	return Result == 0;
}

LString clFileSystem::ValidatePath( LString PathName )
{
	LString Result = PathName;

	for ( size_t i = 0; i != Result.length(); ++i )
	{
		if ( Result[i] == '/' || Result[i] == '\\' )
		{
			Result[i] = PATH_SEPARATOR;
		}
	}

	return Result;
}

bool clFileSystem::FileExistsAndNewer( const LString& OldFile, const LString& NewFile ) const
{
	// 1. Check, if NewFile exists
	if ( !Env->FileSystem->FileExists( NewFile ) ) { return false; }

	// 2. Check, if the NewFile is older then the OldFile
	//    Besides, if we don't have OldFile at all - bypass this
	if ( Env->FileSystem->FileExists( OldFile ) )
	{
		if ( Env->FileSystem->GetFileTime( OldFile ) >
		     Env->FileSystem->GetFileTime( NewFile ) )
		{
			return false;
		}
	}

	return true;
}

bool clFileSystem::FileExists( const LString& Name ) const
{
	if ( Name.empty() || Name == "." ) { return false; }

	/*
	   Don't do it like

	      return FileExistsPhys(VirtualNameToPhysical(Name));

	   because this will break search within archives
	*/

	if ( FileExistsInResource( Name ) ) { return true; }

	iMountPoint* MPD = FindMountPoint( Name );

	return MPD ? MPD->FileExistsAtThisPoint( Name ) : false;
}

bool clFileSystem::IsFullPath( const LString& Path ) const
{
	if ( LStr::ContainsSubStr( Path, ":\\" ) ) { return true; }

	if ( LStr::ContainsSubStr( Path, ":/" ) ) { return true; }

	if ( LStr::ContainsSubStr( Path, ".\\" ) ) { return true; }

#if defined( OS_ANDROID )

	if ( Path.length() && Path[0] == '/' ) { return true; }

#endif

	return false;
}

LString clFileSystem::GetFullPath( const LString& PhysicalPath ) const
{
	if ( IsFullPath( PhysicalPath ) ) { return PhysicalPath; }

#if defined( OS_WINDOWS )
	char Result[ MAX_PATH ];

	GetFullPathName( PhysicalPath.c_str(), sizeof( Result ), ( LPSTR )&Result, NULL );

	return LString( Result );
#else
	return PhysicalPath;
#endif

}

/// Supported types: TAR, RAR, ZIP(APK)
inline bool SupportedArchiveType( const LString& FileName )
{
	LString Name = LStr::GetUpper( FileName );
	size_t Sz = FileName.length();

	return ( ( Name.find( "_TAR" ) == Sz - 4 ) || ( Name.find( ".TAR" ) == Sz - 4 ) ||
	         ( Name.find( "_RAR" ) == Sz - 4 ) || ( Name.find( ".RAR" ) == Sz - 4 ) ||
	         ( Name.find( "_ZIP" ) == Sz - 4 ) || ( Name.find( ".ZIP" ) == Sz - 4 ) ||
	         ( Name.find( "_APK" ) == Sz - 4 ) || ( Name.find( ".APK" ) == Sz - 4 ) );
}

void clFileSystem::Mount( const LString& PhysicalPath )
{
	guard();

	// don't mount anything if virtual file names are disabled
	if ( !FUseVirtualFileNames ) { return; }

	Env->Logger->Log( L_LOG, "Mounting file system: " + PhysicalPath + " mapped to /" );

	iMountPoint* MPD = NULL;

	if ( SupportedArchiveType( PhysicalPath ) )
	{
		if ( FMountPoints.empty() ) { FATAL_MSG( "Unable to mount container file as the root folder" ); }

		clArchiveReader* Reader = Construct<clArchiveReader>( Env );

		if ( !Reader->OpenArchiveFile( PhysicalPath ) )
		{
		}

		clArchiveMP* ArchMP = new clArchiveMP( Reader );
		ArchMP->Env = Env;
		ArchMP->AfterConstruction();

		MPD = ArchMP;
	}
	else
	{
#if !defined( OS_ANDROID )

		// LV : we do not check .rar mount points, because it is checked in RARMountPointDriver creation
		if ( !FileExistsPhys( PhysicalPath ) )
		{
			Env->Logger->Log( L_WARNING, "Unable to mount: '" + PhysicalPath + "' not found" );

			return;
		}

#endif

		MPD = new clMountPoint( PhysicalPath );
		MPD->Env = Env;
		MPD->AfterConstruction();
	}

	if ( MPD )
	{
		MPD->SetName( PhysicalPath );
		AddMountPoint( MPD );
	}

	unguard();
}

void clFileSystem::Unmount( const LString& ThePath )
{
	if ( FMountPoints.empty() ) { return; }

	iMountPoint* MP = FindMountPoint( ThePath );

	if ( !MP ) { return; }

	/// Cannot unmount root
	if ( MP == *( FMountPoints.begin() ) ) { return; }

	RemoveMountPoint( MP );

	MP->DisposeObject();
}

void clFileSystem::AddAliasMountPoint( const LString& SrcPath, const LString& AliasPrefix )
{
	iMountPoint* MP = FindMountPointByName( SrcPath );

	if ( !MP ) { return; }

	clAliasMountPoint* AMP = new clAliasMountPoint( MP );

	AMP->SetAlias( AliasPrefix );

	AddMountPoint( AMP );
}

iMountPoint* clFileSystem::FindMountPointByName( const LString& ThePath )
{
	for ( size_t i = 0 ; i != FMountPoints.size() ; i++ )
	{
		if ( FMountPoints[i]->GetName() == ThePath ) { return FMountPoints[i]; }
	}

	return NULL;
}

void clFileSystem::AddMountPoint( iMountPoint* MP )
{
	if ( !MP ) { return; }

	if ( std::find( FMountPoints.begin(), FMountPoints.end(), MP ) == FMountPoints.end() ) { FMountPoints.push_back( MP ); }
}

void clFileSystem::RemoveMountPoint( iMountPoint* MP )
{
	if ( !MP ) { return; }

	for ( size_t j = 0 ; j < FMountPoints.size(); j++ )
	{
		if ( FMountPoints[j] == MP )
		{
			if ( j < FMountPoints.size() - 1 ) { FMountPoints[j] = FMountPoints[FMountPoints.size() - 1]; }

			FMountPoints.resize( FMountPoints.size() - 1 );
			return;
		}
	}
}

LString clFileSystem::VirtualNameToPhysical( const LString& Path ) const
{
	if ( !FUseVirtualFileNames ) { return Path; }

	if ( IsFullPath( Path ) ) { return Path; }

	iMountPoint* MP = FindMountPoint( Path );

	if ( !MP ) { return Path; }

	return MP->MapToThisPoint( Path );
}

LString clFileSystem::VirtualNameToPhysicalFull( const LString& Path ) const
{
	return GetFullPath( VirtualNameToPhysical( Path ) );
}

iMountPoint* clFileSystem::FindMountPoint( const LString& FileName ) const
{
	if ( FMountPoints.empty() ) { return NULL; }

	if ( ( *FMountPoints.begin() )->FileExistsAtThisPoint( FileName ) )
	{
		return ( *FMountPoints.begin() );
	}

	// reverse order
	for ( LArray<iMountPoint*>::const_reverse_iterator i = FMountPoints.rbegin(); i != FMountPoints.rend(); ++i )
	{
		if ( ( *i )->FileExistsAtThisPoint( FileName ) ) { return ( *i ); }
	}

	return *( FMountPoints.begin() );
}

Lint64 clFileSystem::GetFileTime( const LString& FileName ) const
{
	iMountPoint* MP = FindMountPoint( FileName );

	return MP ? MP->GetFileTime( FileName ) : 0;
}

Lint64 clFileSystem::GetMaxTimeForDirectory( const LString& DirName ) const
{
	LStr::clStringsVector Files;

	EnumerateFiles( DirName, true, &Files );

	Lint64 MaxTime = 0;

	for ( size_t i = 0; i < Files.size(); ++i )
	{
		Lint64 ThisTime = this->GetFileTime( Files[i] );

		if ( ThisTime > MaxTime )
		{
			MaxTime = ThisTime;
		}
	}

	return MaxTime;
}

void WriteXLML( iOStream* OS, mlNode* N, int depth );
void WriteXML( iOStream* OS, mlNode* N, int depth );

bool clFileSystem::SaveXLML( mlNode* Node, const LString& FileName ) const
{
	iOStream* Stream = Env->FileSystem->CreateFileWriter ( FileName );

	bool Res = SaveXLMLToStream( Node, Stream );

	delete Stream;

	return Res;
}

/// Save XLML representation of the mlNode tree to output stream
bool clFileSystem::SaveXLMLToStream( mlNode* Node, iOStream* OStream ) const
{
	// TODO : check this !
	WriteXLML( OStream, Node, 0 );

	return true;
}

bool clFileSystem::SaveXLMLToLog( mlNode* Node ) const
{
	clLogStream Log;
	Log.Env = Env;

	return SaveXLMLToStream( Node, &Log );
}

bool clFileSystem::SaveXMLToLog( mlNode* Node ) const
{
	clLogStream Log;
	Log.Env = Env;

	return SaveXMLToStream( Node, &Log );
}

bool clFileSystem::SaveXML( mlNode* Node, const LString& FileName ) const
{
	iOStream* Stream = Env->FileSystem->CreateFileWriter ( FileName );

	bool Res = SaveXMLToStream( Node, Stream );

	delete Stream;

	return Res;
}

/// Save XML representation of the mlNode tree to output stream
bool clFileSystem::SaveXMLToStream( mlNode* Node, iOStream* OStream ) const
{
	WriteXML( OStream, Node, 0 );

	return true;
}

bool clFileSystem::IsFatalOnErrors() const
{
#ifdef OS_ANDROID
	return false;
#else
	return false; // return true;
#endif
}

mlNode* clFileSystem::LoadXLMLFromStream_Internal( iIStream* IStream, bool NonStrictASEMode ) const
{
	guard( "%s", IStream->GetFileName().c_str() )

	const char* Ptr = reinterpret_cast<const char*>( IStream->MapStream() );
	Luint64 Size    = IStream->GetFileSize();

	mlTreeBuilder Builder;

	Builder.ASEMode = NonStrictASEMode;

	mlNode* Node = Builder.build( Ptr, static_cast<size_t>( Size ) );

	// check if parsing was successfull
	if ( this->IsFatalOnErrors() )
	{
		RAISE_MSG_IF( !Node, clExcept_UnableToParseXLMLScript, LString( "\"" ) + IStream->GetFileName() + "\"" );
	}
	else
	{
		if ( !Node )
		{
			Node = new mlNode();
			Env->Logger->LogP( L_WARNING, "Unable to parse XLML: %s", IStream->GetFileName().c_str() );
		}
	}

	return Node;

	unguard();

}

mlNode* clFileSystem::LoadXLML_Internal( const LString& FileName, bool NonStrictASEMode ) const
{
	iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

	if ( !Stream )
	{
		TODO( "get error code from FileSystem !" )

//    FLastError = LString("Error loading file (") + FileName + LString(")"); //
		// some error happened
		if ( this->IsFatalOnErrors() )
		{
			return NULL;
		}
		else
		{
			// empty node
			return new mlNode();
		}
	}

	mlNode* Res = LoadXLMLFromStream_Internal( Stream, NonStrictASEMode );

	delete Stream;

	return Res;
}

mlNode* clFileSystem::LoadXLMLFromStream_ASE( iIStream* IStream ) const
{
	return LoadXLMLFromStream_Internal( IStream, true );
}

mlNode* clFileSystem::LoadXLMLFromStream( iIStream* IStream ) const
{
	return LoadXLMLFromStream_Internal( IStream, false );
}

mlNode* clFileSystem::LoadXLML_ASE( const LString& FileName ) const
{
	return LoadXLML_Internal( FileName, true );
}

mlNode* clFileSystem::LoadXLML( const LString& FileName ) const
{
	return LoadXLML_Internal( FileName, false );
}

bool clFileSystem::CopyFile( const LString& Src, const LString& Dst ) const
{
	iIStream* In = CreateFileReader( Src );
	iOStream* Out = CreateFileWriter( Dst );

	const Luint64 BufferSize = 65536*2-1;

	char Buffer[ BufferSize ];

	Luint64 BytesLeft = In->GetFileSize();

	while ( BytesLeft )
	{
		Luint64 Size = BytesLeft > sizeof( Buffer ) ? sizeof( Buffer ) : BytesLeft;

		In->BlockRead( Buffer, Size );
		Out->BlockWrite( Buffer, Size );

		BytesLeft -= Size;
	}

	delete( Out );
	delete( In );

	return true;
}

#ifdef L_VFS_USE_PUGIXML

/**
   Convert pugi::xml_node to our mlNode

   The node is mapped as attributes "value" and "param"

   If "param" attribute is present, it is a section node.
*/
mlNode* BuildNodeFromXML( pugi::xml_node& node )
{
	mlNode* ThisNode;

	bool IsSection = false;

	LString ParamValue = LString( node.value() );

	for ( pugi::xml_attribute_iterator a = node.attributes_begin() ; a != node.attributes_end() ; a++ )
	{
		if ( strcmp( a->name(), "value" ) == 0 )
		{
			ParamValue = LString( a->value() );
		}
		else if ( strcmp( a->name(), "param" ) == 0 )
		{
			ParamValue = LString( a->value() );
			IsSection = true;
		}
		else
		{
			// invalid attribute name ?
		}
	}

	ThisNode = new mlNode( node.name(), ParamValue );
	ThisNode->isSection = IsSection;
	/*
	   if ( IsSection )
	   {
	      ThisNode = new mlSectionNode();
	      ThisNode->setID( node.name() );
	      ThisNode->setValue( ParamValue );
	   }
	   else
	   {
	      ThisNode = new mlParamNode( node.name(), ParamValue );
	   }
	*/

	for ( pugi::xml_node_iterator n = node.begin() ; n != node.end() ; n++ )
	{
		mlNode* Sub = BuildNodeFromXML( *n );

		if ( Sub != NULL )
		{
			ThisNode->children.push_back( Sub );
		}
	}

	return ThisNode;
}

#endif

mlNode* clFileSystem::LoadXMLFromStream( iIStream* Stream ) const
{
#ifdef L_VFS_USE_PUGIXML
	pugi::xml_document doc;

	doc.load_buffer( Stream->MapStream(), static_cast<size_t>( Stream->GetFileSize() ) );

	return BuildNodeFromXML( doc );
#else
	return NULL;
#endif
}

mlNode* clFileSystem::LoadXML( const LString& fname ) const
{
	iIStream* Stream = CreateFileReader( fname );

	mlNode* Result = LoadXMLFromStream( Stream );

	// delete only now, because pugi does in-place parsing of XML
	delete Stream;

	return Result;
}

void clFileSystem::OpenURL( const LString& URL )
{
#if defined( OS_ANDROID )
	Android_OpenURL( URL.c_str() );
#elif defined( OS_WINDOWS )
	ShellExecute( NULL, "open", URL.c_str(), NULL, NULL, SW_SHOWNORMAL );
#else
	// implement
#endif
}

void clFileSystem::SetWallpaper( const LString& ImageFileName )
{
#if defined( OS_ANDROID )
	Android_SetWallpaper( VirtualNameToPhysical( ImageFileName ).c_str() );
#elif defined( OS_WINDOWS )
	// implement
#endif
}

void clFileSystem::OpenImageDialog()
{
#if defined( OS_ANDROID )
	Android_OpenImageDialog();
#elif defined( OS_WINDOWS )
	// implement
#endif
}

Luint64 clFileSystem::CompareFiles( const LString& FileName1, const LString& FileName2 ) const
{
	iIStream* S1 = CreateFileReader( FileName1 );
	iIStream* S2 = CreateFileReader( FileName2 );

	Luint64 Size = 0;

	if ( S1->GetFileSize() > S2->GetFileSize() )
	{
		Size = S2->GetFileSize();
		delete( S1 );
		delete( S2 );
		return Size;
	}

	if ( S1->GetFileSize() < S2->GetFileSize() )
	{
		Size = S1->GetFileSize();
		delete( S1 );
		delete( S2 );
		return Size;
	}

	int CMP = memcmp( S1->MapStream(), S2->MapStream(), static_cast<size_t>( S1->GetFileSize() ) );

	if ( CMP > 0 )
	{
		Size = S1->GetFileSize();
		delete( S1 );
		delete( S2 );
		return Size;
	}

	if ( CMP < 0 )
	{
		Size = S2->GetFileSize();
		delete( S1 );
		delete( S2 );
		return Size;
	}

	delete( S1 );
	delete( S2 );

	return 0;
}

void clFileSystem::EnumerateFiles( const LString& VirtualPath, bool SubDirs, LStr::clStringsVector* StrVec ) const
{
	Enumerate( VirtualPath, SubDirs, true, SubDirs, StrVec );
}

void clFileSystem::EnumerateDirs( const LString& VirtualPath, bool SubDirs, LStr::clStringsVector* StrVec ) const
{
	Enumerate( VirtualPath, SubDirs, false, SubDirs, StrVec );
}

// ResourceID is integer for win32
clMemRAWFile* clFileSystem::GetResourceStream( const LString& ResourceID, const LString& ResourceType ) const
{
#ifdef OS_WINDOWS
	HMODULE Handle = GetModuleHandle( NULL );

	HRSRC hResInfo;
	HGLOBAL hResource;

	// first find the resource info block
	if ( ( hResInfo = ::FindResource( Handle, MAKEINTRESOURCE( LStr::ToInt( ResourceID ) ), ResourceType.c_str() ) ) == NULL )
	{
		return( NULL );
	}

	// determine resource size
	int BufSize = SizeofResource( Handle, hResInfo );

	// now get a handle to the resource
	if ( ( hResource = LoadResource( Handle, hResInfo ) ) == NULL )
	{
		return( NULL );
	}

	// finally get and return a pointer to the resource
	void* BufPtr = LockResource( hResource );

	// now create memory stream (FIXME : copy-paste from CreateFileReaderFromMemory)
	clMemRAWFile* RAWFile = new clMemRAWFile();
	RAWFile->Env = Env;
	RAWFile->AfterConstruction();
	RAWFile->CreateFromManagedBuffer( BufPtr, BufSize );

	RAWFile->SetFileName( "" );
	RAWFile->SetVirtualFileName( "" );

	return RAWFile;
#else
	// resources are not supported on Linux, but may be supported in macos bundles
	return NULL;
#endif
}

bool IsUpDir( const LString& Dir )
{
	return ( Dir == "." ) || ( Dir == ".." );
}

#ifdef OS_WINDOWS
//
#else
int IsDirectory( const char* filename )
{
	struct stat file_stats;

	stat ( filename, &file_stats );

	if ( S_ISDIR( file_stats.st_mode ) )
	{
		return 1;
	}

	return 0;
}
#endif

void clFileSystem::Enumerate( const LString& VirtualPath, bool SubDirs, bool Files, bool SaveFullNames, LStr::clStringsVector* StrVec ) const
{
#ifdef OS_WINDOWS
	WIN32_FIND_DATA FileData;

	LString Mask = Env->FileSystem->VirtualNameToPhysical( VirtualPath );

	Mask.append( "\\*.*" );

	HANDLE hSearch = FindFirstFileA( Mask.c_str(), &FileData );

	if ( hSearch == INVALID_HANDLE_VALUE )
	{
		return;
	}

	for ( ;; )
	{
		LString FullName = VirtualPath + '\\';
		FullName.append( FileData.cFileName );

		bool IsDirectory = ( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY;

		if ( IsDirectory )
		{
			if ( !Files )
			{
				StrVec->push_back( SaveFullNames ? FullName : FileData.cFileName );
			}

			if ( SubDirs )
			{
				if ( !IsUpDir( FileData.cFileName ) )
				{
					Enumerate( FullName, SubDirs, true, SubDirs, StrVec );
				}
			}
		}
		else
		{
			if ( Files )
			{
				StrVec->push_back( SaveFullNames ? FullName : FileData.cFileName );
			}
		}

		if ( !FindNextFile( hSearch, &FileData ) )
		{
			break;
		}
	}

	FindClose( hSearch );
#else
	string DirName = Env->FileSystem->VirtualNameToPhysical( VirtualPath );

	char LastChar = DirName.at( DirName.length() - 1 );

	if ( ( LastChar != '\\' ) || ( LastChar != '/' ) ) { DirName += string( "/" ); }

	DIR* d = opendir( DirName.c_str() );

	struct dirent* de;

	if ( d == NULL )
	{
		Env->Logger->LogP( L_WARNING, "Cannot open directory %s", DirName.c_str() );
		return;
	}

	while ( de = readdir( d ) )
	{
		string dname( de->d_name );

		// this is the only possible check (unix-like does not support hidden/system attr)
		if ( dname.at( 0 ) != '.' )
		{
			string FullName = DirName + dname;

			if ( IsDirectory( FullName.c_str() ) )
			{
				if ( !Files )
				{
					StrVec->push_back( SaveFullNames ? FullName : dname );
				}

				if ( SubDirs )
				{
					if ( !IsUpDir( dname ) )
					{
						Enumerate( FullName, SubDirs, true, SubDirs, StrVec );
					}
				}
			}
			else
			{
				if ( Files )
				{
					StrVec->push_back( SaveFullNames ? FullName : dname );
				}
			}
		}
	}

	closedir( d );
#endif
}

// file system helper
iRAWFile* clFileSystem::CreatePhysicalFileReader( const LString& FileName, const LString& VirtualName ) const
{
//#if defined( OS_ANDROID )
//	clRAWFileNonMapped* File = new clRAWFileNonMapped();
//#else
	clRAWFile* File = new clRAWFile();
//#endif

	File->Env = Env;
	File->AfterConstruction();

	if ( !File->Open( ValidatePath( FileName ), VirtualName ) )
	{
		// FLastError = File->GetLastError();
		delete File;
		File = NULL;
	}

	return File;
}

LString GetPrefix( int depth )
{
	LString str = "";

	for ( int i = 0 ; i < depth; i++ ) { str += "\t"; }

	return str;
}

void WriteXLML( iOStream* OS, mlNode* N, int depth )
{
	LString Prefix = GetPrefix( depth );

	if ( N->isSection )
	{
		OS->WriteLine( Prefix + N->getID() + LString( "(" ) + N->getValue() + LString( ")" ) );

		OS->WriteLine( Prefix + "{" );

		if ( !N->children.empty() )
		{
			for ( size_t i = 0; i < N->children.size() ; i++ )
			{
				WriteXLML( OS, N->children[i], depth + 1 );
			}
		}

		OS->WriteLine( Prefix + "}" );
	}
	else
	{
		OS->WriteLine( Prefix + N->getID() + LString( " " ) + N->getValue() );
	}
}

void WriteXML( iOStream* OS, mlNode* N, int depth )
{
	LString Prefix = GetPrefix( depth );

	bool Section = N->isSection; //( dynamic_cast<mlSectionNode*>( N ) != NULL );

	OS->WriteLine( Prefix + LString( "<" ) + N->getID() + LString( " " ) + ( Section ? LString( "param=" ) : LString( "value=" ) ) + LString( "\"" ) + N->getValue() + LString( "\">" ) );

	for ( size_t i = 0; i < N->children.size() ; i++ )
	{
		WriteXLML( OS, N->children[i], depth + 1 );
	}

	OS->WriteLine( Prefix + LString( "<" ) + N->getID() + LString( "/>" ) );
}

#if 0

////// Experimental "semi-binary"serialization (not enough Write..() methods in FileWriter to perform this)

// "True" binary serialization must be autogenerated in LSDC

/**
   Binary serialization of the mlNode
*/
bool clFileSystem::NodeToBinary( mlNode* Node, iOStream* OutStream )
{
	if ( !Node )
	{
		// FLastError = "Invalid node pointer";
		return false;
	}

	/// TODO : handle write errors

	/// 1. Write node type
	if ( !OutStream->WriteInt32( Node->GetTypeCode() ) ) { return false; }

	/// 2. Write node name
	if ( !OutStream->WriteBinaryString( Node->getID() ) ) { return false; }

	/// 3. Write node value
	if ( !OutStream->WriteBinaryString( Node->getValue() ) ) { return false; }

	/// 4. Write number of children
	Lint32 NumCh = static_cast<Luint32>( Node->children.size() );

	if ( !OutStream->WriteInt32( NumCh ) ) { return false; }

	/// 5. Write children
	for ( int i = 0 ; i < NumCh ; i++ )
	{
		if ( !NodeToBinary( Node->children[i], OutStream ) )
		{
			// FLastError = "Error saving child(" + _int2str(i) + ") : " + FLastError;
			return false;
		}
	}

	return true;
}

/**
   Loads "binary-textual" representation of mlNode

   Binary format eliminates the need for any parsing
*/
mlNode* clFileSystem::NodeFromBinary( iIStream* InStream )
{
	/// 1. Create node
	mlNode* Result = new mlNode();

	if ( !Result )
	{
		// FLastError = "Invalid node type";

		return NULL;
	}

	/// 2. Read node name
	LString NodeName;

	if ( !InStream->ReadBinaryString( NodeName ) ) { return false; }

	/// 3. Read node value
	LString NodeValue;

	if ( !InStream->ReadBinaryString( NodeValue ) ) { return false; }

	/// 4. Read number of child nodes
	Lint32 NumCh;

	if ( !InStream->ReadInt32( NumCh ) ) { return false; }

	if ( NumCh < 0 /*  || NumCh > MAX_NUMBER_OF_CH */ )
	{
		// FLastError = "Invalid number of children: " + _int2str( NumCh );
		return false;
	}

	/// 5. Read children
	for ( int i = 0 ; i < NumCh ; i++ )
	{
		mlNode* Node = NodeFromBinary( InStream );

		if ( !Node )
		{
			// store error
			// FLastError = ""...

			// clean already loaded nodes
			delete Node; // Node->DisposeObject();

			return NULL;
		}

		Result->children.push_back( Node );
	}

	return Result;
}

#endif

/*
 * 04/12/2012
     NULL checks for mount points
 * 20/12/2011
     Blob read/writing
    MemFileWriters
 * 26/10/2010
     Experimental serialization
 * 18/05/2010
     ReplaceEnvVars()
     CreateFileReader() accepts $VarName in file name
 * 28/05/2009
     Bug fixed in CreateFileWriter()
 * 15/05/2009
     Conditional PugiXML usage
     GetResourceStream moved here from SystemLayer
 * 29/11/2007
     Removed OpenINIFile()
     Added CreateFileReaderFromString() : MemFile support
 * 02/10/2007
     Reverse iteration for mount points
 * 29/05/2007
     OS-dependent stuff moved to SystemLayer
 * 10/04/2007
     Enumerate() works
 * 09/12/2006
     BuildXLMLTree() detects if file is in XLML or XML format
 * 25/08/2006
     LoadTextFile() removed
 * 25/03/2005
     LoadTextFile()
 * 04/01/2005
     GetFileTime()
 * 03/12/2004
     OpenINIFile()
     CreateDirs()
 * 01/12/2004
     It's here
*/
