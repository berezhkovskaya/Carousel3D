/**
 * \file RAW.h
 * \brief RAW textures loader (2D and 3D)
 * \version 0.5.93
 * \date 18/09/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _RAWLoader_
#define _RAWLoader_

#include "Renderer/iTexture.h"
#include "Utils/Library.h"

// 0 to link statically
#define VOLUME_PROC__DYNAMIC_LINK 1

/// RAW 3D textures loader
class scriptfinal clRAWLoader: public iObject
{
public:
	clRAWLoader();
	virtual ~clRAWLoader();
	virtual void    AfterConstruction();
	//
	// clLoader<iTexture> interface
	//
	virtual bool Load( iIStream* IStream, clBitmap* Resource, bool IsAutoGradient, bool IsAutoESL );
private:
	inline LString GetRescaledFileName( const LString& CachingDir, const LString& FileName )
	{
		return MangleVolumeName( CachingDir, FileName, ".rescaled" );
	}

	inline LString GetGradientsFileName( const LString& CachingDir, const LString& FileName )
	{
		return MangleVolumeName( CachingDir, FileName, ".gradients" );
	}

	inline LString GetESLFileName( const LString& CachingDir, const LString& FileName )
	{
		return MangleVolumeName( CachingDir, FileName, ".esl" );
	}

	/// Add postfix to the file name and return the complete physical path including CachingDir
	LString MangleVolumeName( const LString& CachingDir, const LString& FileName, const LString& Postfix );

	/// Is the rescaled version of the volume available ?
	bool    IsRescaled( const LString& SrcFileName, const LString& ScaledFileName );

	/// Is the precalculated gradients map available ?
	bool    IsGradiented( const LString& SrcFileName, const LString& ESLFileName );

	/// Is the ESL map available ?
	bool    IsESLed( const LString& SrcFileName, const LString& ESLFileName );

	/**
	   Performs some lengthy operation with the volume in IStream
	   VolumeProcessor is the function (processor) poiter
	**/
	bool    ProcessVolume( void* VolumeProcessor, iIStream* IStream,
	                       clBitmap* Resource,
	                       const LString& DestName,
	                       int SizeX, int SizeY, int SizeZ, int BPP,
	                       int NewX, int NewY, int NewZ, int NewBPP );
private:
	int           FMaxSize;
#if VOLUME_PROC__DYNAMIC_LINK
	/// DLL handle
	clLibrary*    FVolumeProcLibrary;
#endif
};


#endif

/*
 * 13/05/2009
     Finally, dynamic linking with volume_proc.dll
 * 02/10/2007
     It's here
*/
