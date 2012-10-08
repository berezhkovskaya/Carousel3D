/**
 * \file RAW.cpp
 * \brief RAW textures loader (2D and 3D)
 * \version 0.5.93
 * \date 18/09/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "RAW.h"

#include "Engine.h"
#include "Environment.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"
#include "Renderer/iTexture.h"
#include "Renderer/iRenderContext.h"
#include "Resources/ResourcesManager.h"
#include "Utils/Library.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/CVars.h"
#include "GUI/GUIManager.h"

#if defined(OS_WINDOWS)
#include <windows.h>
#else
#define WINAPI
typedef Luint ( *LPTHREAD_START_ROUTINE ) ( void* lpThreadParameter );
#endif // OS_WINDOWS

// static link
#include "VolumeLib.h"
/*
class LinderdaumInStream : public InStream
{
public:
   LinderdaumInStream( iIStream* inStr ): inStream(inStr) {}
   virtual ~LinderdaumInStream() {}

   virtual size_t GetSize() { return inStream->GetFileSize(); }
   virtual void SeekTo( size_t Offset ) { inStream->Seek( Offset ); }

   // I know this IS bad, but it is the way everything is by now
   virtual void ReadBytes( VOLUME_PTR Buffer, size_t Size ) { inStream->BlockRead( Buffer, Size ); }
private:
   iIStream* inStream;
};

class LinderdaumOutStream : public OutStream
{
public:
   LinderdaumOutStream( iOStream* outStr ): outStream(outStr) {}
   virtual ~LinderdaumOutStream() {}

   virtual void SeekTo( size_t Offset ) { outStream->Seek( Offset ); }
   virtual void WriteBytes( const VOLUME_PTR data, size_t count ) { outStream->BlockWrite( data, count ); }

private:
   iOStream* outStream;
};
*/

clRAWLoader::clRAWLoader()
{
}

clRAWLoader::~clRAWLoader()
{
}

void clRAWLoader::AfterConstruction()
{
}

/////////////
// Volume library interoperation callbacks

int VL_ReadSeekCallback_Impl( void* DataSource, size_t Offset )
{
	// 32-bit truncation ...
	reinterpret_cast<iIStream*>( DataSource )->Seek( static_cast<int>( Offset ) );
	return 0;
}
size_t VL_ReadCallback_Impl( void* DataSource, VOLUME_PTR block, size_t Size )
{
	reinterpret_cast<iIStream*>( DataSource )->BlockRead( block, static_cast<int>( Size ) );
	return 0;
}
int VL_WriteSeekCallback_Impl( void* DataSource, size_t Offset )
{
	reinterpret_cast<iOStream*>( DataSource )->Seek( static_cast<int>( Offset ) );
	return 0;
}
size_t VL_WriteCallback_Impl( void* DataSource, VOLUME_PTR block, size_t Size )
{
	reinterpret_cast<iOStream*>( DataSource )->BlockWrite( block, static_cast<int>( Size ) );
	return 0;
}
////////////

LString clRAWLoader::MangleVolumeName( const LString& CachingDir, const LString& FileName, const LString& Postfix )
{
	LString Dir = CachingDir;

	LStr::AddTrailingChar( &Dir, PATH_SEPARATOR );

	LString FN = clResourcesManager::ConvertName( FileName );

	return Dir + FN + Postfix;
}

bool clRAWLoader::IsRescaled( const LString& SrcFileName, const LString& ScaledFileName )
{
	return Env->FileSystem->FileExistsAndNewer( SrcFileName, ScaledFileName );
}

bool clRAWLoader::IsGradiented( const LString& SrcFileName, const LString& GradFileName )
{
	return Env->FileSystem->FileExistsAndNewer( SrcFileName, GradFileName );
}

bool clRAWLoader::IsESLed( const LString& SrcFileName, const LString& ESLFileName )
{
	return Env->FileSystem->FileExistsAndNewer( SrcFileName, ESLFileName );
}

#define __USE_ENGINE_INPUT 0
#define __USE_ENGINE_OUTPUT 0
#define __THREADED_PROCESSING 0

#define __MEM_TO_MEM_PROCESSING 0

struct RescaleThreadParams_t
{
	sEnvironment* Env;
	iIStream* IStream;
	unsigned char* InMemBuf;
	void* OutWriter;
	int* Progress;
	int SizeX, SizeY, SizeZ, BPP;
	int NewX, NewY, NewZ, NewBPP;
	int ThisStart, ThisEnd;
};

