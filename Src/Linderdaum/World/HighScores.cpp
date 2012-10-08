/**
 * \file HighScores.cpp
 * \brief High scores table with autosorting/loading/saving
 * \version 0.6.00
 * \date 28/06/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "HighScores.h"
#include "Environment.h"
#include "Core/Logger.h"
#include "Core/VFS/iIStream.h"
#include "Core/VFS/iOStream.h"
#include "Core/VFS/FileSystem.h"

bool clHighScores::SaveToStream( iOStream* Stream ) const
{
	size_t Size = FHighScores.size();
	size_t CheckSum = Size;

	Stream->BlockWrite( &Size, sizeof( Size ) );

	for ( size_t i = 0; i != Size; i++ )
	{
		Stream->BlockWrite( &FHighScores[i], sizeof( FHighScores[i] ) );

		CheckSum = CheckSum ^ FHighScores[i].FScore ^ FHighScores[i].FTag;
	}

	Stream->BlockWrite( &CheckSum, sizeof( CheckSum ) );

	Env->Logger->LogP( L_DEBUG, "Saved to stream %i highscore entries", ( int )Size );
	Env->Logger->LogP( L_DEBUG, "Checksum is %i", ( int )CheckSum );

	return true;
}

bool clHighScores::LoadFromStream( iIStream* Stream )
{
	size_t Size = 0;

	Stream->BlockRead( &Size, sizeof( Size ) );

	size_t CheckSum = Size;
	size_t ActualSize = std::min( Size, FHighScores.size() );

	ClearFrom( 0 );

	for ( size_t i = 0; i != ActualSize; i++ )
	{
		if ( Stream->GetBytesLeft() < sizeof( FHighScores[i] ) )
		{
			Env->Logger->LogP( L_DEBUG, "Expected highscore value" );

			// something is wrong
			ClearFrom( 0 );

			return false;
		}

		Stream->BlockRead( &FHighScores[i], sizeof( FHighScores[i] ) );

		CheckSum = CheckSum ^ FHighScores[i].FScore ^ FHighScores[i].FTag;
	}

	// read checksum
	size_t SavedCheckSum;

	if ( Stream->GetBytesLeft() < sizeof( SavedCheckSum ) )
	{
		Env->Logger->LogP( L_DEBUG, "Missing checksum" );

		// something is wrong
		ClearFrom( 0 );

		return false;
	}

	Stream->BlockRead( &SavedCheckSum, sizeof( SavedCheckSum ) );

	// validate checksum
	if ( CheckSum != SavedCheckSum )
	{
		Env->Logger->LogP( L_DEBUG, "Wrong checksum" );

		// something is wrong
		ClearFrom( 0 );

		return false;
	}

	Env->Logger->LogP( L_DEBUG, "Loaded from stream %i highscore entries", ( int )Size );
	Env->Logger->LogP( L_DEBUG, "Checksum is %i", ( int )CheckSum );

	return true;
}

bool clHighScores::LoadFromFile( const LString& HighScoreFile )
{
	if ( Env->FileSystem->FileExists( HighScoreFile ) )
	{
		iIStream* Reader = Env->FileSystem->CreateFileReader( HighScoreFile );

		bool Res = LoadFromStream( Reader );

		delete( Reader );

		return Res;
	}

	return false;
}

bool clHighScores::SaveToFile( const LString& HighScoreFile ) const
{
	iOStream* Writer = Env->FileSystem->CreateFileWriter( HighScoreFile );

	bool Res = SaveToStream( Writer );

	delete( Writer );

	return Res;
}

size_t clHighScores::UpdateHighScores( int NewScore, int Tag )
{
	for ( size_t i = 0; i != FHighScores.size(); i++ )
	{
		if ( FHighScores[i].FScore < NewScore )
		{
			for ( size_t j = FHighScores.size() - 1; j != i; j-- ) { FHighScores[j] = FHighScores[j-1]; }

			FHighScores[i] = sHighScoreRec( NewScore, Tag );

			return i;
		}
	}

	return FHighScores.size();
}

size_t clHighScores::GetNumEntries() const
{
	return FHighScores.size();
}

clHighScores::sHighScoreRec clHighScores::GetEntry( size_t Idx ) const
{
	return ( Idx >= FHighScores.size() ) ? clHighScores::sHighScoreRec( -1, -1 ) : FHighScores[ Idx ];
}

void clHighScores::Clear( size_t Size )
{
	FHighScores.resize( Size );

	for ( size_t i = 0; i != Size; i++ ) { FHighScores[i] = clHighScores::sHighScoreRec( 0, 0 ); }
}

void clHighScores::ClearFrom( size_t StartIndex )
{
	for ( size_t i = StartIndex; i < FHighScores.size(); i++ ) { FHighScores[i] = clHighScores::sHighScoreRec( 0, 0 ); }
}

/*
 * 28/06/2011
     It's here
*/
