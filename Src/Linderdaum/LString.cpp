/**
 * \file LString.cpp
 * \brief String utils implementation
 * \version 0.5.93
 * \date 08/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Platform.h"

#ifndef OS_WINDOWS
#  define __STDC_FORMAT_MACROS
#  include <inttypes.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>
#include <algorithm>
#include <sstream>

#include "LString.h"
#include "Math/LMath.h"

const int BUFFER = 255;

LString LStr::ToStr( int FromInt, int Digits )
{
	LString Str = ToStr( FromInt );

	size_t Len = static_cast<size_t>( Digits );

	while ( Str.length() < Len )
	{
		Str = "0" + Str;
	}

	return Str;
}


unsigned char HexCharToByte( char c )
{
	if ( c >= 'A' && c <= 'F' ) { return ( unsigned char )( 10 + ( int )( c - 'A' ) ); }

	if ( c >= 'a' && c <= 'f' ) { return ( unsigned char )( 10 + ( int )( c - 'a' ) ); }

	return ( unsigned char )( c - '0' );
}

unsigned char HexToByte( const LString& s )
{
	unsigned char res = 0x0;

	if ( s.length() > 0 )
	{
		res = HexCharToByte( s[0] );
	}

	if ( s.length() > 1 )
	{
		res += ( unsigned char )( HexCharToByte( s[1] ) << 4 );
	}

	return res;
}

int64_t LStr::StrToHex( const LString& s )
{
	int64_t i = 0x0;

	std::stringstream Convert( s );

	Convert >> std::hex >> i;

	return i;

	/*
	int64_t w = 0x0;
	// sscanf(s.c_str(), "%x", &w);

	unsigned char b;

	for ( int i = 0 ; i != ( int )s.length() ; i++ )
	{
	   b = HexCharToByte( s[i] );
	   w = ( w << 4 ) + b;
	}

	return w;
	*/
}

LString LStr::ToStr( uint32_t FromUInt32 )
{
	char buf[BUFFER];

	Lsnprintf( buf, BUFFER - 1, "%i", FromUInt32 );

	return LString( buf );
}

LString LStr::ToStr( int32_t FromInt32 )
{
	char buf[BUFFER];

	Lsnprintf( buf, BUFFER - 1, "%i", FromInt32 );

	return LString( buf );
}
LString LStr::ToStrHex( uint64_t FromInt64 )
{
	char buf[BUFFER];

#ifdef OS_WINDOWS
#if _MSC_VER >= 1400
	_ui64toa_s( FromInt64, buf, BUFFER - 1, 16 );
#else
	_ui64toa( FromInt64, buf, 16 );
#endif
#else
	Lsnprintf( buf, BUFFER - 1, "%"PRIu64"x", FromInt64 );
#endif

	return "0x" + LStr::GetUpper( LString( buf ) );
}

LString LStr::ToStr( uint64_t FromUInt64 )
{
	char buf[BUFFER];

#ifdef OS_WINDOWS
#if _MSC_VER >= 1400
	_ui64toa_s( FromUInt64, buf, BUFFER - 1, 10 );
#else
	_ui64toa( FromUInt64, buf, 10 );
#endif
#else
	Lsnprintf( buf, BUFFER - 1, "%"PRIu64, FromUInt64 );
#endif

	return LString( buf );
}

LString LStr::ToStr( int64_t FromInt64 )
{
	char buf[BUFFER];

#ifdef OS_WINDOWS
#if _MSC_VER >= 1400
	_i64toa_s( FromInt64, buf, BUFFER - 1, 10 );
#else
	_i64toa( FromInt64, buf, 10 );
#endif
#else
	Lsnprintf( buf, BUFFER - 1, "%"PRIi64, FromInt64 );
#endif

	return LString( buf );
}

LString LStr::ToStr( double FromDouble, int Digits )
{
	char buf[BUFFER];

	LString Format = "%." + LStr::ToStr( Digits ) + "f";

	Lsnprintf( buf, BUFFER - 1, Format.c_str(), FromDouble );

	Format.assign( buf );

	return Format;
}