unsigned long WINAPI RescaleThreadProc( void* P )
{
	RescaleThreadParams_t* Params = reinterpret_cast<RescaleThreadParams_t*>( P );

#if __MEM_TO_MEM_RESCALE
	void* InStream = VL_CreateMemoryInputStream( Params->SizeX * Params->SizeY * Params->SizeZ * Params->BPP / 8, Params->InMemBuf, 0 );
#else
#if __USE_ENGINE_INPUT
	void* InStream = VL_CreateAdapterInputStream( Params->IStream, VL_ReadSeekCallback_Impl, VL_ReadCallback_Impl );
#else
	// Use WinAPI IO routines
	LString PhysInName = /*Params->Env->FileSystem->VirtualNameToPhysical(*/Params->IStream->GetFileName()/*)*/;
	void* InStream = VL_CreateFileInputStream( PhysInName.c_str() );
#endif
#endif
	void* InVolume = VL_CreateVolumeSlicer( Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8 );
	VL_SetInputStreamForSlicer( InVolume, InStream );

	// rescale everything to NewBPP*8 bits
	VL_RescaleVolume( InVolume, Params->OutWriter,
	                  Params->NewX, Params->NewY, Params->NewZ, Params->NewBPP,
	                  Params->ThisStart, Params->ThisEnd, Params->Progress );

	VL_DeleteVolumeSlicer( InVolume );
	VL_DeleteStream( InStream );

	return 0;
}

unsigned long WINAPI GradThreadProc( void* P )
{
	RescaleThreadParams_t* Params = reinterpret_cast<RescaleThreadParams_t*>( P );

#if __MEM_TO_MEM_RESCALE
	void* InStream = VL_CreateMemoryInputStream( Params->SizeX * Params->SizeY * Params->SizeZ * Params->BPP / 8, Params->InMemBuf, 0 );
#else
#if __USE_ENGINE_INPUT
	void* InStream = VL_CreateAdapterInputStream( Params->IStream, VL_ReadSeekCallback_Impl, VL_ReadCallback_Impl );
#else
	// Use WinAPI IO routines
	LString PhysInName = /*Params->Env->FileSystem->VirtualNameToPhysical(*/Params->IStream->GetFileName()/*)*/;
	void* InStream = VL_CreateFileInputStream( PhysInName.c_str() );
#endif
#endif
	void* InVolume = VL_CreateVolumeSlicer( Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8 );
	VL_SetInputStreamForSlicer( InVolume, InStream );

	// calculate gradients for volume block
	if ( Params->BPP == 8 )
	{
		// for 8-bit data
		VL_CalculateGradients_Sobel8_UChar_Rescale( InVolume, Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8,
		                                            Params->NewX, Params->NewY, Params->NewZ, 4, Params->OutWriter, Params->ThisStart, Params->ThisEnd, Params->Progress );
	}

	if ( Params->BPP == 16 )
	{
		// for 16-bit data
		VL_CalculateGradients_Sobel16_UChar_Rescale( InVolume, Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8,
		                                             Params->NewX, Params->NewY, Params->NewZ, 4, Params->OutWriter, Params->ThisStart, Params->ThisEnd, Params->Progress );
	}

	VL_DeleteVolumeSlicer( InVolume );
	VL_DeleteStream( InStream );

	return 0;
}

unsigned long WINAPI ESLThreadProc( void* P )
{
	RescaleThreadParams_t* Params = reinterpret_cast<RescaleThreadParams_t*>( P );

#if __MEM_TO_MEM_RESCALE
	void* InStream = VL_CreateMemoryInputStream( Params->SizeX * Params->SizeY * Params->SizeZ * Params->BPP / 8, Params->InMemBuf, 0 );
#else
#if __USE_ENGINE_INPUT
	void* InStream = VL_CreateAdapterInputStream( Params->IStream, VL_ReadSeekCallback_Impl, VL_ReadCallback_Impl );
#else
	// Use WinAPI IO routines
	LString PhysInName = /*Params->Env->FileSystem->VirtualNameToPhysical(*/Params->IStream->GetFileName()/*)*/;
	void* InStream = VL_CreateFileInputStream( PhysInName.c_str() );
#endif
#endif
	void* InVolume = VL_CreateVolumeSlicer( Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8 );
	VL_SetInputStreamForSlicer( InVolume, InStream );

	// calculate ESL map for the volume block

	if ( Params->BPP == 8 )
	{
		VL_CalculateESLMap8( InVolume, Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8, ( Params->SizeX / Params->NewX ), Params->OutWriter, Params->ThisStart, Params->ThisEnd, Params->Progress );
	}

	if ( Params->BPP == 16 )
	{
		VL_CalculateESLMap16( InVolume, Params->SizeX, Params->SizeY, Params->SizeZ, Params->BPP / 8, ( Params->SizeX / Params->NewX ), Params->OutWriter, Params->ThisStart, Params->ThisEnd, Params->Progress );
	}

	VL_DeleteVolumeSlicer( InVolume );
	VL_DeleteStream( InStream );

	return 0;
}

