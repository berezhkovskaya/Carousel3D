/**
 * \file DateTime.cpp
 * \brief Dates, times and high-resolution timestamps
 * \version 0.5.91
 * \date 10/08/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */


#include "DateTime.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define FETCH_TIME() \
    struct tm timeinfo; \
    localtime_s( &timeinfo, &timet );
#else
#ifdef __WATCOMC__

#define FETCH_TIME() \
        localtime_r(&timet, &tm);
#else

#define FETCH_TIME() \
        struct tm* timeinfo_ptr; \
        timeinfo_ptr = localtime ( &timet ); \
        struct tm& timeinfo = *timeinfo_ptr;

#endif
#endif

unsigned int days_in_months[2][13] =
{
	{  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{  0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } /* leap year */
};

unsigned int days_in_year[2][14] =
{
	{  0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	{  0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 } /* leap year */
};


LDate::LDate()
{
	time_t timet = time( NULL );
	SetTime( timet );
}

LDate::LDate( unsigned int y, unsigned int m, unsigned int d )
{
	Julian = false;
	Year = y;
	Month = m;
	Day = d;
	YMD = true;
}

void LDate::SetTime( time_t timet )
{
	FETCH_TIME()

	Julian = false;

	Year   = timeinfo.tm_year + 1900;
	Month  = timeinfo.tm_mon + 1;
	Day    = timeinfo.tm_mday;
	YMD    = true;
}


void LDate::UpdateJulian() const
{
	if ( !YMD || Julian )
	{
		return;
	}

	unsigned int yy = Year - 1;

	JulianDays = yy * 365U;
	JulianDays += ( yy >>= 2 );
	JulianDays -= ( yy /= 25 );
	JulianDays += yy >> 2;

	JulianDays += days_in_year[IsLeapYear( Year ) ? 1 : 0][Month] + Day;
	Julian = true;
}

void LDate::UpdateYMD() const
{
	if ( YMD || !Julian )
	{
		return;
	}

	unsigned int a = JulianDays + 1721425 + 32045;
	unsigned int b = ( 4 * ( a + 36524 ) ) / 146097 - 1;
	unsigned int c = a - ( 146097 * b ) / 4;
	unsigned int d = ( 4 * ( c + 365 ) ) / 1461 - 1;
	unsigned int e = c - ( ( 1461 * d ) / 4 );
	unsigned int m = ( 5 * ( e - 1 ) + 2 ) / 153;

	Year  = 100 * b + d - 4800 + ( m / 10 );
	Month = m + 3 - ( 12 * ( m / 10 ) );
	Day   = e - ( 153 * m + 2 ) / 5;
	YMD   = true;
}

const unsigned int year_minimal = 1970;
const unsigned int year_maximal = 2099;

// YYYY-MM-DD
bool LDate::FromString( const char* str )
{
	char sz[11];
	unsigned int y, m, d;

#ifdef _MSC_VER
	strcpy_s( sz, 11, str );
#else
	strcpy( sz, str );
#endif

// strncpy(sz, str, 4);
	sz[4] = '\0';
	y = atoi( sz );

//    strncpy(sz, &str[5], 2);
	sz[7] = '\0';
	m = atoi( &sz[5] );

//    strncpy(sz, &str[8], 2);
	sz[10] = '\0';
	d = atoi( &sz[8] );

	if ( !IsValidYMD( y, m, d ) || y < year_minimal || y > year_maximal )
	{
		return false;
	}

	Year   = y;
	Month  = m;
	Day    = d;
	YMD    = true;
	Julian = false;

	return true;
}

bool LDate::FromString( const LString& str )
{
	return FromString( str.c_str() );
}

LString LDate::ToString( char separator ) const
{
	char buf[11];

	if ( separator == '\0' )
#ifdef _MSC_VER
		_snprintf_s( buf, sizeof( buf ), 11, "%02u%02u%04u", GetDay(), GetMonth(), GetYear() );

#else
		snprintf( buf, sizeof( buf ), "%02u%02u%04u", GetDay(), GetMonth(), GetYear() );
#endif
	else
#ifdef _MSC_VER
		_snprintf_s( buf, sizeof( buf ), 11, "%02u%c%02u%c%04u", GetDay(), separator, GetMonth(), separator, GetYear() );

#else
		snprintf( buf, sizeof( buf ), "%02u%c%02u%c%04u", GetDay(), separator, GetMonth(), separator, GetYear() );
#endif

	return LString( buf );
}

LString LDate::ToIsoString() const
{
	char buf[11];
#ifdef _MSC_VER
	_snprintf_s( buf, sizeof( buf ), 11, "%04u-%02u-%02u", GetYear(), GetMonth(), GetDay() );
#else
	snprintf( buf, sizeof( buf ), "%04u-%02u-%02u", GetYear(), GetMonth(), GetDay() );
#endif
	return LString( buf );
}

time_t LDate::GetTime() const
{
	tm stm;
	stm.tm_year  = GetYear() - 1900;
	stm.tm_mon   = GetMonth() - 1;
	stm.tm_mday  = GetDay();
	stm.tm_hour  = 0;
	stm.tm_min   = 0;
	stm.tm_sec   = 0;
	stm.tm_isdst = -1; //negative means not known
	return mktime( &stm );
}

LTime::LTime()
{
	SetTime( time( NULL ) );
}

void LTime::SetTime( time_t t )
{
	time_t timet = t;
	FETCH_TIME()

	Hour   = timeinfo.tm_hour;
	Minute = timeinfo.tm_min;
	Second = timeinfo.tm_sec;
}

time_t LTime::GetTime() const
{
	return Second + Minute * 60 + Hour * 3600;
}

LString LTime::ToString() const
{
	char buf[9]; // HH:MM:SS, null
#ifdef _MSC_VER
	sprintf_s( buf, 9, "%.2d:%.2d:%.2d", Hour, Minute, Second );
#else
	sprintf( buf, "%.2d:%.2d:%.2d", Hour, Minute, Second );
#endif

	return LString( buf );
}

#include <iostream>
using namespace std;

bool LTime::FromString( const LString& in )
{
	if ( in.length() < 8 )
	{
		return false; // Unrecognized time format
	}

	char buf[9];

#ifdef _MSC_VER
	strcpy_s( buf, 9, in.c_str() );
#else
	strcpy( buf, in.c_str() );
#endif

	buf[2] = 0;
	buf[5] = 0;

	Hour   = strtol( buf,    NULL, 10 );
	Minute = strtol( &buf[3], NULL, 10 );
	Second = strtol( &buf[6], NULL, 10 );

	return true;
}

LTimeStamp::LTimeStamp()
{
	this->SetTime( time( NULL ) );
}

void LTimeStamp::SetTime( time_t t )
{
	time_t timet = t;

	FETCH_TIME()

	FDate.Year   = timeinfo.tm_year + 1900;
	FDate.Month  = timeinfo.tm_mon + 1;
	FDate.Day    = timeinfo.tm_mday;

	FTime.Hour   = timeinfo.tm_hour;
	FTime.Minute = timeinfo.tm_min;
	FTime.Second = timeinfo.tm_sec;

	FNanos = 0;
}

LString LTimeStamp::ToString() const
{
	LString Res = FDate.ToIsoString() + LString( " " ) + FTime.ToString();

	if ( FNanos > 0 )
	{
		Res += "." + LStr::ToStr( FNanos );
	}

	return Res;
}

bool LTimeStamp::FromString( const LString& in )
{
	// YYYY-MM-DD HH:MM:SS.xxxxxxxxxx (max 30 chars)
	if ( in.length() < 19 || in.length() > 30 )
	{
		return false; // Unrecognized timestamp format
	}

	char buf[31];

#ifdef _MSC_VER
	strcpy_s( buf, 31, in.c_str() );
#else
	strcpy( buf, in.c_str() );
#endif

	buf[10] = 0;
	buf[19] = 0;

	if ( !FDate.FromString( buf ) )
	{
		return false;
	}

	if ( !FTime.FromString( &buf[11] ) )
	{
		return false;
	}

	FNanos = ( in.length() > 20 ) ? strtol( &buf[20], NULL, 10 ) : 0;

	return true;
}

LTimeStamp operator - ( const LTimeStamp& t, int sec )
{
	int fulldays = sec / SecondsInDay;
	sec %= SecondsInDay;

	int seconds = t.FTime.GetSeconds();

	if ( seconds < sec )
	{
		seconds += SecondsInDay;
		fulldays++;
	}

	int s = seconds % SecondsInMinute;
	seconds /= SecondsInMinute;

	int m = seconds % SecondsInMinute;
	seconds /= SecondsInHour;

	LDate d ( t.FDate.GetYear(), t.FDate.GetMonth(), t.FDate.GetDay() );

	LDate d1( ( unsigned int )( d.GetJulianDay() - fulldays ) );

	return LTimeStamp( d1.GetYear(), d1.GetMonth(), d1.GetDay(), seconds, m, s, t.FNanos );
}

/*
 * 10/08/2010
     It's here
*/
