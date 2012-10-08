/**
 * \file LURL.h
 * \brief Universal resource locator/identifier
 * \version 0.6.00
 * \date 23/10/2011
 * \author Viktor Latypov, 2011
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#pragma once

#ifndef __LURL__h__included__
#define __LURL__h__included__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <malloc.h>

#ifndef DO_NOT_USE_ENGINE

#  include "LString.h"

#else

#  include <string>
#  define LString std::string

#endif

/**

RFC 3986  URL/URI class

  foo://username:password@example.com:8042/over/there/index.dtb?type=animal;name=narwhal#nose
  \_/   \_______________/ \_________/ \__/            \___/ \_/ \______________________/ \__/
   |           |               |       |                |    |            |                |
   |       userinfo         hostname  port              |    |          query          fragment
   |    \________________________________/\_____________|____|/
scheme                  |                          |    |    |
   |                authority                    path   |    |
   |                                                    |    |
   |            path                       interpretable as filename
   |   ___________|____________                              |
  / \ /                        \                             |
  urn:example:animal:ferret:nose               interpretable as extension

*/

#include <limits.h>

namespace punycode_helpers
{

	/*
	   This code is taken from

	   punycode.c from RFC 3492prop
	   http://www.nicemice.net/idn/
	   Adam M. Costello
	   http://www.nicemice.net/amc/
	*/

	enum punycode_status
	{
		punycode_success,
		punycode_bad_input,   /* Input is invalid.                       */
		punycode_big_output,  /* Output would exceed the space provided. */
		punycode_overflow     /* Input needs wider integers to process.  */
	};

#if UINT_MAX >= (1 << 26) - 1
	typedef unsigned int punycode_uint;
#else
	typedef unsigned long punycode_uint;
#endif

	/*** Platform-specific constants ***/

	/* maxint is the maximum value of a punycode_uint variable: */
	const punycode_uint maxint = -1;
	/* maxint is unsigned, so -1 becomes the maximum value. */

	inline static enum punycode_status punycode_encode(
	   punycode_uint input_length,
	   const punycode_uint input[],
	   punycode_uint* output_length,
	   char output[] );

	inline static enum punycode_status punycode_decode(
	   punycode_uint input_length,
	   const char input[],
	   punycode_uint* output_length,
	   punycode_uint output[] );

} // namespace punycode_helpers

/**
   \brief URL/URI class

   Representation, conversion and encoding of the URLs

   Can be used in webserver, filesystem and xml-importers (like collada) for cross-file references

   Features:

   - RFC 3986 URL parsing
   - Punycode for domain names
   - UTF8/Percent Encoding for file names
*/
class LURL
{
public:
	/// Default constructor
	LURL() {}

	/// Only the explicit construction is allowed
	explicit LURL( const LString& Src ) { ParseURL( Src.c_str() ); }

	/// Load URL from string
	void FromString( const LString& Src ) { ParseURL( Src.c_str() ); }

	/// Textual representation of the URL/URI
	LString ToString() const
	{
		LString dest( "" );

		if ( !FScheme.empty() ) { dest = FScheme + "://"; }

		if ( !FUser.empty() )
		{
			dest += FUser;

			if ( !FPass.empty() ) { dest += LString( ":" ) + FPass + LString( "@" ); }
		}

		if ( !FDomain.empty() )
		{
			dest += FDomain;

			if ( FPort > -1 )
			{
				char tmp[100];
				dest += LString( ":" ) + LString( itoa( FPort, tmp, 10 ) );
			}
		}

		if ( !FFilename.empty() ) { dest += FFilename; }

		if ( !FQuery.empty() ) { dest += LString( "?" ) + FQuery; }

		if ( !FFragment.empty() ) { dest += LString( "#" ) + FFragment; }

		return dest;
	}

	/**
	   Accessor methods are not for the OOP-like interface,
	   but they are required to make some conversion (percent-encoding, punycode)
	   in the Set..() methods
	*/

#pragma region Accessor methods

	/// Get scheme description
	LString GetScheme() const { return FScheme; }

	/// Get host name and port
	LString GetDomainAndPort() const { return FDomainAndPort; }