LString LStr::ToStr_Bool( bool Value )
{
	return Value ? LString( "True" ) : LString( "False" );
}

LString LStr::ToStrEpsilon( double FromDouble )
{
	for ( int i = 0; i != 10; i++ )
	{
		double Error = abs( FromDouble - Linderdaum::Math::Round( FromDouble, i ) );

		if ( Error < Linderdaum::Math::EPSILON ) { return ToStr( FromDouble, i ); }
	}

	return ToStr( FromDouble, 10 );
}

LStringBuffer::LStringBuffer( const LString& Str )
{
	strncpy( FBuffer, Str.c_str(), LStr::BufferSize - 1 );
}

LStringBuffer::LStringBuffer( const char* Str )
{
	strncpy( FBuffer, Str, LStr::BufferSize - 1 );
}

void LStr::AppendInt( LString* Str, int I, int Format )
{
	LString Buff = LStr::ToStr( I );

	size_t LeadZeroes = Format - Buff.length();

	for ( size_t i = 1; i <= LeadZeroes; ++i )
	{
		Str->append( "0" );
	}

	Str->append( Buff );
}

void LStr::AddTrailingChar( LString* Str, char Ch )
{
	if ( ( !Str->empty() ) && ( Str->data()[Str->length()-1] == Ch ) )
	{
		return;
	}

	Str->push_back( Ch );
}

bool LStr::IsLastChar( const LString& Str, char Char )
{
	if ( Str.empty() )
	{
		return false;
	}

	return ( Str.at( Str.length() - 1 ) == Char );
}

bool LStr::IsFirstChar( const LString& Str, char Char )
{
	if ( Str.empty() )
	{
		return false;
	}

	return ( Str.at( 0 ) == Char );
}

void LStr::TrimSpaces( LString* Str )
{
	const char* CStr  = Str->c_str();
	const char* CStrj = Str->c_str() + Str->length() - 1;

	/// return if string is unmodified
	if ( *CStr && !LStr::IsSeparator( *CStr ) && !LStr::IsSeparator( *CStrj ) )
	{
		return;
	}

	while ( *CStr && LStr::IsSeparator( *CStr ) )
	{
		CStr++;
	}

	while ( CStrj > CStr && LStr::IsSeparator( *CStrj ) )
	{
		CStrj--;
	}

	Str->assign( LString( CStr, CStrj + 1 ) );
}

void LStr::TrimBrackets( LString* Str )
{
	LString T = *Str;

	LStr::TrimSpaces( &T );

	if ( T.size() < 2 )
	{
		return;
	}

	if ( T.at( 0 ) == '"' && T.at( T.length() - 1 ) == '"' )
	{
		T = T.substr( 1, T.length() - 2 );

		if ( T.find( "\"" ) == T.npos )
		{
			Str->assign( T );
		}
	}
}

void LStr::ToUpper( LString* Str )
{
	size_t Size = Str->length();

	for ( size_t i = 0; i != Size; i++ )
	{
		if ( LStr::IsLower( Str->data()[i] ) )
		{
			Str->at( i ) -= ( 'a' - 'A' );
		}
	}
}

void LStr::ToLower( LString* Str )
{
	size_t Size = Str->length();

	for ( size_t i = 0; i != Size; i++ )
	{
		if ( LStr::IsUpper( Str->data()[i] ) )
		{
			Str->at( i ) += ( 'a' - 'A' );
		}
	}
}

void LStr::ReplaceAll( LString* Str, char OldChar, char NewChar )
{
	for ( size_t i = 0; i < Str->length(); ++i )
	{
		if ( Str->data()[i] == OldChar )
		{
			Str->at( i ) = NewChar;
		}
	}
}

LString LStr::ReplaceAll( const LString& Str, char OldChar, char NewChar )
{
	LString TempStr( Str );

	LStr::ReplaceAll( &TempStr, OldChar, NewChar );

	return TempStr;
}

