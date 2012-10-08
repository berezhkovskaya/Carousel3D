/**
 * \file iGUIRegion.h
 * \brief GUI region
 * \version 0.5.69
 * \date 07/08/2006
 * \author Sergey Kosarevsky, 2005-2006
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iGUIRegion_
#define _iGUIRegion_

#include "Core/iObject.h"

#include "Math/LRect.h"
#include "Math/LVector.h"
#include "iGUIResponder.h"
#include "Renderer/Canvas.h"

class clRenderState;

enum LRegionState
{
	REGION_MINIMIZED,  REGION_MAXIMIZED,
	REGION_MINIMIZING, REGION_MAXIMIZING,
	REGION_SHRINKING,  REGION_SHRINKED,
	REGION_DISSOLVING, REGION_DISSOLVED
};

/// Rectangular region inside internal GUI subsystem
class scriptfinal iGUIRegion: public iObject, public iGUIResponder
{
public:
	iGUIRegion(): FInLocalCoords( true ),
		FRegionTitle(),
		FRect( 0.0f ),
		FMaximizedHeight( 0 ),
		FRegionState( REGION_MAXIMIZED ),
		FNeedToCenterWidth( false ),
		FNeedToCenterHeight( false ),
		FOpacity( 0.0f ),
		FLifeTime( 0.0f ),
		FTextOffsets( 0.0f ) {};
	virtual ~iGUIRegion() {};
	//
	// iObject interface
	//

	NET_EXPORTABLE()
	SERIALIZABLE_CLASS();
	//
	// iGUIRegion
	//
	virtual bool         IsInLocalCoords() const { return FInLocalCoords; };
	virtual float        GetLifeTime() const { return FLifeTime; };
	virtual void         SetRegionState( LRegionState State ) { FRegionState = State; };
	virtual LRegionState GetRegionState() const { return FRegionState; };
	virtual void         OutTextXY( const float X, const float Y, const LString& Text, const LVector4& Color );
	virtual LVector2     Screen2Local( const LVector2& ScreenCoords ) const;
	virtual LVector2     Local2Screen( const LVector2& LocalCoords ) const;
	virtual bool         Contains( const LVector2& Pnt ) const;
	virtual bool         ContainsCaption( const LVector2& Pnt ) const;
	virtual bool         ContainsResizeCorner( const LVector2& Pnt ) const;
	virtual bool         ContainsCloseButton( const LVector2& Pnt ) const;
	virtual void         MoveTo( const LVector2& Pnt );
	virtual void         MoveRel( const LVector2& Delta );
	virtual void         SetSize( const float Width, const float Height );
	virtual float        CenterTextHorizontal( const LString& Str );
	virtual float        CenterTextVertical();
	virtual float        GetMaximizedHeight() const { return FMaximizedHeight; };
	virtual void         SetMaximizedHeight( float MaxHeight ) { FMaximizedHeight = MaxHeight; };
	virtual float        GetWidth() const { return FRect.GetWidth(); };
	virtual float        GetHeight() const { return FRect.GetHeight(); };
	virtual float        GetX1() const { return FRect.X1(); };
	virtual float        GetY1() const { return FRect.Y1(); };
	virtual float        GetX2() const { return FRect.X2(); };
	virtual float        GetY2() const { return FRect.Y2(); };
	virtual LVector4     GetUniformsVec() const { return LVector4( GetX1(), GetY1(), GetX2(), GetY2() ); };
	virtual void         UpdateUniforms( clRenderState* Shader ) const;
	virtual void         UpdateUniformsV( clRenderState* Shader, const LVector4& Values ) const;
	virtual void         SetWidth( float Width ) { FRect.SetWidth( Width ); };
	virtual void         SetHeight( float Height ) { FRect.SetHeight( Height ); };
	virtual LString      GetTitle() const { return FRegionTitle; };
	virtual void         SetTitle( const LString& Title ) { FRegionTitle = Title; };
	virtual void         SetCoords( float X, float Y, float W, float H );
	virtual void         SetCoordsV( const LVector4& C ) { SetCoords( C.X, C.Y, C.Z, C.W ); };
	virtual void         ParseCoords( const LString& Coords );
	virtual LString      GetInitialCoords() const { return FSavedCoords; };
	virtual LVector4     GetCoords() const { return LVector4( GetX1(), GetY1(), GetWidth(), GetHeight() ); };
	virtual void         SetOpacity( float Opacity ) { FOpacity = Opacity; };
	virtual float        GetOpacity() const { return FOpacity; };
	virtual void         SetTextOffsets( const LVector4& Offsets ) { FTextOffsets = Offsets; };
	virtual LVector4     GetTextOffsets() { return FTextOffsets; };
	virtual iFontProperties* GetFont() const { return NULL; };
	/* Property( Name="Coords",        Type=string, Setter=ParseCoords,   Getter=GetInitialCoords ) */
	/* Property( Name="CurrentCoords", Type=vec4,   Setter=SetCoordsV,    Getter=GetCoords        ) */
	/* Property( Name="Title",         Type=string, Setter=SetTitle,      Getter=GetTitle         ) */
	/* Property( Name="Opacity",       Type=float,  Setter=SetOpacity,    Getter=GetOpacity       ) */
	/* Property( Name="TextOffsets",   Type=vec4,   Setter=SetTextOffsets,Getter=GetTextOffsets   ) */
public:
	virtual void    Event_Timer( iGUIResponder* Source, float DeltaTime );
protected:
	virtual void    Precenter( iGUIRegion* Parent );
	virtual void    DockCoordsToThisRegion( float* X, float* Y, const float W, const float H );
	virtual void    DockSizeToThisRegion( const float X, const float Y, float* W, float* H );
private:
	LString         FSavedCoords;
	LString         FRegionTitle;
	LRect           FRect;
	float           FMaximizedHeight;
	LRegionState    FRegionState;
	bool            FNeedToCenterWidth;
	bool            FNeedToCenterHeight;
	float           FOpacity;
	float           FLifeTime;
	bool            FInLocalCoords;
	LVector4        FTextOffsets;
};

#endif

/*
 * 07/08/2006
     Inherited from iObject
 * 02/05/2005
     SetTitle()
 * 06/02/2005
     Docking
 * 05/02/2005
     Get*() helpers
 * 31/01/2005
     AcceptCoords()
     SetCoords()
 * 30/01/2005
     It's here
*/