	/// Get domain name, if any
	LString GetDomain() const { return FDomain; }

	/// Get server port number
	int     GetPort() const { return FPort; }

	/// Get username
	LString GetUser() const { return FUser; }

	/// Get password
	LString GetPassword() const { return FPass; }

	/// Part of the document
	LString GetFragment() const { return FFragment; }

	/// Query and its parameters
	LString GetQuery() const { return FQuery; }

	/// Part of the path
	LString GetFilename() const { return PercentDecode( FFilename ); }

	/** Now the setters part */

	void SetUser( const LString& User ) { FUser = User; }

	void SetPassword( const LString& Pass ) { FPass = Pass; }

	void SetScheme( const LString& Scheme ) { FScheme = Scheme; }

	void SetQuery( const LString& Query ) { FQuery = Query; }

	void SetFragment( const LString& Fragment ) { FFragment = Fragment; }

	void SetFilename( const LString& TheFileName ) { FFilename = PercentEncode( TheFileName ); }

	void SetDomain( const LString& Domain )
	{
		/*
		      // TODO: convert each part to Punycode
		      LString ResDomain;

		      vector<string> Parts;

		      SplitByDot(Domain, Parts);

		      for(size_t i = 0 ; i != Parts.size() ; i++)
		      {
		         ResDomain += (HasUTF8Symbols(Parts[i]) ? LString("xn--" + ConvertToPunycode(Parts[i])) : Parts[i];

		         if(i != Parts.size() - 1) { ResDomain += "."; }
		      }

		      FDomain = ResDomain;
		*/
		FDomain = Domain;
	}

	void SetPort( int Port ) { FPort = Port; }

#pragma endregion