LString LStr::Populate( const char Ch, int Count )
{
	LString TempStr;

	for ( int i = 0; i != Count; ++i )
	{
		TempStr.push_back( Ch );
	}

	return TempStr;
}

void LStr::PadRight( LString* Str, size_t Len, char Pad )
{
	while ( Str->length() < Len )
	{
		Str->push_back( Pad );
	}
}

LString LStr::GetPadRight( const LString& Str, size_t Len, char Pad )
{
	LString TempStr( Str );

	LStr::PadRight( &TempStr, Len, Pad );

	return TempStr;
}

void LStr::PadLeft( LString* Str, size_t Len, char Pad )
{
	while ( Str->length() < Len )
	{
		*Str = Pad + *Str;
	}
}

LString LStr::GetPadLeft( const LString& Str, size_t Len, char Pad )
{
	LString TempStr( Str );

	LStr::PadLeft( &TempStr, Len, Pad );

	return TempStr;
}

LString LStr::GetUpper( const LString& Str )
{
	LString TempStr( Str );

	LStr::ToUpper( &TempStr );

	return TempStr;
}

LString LStr::GetLower( const LString& Str )
{
	LString TempStr( Str );

	LStr::ToLower( &TempStr );

	return TempStr;
}

LString LStr::GetQuotedString( const LString& Str )
{
	LString TempStr = '"' + Str + '"';

	return TempStr;
}

LString LStr::GetToken_NoTrim( const LString& Str, size_t Num )
{
	size_t TokenNum = 0;
	size_t i = 0;
	size_t j = 0;

	while ( i != Str.length() )
	{
		// bypass spaces & delimiting chars
		while ( i != Str.length() && LStr::IsSeparator( Str.at( i ) ) )
		{
			++i;
		}

		if ( i == Str.length() )
		{
			return "";
		}

		bool InsideQuotes = ( Str.at( i ) == '\"' );

		if ( InsideQuotes )
		{

			// inside quotes
			j = ++i;                     // exclude first " from token

			while ( j != Str.length() && Str.at( j ) != '\"' )
			{
				j++;
			}
		}
		else
		{
			// outside quotes
			j = i;

			while ( j != Str.length() && !LStr::IsSeparator( Str.at( j ) ) )
			{
				j++;
			}
		}

		// extract token
		if ( i != j )
		{
			TokenNum++;

			if ( TokenNum == Num )
			{
				LString TokenFound = Str.substr( i, j - i );

				// no space trimming !
				return TokenFound;
			}

			i = j;

			if ( i != Str.length() )
			{
				++i;   // exclude last " from token, handle EOL
			}
		}
	}

	return LString( "" );
}

LString LStr::GetFromToken( const LString& Str, size_t Num )
{
	size_t TokenNum = 0;
	size_t i = 0;
	size_t j = 0;

	while ( i != Str.length() )
	{
		// bypass spaces & delimiting chars
		while ( i != Str.length() && LStr::IsSeparator( Str.at( i ) ) )
		{
			++i;
		}

		if ( i == Str.length() )
		{
			return "";
		}

		bool InsideQuotes = ( Str.at( i ) == '\"' );

		if ( InsideQuotes )
		{
			// inside quotes
			j = ++i;                     // exclude first " from token

			while ( j != Str.length() && Str.at( j ) != '\"' )
			{
				j++;
			}
		}
		else
		{
			// outside quotes
			j = i;

			while ( j != Str.length() && !LStr::IsSeparator( Str.at( j ) ) )
			{
				j++;
			}
		}

		// extract token
		if ( i != j )
		{
			TokenNum++;

			if ( TokenNum == Num )
			{
				LString TokenFound = Str.substr( j, Str.length() - 1 );

				LStr::TrimSpaces( &TokenFound );

				return TokenFound;
			}

			i = j;

			if ( i != Str.length() )
			{
				++i;   // exclude last " from token, handle EOL
			}
		}
	}

	return LString( "" );
}

