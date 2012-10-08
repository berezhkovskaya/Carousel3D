/**
 * \file iActor.h
 * \brief Game actor
 * \version 0.6.00
 * \date 08/01/2011
 * \author Sergey Kosarevsky, 2005-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iActor_
#define _iActor_

#include "Platform.h"
#include "Math/LMatrix.h"
#include "Core/iObject.h"

class clScene;
class clWorld;

/// Base class for all game objects
class netexportable iActor: public iObject
{
public:
	iActor();
	virtual ~iActor() {};
	//
	// iObject interface
	//

	SERIALIZABLE_CLASS();
	//
	// iActor
	//
	virtual clWorld*   GetWorld() const { return FWorld; };
	virtual clScene*   GetScene() const;
	virtual void       LeaveWorld();
	virtual void       UpdateActor( float DeltaSeconds ) = 0;
	virtual void       RenderOverlay( const LMatrix4& CameraProjection, const LMatrix4& CameraTransform ) {};
	virtual bool       IsUpdatedInPause() const { return false; };
	virtual bool       IsPersistent() const { return false; };
	virtual int        GetOrder() const { return FOrder; };
	virtual void       SetOrder( int Order ) { FOrder = Order; };

#pragma region Performance statistics
	virtual void       SetTimeProfile( double Time ) { FTimeProfile = Time; };
	virtual double     GetTimeProfile() const { return FTimeProfile; };
#pragma endregion

#pragma region Actor events
	virtual void    Actor_EnteredWorld( clWorld* World );
	virtual void    Actor_LeavedWorld() {};
	virtual void    Actor_PostBeginPlay() {};
	virtual void    Actor_SomeoneEnteredWorld( iActor* Actor ) {};
	virtual void    Actor_SomeoneLeavedWorld( iActor* Actor ) {};
	virtual void    Actor_Event( const LString& EventName, bool Trigger, const LString& InstigatorID ) {};
	virtual void    SendEvent( const LString& EventName, bool Trigger ) const;
	virtual bool    ReceiveSelfEvents() const { return false; };
#pragma endregion

private:
	double      FTimeProfile;

	/// world this actor belongs to
	clWorld*    FWorld;

#pragma region Properties
	int         FOrder;
	/* Property(Name="Order", Type=int,  Getter=GetOrder,   Setter=SetOrder ) */
#pragma endregion
};

#endif

/*
 * 08/01/2011
     Actor_SomeoneEnteredWorld()
 * 20/10/2007
     IsComposite()
 * 18/07/2007
     SendEvent()
     Actor_Event()
     ReceiveSelfEvents()
 * 24/04/2007
     RenderOverlay() recieves camera as a parameter
 * 13/04/2007
     IDs from iObject used
 * 04/04/2007
     IsPersistent()
 * 27/02/2007
     SetTimeProfile()
     GetTimeProfile()
 * 14/02/2007
     Actor_PostBeginPlay()
 * 13/02/2007
     Actor_SomeoneLeavedWorld()
 * 12/02/2007
     SaveToXLMLStream()
 * 15/01/2007
     IsUpdatedInPause()
 * 06/01/2007
     RenderOverlay()
 * 05/05/2005
     GetOwner()
 * 20/02/2005
     It's here
*/