	/// Check if this is a local file. FIXME: implement
	bool IsLocalFile() const { return false; }

public:
	/// Internal URL parsing function
	void ParseURL( const char* url )
	{
		FPort = -1;

		FDomainAndPort.clear();
		FUserInfo.clear();
		FFullPath.clear();

		FScheme.clear();

		FUser.clear();
		FPass.clear();

		FDomain.clear();
		FFilename.clear();
		FQuery.clear();
		FFragment.clear();

		/// Sanity checks
		if ( !url ) { return; }

		if ( strlen( url ) == 0 ) { return; }

		/* 0. check for simple 'this-document' reference */
		if ( url[0] == '#' ) { FFragment = LString( &url[1] ); }

		/* 1. find '://' */
		int scheme_delim = 3;
		int no_scheme = 0;
		int scheme_len = 0;

		const char* url_noscheme = strstr( url, "://" );
		const char* path_part = 0;

		if ( !url_noscheme )
		{
			/* There can be a simple ':' delimiter*/
			url_noscheme = strstr( url, ":" );

			if ( !url_noscheme )
			{
				url_noscheme = url;

				no_scheme = 1;
			}
			else
			{
				scheme_delim = 1;
			}
		}

		if ( !no_scheme )
		{
			scheme_len = ( int )( url_noscheme - url );

			FScheme = ExtractSubStr( url, scheme_len );

			url_noscheme += scheme_delim;
		}

		/* 2. find '@' */
		const char* username_sep = strstr( url_noscheme, "@" );

		if ( username_sep )
		{
			int user_len = ( int )( username_sep - url_noscheme );
			FUserInfo = ExtractSubStr( url_noscheme, user_len );

			url_noscheme = username_sep + 1;

			/* 2.1. Split username and password */
			const char* pass_delim = strstr( FUserInfo.c_str(), ":" );

			if ( pass_delim )
			{
				user_len = ( int )( pass_delim - FUserInfo.c_str() );

				int pass_len = ( int )( FUserInfo.length() - user_len );
				FPass = ExtractSubStr( pass_delim + 1, pass_len );

				FUser = ExtractSubStr( FUserInfo.c_str(), user_len );
			}
			else
			{
				FUser = FUserInfo;
			}
		}

		/* 3.1. Extract domain name - find first ':' or '/' (after :// and @) */
		const char* first_slash = strstr( url_noscheme, "/" );

		int no_path = 1;

		if ( !first_slash )
		{
			/* no ports, no path */
			FDomainAndPort = LString( url_noscheme );
		}
		else
		{
			int domain_len = ( first_slash - url_noscheme );
			/* we have something at the end - either path, or just a trailing '/' */
			FDomainAndPort = ExtractSubStr( url_noscheme, domain_len );

			/* split domain_port to domain and port */
			const char* port_sep = strstr( FDomainAndPort.c_str(), ":" );

			if ( port_sep )
			{
				FPort = atoi( port_sep + 1 );

				int domain_len = ( int )( port_sep - FDomainAndPort.c_str() );
				FDomain = ExtractSubStr( FDomainAndPort.c_str(), domain_len );
			}
			else
			{
				FDomain = FDomainAndPort;
				FPort = -1;
			}

			/* it wasn't only a trailing slash */
			if ( url_noscheme[( int )( url_noscheme - first_slash + 1 )] != '\0' )
			{
				FFullPath = LString( first_slash );

				no_path = 0;

				path_part = first_slash;
			}
		}

		/* 3.2. Extract port number */

		if ( !no_path )
		{
			const char* query_delim = strstr( path_part, "?" );

			const char* fragment_delim = strstr( path_part, "#" );

			/* process path even further, split page name and queries */
			const char* last_non_query = 0;

			if ( fragment_delim )
			{
				last_non_query = fragment_delim - 1;

				int frag_len = ( int )( fragment_delim - path_part - 1 );
				FFragment = ExtractSubStr( fragment_delim + 1, frag_len );
			}
			else
			{
				last_non_query = FFullPath.c_str() + FFullPath.length();
			}

			if ( query_delim )
			{
				int query_len = last_non_query - query_delim;

				FQuery = ExtractSubStr( query_delim + 1, query_len );
			}

			/* Finally, extract the 'file name' without query */
			int fname_len = 0;

			if ( query_delim || fragment_delim )
			{
				fname_len = ( int )( ( query_delim ? query_delim : fragment_delim ) - path_part );
				FFilename = ExtractSubStr( FFullPath.c_str(), fname_len );
			}
			else
			{
				FFilename = FFullPath;
			}
		}
	}

private:
	inline static const char* ExtractSubStr( const char* src, int len )
	{
		LString Res( "" );
		Res.resize( len + 1 );

		char* dest = &Res[0];

		for ( int i = 0 ; i < len ; i++ ) { *dest++ = *src++; }

		*dest = '\0';

		/// This values is used immediately after call
		return Res.c_str();
	}

	/// RFC 3986 unreserved (usable) symbols. Anything else is percent-encoded
	static bool IsUnreserved( char c )
	{
		return ( c > '0' - 1 && c < '9' + 1 ) || ( c > 'A' - 1 && c < 'Z' + 1 ) || ( c > 'a' - 1 && c < 'z' + 1 ) || ( c == '-' ) || ( c == '_' ) || ( c == '.' ) || ( c == '~' );
	}

public:

	/// RFC 3986 'percent decoding'
	static LString PercentDecode( const LString& in )
	{
		LString res( "" );

		for ( size_t i = 0 ; i < in.length() ; i++ )
		{
			if ( in[i] == '%' )
			{
				// error, truncated string
				if ( i > in.length() - 2 ) { return res; }

				unsigned char c[] = { in[i + 1], in[i + 2] };

				for ( int jj = 0 ; jj < 2 ; jj++ )
					if ( c[jj] > 'A' ) { c[jj] -= 'A'; c[jj] += 10; }
					else { c[jj] -= '0'; }

				LString ch = "X";
				ch[0] = ( unsigned char )( ( ( ( int )c[0] ) << 4 ) | ( c[1] ) );
				res += ch;

				i += 2;
			}
			else
			{
				LString ch = "X";
				ch[0] = in[i];
				res += ch;
			}
		}

		return res;
	}

