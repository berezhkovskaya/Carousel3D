/**
 * \file Localizer.h
 * \brief Localization subsystem
 * \version 0.5.99c
 * \date 25/04/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clLocalizer_
#define _clLocalizer_

#include "Platform.h"
#include "Core/iObject.h"

#include <map>

class netexportable scriptfinal clLocalizer: public iObject
{
public:
	clLocalizer() {};
	virtual ~clLocalizer() {};
	//
	// iObject interface
	//
	virtual void AfterConstruction();
	//
	// clLocalizer
	//

	/// set current locale and load translations (if present)
	virtual void SetLocale( const LString& LocaleName );

	/// changed the path to localization files (default is Localizer folder on virtual file system)
	virtual void SetLocalePath( const LString& LocalePath ) { FLocalePath = LocalePath; };

	/// Global left-to-right/right-to-left text rendering
	virtual bool IsLeftToRight() const;

	virtual LString LocalizeString( const LString& Str ) const;
	virtual LString LocalizeID( int StrID ) const;
public:
	FWD_EVENT_HANDLER( Event_LOCALE_CHANGED );
private:
	void ClearLocalization();
private:
	LString                   FLocalePath;
	LString                   FLocaleName;
	std::map<LString, LString> FTranslations;
};

#endif

/*
 * 11/06/2010
     Log section added
*/
