/*
   (Part of Linderdaum Engine)
   Version 0.5.84
   (21/11/2007)
   (C) Sergey Kosarevsky, 2007
   support@linderdaum.com
   http://www.linderdaum.com
*/

#include "Environment.h"

#include "Resources/VFW.h"

#include "Engine.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/VFS/FileSystem.h"
#include "Utils/Viewport.h"
#include "Math/LMath.h"

#ifdef L_VIDEO_USE_VFW
#include <windows.h>
#include <vfw.h>
#endif

clVFWVideoDecoder::clVFWVideoDecoder()
{
}

void clVFWVideoDecoder::OpenStream( const LString& FileName )
{
#ifdef L_VIDEO_USE_VFW
	guard( "%s", FileName.c_str() );

	AVIFileInit();

	LString PhysFileName = Env->FileSystem->VirtualNameToPhysical( FileName );

	Env->Logger->Log( L_NOTICE, "Opening video file: " + PhysFileName );
//   PhysFileName = "E:\\Sergey\\Engine5\\Apps\\AcesHigh\\Data\\test.avi";

	if ( AVIStreamOpenFromFile( &FAVI,
	                            PhysFileName.c_str(),
	                            streamtypeVIDEO,
	                            0,
	                            OF_READ,
	                            NULL ) != 0 )
	{
		Env->Logger->Log( L_WARNING, "Error code: " + LStr::ToStr( static_cast<int>( GetLastError() ) ) );
		FATAL_MSG( "Failed to open the AVI stream" );
	}

	AVIStreamInfo( FAVI, &FPSI, sizeof( FPSI ) );

	FInternalWidth  = FPSI.rcFrame.right  - FPSI.rcFrame.left;
	FInternalHeight = FPSI.rcFrame.bottom - FPSI.rcFrame.top;

	FWidth  = ::Linderdaum::Math::GetNextPowerOf2( FInternalWidth );
	FHeight = ::Linderdaum::Math::GetNextPowerOf2( FInternalHeight  );

	FLastFrame = AVIStreamLength( FAVI );

	// Calculate Rough Milliseconds Per Frame
	FMPF = AVIStreamSampleToTime( FAVI, FLastFrame ) / FLastFrame;

	FFrameSeconds = 0.001f * FMPF;

	FHDD = DrawDibOpen();
	FDC  = CreateCompatibleDC( 0 );

	FBMIH.biSize        = sizeof ( BITMAPINFOHEADER );
	FBMIH.biPlanes      = 1;
	FBMIH.biBitCount    = 24;
	FBMIH.biWidth       = FWidth;
	FBMIH.biHeight      = FHeight;
	FBMIH.biCompression = BI_RGB;

	FBitmap = CreateDIBSection( FDC,
	                            ( BITMAPINFO* )( &FBMIH ),
	                            DIB_RGB_COLORS,
	                            ( void** )( &FData ),
	                            NULL,
	                            NULL );

	SelectObject( FDC, FBitmap );

	FPGF = AVIStreamGetFrameOpen( FAVI, NULL );

	FATAL( FPGF == NULL, "Failed to get the AVI frame" );

	unguard();
#endif
}

clVFWVideoDecoder::~clVFWVideoDecoder()
{
#ifdef L_VIDEO_USE_VFW
	DeleteObject( FBitmap );
	DrawDibClose( FHDD );
	AVIStreamGetFrameClose( FPGF );
	AVIStreamRelease( FAVI );
	AVIFileExit();
#endif
}

int clVFWVideoDecoder::GetMaxFrames() const
{
#ifdef L_VIDEO_USE_VFW
	return FLastFrame;
#else
	return 0;
#endif
}

void* clVFWVideoDecoder::ExtractFrame( int FrameNum )
{
#ifdef L_VIDEO_USE_VFW
	LPBITMAPINFOHEADER LPBI;

	if ( FrameNum > FLastFrame )
	{
		FrameNum = 0;
	}

	LPBI = ( LPBITMAPINFOHEADER )AVIStreamGetFrame( FPGF, FrameNum );

	FPData = ( char* )LPBI + LPBI->biSize + LPBI->biClrUsed * sizeof( RGBQUAD );

	DrawDibDraw( FHDD, FDC, 0, 0, FWidth, FHeight, LPBI, FPData, 0, 0, FInternalWidth, FInternalHeight, 0 );

	return FData;
#else
	return 0;
#endif
}


clVFWVideoEncoder::clVFWVideoEncoder()
#ifdef L_VIDEO_USE_VFW
	: FAVIStream( NULL ),
	  FAVICompStream( NULL ),
	  FAVIFile( NULL )
#endif
{
#ifdef L_VIDEO_USE_VFW
	AVIFileInit();
#endif
}

clVFWVideoEncoder::~clVFWVideoEncoder()
{
#ifdef L_VIDEO_USE_VFW

	if ( FAVIStream ) { AVIStreamRelease( FAVIStream ); }

	if ( FAVICompStream ) { AVIStreamRelease( FAVICompStream ); }

	if ( FAVIFile ) { AVIFileRelease( FAVIFile ); }

	AVIFileExit();
#endif
}

