/**
 * \file FeedbackScreen.h
 * \brief Image-space feedback effect
 * \version 0.5.74
 * \date 06/06/2007
 * \author Sergey Kosarevsky, 2007-2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _FeedbackScreenPostprocessor_
#define _FeedbackScreenPostprocessor_

#include "Scene/Postprocess/iPostprocessor.h"

class clRenderState;
class iRenderTarget;

class scriptfinal clFeedbackScreenPostprocessor: public iPostprocessor
{
public:
	clFeedbackScreenPostprocessor();
	virtual ~clFeedbackScreenPostprocessor();

	NET_EXPORTABLE();
	SERIALIZABLE_CLASS();

	//
	// iObject interface
	//
	virtual void AfterConstruction();
	//
	// iPostprocessor interface
	//
	virtual bool             Render( iRenderTarget* RenderTarget, iRenderTarget* Input, iRenderTarget* Output );
	virtual bool             IsEnabled() const;
private:
	clRenderState*     FShader;
	clRenderState*     FShaderAdditive;
	bool               FEnabled;
	float              FEffectStartTime;
	float              FEffectDuration;
public:
	// parameters [public for property access]
	float              FUnitHitDuration;
	LVector4           FUnitHitColor1;
	LVector4           FUnitHitColor2;
	float              FUnitHitShrinkCoef;
	float              FPowerUpDuration;
	LVector4           FPowerUpColor1;
	LVector4           FPowerUpColor2;
	float              FPowerUpShrinkCoef;
	float              FPerkDuration;
	LVector4           FPerkColor1;
	LVector4           FPerkColor2;
	float              FPerkShrinkCoef;
	float              FShotDuration;
	LVector4           FShotColor1;
	LVector4           FShotColor2;
	float              FShotShrinkCoef;

	/* Property(Name="UnitHitDuration",   Type=float, FieldName=FUnitHitDuration) */
	/* Property(Name="UnitHitColor1",     Type=vec4,  FieldName=FUnitHitColor1) */
	/* Property(Name="UnitHitColor2",     Type=vec4,  FieldName=FUnitHitColor2) */
	/* Property(Name="UnitHitShrinkCoef", Type=float, FieldName=FUnitHitShrinkCoef) */

	/* Property(Name="PowerUpDuration",   Type=float, FieldName=FPowerUpDuration) */
	/* Property(Name="PowerUpColor1",     Type=vec4,  FieldName=FPowerUpColor1) */
	/* Property(Name="PowerUpColor2",     Type=vec4,  FieldName=FPowerUpColor2) */
	/* Property(Name="PowerUpShrinkCoef", Type=float, FieldName=FPowerUpShrinkCoef) */

	/* Property(Name="PerkDuration",      Type=float, FieldName=FPerkDuration) */
	/* Property(Name="PerkColor1",        Type=vec4,  FieldName=FPerkColor1) */
	/* Property(Name="PerkColor2",        Type=vec4,  FieldName=FPerkColor2) */
	/* Property(Name="PerkShrinkCoef",    Type=float, FieldName=FPerkShrinkCoef) */

	/* Property(Name="ShotDuration",      Type=float, FieldName=FShotDuration) */
	/* Property(Name="ShotColor1",        Type=vec4,  FieldName=FShotColor1) */
	/* Property(Name="ShotColor2",        Type=vec4,  FieldName=FShotColor2) */
	/* Property(Name="ShotShrinkCoef",    Type=float, FieldName=FShotShrinkCoef) */

private:
	// commands handlers
	virtual void    FeedbackScreenEffectC( const LString& Param );
};

#endif

/*
 * 06/06/2007
     IsEnabled()
 * 28/04/2007
     It's here
*/