	/// RFC 3986 'percent encoding'
	static LString PercentEncode( const LString& in )
	{
		static const char* const hex_tab = "0123456789ABCDEF";

		LString res( "" );

		for ( size_t i = 0 ; i < in.length() ; i++ )
		{
			if ( IsUnreserved( in[i] ) )
			{
				LString ch = "X";
				ch[0] = in[i];
				res += ch;
			}
			else
			{
				LString part = "%__";
				part[1] = hex_tab[( in[i] >> 4 ) & 0x0F];
				part[2] = hex_tab[( in[i]     ) & 0x0F];
				res += part;
			}
		}

		return res;
	}

	static bool HasUTF8Symbols( const unsigned char* str )
	{
		const unsigned char* p = str;
		unsigned int len = 0;

		while ( *p )
		{
			if ( *p > 0x7F ) { return true; }

			p++;
		}

		return false;
	}

public:

	static LString ConvertToPunycode( const char* src_utf8_str )
	{
		/// 1. Convert UTF8 to UCS-4 (copy'n'paste black magic somewhere from FreeType font renderer)

		int j, charlen;
		int i = 0;
		const unsigned char* p = ( const unsigned char* )src_utf8_str;

		int utf8len = 0;

		/// Calculate the number of utf8 characters
		while ( *p )
		{
			if      ( *p < 0x80 ) { p++; }
			else if ( *p < 0xC0 ) { p = 0; }
			else if ( *p < 0xE0 ) { p += 2; }
			else if ( *p < 0xF0 ) { p += 3; }
			else if ( *p < 0xF8 ) { p += 4; }
			else if ( *p < 0xFC ) { p += 5; }

			if ( p == 0 ) { break; }

			if ( *p == 0 ) { utf8len++; break; }

			utf8len++;
		}

		p = ( const unsigned char* )src_utf8_str;

		int out_len = utf8len * 4;
		char* out = ( char* )malloc( out_len );

		int* utf8str_i = ( int* )malloc( out_len );

		for ( i = 0; i < utf8len ; i++ )
		{
			int c = ( ( const unsigned char* )p )[0];

			if      ( c < 0x80 ) { charlen = 1; if ( !c ) { i++; break; } }
			else if ( c < 0xE0 ) { charlen = 2; c &= 0x1F; }
			else if ( c < 0xF0 ) { charlen = 3; c &= 0x0F; }
			else if ( c < 0xF8 ) { charlen = 4; c &= 0x07; }
			else if ( c < 0xFC ) { charlen = 5; c &= 0x03; }
			else               { charlen = 6; c &= 0x01; }

			for ( j = 1; j < charlen; j++ ) { c = ( c << 6 ) | ( ( ( unsigned char* ) p )[j] & 0x3F ); }

			utf8str_i[i] = c;
			p += charlen;
		}

		utf8str_i[i] = 0;
		utf8len = i;

		/// 2. Convert UCS-4 to Punycode

		enum punycode_helpers::punycode_status st =
		   punycode_helpers::punycode_encode( utf8len,
		                                      ( const punycode_helpers::punycode_uint* )utf8str_i,
		                                      ( punycode_helpers::punycode_uint* )&out_len, out );

		out[out_len] = 0;

		/// 3. Cleanup and store result

		LString Res = out;

		free( utf8str_i );
		free( out );

		return Res;
	}

private:
	/// Non-mandatory scheme description
	LString FScheme;

	/// Optional username and password in one colon-separated string
	LString FUserInfo;

	/// Host name and port
	LString FDomainAndPort;

	/// Domain-only
	LString FDomain;

	/// Server port. -1 if none
	int     FPort;

	/// Full path to the resource
	LString FFullPath;

	/// Optional username
	LString FUser;

	/// Optional password (or password hash)
	LString FPass;

	/// Part of the document
	LString FFragment;

	/// Query and its parameters
	LString FQuery;

	/// Part of the path
	LString FFilename;
};

#pragma region Inline Punycode stuff

// Once again, all this code is
/*
   punycode.c from RFC 3492prop
   http://www.nicemice.net/idn/
   Adam M. Costello
   http://www.nicemice.net/amc/
*/

namespace punycode_helpers
{

	/*** Bootstring parameters for Punycode ***/

