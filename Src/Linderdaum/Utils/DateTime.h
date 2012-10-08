/**
 * \file DateTime.h
 * \brief Dates, times and high-resolution timestamps
 * \version 0.5.91
 * \date 10/08/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef LDateTime__h__included__
#define LDateTime__h__included__

#include <time.h>
#include "LString.h"
#include "Platform.h"

extern unsigned int days_in_months[2][13];
extern unsigned int days_in_year  [2][14];

inline bool IsLeapYear( unsigned int Year )
{
	return ( ( ( ( Year % 4 ) == 0 ) && ( ( Year % 100 ) != 0 ) ) || ( Year % 400 ) == 0 );
}

inline bool IsValidYMD( unsigned int y, unsigned int m, unsigned int d )
{
	return ( ( m > 0 ) && ( m < 13 ) &&
	         ( d > 0 ) && ( y > 0 ) &&
	         ( d <= ( IsLeapYear( y ) ? days_in_months[1][m] :
	                  days_in_months[0][m] ) ) );
}

struct GCC_PACK( 1 ) LDate
{
   public:
   LDate();
   LDate( unsigned int y, unsigned int m, unsigned int d );
   explicit LDate( unsigned int jd ) : JulianDays( jd ), Julian( true ), YMD( false ) {}
explicit LDate( time_t tt )
{
	SetTime( tt );
}
explicit LDate( const LString& DD )
{
	FromString( DD );
}

inline bool IsValid() const
{
	return ( ( YMD && IsValidYMD( Year, Month, Day ) ) || ( Julian && JulianDays > 0 ) );
}

inline int GetYear() const
{
	if ( !YMD ) { UpdateYMD(); }

	return Year;
}

inline int GetMonth() const
{
	if ( !YMD ) { UpdateYMD(); }

	return Month;
}

inline int GetDay() const
{
	if ( !YMD ) { UpdateYMD(); }

	return Day;
}

inline int GetJulianDay() const
{
	if ( !Julian ) { UpdateJulian(); }

	return JulianDays;
}

inline int GetWeekDay() const
{
	if ( !Julian ) { UpdateJulian(); }

	return ( ( JulianDays - 1 ) % 7 ) + 1;
}

inline int GetYearDay() const
{
	if ( !YMD ) { UpdateYMD(); }

	return days_in_year[ IsLeapYear( Year ) ? 1 : 0][Month] + Day;
}

void SetYear( int Y );
void SetMonth( int M );

void SetTime( time_t timet );
time_t GetTime() const;

bool FromString( const char* str );
bool FromString( const LString& str );

LString ToString( char separator = '\0' ) const;
LString ToIsoString() const;

LDate& operator += ( int delta )
{
	if ( !Julian ) { UpdateJulian(); }

	YMD = false;
	JulianDays += delta;
	return *this;
}

LDate& operator -= ( int delta )
{
	return ( ( *this ) += ( -delta ) );
}

LDate  operator  + ( int delta )
{
	return LDate( ( unsigned int )( this->GetJulianDay() + delta ) );
}

LDate  operator  - ( int delta )
{
	return LDate( ( unsigned int )( this->GetJulianDay() - delta ) );
}

int    operator  - ( const LDate& rhs ) const
{
	return ( this->GetJulianDay() - rhs.GetJulianDay() );
}

/// Fields are declared public
public:
mutable  unsigned int JulianDays;
mutable  bool Julian;
mutable  bool YMD;
mutable  unsigned int Year;
mutable  unsigned int Month;
mutable  unsigned int Day;
private:
void UpdateJulian() const;
void UpdateYMD() const;
};

const int SecondsInDay    = 86400;
const int SecondsInHour   = 3600;
const int SecondsInMinute = 60;

const int MinutesInHour   = 60;
const int MinutesInDay    = 1440;

struct GCC_PACK( 1 ) LTime
{
   public:
   int Hour;
   int Minute;
   int Second;
   public:
   LTime( int hour, int minute, int second )
{
	Hour   = hour;
	Minute = minute;
	Second = second;
}

/// Sets the time to now.
LTime();

/// Sets the time to the specified <tt>time_t</tt> value.
LTime( time_t t )
{
	SetTime( t );
}

/// Sets the time to the specified string in the <tt>HH:MM:SS</tt> format.
LTime( const LString& str )
{
	FromString( str );
}

LTime( const LTime& t )
{
	Assign( t );
}

LTime& operator = ( const LTime& t )
{
	Assign( t );

	return *this;
}

void Assign( const LTime& t )
{
	Hour   = t.Hour;
	Minute = t.Minute;
	Second = t.Second;
}

void AdjustTime()
{
	Minute += Second / 60;
	Second  = Second % 60;

	Hour   += Minute / 60;
	Minute  = Minute % 60;
}

void AddSeconds( int Seconds )
{
	Second += Seconds;
	// check for second/minute overflow
	AdjustTime();
}

inline int GetSeconds() const
{
	return SecondsInHour * Hour + SecondsInMinute * Minute + Second;
}

/// Sets the time to the specified <tt>time_t</tt> value
void SetTime( time_t t );

/// Returns the <tt>time_t</tt> value of <tt>1970-01-01</tt> at this time
time_t GetTime() const;

/// Sets this time from a string in the <tt>HH:MM:SS</tt> format
bool FromString( const LString& str );

LString ToString() const;
};


/// Aggregates Date and Time
struct GCC_PACK( 1 ) LTimeStamp
{
   public:
   int FNanos;

   LDate FDate;
   LTime FTime;
   public:

   LTimeStamp( int year, int month, int day, int hour, int minute, int second, int nanos = 0 ) :
	   FDate( year, month, day ), FTime( hour, minute, second ), FNanos( nanos )
{
}

/// Sets the timestamp to now.
LTimeStamp();

/// Sets timestamp to the specified <tt>time_t</tt> value.
LTimeStamp( time_t t )
{
	SetTime( t );
}

/// Sets this timestamp from a <tt>YYYY-MM-DD HH:MM:SS[.NNN...]</tt> format
LTimeStamp( const LString& s )
{
	FromString( s );
}

LTimeStamp( const LTimeStamp& t )
{
	Assign( t );
}

LTimeStamp& operator = ( const LTimeStamp& t )
{
	Assign( t );
	return *this;
}

void Assign( const LTimeStamp& t )
{
	FDate = t.FDate;
	FTime = t.FTime;

	FNanos = t.FNanos;
}

/// Sets timestamp to the specified <tt>time_t</tt> value
void   SetTime( time_t t );

/// time_t value of this timestamp
time_t GetTime()
{
	return FDate.GetTime() + FTime.GetTime();
}

/// Convert timestamp from a <tt>YYYY-MM-DD HH:MM:SS[.NNN...]</tt> format
bool FromString( const LString& in );

LString ToString() const;
};

/// To- and From- string conversions
inline LDate StringToDate( const LString& Str )
{
	return LDate( Str );
}
inline LTime StringToTime( const LString& Str )
{
	return LTime( Str );
}

inline LString DateToString( const LDate& D )
{
	return D.ToString();
}
inline LString TimeToString( const LTime& T )
{
	return T.ToString();
}

inline LTimeStamp StringToTimeStamp( const LString& Str )
{
	LTimeStamp TS;
	TS.FromString( Str );
	return TS;
}
inline LString    TimeStampToString( const LTimeStamp& TS )
{
	return TS.ToString();
}

inline bool operator == ( const LTime& lhs, const LTime& rhs )
{
	return ( rhs.GetSeconds() == lhs.GetSeconds() );
}

inline bool operator <  ( const LTime& lhs, const LTime& rhs )
{
	return ( rhs.GetSeconds() <  lhs.GetSeconds() );
}

inline bool operator == ( const LDate& lhs, const LDate& rhs )
{
	return ( lhs.GetJulianDay() == rhs.GetJulianDay() );
}

inline bool operator  < ( const LDate& lhs, const LDate& rhs )
{
	return ( lhs.GetJulianDay() <  rhs.GetJulianDay() );
}

LTimeStamp operator - ( const LTimeStamp& t, int sec );

#endif

/*
 * 10/08/2010
     It's here
*/
