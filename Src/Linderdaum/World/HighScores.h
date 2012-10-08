/**
 * \file HighScores.h
 * \brief High scores table with autosorting/loading/saving
 * \version 0.6.00
 * \date 28/06/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clHighScores_
#define _clHighScores_

#include "Platform.h"
#include "Core/iObject.h"

class iIStream;
class iOStream;

#include <vector>

/// High scores table with autosorting/loading/saving
class netexportable clHighScores: public iObject
{
public:
	struct sHighScoreRec
	{
		sHighScoreRec(): FScore( 0 ), FTag( 0 ) {};
		sHighScoreRec( int Score, int Tag ): FScore( Score ), FTag( Tag ) {};
		int FScore;
		int FTag;
	};
public:
	//
	// iObject interface
	//

	SERIALIZABLE_CLASS();
	//
	// clHighScores
	//
#pragma region Save and load in internal binary format
	scriptmethod bool    SaveToStream( iOStream* Stream ) const;
	scriptmethod bool    LoadFromStream( iIStream* Stream );

	scriptmethod bool   SaveToFile( const LString& HighScoreFile ) const;
	scriptmethod bool   LoadFromFile( const LString& HighScoreFile );
#pragma endregion

	/// Put NewScore into highscores table, resort it and return position of the NewScore
	scriptmethod size_t UpdateHighScores( int NewScore, int Tag );
	scriptmethod size_t GetNumEntries() const;
	scriptmethod sHighScoreRec GetEntry( size_t Idx ) const;
	/// Resize the table to Size entries and fill it with zeroes
	scriptmethod void   Clear( size_t Size );
private:
	/// clear starting from StartIndex up to the end
	void    ClearFrom( size_t StartIndex );
private:
	std::vector<sHighScoreRec> FHighScores;
};

#endif

/*
 * 28/06/2011
     It's here
*/