	enum { base = 36, tmin = 1, tmax = 26, skew = 38, damp = 700, initial_bias = 72, initial_n = 0x80, delimiter = 0x2D };

	/* test whether cp is a basic code point */
	inline static int basic( punycode_uint cp ) { return ( ( punycode_uint )( cp ) < 0x80 ); }

	/* decode_digit(cp) returns the numeric value of a basic code */
	/* point (for use in representing integers) in the range 0 to */
	/* base-1, or base if cp is does not represent a value.       */

	inline static punycode_uint decode_digit( punycode_uint cp )
	{
		return  cp - 48 < 10 ? cp - 22 :  cp - 65 < 26 ? cp - 65 : cp - 97 < 26 ? cp - 97 :  base;
	}

	/*  0..25 map to ASCII a..z or A..Z, 26..35 map to ASCII 0..9         */
	inline static char encode_digit( punycode_uint d ) { return d + 22 + 75 * ( d < 26 ); }

	/* encode_basic(bcp,flag) forces a basic code point to lowercase */
	inline static char encode_basic( punycode_uint bcp )
	{
		bcp -= ( bcp - 97 < 26 ) << 5;
		return bcp + ( ( bcp - 65 < 26 ) << 5 );
	}

	/*** Bias adaptation function ***/
	inline static punycode_uint adapt( punycode_uint delta, punycode_uint numpoints, int firsttime )
	{
		punycode_uint k;

		delta = firsttime ? delta / damp : delta >> 1;
		delta += delta / numpoints;

		for ( k = 0;  delta > ( ( base - tmin ) * tmax ) / 2;  k += base ) { delta /= base - tmin; }

		return k + ( base - tmin + 1 ) * delta / ( delta + skew );
	}

	/*** Main encode function ***/
	inline static enum punycode_status punycode_encode(
	   punycode_uint input_length,
	   const punycode_uint input[],
	   punycode_uint* output_length,
	   char output[] )
	{
		punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

		/* Initialize the state: */
		n = initial_n;
		delta = out = 0;
		max_out = *output_length;
		bias = initial_bias;

		/* Handle the basic code points: */
		for ( j = 0;  j < input_length;  ++j )
		{
			if ( basic( input[j] ) )
			{
				if ( max_out - out < 2 ) { return punycode_big_output; }

				output[out++] = input[j];
			}

			/* else if (input[j] < n) return punycode_bad_input; */
			/* (not needed for Punycode with unsigned code points) */
		}

		h = b = out;

		/* h is the number of code points that have been handled, b is the  */
		/* number of basic code points, and out is the number of characters */
		/* that have been output.                                           */
		if ( b > 0 ) { output[out++] = delimiter; }

		/* Main encoding loop: */
		while ( h < input_length )
		{
			/* All non-basic code points < n have been     */
			/* handled already.  Find the next larger one: */

			for ( m = maxint, j = 0;  j < input_length;  ++j )
			{
				/* if (basic(input[j])) continue; */
				/* (not needed for Punycode) */
				if ( input[j] >= n && input[j] < m ) { m = input[j]; }
			}

			/* Increase delta enough to advance the decoder's    */
			/* <n,i> state to <m,0>, but guard against overflow: */
			if ( m - n > ( maxint - delta ) / ( h + 1 ) ) { return punycode_overflow; }

			delta += ( m - n ) * ( h + 1 );
			n = m;

			for ( j = 0;  j < input_length;  ++j )
			{
				/* Punycode does not need to check whether input[j] is basic: */
				if ( input[j] < n ) { if ( ++delta == 0 ) { return punycode_overflow; } }

				if ( input[j] == n )
				{
					/* Represent delta as a generalized variable-length integer: */

					for ( q = delta, k = base;  ;  k += base )
					{
						if ( out >= max_out ) { return punycode_big_output; }

						t = k <= bias /* + tmin */ ? tmin : /* +tmin not needed */ k >= bias + tmax ? tmax : k - bias;

						if ( q < t ) { break; }

						output[out++] = encode_digit( t + ( q - t ) % ( base - t ) );
						q = ( q - t ) / ( base - t );
					}

					output[out++] = encode_digit( q );
					bias = adapt( delta, h + 1, h == b );
					delta = 0;
					++h;
				}
			}

			++delta, ++n;
		}

		*output_length = out;
		return punycode_success;
	}

