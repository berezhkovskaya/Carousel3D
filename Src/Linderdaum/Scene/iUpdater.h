/**
 * \file iUpdater.h
 * \brief Generic updater interface
 * \version 0.5.93
 * \date 26/09/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iUpdater_
#define _iUpdater_

#include "Core/iObject.h"

class clScene;

/**
   \brief Used in the Scene to update states of geom instances inside the Scene.

   Can be attached & detached to any geom instance.
**/
class iUpdater: public iObject
{
public:
	iUpdater(): FIdx( -1 ),
		FScene( NULL ) {};
	virtual ~iUpdater() {};
	//
	// iUpdater
	//
	virtual void    Update( float DeltaTime ) = 0;
	virtual void    UpdateInPause( float DeltaTime ) {};
	/// attach this updater to the item Idx in the Scene
	virtual void    Attach( int Idx, clScene* Scene )
	{
		FIdx   = Idx;
		FScene = Scene;
	};
	/// return 'true' if Updater is attached to item Idx
	virtual bool    IsIndex( int Idx ) const
	{
		return FIdx == Idx;
	};
protected:
	int         FIdx;
	clScene*    FScene;
};

/// Updates animation state of the Geom instance inside the Scene
class scriptfinal clAnimationUpdater: public iUpdater
{
public:
	//
	// iUpdater interface
	//
	virtual void    Update( float DeltaTime );
};

#endif

/*
 * 26/09/2010
     UpdateInPause()
 * 04/07/2010
     It is here
*/
