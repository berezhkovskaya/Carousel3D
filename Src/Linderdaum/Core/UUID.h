/*
**  OSSP uuid - Universally Unique Identifier
**  Copyright (c) 2004-2007 Ralf S. Engelschall <rse@engelschall.com>
**  Copyright (c) 2004-2007 The OSSP Project <http://www.ossp.org/>
**
**  This file is part of OSSP uuid, a library for the generation
**  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
**
**  Permission to use, copy, modify, and distribute this software for
**  any purpose with or without fee is hereby granted, provided that
**  the above copyright notice and this permission notice appear in all
**  copies.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
**  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
**  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
**
**  uuid.h: library API definition
*/

#ifndef __UUID_H__
#define __UUID_H__

/* workaround conflicts with system headers */
#undef uuid_t
#define uuid_t       __vendor_uuid_t
#define uuid_create  __vendor_uuid_create
#define uuid_compare __vendor_uuid_compare
#include <sys/types.h>
#include <unistd.h>
#undef  uuid_t
#undef  uuid_create
#undef  uuid_compare

/* required system headers */

/* minimum C++ support */
#ifdef __cplusplus
#define DECLARATION_BEGIN extern "C" {
#define DECLARATION_END   }
#else
#define DECLARATION_BEGIN
#define DECLARATION_END
#endif


#include <stdarg.h>
#include <string.h>

#define STR_PREFIX uuid_

/* embedding support */
#ifdef STR_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __STR_CONCAT(x,y) x ## y
#define STR_CONCAT(x,y) __STR_CONCAT(x,y)
#else
#define __STR_CONCAT(x) x
#define STR_CONCAT(x,y) __STR_CONCAT(x)y
#endif
#define str_vsnprintf  STR_CONCAT(STR_PREFIX,str_vsnprintf)
#define str_snprintf   STR_CONCAT(STR_PREFIX,str_snprintf)
#define str_vrsprintf  STR_CONCAT(STR_PREFIX,str_vrsprintf)
#define str_rsprintf   STR_CONCAT(STR_PREFIX,str_rsprintf)
#define str_vasprintf  STR_CONCAT(STR_PREFIX,str_vasprintf)
#define str_asprintf   STR_CONCAT(STR_PREFIX,str_asprintf)
#endif

extern int   str_vsnprintf ( char*, size_t, const char*, va_list );
extern int   str_snprintf  ( char*, size_t, const char*, ... );
extern int   str_vrsprintf ( char**,         const char*, va_list );
extern int   str_rsprintf  ( char**,         const char*, ... );
extern char* str_vasprintf (                 const char*, va_list );
extern char* str_asprintf  (                 const char*, ... );


#define UI64_PREFIX uuid_

/* embedding support */
#ifdef UI64_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __UI64_CONCAT(x,y) x ## y
#define UI64_CONCAT(x,y) __UI64_CONCAT(x,y)
#else
#define __UI64_CONCAT(x) x
#define UI64_CONCAT(x,y) __UI64_CONCAT(x)y
#endif
#define ui64_t     UI64_CONCAT(UI64_PREFIX,ui64_t)
#define ui64_zero  UI64_CONCAT(UI64_PREFIX,ui64_zero)
#define ui64_max   UI64_CONCAT(UI64_PREFIX,ui64_max)
#define ui64_n2i   UI64_CONCAT(UI64_PREFIX,ui64_n2i)
#define ui64_i2n   UI64_CONCAT(UI64_PREFIX,ui64_i2n)
#define ui64_s2i   UI64_CONCAT(UI64_PREFIX,ui64_s2i)
#define ui64_i2s   UI64_CONCAT(UI64_PREFIX,ui64_i2s)
#define ui64_add   UI64_CONCAT(UI64_PREFIX,ui64_add)
#define ui64_addn  UI64_CONCAT(UI64_PREFIX,ui64_addn)
#define ui64_sub   UI64_CONCAT(UI64_PREFIX,ui64_sub)
#define ui64_subn  UI64_CONCAT(UI64_PREFIX,ui64_subn)
#define ui64_mul   UI64_CONCAT(UI64_PREFIX,ui64_mul)
#define ui64_muln  UI64_CONCAT(UI64_PREFIX,ui64_muln)
#define ui64_div   UI64_CONCAT(UI64_PREFIX,ui64_div)
#define ui64_divn  UI64_CONCAT(UI64_PREFIX,ui64_divn)
#define ui64_and   UI64_CONCAT(UI64_PREFIX,ui64_and)
#define ui64_or    UI64_CONCAT(UI64_PREFIX,ui64_or)
#define ui64_xor   UI64_CONCAT(UI64_PREFIX,ui64_xor)
#define ui64_not   UI64_CONCAT(UI64_PREFIX,ui64_not)
#define ui64_rol   UI64_CONCAT(UI64_PREFIX,ui64_rol)
#define ui64_ror   UI64_CONCAT(UI64_PREFIX,ui64_ror)
#define ui64_len   UI64_CONCAT(UI64_PREFIX,ui64_len)
#define ui64_cmp   UI64_CONCAT(UI64_PREFIX,ui64_cmp)
#endif