	/*** Main decode function ***/
	inline static enum punycode_status punycode_decode(
	   punycode_uint input_length,
	   const char input[],
	   punycode_uint* output_length,
	   punycode_uint output[] )
	{
		punycode_uint n, out, i, max_out, bias, b, j, in, oldi, w, k, digit, t;

		/* Initialize the state: */
		n = initial_n;
		out = i = 0;
		max_out = *output_length;
		bias = initial_bias;

		/* Handle the basic code points:  Let b be the number of input code */
		/* points before the last delimiter, or 0 if there is none, then    */
		/* copy the first b code points to the output.                      */

		for ( b = j = 0;  j < input_length;  ++j ) { if ( input[j] == delimiter ) { b = j; } }

		if ( b > max_out ) { return punycode_big_output; }

		for ( j = 0;  j < b;  ++j )
		{
			if ( !basic( input[j] ) ) { return punycode_bad_input; }

			output[out++] = input[j];
		}

		/* Main decoding loop:  Start just after the last delimiter if any  */
		/* basic code points were copied; start at the beginning otherwise. */

		for ( in = b > 0 ? b + 1 : 0;  in < input_length;  ++out )
		{
			/* in is the index of the next character to be consumed, and */
			/* out is the number of code points in the output array.     */

			/* Decode a generalized variable-length integer into delta,  */
			/* which gets added to i.  The overflow checking is easier   */
			/* if we increase i as we go, then subtract off its starting */
			/* value at the end to obtain delta.                         */

			for ( oldi = i, w = 1, k = base;  ;  k += base )
			{
				if ( in >= input_length ) { return punycode_bad_input; }

				digit = decode_digit( input[in++] );

				if ( digit >= base ) { return punycode_bad_input; }

				if ( digit > ( maxint - i ) / w ) { return punycode_overflow; }

				i += digit * w;
				t = k <= bias /* + tmin */ ? tmin : /* +tmin not needed */ k >= bias + tmax ? tmax : k - bias;

				if ( digit < t ) { break; }

				if ( w > maxint / ( base - t ) ) { return punycode_overflow; }

				w *= ( base - t );
			}

			bias = adapt( i - oldi, out + 1, oldi == 0 );

			/* i was supposed to wrap around from out+1 to 0,   */
			/* incrementing n each time, so we'll fix that now: */

			if ( i / ( out + 1 ) > maxint - n ) { return punycode_overflow; }

			n += i / ( out + 1 );
			i %= ( out + 1 );

			/* Insert n at position i of the output: */
			if ( out >= max_out ) { return punycode_big_output; }

			memmove( output + i + 1, output + i, ( out - i ) * sizeof * output );
			output[i++] = n;
		}

		*output_length = out;
		return punycode_success;
	}

} // namespace punycode_helpers

/// \cond

/**

/// Simple unit test

#include "LURL.h"

int main()
{
   LURL url;

   url.ParseURL("foo://username:password@example.com:8042/over/there/index.dtb?type=animal;name=narwhal#nose");

   printf("OutUrl = %s\n", url.ToString().c_str());

// url.ParseURL("microsoft.com:8012/bill/gates");
// url.ParseURL("http://sf.net/index.html");
// url.ParseURL("mail.ru");
// url.ParseURL("mailto:someone@gmail.com");
// url.ParseURL("https://l.com/doc/index.html#part2");

   printf( "%s\n", LURL::ConvertToPunycode("test-почта-россии\0\0").c_str() );

   LString t1 = "test_тест?[#0]";
   printf("src: %s\n", t1.c_str());

   LString t2 = LURL::PercentEncode(t1);
   printf("enc: %s\n", t2.c_str());

   LString t3 = LURL::PercentDecode(t2);
   printf("dec: %s\n", t3.c_str());

   printf("ok? : %d\n", (t3 == t1));

   return 0;
}

*/

/// \endcond

// Inline Punycode stuff
#pragma endregion

#endif
