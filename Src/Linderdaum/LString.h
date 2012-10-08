/**
 * \file LString.h
 * \brief String utils based on std::string
 * \version 0.5.93
 * \date 08/11/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LString_
#define _LString_

#include <string>
#include <vector>
#include <stdlib.h>
#include <memory.h>

#ifndef _MSC_VER
#  include <stdint.h>
#  define LSTR_PACK
#else
typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;
typedef int int32_t;
typedef unsigned int uint32_t;
// to make GCC happy with nested structs
#  define LSTR_PACK __attribute__((packed,aligned(1)))
#endif

#if _MSC_VER >= 1400
#  define Lvsnprintf _vsnprintf_s
#  define Lsnprintf  _snprintf_s
#else
#  define Lvsnprintf vsnprintf
#  define Lsnprintf  snprintf
#endif

#define LString std::string

/// Strings utility library based on std::string
namespace LStr
{
	const int BufferSize    = 512;
	const char Separators[] = { ' ', 9 };

	typedef std::vector<LString> clStringsVector;
}

/// internal representation of strings inside LinderScript virtual machine
struct
#ifdef __GNUC__
		LSTR_PACK
#endif
		LStringBuffer
{
public:
	LStringBuffer() { FBuffer[0] = 0; FBuffer[ LStr::BufferSize-1 ] = 0; };
	explicit LStringBuffer( const LString& Str );
	explicit LStringBuffer( const char* Str );
	//
	// LStringBuffer
	//
	inline LString    ToString() const
	{
		return LString( FBuffer );
	}
private:
	char    FBuffer[ LStr::BufferSize ];
};

namespace LStr
{
	inline LString  GetTrimmedSpaces( const LString& Str );
	void            TrimSpaces( LString* Str );
	/// remove outermost '"' from string
	void            TrimBrackets( LString* Str );
	void            ToUpper( LString* Str );
	LString         GetUpper( const LString& Str );
	void            ToLower( LString* Str );
	LString         GetLower( const LString& Str );
	void            AppendInt( LString* Str, int I, int Format );
	void            AddTrailingChar( LString* Str, char Ch );
	void            PadRight( LString* Str, size_t Len, char Pad );
	LString         GetPadRight( const LString& Str, size_t Len, char Pad );
	void            PadLeft( LString* Str, size_t Len, char Pad );
	LString         GetPadLeft( const LString& Str, size_t Len, char Pad );
	LString         GetQuotedString( const LString& Str );
	inline LString  GetToken( const LString& Str, size_t Num );
	LString         GetToken_NoTrim( const LString& Str, size_t Num );
	LString         GetFromToken( const LString& Str, size_t Num );
	/// Get each token to the 'Components' array. Array should be preallocated for ToToken+1 items
	void            FastSplitLine( size_t FromToken, size_t ToToken, const LString& Str, clStringsVector& Components, bool ShouldTrimSpaces );
	LString         GetFormattedString( const LString& Str, const int ExpandZeroes, const int Num );
	LString         CopyFromPosToChar( const LString& Str, size_t Pos, char Char, size_t* EndedAtCharPos );
	LString         CopyFromPosToCharAndTrim( LString* Str, size_t Pos, char Char, bool TrimChar );
	LString         CopyFromPosToEnd( const LString& Str, size_t Pos );
	/// create string formatted according to Pattern
	void            Format( LString* Pattern, ... );
	/// get string using this as a pattern
	LString         GetFormatted( const char* Pattern, ... );

	bool            IsLastChar( const LString& Str, char Char );
	bool            IsFirstChar( const LString& Str, char Char );
	LString         ExtractKeyName( const LString& Str, const char Separator );
	LString         ExtractKeyValue( const LString& Str, const char Separator );

	/// Check if this line contains EOLN symbols
	bool            IsMultiline( const LString& Str );

	// Split line with EOLNs to multiple lines (used in assembly code processing)
	void            LineToMultiline( const LString& Str, clStringsVector& Out );

	/// Add escape-sequences (\n or \") if the line contains special characters
	LString         EscapifyString( const LString& Str );

	//
	inline bool     ContainsSubStr( const LString& Str, const LString& SubStr );
	inline bool     StartsWith( const LString& Str, const LString& SubStr );
	//
	inline bool     IsSeparator( const char Ch );
	inline bool     IsDigit( const char Ch );
	inline bool     IsUpper( const char Ch );
	inline bool     IsLower( const char Ch );
	void            ReplaceAll( LString* Str, char OldChar, char NewChar );
	LString         ReplaceAll( const LString& Str, char OldChar, char NewChar );
	LString         ReplaceAllSubStr( const LString& Str, const LString& OldSubStr, const LString& NewSubStr );
	LString         Populate( const char Ch, int Count );
	//
	void            pop_back( LString* Str );
	void            pop_front( LString* Str );
	//
	inline bool     IsCorrectDouble( const LString& Str );
	inline bool     IsCorrectInt( const LString& Str );
	inline bool     IsCorrectBool( const LString& Str );
	//
	inline int64_t  ToInt64( const LString& Str );
	inline uint64_t ToInt64u( const LString& Str );
	inline int      ToInt( const LString& Str );
	inline bool     ToBool( const LString& Str );
	inline double   ToDouble( const LString& Str );
	inline float    ToFloat( const LString& Str );

	int64_t StrToHex( const LString& Str );

	//
	LString  ToStrHex( uint64_t FromInt64 );

	LString  ToStr( int FromInt, int Digits );

	LString  ToStr( uint32_t FromUInt32 );
	LString  ToStr( int32_t FromInt32 );
	LString  ToStr( uint64_t FromUInt32 );
	LString  ToStr( int64_t FromInt32 );

	LString ToStr_Bool( bool Value );

	inline LString  ToStr( double FromDouble );
	LString  ToStr( double FromDouble, int Digits );
	LString  ToStrEpsilon( double FromDouble );
	inline LString  ToStr( const unsigned char* Str, int StrLength );
	//
	inline LStringBuffer BufferFromString( const LString& Str )
	{
		return LStringBuffer( Str );
	}
	inline LString       StringFromBuffer( const LStringBuffer& Buf )
	{
		return Buf.ToString();
	}
};


//
// Implementation
//

int64_t LStr::ToInt64( const LString& Str )
{
#if _MSC_VER
	return _atoi64( Str.c_str() );
#else
	// input kludge
	const char* BufferPtr = Str.c_str();

	int64_t Result = 0;


	while ( IsDigit( *BufferPtr ) )
	{
		Result = Result * 10 + ( *BufferPtr - '0' );
		BufferPtr++;
	}

	return ( Str.c_str()[0] == '-' ) ? -Result : Result;
#endif
}

uint64_t LStr::ToInt64u( const LString& Str )
{
#if _MSC_VER
	// FIXME: unsigned version required
	return _atoi64( Str.c_str() );
#else
	// input kludge
	const char* BufferPtr = Str.c_str();

	uint64_t Result = 0;

	while ( IsDigit( *BufferPtr ) )
	{
		Result = Result * 10 + ( *BufferPtr - '0' );
		BufferPtr++;
	}

	return Result;
#endif
}

int LStr::ToInt( const LString& Str )
{
	return atoi( Str.c_str() );
}

double LStr::ToDouble( const LString& Str )
{
	return atof( Str.c_str() );
}

float LStr::ToFloat( const LString& Str )
{
	return static_cast<float>( atof( Str.c_str() ) );
}

bool LStr::ToBool( const LString& Str )
{
	return ( LStr::GetUpper( Str ) == "TRUE" ) ? true : false;
}

LString LStr::ToStr( double FromDouble )
{
	return ToStr( FromDouble, 3 );
}

LString LStr::ToStr( const unsigned char* Str, int StrLength )
{
	return LString( reinterpret_cast<const char*>( Str ), StrLength );
}

bool LStr::IsCorrectDouble( const LString& Str )
{
	if ( Str.length() == 0 )
	{
		return false;
	}

	bool DecimalDot = false;

	for ( size_t i = 0; i != Str.length(); ++i )
	{
		if ( Str.at( i ) == '-' || Str.at( i ) == '+' )
		{
			if ( i != 0 )
			{
				return false;
			}

			if ( Str.length() < 2 )
			{
				return false;
			}

			continue;
		}

		if ( Str.at( i ) == '.' )
		{
			if ( DecimalDot )
			{
				return false;
			}
			else
			{
				DecimalDot = true;
			}

			if ( Str.length() < 2 )
			{
				return false;
			}

			continue;
		}

		if ( !LStr::IsDigit( Str.at( i ) ) )
		{
			return false;
		}
	}

	return true;
}

bool LStr::IsCorrectInt( const LString& Str )
{
	if ( Str.length() == 0 )
	{
		return false;
	}

	for ( size_t i = 0; i != Str.length(); ++i )
	{
		if ( Str.at( i ) == '-' || Str.at( i ) == '+' )
		{
			if ( i != 0 )
			{
				return false;
			}

			if ( Str.length() < 2 )
			{
				return false;
			}

			continue;
		}

		if ( !LStr::IsDigit( Str.at( i ) ) )
		{
			return false;
		}
	}

	return true;
}

bool LStr::IsCorrectBool( const LString& Str )
{
	LString S( Str );
	LStr::ToUpper( &S );
	return ( ( S == "TRUE" ) || ( S == "FALSE" ) );
}

bool LStr::ContainsSubStr( const LString& Str, const LString& SubStr )
{
	return ( Str.find( SubStr ) != std::string::npos );
}

bool LStr::StartsWith( const LString& Str, const LString& SubStr )
{
	const char* S  = Str.c_str();
	const char* SS = SubStr.c_str();

	while ( *SS != 0 ) if ( *S++ != *SS++ )
		{
			return false;
		}

	return true;
}

bool LStr::IsSeparator( const char Ch )
{
	for ( size_t i = 0; i != sizeof( Separators ); i++ )
	{
		if ( Separators[i] == Ch )
		{
			return true;
		}
	}

	return false;
}

bool LStr::IsDigit( const char Ch )
{
	return ( ( Ch >= '0' ) && ( Ch <= '9' ) );
}

bool LStr::IsLower( char Ch )
{
	return ( ( Ch >= 'a' ) && ( Ch <= 'z' ) );
}

bool LStr::IsUpper( char Ch )
{
	return ( ( Ch >= 'A' ) && ( Ch <= 'Z' ) );
}

LString LStr::GetToken( const LString& Str, size_t Num )
{
	LString Res = LStr::GetToken_NoTrim( Str, Num );

	LStr::TrimSpaces( &Res );

	return Res;
}

LString LStr::GetTrimmedSpaces( const LString& Str )
{
	LString Res = Str;

	LStr::TrimSpaces( &Res );

	return Res;
}

#endif

/*
 * 08/11/2010
     Format()
     GetFormatted()
 * 18/10/2010
     ReplaceAllSubStr()
 * 07/10/2010
     ToInt64()
     ToInt64u()
 * 13/07/2010
     StartsWith()
     FastSplitLine()
 * 20/06/2010
     PadLeft()
     PadRight()
 * 18/05/2010
     CopyFromPosToChar()
 * 28/03/2009
     SplitLine()
 * 10/02/2009
     Interface changed
 * 03/12/2007
     IsLastChar()
     IsFirstChar()
 * 05/10/2007
     GetToken_NoTrim()
 * 04/04/2007
     New constructor added LString(GUID FromGUID)
 * 25/02/2007
     GetUpper()
 * 13/01/2007
     Populate()
 * 24/07/2006
     New constructor added LString(unsigned char*, int)
 * 13/06/2006
     ExtractKeyName()
     ExtractKeyValue()
 * 05/12/2005
     Fixed bug in IsCorrectInt()
     Fixed bug in IsCorrectDouble()
 * 28/11/2005
     IsCorrectInt() parses signed numbers
     IsCorrectDouble() parses signed numbers
 * 12/11/2005
     Static variant of ReplaceAll() added
 * 07/03/2005
     ContainsSubStr()
 * 07/01/2005
     ToFloat()
 * 21/12/2004
     GetToken()
 * 25/11/2004
     It's here
*/