typedef struct
{
	unsigned char x[8]; /* x_0, ..., x_7 */
} ui64_t;

#define ui64_cons(x7,x6,x5,x4,x3,x2,x1,x0) \
    { { 0x##x0, 0x##x1, 0x##x2, 0x##x3, 0x##x4, 0x##x5, 0x##x6, 0x##x7 } }

/* particular values */
extern ui64_t        ui64_zero ( void );
extern ui64_t        ui64_max  ( void );

/* import and export via ISO-C "unsigned long" */
extern ui64_t        ui64_n2i  ( unsigned long n );
extern unsigned long ui64_i2n  ( ui64_t x );

/* import and export via ISO-C string of arbitrary base */
extern ui64_t        ui64_s2i  ( const char* str, char** end, int base );
extern char*         ui64_i2s  ( ui64_t x, char* str, size_t len, int base );

/* arithmetical operations */
extern ui64_t        ui64_add  ( ui64_t x, ui64_t y, ui64_t* ov );
extern ui64_t        ui64_addn ( ui64_t x, int    y, int*    ov );
extern ui64_t        ui64_sub  ( ui64_t x, ui64_t y, ui64_t* ov );
extern ui64_t        ui64_subn ( ui64_t x, int    y, int*    ov );
extern ui64_t        ui64_mul  ( ui64_t x, ui64_t y, ui64_t* ov );
extern ui64_t        ui64_muln ( ui64_t x, int    y, int*    ov );
extern ui64_t        ui64_div  ( ui64_t x, ui64_t y, ui64_t* ov );
extern ui64_t        ui64_divn ( ui64_t x, int    y, int*    ov );

/* bit operations */
extern ui64_t        ui64_and  ( ui64_t x, ui64_t y );
extern ui64_t        ui64_or   ( ui64_t x, ui64_t y );
extern ui64_t        ui64_xor  ( ui64_t x, ui64_t y );
extern ui64_t        ui64_not  ( ui64_t x );
extern ui64_t        ui64_rol  ( ui64_t x, int s, ui64_t* ov );
extern ui64_t        ui64_ror  ( ui64_t x, int s, ui64_t* ov );

/* other operations */
extern int           ui64_len  ( ui64_t x );
extern int           ui64_cmp  ( ui64_t x, ui64_t y );

#define UI128_PREFIX uuid_

/* embedding support */
#ifdef UI128_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __UI128_CONCAT(x,y) x ## y
#define UI128_CONCAT(x,y) __UI128_CONCAT(x,y)
#else
#define __UI128_CONCAT(x) x
#define UI128_CONCAT(x,y) __UI128_CONCAT(x)y
#endif
#define ui128_t     UI128_CONCAT(UI128_PREFIX,ui128_t)
#define ui128_zero  UI128_CONCAT(UI128_PREFIX,ui128_zero)
#define ui128_max   UI128_CONCAT(UI128_PREFIX,ui128_max)
#define ui128_n2i   UI128_CONCAT(UI128_PREFIX,ui128_n2i)
#define ui128_i2n   UI128_CONCAT(UI128_PREFIX,ui128_i2n)
#define ui128_s2i   UI128_CONCAT(UI128_PREFIX,ui128_s2i)
#define ui128_i2s   UI128_CONCAT(UI128_PREFIX,ui128_i2s)
#define ui128_add   UI128_CONCAT(UI128_PREFIX,ui128_add)
#define ui128_addn  UI128_CONCAT(UI128_PREFIX,ui128_addn)
#define ui128_sub   UI128_CONCAT(UI128_PREFIX,ui128_sub)
#define ui128_subn  UI128_CONCAT(UI128_PREFIX,ui128_subn)
#define ui128_mul   UI128_CONCAT(UI128_PREFIX,ui128_mul)
#define ui128_muln  UI128_CONCAT(UI128_PREFIX,ui128_muln)
#define ui128_div   UI128_CONCAT(UI128_PREFIX,ui128_div)
#define ui128_divn  UI128_CONCAT(UI128_PREFIX,ui128_divn)
#define ui128_and   UI128_CONCAT(UI128_PREFIX,ui128_and)
#define ui128_or    UI128_CONCAT(UI128_PREFIX,ui128_or)
#define ui128_xor   UI128_CONCAT(UI128_PREFIX,ui128_xor)
#define ui128_not   UI128_CONCAT(UI128_PREFIX,ui128_not)
#define ui128_rol   UI128_CONCAT(UI128_PREFIX,ui128_rol)
#define ui128_ror   UI128_CONCAT(UI128_PREFIX,ui128_ror)
#define ui128_len   UI128_CONCAT(UI128_PREFIX,ui128_len)
#define ui128_cmp   UI128_CONCAT(UI128_PREFIX,ui128_cmp)
#endif

typedef struct
{
	unsigned char x[16]; /* x_0, ..., x_15 */
} ui128_t;

#define ui128_cons(x15,x14,x13,x12,x11,x10,x9,x8,x7,x6,x5,x4,x3,x2,x1,x0) \
    { { 0x##x0, 0x##x1, 0x##x2,  0x##x3,  0x##x4,  0x##x5,  0x##x6,  0x##x7, \
    { { 0x##x8, 0x##x9, 0x##x10, 0x##x11, 0x##x12, 0x##x13, 0x##x14, 0x##x15 } }

/* particular values */
extern ui128_t        ui128_zero ( void );
extern ui128_t        ui128_max  ( void );

/* import and export via ISO-C "unsigned long" */
extern ui128_t        ui128_n2i  ( unsigned long n );
extern unsigned long  ui128_i2n  ( ui128_t x );

/* import and export via ISO-C string of arbitrary base */
extern ui128_t        ui128_s2i  ( const char* str, char** end, int base );
extern char*          ui128_i2s  ( ui128_t x, char* str, size_t len, int base );

/* arithmetical operations */
extern ui128_t        ui128_add  ( ui128_t x, ui128_t y, ui128_t* ov );
extern ui128_t        ui128_addn ( ui128_t x, int     y, int*     ov );
extern ui128_t        ui128_sub  ( ui128_t x, ui128_t y, ui128_t* ov );
extern ui128_t        ui128_subn ( ui128_t x, int     y, int*     ov );
extern ui128_t        ui128_mul  ( ui128_t x, ui128_t y, ui128_t* ov );
extern ui128_t        ui128_muln ( ui128_t x, int     y, int*     ov );
extern ui128_t        ui128_div  ( ui128_t x, ui128_t y, ui128_t* ov );
extern ui128_t        ui128_divn ( ui128_t x, int     y, int*     ov );

/* bit operations */
extern ui128_t        ui128_and  ( ui128_t x, ui128_t y );
extern ui128_t        ui128_or   ( ui128_t x, ui128_t y );
extern ui128_t        ui128_xor  ( ui128_t x, ui128_t y );
extern ui128_t        ui128_not  ( ui128_t x );
extern ui128_t        ui128_rol  ( ui128_t x, int s, ui128_t* ov );
extern ui128_t        ui128_ror  ( ui128_t x, int s, ui128_t* ov );

/* other operations */
extern int            ui128_len  ( ui128_t x );
extern int            ui128_cmp  ( ui128_t x, ui128_t y );


#define MD5_PREFIX uuid_

/* embedding support */
#ifdef MD5_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __MD5_CONCAT(x,y) x ## y
#define MD5_CONCAT(x,y) __MD5_CONCAT(x,y)
#else
#define __MD5_CONCAT(x) x
#define MD5_CONCAT(x,y) __MD5_CONCAT(x)y
#endif
#define md5_st      MD5_CONCAT(MD5_PREFIX,md5_st)
#define md5_t       MD5_CONCAT(MD5_PREFIX,md5_t)
#define md5_create  MD5_CONCAT(MD5_PREFIX,md5_create)
#define md5_init    MD5_CONCAT(MD5_PREFIX,md5_init)
#define md5_update  MD5_CONCAT(MD5_PREFIX,md5_update)
#define md5_store   MD5_CONCAT(MD5_PREFIX,md5_store)
#define md5_format  MD5_CONCAT(MD5_PREFIX,md5_format)
#define md5_destroy MD5_CONCAT(MD5_PREFIX,md5_destroy)
#endif

struct md5_st;
typedef struct md5_st md5_t;

#define MD5_LEN_BIN 16
#define MD5_LEN_STR 32

typedef enum
{
	MD5_RC_OK  = 0,
	MD5_RC_ARG = 1,
	MD5_RC_MEM = 2
} md5_rc_t;

extern md5_rc_t md5_create  ( md5_t** md5 );
extern md5_rc_t md5_init    ( md5_t*  md5 );
extern md5_rc_t md5_update  ( md5_t*  md5, const void*  data_ptr, size_t  data_len );
extern md5_rc_t md5_store   ( md5_t*  md5,       void** data_ptr, size_t* data_len );
extern md5_rc_t md5_format  ( md5_t*  md5,       char** data_ptr, size_t* data_len );
extern md5_rc_t md5_destroy ( md5_t*  md5 );

#define SHA1_PREFIX uuid_

/* embedding support */
#ifdef SHA1_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __SHA1_CONCAT(x,y) x ## y
#define SHA1_CONCAT(x,y) __SHA1_CONCAT(x,y)
#else
#define __SHA1_CONCAT(x) x
#define SHA1_CONCAT(x,y) __SHA1_CONCAT(x)y
#endif
#define sha1_st      SHA1_CONCAT(SHA1_PREFIX,sha1_st)
#define sha1_t       SHA1_CONCAT(SHA1_PREFIX,sha1_t)
#define sha1_create  SHA1_CONCAT(SHA1_PREFIX,sha1_create)
#define sha1_init    SHA1_CONCAT(SHA1_PREFIX,sha1_init)
#define sha1_update  SHA1_CONCAT(SHA1_PREFIX,sha1_update)
#define sha1_store   SHA1_CONCAT(SHA1_PREFIX,sha1_store)
#define sha1_format  SHA1_CONCAT(SHA1_PREFIX,sha1_format)
#define sha1_destroy SHA1_CONCAT(SHA1_PREFIX,sha1_destroy)
#endif

struct sha1_st;
typedef struct sha1_st sha1_t;

#define SHA1_LEN_BIN 20
#define SHA1_LEN_STR 40

typedef enum
{
	SHA1_RC_OK  = 0,
	SHA1_RC_ARG = 1,
	SHA1_RC_MEM = 2,
	SHA1_RC_INT = 3
} sha1_rc_t;

extern sha1_rc_t sha1_create  ( sha1_t** sha1 );
extern sha1_rc_t sha1_init    ( sha1_t*  sha1 );
extern sha1_rc_t sha1_update  ( sha1_t*  sha1, const void*  data_ptr, size_t  data_len );
extern sha1_rc_t sha1_store   ( sha1_t*  sha1,       void** data_ptr, size_t* data_len );
extern sha1_rc_t sha1_format  ( sha1_t*  sha1,       char** data_ptr, size_t* data_len );
extern sha1_rc_t sha1_destroy ( sha1_t*  sha1 );


#define MAC_PREFIX uuid_

/* embedding support */
#ifdef MAC_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __MAC_CONCAT(x,y) x ## y
#define MAC_CONCAT(x,y) __MAC_CONCAT(x,y)
#else
#define __MAC_CONCAT(x) x
#define MAC_CONCAT(x,y) __MAC_CONCAT(x)y
#endif
#define mac_address MAC_CONCAT(MAC_PREFIX,mac_address)
#endif

#define MAC_LEN 6

extern int mac_address( unsigned char* _data_ptr, size_t _data_len );


#define PRNG_PREFIX uuid_

/* embedding support */
#ifdef PRNG_PREFIX
#if defined(__STDC__) || defined(__cplusplus)
#define __PRNG_CONCAT(x,y) x ## y
#define PRNG_CONCAT(x,y) __PRNG_CONCAT(x,y)
#else
#define __PRNG_CONCAT(x) x
#define PRNG_CONCAT(x,y) __PRNG_CONCAT(x)y
#endif
#define prng_st      PRNG_CONCAT(PRNG_PREFIX,prng_st)
#define prng_t       PRNG_CONCAT(PRNG_PREFIX,prng_t)
#define prng_create  PRNG_CONCAT(PRNG_PREFIX,prng_create)
#define prng_data    PRNG_CONCAT(PRNG_PREFIX,prng_data)
#define prng_destroy PRNG_CONCAT(PRNG_PREFIX,prng_destroy)
#endif

struct prng_st;
typedef struct prng_st prng_t;

typedef enum
{
	PRNG_RC_OK  = 0,
	PRNG_RC_ARG = 1,
	PRNG_RC_MEM = 2,
	PRNG_RC_INT = 3
} prng_rc_t;

extern prng_rc_t prng_create  ( prng_t** prng );
extern prng_rc_t prng_data    ( prng_t*  prng, void* data_ptr, size_t data_len );
extern prng_rc_t prng_destroy ( prng_t*  prng );

DECLARATION_BEGIN

/* OSSP uuid version (compile-time information) */
#define UUID_VERSION  @UUID_VERSION_HEX@

/* encoding octet stream lengths */
#define UUID_LEN_BIN  (128 /*bit*/ / 8 /*bytes*/)
#define UUID_LEN_STR  (128 /*bit*/ / 4 /*nibbles*/ + 4 /*hyphens*/)
#define UUID_LEN_SIV  (39  /*int(log(10,exp(2,128)-1)+1) digits*/)

/* API return codes */
typedef enum
{
	UUID_RC_OK   = 0,        /* everything ok    */
	UUID_RC_ARG  = 1,        /* invalid argument */
	UUID_RC_MEM  = 2,        /* out of memory    */
	UUID_RC_SYS  = 3,        /* system error     */
	UUID_RC_INT  = 4,        /* internal error   */
	UUID_RC_IMP  = 5         /* not implemented  */
} uuid_rc_t;

/* UUID make modes */
enum
{
	UUID_MAKE_V1 = ( 1 << 0 ), /* DCE 1.1 v1 UUID */
	UUID_MAKE_V3 = ( 1 << 1 ), /* DCE 1.1 v3 UUID */
	UUID_MAKE_V4 = ( 1 << 2 ), /* DCE 1.1 v4 UUID */
	UUID_MAKE_V5 = ( 1 << 3 ), /* DCE 1.1 v5 UUID */
	UUID_MAKE_MC = ( 1 << 4 ) /* enforce multi-cast MAC address */
};

/* UUID import/export formats */
typedef enum
{
	UUID_FMT_BIN = 0,        /* binary representation (import/export) */
	UUID_FMT_STR = 1,        /* string representation (import/export) */
	UUID_FMT_SIV = 2,        /* single integer value  (import/export) */
	UUID_FMT_TXT = 3         /* textual description   (export only)   */
} uuid_fmt_t;

/* UUID abstract data type */
struct uuid_st;
typedef struct uuid_st uuid_t;

/* UUID object handling */
extern uuid_rc_t     uuid_create   (      uuid_t** _uuid );
extern uuid_rc_t     uuid_destroy  (      uuid_t*  _uuid );
extern uuid_rc_t     uuid_clone    ( const uuid_t*  _uuid, uuid_t** _clone );

/* UUID generation */
extern uuid_rc_t     uuid_load     (      uuid_t*  _uuid, const char* _name );
extern uuid_rc_t     uuid_make     (      uuid_t*  _uuid, unsigned int _mode, ... );

/* UUID comparison */
extern uuid_rc_t     uuid_isnil    ( const uuid_t*  _uuid,                       int* _result );
extern uuid_rc_t     uuid_compare  ( const uuid_t*  _uuid, const uuid_t* _uuid2, int* _result );

/* UUID import/export */
extern uuid_rc_t     uuid_import   (      uuid_t*  _uuid, uuid_fmt_t _fmt, const void*  _data_ptr, size_t  _data_len );
extern uuid_rc_t     uuid_export   ( const uuid_t*  _uuid, uuid_fmt_t _fmt,       void*  _data_ptr, size_t* _data_len );

/* library utilities */
extern const char*   uuid_error    ( uuid_rc_t _rc );
extern unsigned long uuid_version  ( void );

DECLARATION_END


/*
   Universal UID generator
   Wrapper for uuid-1.6.0 library

   Part of Linderdaum Engine
*/

extern uuid_t* uuid;
extern uuid_rc_t uuid_rc;

extern char uuid_buffer[UUID_LEN_STR+1];
extern void* uuid_buffer_ptr;

/* initializes uuid_generator */
#ifdef __cplusplus
extern "C"
#endif
const char* UUID_Start();

/*
   generates new UUID and stores it in uuid_buffer
   returns error code
*/
#ifdef __cplusplus
extern "C"
#endif
const char* UUID_Next();

/* deallocates temporary buffers */
#ifdef __cplusplus
extern "C"
#endif
const char* UUID_Stop();

#endif /* __UUID_H__ */

/*
 * 03/10/2008
     Log section added
*/