void LStr::FastSplitLine( size_t FromToken, size_t ToToken, const LString& Str, clStringsVector& Components, bool ShouldTrimSpaces )
{
	size_t TokenNum = 0;
	size_t Offset   = FromToken - 1;

	const char* CStr  = Str.c_str();
	const char* CStrj = Str.c_str();

	while ( *CStr )
	{
		// bypass spaces & delimiting chars
		while ( *CStr && LStr::IsSeparator( *CStr ) )
		{
			CStr++;
		}

		if ( !*CStr )
		{
			return;
		}

		bool InsideQuotes = ( *CStr == '\"' );

		if ( InsideQuotes )
		{
			// inside quotes
			for ( CStrj = ++CStr; *CStrj && *CStrj != '\"'; CStrj++ );
		}
		else
		{
			// outside quotes
			for ( CStrj = CStr; *CStrj && !LStr::IsSeparator( *CStrj ); CStrj++ );
		}

		// extract token
		if ( CStr != CStrj )
		{
			TokenNum++;

			// store each token found
			if ( TokenNum >= FromToken )
			{
				Components[ TokenNum-Offset ].assign( CStr, CStrj );

				if ( ShouldTrimSpaces )
				{
					LStr::TrimSpaces( &Components[ TokenNum-Offset ] );
				}

				// proceed to next token
				if ( TokenNum >= ToToken )
				{
					return;
				}
			}

			CStr = CStrj;

			if ( *CStr )
			{
				CStr++;   // exclude last " from token, handle EOL
			}
		}
	}
}

void LStr::LineToMultiline( const LString& Str, clStringsVector& Out )
{
	Out.clear();

	LString Code = Str;

	// HACK !
	Code += "\n";

	LString s = "";

	// iterate characters, skip EOLN markers
	for ( size_t i = 0 ; i < Code.length() ; i++ )
	{
		bool isEoln = false;
		bool isLocalEoln = false;

		char ch = Code.at( i );

		// skip EOLNs
		while ( ch == 10 || ch == 13 )
		{
			if ( i < Code.length() - 1 )
			{
				i++;
				ch = Code.at( i );
			}
			else
			{
				isEoln = true;
				break;
			}

			isLocalEoln = true;
		}

		if ( isLocalEoln )
		{
			Out.push_back( s );

			if ( ch != 10 && ch != 13 )
			{
				s = ch;
			}
			else
			{
				s = "";
			}
		}
		else
		{
			s += ch;
		}
	}
}

LString LStr::GetFormattedString( const LString& Str, const int ExpandZeroes, const int Num )
{
	LString OutputString = Str;

	size_t Position = OutputString.find( "%i" );

	if ( Position > 0 )
	{
		LString S = LStr::ToStr( Num );

		while ( S.length() < ( size_t )ExpandZeroes )
		{
			S = "0" + S;
		}

		OutputString.erase( Position, 2 );
		OutputString.insert( Position, S );
	}

	return OutputString;
}

LString LStr::ExtractKeyName( const LString& Str, const char Separator )
{
	size_t SeparatorPos = Str.find( Separator, 0 );

	LString KeyName = ( SeparatorPos == std::string::npos ) ? Str : LString( Str.substr( 0, SeparatorPos ) );

	LStr::TrimSpaces( &KeyName );

	return KeyName;
}

LString LStr::ExtractKeyValue( const LString& Str, const char Separator )
{
	size_t SeparatorPos = Str.find( Separator, 0 );

	LString KeyValue = ( SeparatorPos == std::string::npos ) ? "" : LString( Str.substr( SeparatorPos + 1, Str.length() - SeparatorPos - 1 ) );

	LStr::TrimSpaces( &KeyValue );

	return KeyValue;
}

void LStr::pop_back( LString* Str )
{
	if ( !Str->empty() )
	{
		Str->assign( Str->substr( 0, Str->length() - 1 ) );
	}
}

void LStr::pop_front( LString* Str )
{
	if ( !Str->empty() )
	{
		Str->assign( Str->substr( 1, Str->length() - 1 ) );
	}
}