bool clVFWVideoEncoder::CreateStream( const LString& FileName,
                                      int Width,
                                      int Height,
                                      int FrameRate )
{
#ifdef L_VIDEO_USE_VFW
	guard( "%s,%i,%i,%i", FileName.c_str(), Width, Height, FrameRate );

	if ( FileName.empty() )
	{
		Env->Console->DisplayError( "CreateStream(): no file name was given" );
		return false;
	}

	memset( &FBMIH, 0, sizeof( BITMAPINFOHEADER ) );
	FBMIH.biSize        = sizeof( BITMAPINFOHEADER );
	FBMIH.biWidth       = Width;
	FBMIH.biHeight      = Height;
	FBMIH.biPlanes      = 1;
	FBMIH.biBitCount    = 24;
	FBMIH.biSizeImage   = Width * Height * 3;
	FBMIH.biCompression = BI_RGB;

	LString PhysFileName = Env->FileSystem->VirtualNameToPhysical( FileName );

	Env->Logger->Log( L_NOTICE, "Creating video file: " + PhysFileName );

	HRESULT Res = AVIFileOpen( &FAVIFile,
	                           PhysFileName.c_str(),
	                           OF_WRITE | OF_CREATE,
	                           NULL );

	if ( Res != AVIERR_OK )
	{
		switch ( Res )
		{
			case AVIERR_BADFORMAT:
				Env->Console->DisplayError( "AVIERR_BADFORMAT" );
				break;
			case AVIERR_MEMORY:
				Env->Console->DisplayError( "AVIERR_MEMORY" );
				break;
			case AVIERR_FILEREAD:
				Env->Console->DisplayError( "AVIERR_FILEREAD" );
				break;
			case AVIERR_FILEOPEN:
				Env->Console->DisplayError( "AVIERR_FILEOPEN" );
				break;
			case REGDB_E_CLASSNOTREG:
				Env->Console->DisplayError( "REGDB_E_CLASSNOTREG" );
				break;
			default:
				Env->Console->DisplayError( "Unknown AVIFileOpen() error" );
				break;
		}

		return false;
	}

	AVISTREAMINFO Header;

	memset( &Header, 0, sizeof( Header ) );
	Header.fccType               = streamtypeVIDEO;
	Header.fccHandler            = 0;
	Header.dwScale               = 1;
	Header.dwRate                = FrameRate;
	Header.dwSuggestedBufferSize = FBMIH.biSizeImage;
	SetRect( &Header.rcFrame, 0, 0, Width, Height );

	Res = AVIFileCreateStream( FAVIFile, &FAVIStream, &Header );

	if ( Res != AVIERR_OK )
	{
		Env->Console->DisplayError( "AVIFileCreateStream() failed" );

		return false;
	}

	AVICOMPRESSOPTIONS Compression;
	AVICOMPRESSOPTIONS FAR* CompressionPtr[1] = {&Compression};

	memset( &Compression, 0, sizeof( Compression ) );

	if ( !AVISaveOptions( NULL, 0, 1, &FAVIStream, ( LPAVICOMPRESSOPTIONS FAR* )&CompressionPtr ) )
	{
		AVISaveOptionsFree( 1, ( LPAVICOMPRESSOPTIONS FAR* )&CompressionPtr );

		return false;
	}

	Res = AVIMakeCompressedStream( &FAVICompStream, FAVIStream, &Compression, NULL );

	if ( Res != AVIERR_OK )
	{
		Env->Console->DisplayError( "AVIMakeCompressedStream() failed" );

		return false;
	}

	Res = AVISaveOptionsFree( 1, ( LPAVICOMPRESSOPTIONS FAR* )&CompressionPtr );

	if ( Res != AVIERR_OK )
	{
		Env->Console->DisplayError( "AVISaveOptionsFree() failed" );

		return false;
	}

	Res = AVIStreamSetFormat( FAVICompStream,
	                          0,
	                          &FBMIH,
	                          FBMIH.biSize + FBMIH.biClrUsed * sizeof( RGBQUAD ) );

	if ( Res != AVIERR_OK )
	{
		Env->Console->DisplayError( "AVIStreamSetFormat() failed" );

		return false;
	}

	FFrame = 0;

	return true;

	unguard();
#else
	return false;
#endif
}

void clVFWVideoEncoder::AddVideoFrame( void* Frame )
{
#ifdef L_VIDEO_USE_VFW
	/*   HRESULT Res =*/
	AVIStreamWrite( FAVICompStream,
	                FFrame,
	                1,
	                Frame,
	                FBMIH.biSizeImage,
	                AVIIF_KEYFRAME,
	                NULL,
	                NULL );

	FFrame++;
#endif
}

/*
 * 21/11/2007
     It's here
*/
