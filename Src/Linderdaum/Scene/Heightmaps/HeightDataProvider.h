/**
 * \file HeightDataProvider.h
 * \brief Height data provider for 8 and 16-bit heightmap files
 * \version 0.5.74
 * \date 03/09/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clHeightDataProvider_
#define _clHeightDataProvider_

#include "Engine.h"
#include "Core/VFS/FileSystem.h"

#include "Scene/Heightmaps/LHeightMap.h"

class iIStream;

/// Template for implementation on 8 and 16 bit height data provider
template <typename T> class clHeightDataProvider: public iHeightDataProvider
{
public:
	clHeightDataProvider( int SizeX,
	                      int SizeY,
	                      const LString& RawFileName );
	virtual ~clHeightDataProvider()
	{
		delete( FCopy );

		delete( FRawFile );
	}
	//
	// iObject interface
	//
	virtual void   AfterConstruction();
	//
	// iHeightDataProvider interface
	//
	virtual int    GetSizeX() const
	{
		return FSizeX;
	}
	virtual int    GetSizeY() const
	{
		return FSizeY;
	}
	virtual int    GetHeight( int X, int Y ) const
	{
		if ( X < 0 )
		{
			return 0;
		}

		if ( Y < 0 )
		{
			return 0;
		}

		if ( X >= FSizeX )
		{
			return 0;
		}

		if ( Y >= FSizeY )
		{
			return 0;
		}

		return FHeightData[ X + FSizeX * Y ];
	}
	virtual void   SetHeight( int X, int Y, int Height )
	{
		if ( X < 0 )
		{
			return;
		}

		if ( Y < 0 )
		{
			return;
		}

		if ( X >= FSizeX )
		{
			return;
		}

		if ( Y >= FSizeY )
		{
			return;
		}

		// copy on write
		if ( !FCopy )
		{
			FCopy = new T[FSizeX*FSizeY];

			memcpy( FCopy, FHeightData, FSizeX * FSizeY * sizeof( T ) );

			FHeightData = FCopy;
		}

		T Mask = ( 1 << ( sizeof( T ) * 8 ) ) - 1;

		FCopy[ X + FSizeX* Y ] = static_cast<T>( Height & Mask );
	}
	virtual int    GetMaximalHeight( int X1, int Y1, int X2, int Y2 ) const;
	virtual int    GetMinimalHeight( int X1, int Y1, int X2, int Y2 ) const;
private:
	iIStream*        FRawFile;
	int              FSizeX;
	int              FSizeY;
	const T*         FHeightData;
	T*               FCopy;
	LString          FRawFileName;
};

template <typename T> clHeightDataProvider<T>::clHeightDataProvider( int SizeX,
                                                                     int SizeY,
                                                                     const LString& RawFileName )
{
	FSizeX = SizeX;
	FSizeY = SizeY;
	FRawFileName = RawFileName;
}

template <typename T> void clHeightDataProvider<T>::AfterConstruction()
{
	guard( "%i,%i,%s", FSizeX, FSizeY, FRawFileName.c_str() );

	FRawFile = Env->FileSystem->CreateFileReader( FRawFileName );
	FHeightData = reinterpret_cast<const T*>( FRawFile->MapStream() );
	FCopy = NULL;

	FATAL( sizeof( T ) * FSizeX* FSizeY != FRawFile->GetFileSize(), "Invalid heightmap file size" );

	unguard();
}

template <typename T> int clHeightDataProvider<T>::GetMaximalHeight( int X1, int Y1, int X2, int Y2 ) const
{
	int MaxHeight = 0;

	for ( int j = Y1; j != Y2; ++j )
	{
		for ( int i = X1; i != X2; ++i )
		{
			int Height = GetHeight( i, j );

			if ( Height > MaxHeight )
			{
				MaxHeight = Height;
			}
		}
	}

	return MaxHeight;
}

template <typename T> int clHeightDataProvider<T>::GetMinimalHeight( int X1, int Y1, int X2, int Y2 ) const
{
	// NOTE: not 64-bit safe
	int MinHeight = 0x0FFFFFFF;

	for ( int j = Y1; j != Y2; ++j )
	{
		for ( int i = X1; i != X2; ++i )
		{
			int Height = GetHeight( i, j );

			if ( Height < MinHeight )
			{
				MinHeight = Height;
			}
		}
	}

	return MinHeight;
}

#endif

/*
 * 03/09/2007
     Fixed bug in SetHeight()
 * 30/06/2007
     SetHeight()
 * 07/02/2007
     Template class
 * 27/01/2007
     It's here
*/