bool LStr::IsMultiline( const LString& Str )
{
	for ( const char* Ch = Str.c_str(); *Ch; Ch++ )
	{
		if ( *Ch == 13 || *Ch == 10 )
		{
			return true;
		}
	}

	return false;
}

LString LStr::EscapifyString( const LString& Str )
{
	LString res = "";

	for ( size_t i = 0 ; i < Str.length() ; i++ )
	{
		if ( Str.at( i ) == '"' )
		{
			// add ESC-sequence marker
			res += "\\";
		}

		res += Str.at( i );
	}

	return res;
}

LString LStr::CopyFromPosToChar( const LString& Str, size_t Pos, char Char, size_t* EndedAtCharPos )
{
	size_t i   = Pos;
	size_t Len = Str.length();

	while ( i < Len && Str[i] != Char )
	{
		i++;
	}

	if ( EndedAtCharPos ) { *EndedAtCharPos = i; }

	return Str.substr( Pos, i - Pos );
}

LString LStr::CopyFromPosToCharAndTrim( LString* Str, size_t Pos, char Char, bool TrimChar )
{
	size_t FoundPos = 0;

	LString Result = CopyFromPosToChar( *Str, Pos, Char, &FoundPos );

	if ( TrimChar )
	{
		if ( FoundPos < Str->length() ) { FoundPos++; }
	}

	Str->assign( CopyFromPosToEnd( *Str, FoundPos ) );

	TrimSpaces( Str );

	return Result;
}

LString LStr::CopyFromPosToEnd( const LString& Str, size_t Pos )
{
	return Str.substr( Pos, Str.length() - Pos );
}

LString LStr::ReplaceAllSubStr( const LString& Str, const LString& OldSubStr, const LString& NewSubStr )
{
	LString Result = Str;

	for ( size_t Pos = Result.find( OldSubStr ); Pos != LString::npos; Pos = Result.find( OldSubStr ) )
	{
		Result.replace( Pos, OldSubStr.length(), NewSubStr );
	}

	return Result;
}

void LStr::Format( LString* Pattern, ... )
{
	char buf[BUFFER];

	va_list p;
	va_start( p, Pattern );

	Lvsnprintf( buf, BUFFER - 1, Pattern->c_str(), p );

	va_end( p );

	Pattern->assign( buf );
}

LString LStr::GetFormatted( const char* Pattern, ... )
{
	char buf[BUFFER];

	va_list p;
	va_start( p, Pattern );

	Lvsnprintf( buf, BUFFER - 1, Pattern, p );

	va_end( p );

	return LString( buf );
}

/*
 * 08/11/2010
     Format()
     GetFormatted()
 * 17/10/2010
     ReplaceAllSubStr()
 * 28/09/2010
     ToUpperStr() removed ( duplicate with GetUpper() )
     Fixed pointer bug in PadLeft()
 * 20/06/2010
     PadLeft()
     PadRight()
 * 18/05/2010
     CopyFromPosToChar()
 * 23/09/2007
     VC 2003 compatibility fixes
 * 25/06/2007
     Escapify(), IsMultiline()
 * 01/05/2007
     _itoa() changed to _snprintf()
 * 04/04/2007
     New constructor added LString(GUID FromGUID)
 * 13/01/2007
     Populate()
 * 24/07/2006
     New constructor added LString(unsigned char*, int)
 * 08/07/2006
     Tiny fix to make it compilable via GCC
 * 13/06/2006
     ExtractKeyName()
     ExtractKeyValue()
 * 07/08/2005
     sprintf() changed to snprintf()
 * 10/02/2005
     Bug fixed in TrimBrackets()
 * 04/02/2005
     pop_back()
     pop_front()
 * 09/01/2005
     GetToken() rewritten from scratch
     TrimSpaces() rewritten from scratch
 * 21/12/2004
     GetToken()
 * 17/12/2004
     ReplaceAll()
 * 07/12/2004
     Constructor LString(double, int) rewritten to use stdlib
 * 28/11/2004
     ToUpper()
     CharIsLower()
 * 25/11/2004
     It's here
*/