LString GetTempDir()
{
	return "Volumes.TMP";
}
bool clRAWLoader::ProcessVolume( void* VolumeProcessor, iIStream* IStream,
                                 clBitmap* Resource,
                                 const LString& DestName,
                                 int SizeX, int SizeY, int SizeZ, int BPP,
                                 int NewX, int NewY, int NewZ, int NewBPP )
{
	unsigned char* InMemBuf = NULL;

#if __MEM_TO_MEM_PROCESSING
	// map input buffer from stream
	InMemBuf = const_cast<unsigned char*>( reinterpret_cast<const unsigned char*>( IStream->MapStreamFromCurrentPos() ) );
	// read input stream
	// allocate mem stream adapter
	unsigned char* outBuf = new unsigned char[NewX*NewY*NewZ*NewBPP];
	void* OutStream = VL_CreateMemoryOutputStream( NewX * NewY * NewZ * NewBPP, outBuf, 0 );
#else
	// Standart File-to-file processing
	LString Dir = GetTempDir();
	Env->FileSystem->CreateDirs( Dir );
#if __USE_ENGINE_OUTPUT
	// use engine's IO
	iOStream* GradientsStream = Env->FileSystem->CreateFileWriter( DestName );
	void* OutStream = VL_CreateAdapterOutputStream( GradientsStream, VL_WriteSeekCallback_Impl, VL_WriteCallback_Impl );
#else
	// use WinAPI I/O
	LString PhysOutName = Env->FileSystem->VirtualNameToPhysical( DestName );
	VL_CreateEmptyFile( PhysOutName.c_str() );
	void* OutStream = VL_CreateFileOutputStream( PhysOutName.c_str() );
#endif
#endif

	void* OutWriter = VL_CreateLinearVolumeWriter();
	// last '1' tells OutWriter to delete OutWriter on exit
	VL_SetOutputStreamForWriter( OutWriter, OutStream, 1 );
	/*
	   clCVar* NumBlocksVar = Env->Console->GetVar( "Vol.NumProcessingThreads" );
	   // number of processing threads/blocks
	   int NUM_BLOCKS =  NumBlocksVar->GetInt();
	   if (NUM_BLOCKS <= 0 ) NUM_BLOCKS = 1;
	   if (NUM_BLOCKS > 16) NUM_BLOCKS = 16;
	*/
	int NUM_BLOCKS = 16;

	// array to hold progresses for each processed volume block
	int* Progresses = new int[NUM_BLOCKS];

	RescaleThreadParams_t* ThreadParams = new RescaleThreadParams_t[NUM_BLOCKS];
#if __THREADED_PROCESSING
	// allocate memory for thread-related stuff
	int* OldProgresses = new int[NUM_BLOCKS];
	HANDLE* threadHandles = new HANDLE[NUM_BLOCKS];
#else
	int TotalProgress = 0;
#endif

	// start 'NUM_BLOCKS' threads
	for ( int i = 0 ; i < NUM_BLOCKS ; i++ )
	{
		float Increment = static_cast<float>( NewZ ) / static_cast<float>( NUM_BLOCKS );
		int ThisStart   = static_cast<int>( Increment * i     );
		int ThisEnd     = static_cast<int>( Increment * ( i + 1 ) );

		ThreadParams[i].Env = Env;
		ThreadParams[i].OutWriter = OutWriter;
		ThreadParams[i].IStream = IStream;
		// this buffer is not null only when we are mapping mem-to-mem
		ThreadParams[i].InMemBuf = InMemBuf;
		ThreadParams[i].SizeX = SizeX;
		ThreadParams[i].SizeY = SizeY;
		ThreadParams[i].SizeZ = SizeZ;
		ThreadParams[i].NewBPP = NewBPP;
		ThreadParams[i].NewX = NewX;
		ThreadParams[i].NewY = NewY;
		ThreadParams[i].NewZ = NewZ;
		ThreadParams[i].BPP = BPP;
		ThreadParams[i].Progress = &Progresses[i];
		ThreadParams[i].ThisStart = ThisStart;
		ThreadParams[i].ThisEnd = ThisEnd;

#if __THREADED_PROCESSING
		OldProgresses[i] = 0;

		// create dedicated thread for specified [Start,End] volume block
		threadHandles[i] = CreateThread( NULL, 16384, &( reinterpret_cast<LPTHREAD_START_ROUTINE>( VolumeProcessor ) ), &ThreadParams[i], NULL, NULL );
#else
		// just execute processor for the part of volume
		reinterpret_cast<LPTHREAD_START_ROUTINE>( VolumeProcessor )( &ThreadParams[i] );
		TotalProgress += Progresses[i];

		// update GUI
		clCVar* ProgressVar = Env->Console->GetVar( "Vol.ResizeProgress" );
		ProgressVar->SetFloat( ( float )TotalProgress / NewZ );
		Env->RenderFrame( 0, 0 );
		Env->GUI->Event_TIMER( L_EVENT_TIMER, LEventArgs( 0.01f ) );
#endif
	}

#if __THREADED_PROCESSING

	// wait while each processing thread finishes execution
	while ( true )
	{
		if ( WaitForMultipleObjects( NUM_BLOCKS, &threadHandles[0], true, 0/*INFINITE*/ ) )
		{
			// not yet ...
			Sleep( 100 );
			// calculate total progress
			int TotalProgress = 0;

			for ( int i = 0 ; i < NUM_BLOCKS ; i++ )
			{
				int ThisProgress = Progresses[i];

				if ( ThisProgress <= SizeZ / NUM_BLOCKS )
				{
					OldProgresses[i] = ThisProgress;
				}

				TotalProgress += OldProgresses[i];

			}

			// send progress and update GUI
			clCVar* ProgressVar = Env->Console->GetVar( "Vol.ResizeProgress" );
			ProgressVar->SetFloat( ( float )TotalProgress / NewZ );
			Env->RenderFrame( 0, 0 );
			Env->GUI->Event_TIMER( "", 0.01f, NULL, false );
		}
		else
		{
			// each thread is finished
			break;
		}
	}

	// delete auxillary thread-related stuff
	delete [] OldProgresses;
	delete [] threadHandles;
#endif

	// delete volume writer
// VL_DeleteStream(OutStream);
	VL_DeleteVolumeWriter( OutWriter );

	delete [] ThreadParams;
	delete [] Progresses;

#if __MEM_TO_MEM_PROCESSING
	iOStream* outStream = Env->FileSystem->CreateFileWriter( DestName );
	outStream->BlockWrite( outBuf, GradX * GradY * GradZ * NewBPP );
	delete outStream;

	delete [] outBuf;
#endif

#if __USE_ENGINE_OUTPUT
	delete GradientsStream;
#endif

	return true;
}

