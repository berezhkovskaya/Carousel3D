/**
 * \file Localizer.cpp
 * \brief Localization subsystem
 * \version 0.5.99c
 * \date 25/04/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Localizer.h"
#include "Engine.h"
#include "Environment.h"
#include "Core/VFS/FileSystem.h"
#include "Core/VFS/iIStream.h"

void clLocalizer::AfterConstruction()
{
	iObject::AfterConstruction();

	FLocalePath = "Localizer";

	Env->Connect( L_EVENT_LOCALE_CHANGED, BIND( &clLocalizer::Event_LOCALE_CHANGED ) );
}

void clLocalizer::SetLocale( const LString& LocaleName )
{
	guard( "%s", LocaleName.c_str() );

	ClearLocalization();

	FLocaleName = LocaleName;

	const LString FileName( FLocalePath + "/Localizer-" + LocaleName + ".txt" );

	if ( Env->FileSystem->FileExists( FileName ) )
	{
		Env->Logger->LogP( L_NOTICE, "Reading locale from %s", FileName.c_str() );

		iIStream* Stream = Env->FileSystem->CreateFileReader( FileName );

		while ( !Stream->Eof() )
		{
			LString Line = Stream->ReadLine();

			size_t SepPos = Line.find( "~" );

			FATAL( SepPos == Line.npos, "Invalid locale translation file format: missing ~" );

			LString Text( Line.substr( 0, SepPos ) );
			LString Translation( Line.substr( SepPos + 1, Line.length() - SepPos - 1 ) );

			FTranslations[ Text ] = Translation;
		}

		delete( Stream );
	}
	else
	{
		Env->Logger->LogP( L_NOTICE, "Locale %s not found", FileName.c_str() );
	}

	this->SendAsync( L_EVENT_LOCALE_CHANGED, LEventArgs(), false );

	unguard();
}

void clLocalizer::ClearLocalization()
{
	FTranslations.clear();
}

bool clLocalizer::IsLeftToRight() const
{
	return true;
}

LString clLocalizer::LocalizeString( const LString& Str ) const
{
	std::map<LString, LString>::const_iterator i = FTranslations.find( Str );

	if ( i != FTranslations.end() ) { return i->second; }

	return Str;
}

LString clLocalizer::LocalizeID( int StrID ) const
{
	return "";
}

void EVENT_HANDLER( clLocalizer::Event_LOCALE_CHANGED )
{
}

/*
 * 25/04/2011
     Log section added
*/