#undef min

bool clRAWLoader::Load( iIStream* IStream, clBitmap* Resource, bool IsAutoGradient, bool IsAutoESL )
{
	guard( "%s", IStream->GetFileName().c_str() );

	iIStream* RescaledStream = NULL;

	int SizeX = Resource->FBitmapParams.FWidth;
	int SizeY = Resource->FBitmapParams.FHeight;
	int SizeZ = Resource->FBitmapParams.FDepth;

	int BPP = static_cast<int>( ( IStream->GetFileSize() * 8 ) / ( SizeX * SizeY * SizeZ ) );

	FMaxSize = 256;

	// First, rescale if necessary
	if ( SizeX > FMaxSize || SizeY > FMaxSize || SizeZ > FMaxSize )
	{
		LString DestName = GetRescaledFileName( GetTempDir(), IStream->GetFileName() );

		int NewX = 0;
		int NewY = 0;
		int NewZ = 0;

//      VL_SuggestSize( SizeX, SizeY, SizeZ, BPP, 1, 512*512*512, &NewX, &NewY, &NewZ );
		NewX = SizeX;
		NewY = SizeY ;
		NewZ = SizeZ;

		int NewBPP = 1;

		if ( BPP > 16 )
		{
			NewBPP = BPP / 8;
		}

		if ( !IsRescaled( IStream->GetFileName(), DestName ) )
		{
			Env->Logger->Log( L_NOTICE, "Scaling to: " + DestName );

			ProcessVolume( reinterpret_cast<void*>( RescaleThreadProc ), IStream, Resource, DestName, SizeX, SizeY, SizeZ, BPP, NewX, NewY, NewZ, NewBPP );
		}

		IStream = Env->FileSystem->CreateFileReader( DestName );

		SizeX = NewX;
		SizeY = NewY;
		SizeZ = NewZ;
		BPP = 8;
	}

	// Second, calculate gradients if necessary
	if ( IsAutoGradient )
	{
		LString DestName = GetGradientsFileName( GetTempDir(), IStream->GetFileName() );

		int GradX = SizeX;
		int GradY = SizeY;
		int GradZ = SizeZ;

		if ( GradX > 256 )
		{
			GradX = 256;
		}

		if ( GradY > 256 )
		{
			GradY = 256;
		}

		if ( GradZ > 256 )
		{
			GradZ = 256;
		}

		if ( !IsGradiented( IStream->GetFileName(), DestName ) )
		{
			Env->Logger->Log( L_NOTICE, "Calculating gradients to: " + DestName );

			ProcessVolume( reinterpret_cast<void*>( GradThreadProc ), IStream, Resource, DestName, SizeX, SizeY, SizeZ, BPP, GradX, GradY, GradZ, 4 );
		}

		IStream = Env->FileSystem->CreateFileReader( DestName );

		SizeX = GradX;
		SizeY = GradY;
		SizeZ = GradZ;

		BPP = 32;
	}

	TODO( "pass from outside, because we might need rescaling if the volume is _too_ large" )
	// the size of ESL cube
	const int ESLFactor = 8;

	// Third, calculate ESL map, if necessary
	if ( IsAutoESL )
	{
		LString DestName = GetESLFileName( GetTempDir(), IStream->GetFileName() );

		int ESLX = SizeX / ESLFactor;
		int ESLY = SizeY / ESLFactor;
		int ESLZ = SizeZ / ESLFactor;

		if ( ESLX > 256 )
		{
			ESLX = 256;
		}

		if ( ESLY > 256 )
		{
			ESLY = 256;
		}

		if ( ESLZ > 256 )
		{
			ESLZ = 256;
		}

		FIXME( "pass modifier ESLFactor !" )

		if ( !IsESLed( IStream->GetFileName(), DestName ) )
		{
			Env->Logger->Log( L_NOTICE, "Calculating ESL map to: " + DestName );

			ProcessVolume( reinterpret_cast<void*>( ESLThreadProc ), IStream, Resource, DestName, SizeX, SizeY, SizeZ, BPP, ESLX, ESLY, ESLZ, 4 );
		}

		IStream = Env->FileSystem->CreateFileReader( DestName );

		SizeX = ESLX;
		SizeY = ESLY;
		SizeZ = ESLZ;

		// 3-component texture
		BPP = 24;
	}


	bool Float = BPP > 8;

	if ( BPP == 24 || BPP == 32 )
	{
		Float = false;   // vec3/4 with byte-valued components
	}

	LBitmapFormat BitmapFormat = sBitmapParams::SuggestBitmapFormat( Env, BPP, Float );

	sBitmapParams BMPRec = sBitmapParams( Env, SizeX, SizeY, SizeZ, BitmapFormat, L_TEXTURE_3D );

//   Lubyte* RawBGRBitmap = NULL;

	Resource->ReallocImageData( &BMPRec );

	IStream->BlockRead( Resource->FBitmapData, BMPRec.GetStorageSize() );

	//Resource->AttachBitmap( BMPRec, IStream->MapStreamFromCurrentPos() );

//   free( RawBGRBitmap );

	delete( RescaledStream );

	if ( IsAutoGradient || IsAutoESL )
	{
		delete( IStream );
	}

	return true;

	unguard();
}

/*
 * 02/03/2009
     Automatic rescaling
 * 14/11/2008
     16, 24 and 32-bit textures support
 * 08/06/2006
     More debug logging info added
 * 03/06/2006
     Bitmap format determination updated
 * 30/03/2005
     Fixed bug in cube maps loading
 * 28/03/2005
     Fixed bug in header loading
 * 22/02/2005
     It's here
*/
