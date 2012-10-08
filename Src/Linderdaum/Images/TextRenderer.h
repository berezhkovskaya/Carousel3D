/**
 * \file TextRenderer.h
 * \brief FreeType-based text renderer
 * \version 0.5.99
 * \date 13/04/2011
 * \author Viktor Latypov, 2011
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef __TextRenderer__h__included__
#define __TextRenderer__h__included__

#include "Core/iObject.h"
#include "Images/ImageCache.h"

class clBitmap;
class iVertexArray;
class clVertexAttribs;

#include <map>

#include <cstring>

#include "ft.h"

enum LTextEncoding
{
	/// Encoding constant for UTF8 (default)
	Encoding_UTF8  = 0,

	/// Encoding for direct usage of UCS2 strings
	Encoding_UCS2  = 1
};

/**
   \brief Single font face

   Declared as an iObject to allow .NET/script marshalling

   Family and style cannot be used yet
*/
class scriptfinal netexportable clFontFace: public iObject
{
public:
	clFontFace(): FFileName( "" ), FFamily( "" ), FStyle( 0 ) {}
	virtual ~clFontFace() {}

public:
	/** Property(Name=FileName, Type=string, FieldName=FFileName) */
	/** Property(Name=Family,   Type=string, FieldName=FFamily)   */
	/** Property(Name=Style,    Type=int,    FieldName=FStyle)    */

	/// Source file name
	LString FFileName;

	/// Font family
	LString FFamily;

	/// Face style flags (bold, italic, strike-out, underline)
	unsigned int FStyle;
};

struct sVAContainer
{
	sVAContainer():FVertexArray(NULL),FVertexAttribs(NULL) {};
	~sVAContainer();

	bool empty() const
	{
		return FVertexArray == NULL || FVertexAttribs == NULL;
	}

	void Init( sEnvironment* Env );
	void Done();
	void Reset();

	iVertexArray*    FVertexArray;
	clVertexAttribs* FVertexAttribs;
};

/**
   \brief FreeType-based text renderer

   Single instance of this class is not thread-safe,
   but multiple instances share only the pointers to FreeType functions
   and it is safe to use them simultaneously.

   This is a simple wrapper for FreeType to allow single-line text rendering with simple kerning.

   It is not advanced and does not support ligatures, mixed left-to-right/left-to-right rendering,
   alignment, propert line breaks etc.

   It is just a little better than fixed-size bitmapped fonts we had before.

   Internally it is used to create text banners (textured quad filled with rendered string)

   Usage
   =====

   1. To render the line of text one might use RenderLineOnBitmap().

   Font handle can be obtained by using the GetFontHandle() method. If the required font file is not loaded,
   such an attempt is made. No font substitution is provided, so the function just exits quietly if some internal
   error happens.

   For the Right-To-Left scripts negative glyph sizes are used. So for the proper output one must
   calculate the line width and adjust the StartX parameter.

   TheFont = TR->GetFontHandle("SomeFontFileSupportedByFreeType");

   TR->RenderRenderLineOnBitmap();

   If a lot of fonts are used, then it is useful to call PreloadFont() method for each of them to avoid "JIT" loading,
   which can slow things down.

   2. To measure the horizontal size of the text string the CalculateLineWidth() method is provided which operates
   on internal buffer.

   TR->LoadTextString("Text", SomeFont, Height);

   int TextWidthInPixels = TR->CalculateLineWidth();
*/
class scriptfinal netexportable clTextRenderer: public iObject
{
public:
	clTextRenderer();
	virtual ~clTextRenderer();

	/// Check if the FreeType library is initialized
	scriptmethod bool IsInitialized() const { return FInitialized; }

	/// Select used font
	scriptmethod void SetFont( clFontFace* Font, int FontHeight ) { FCurrentFont = Font; FCurrentFontHeight = FontHeight; }

	/// Select font using symbolic name or filename
	scriptmethod void SetFontByName( const LString& FontName, int FontHeight ) { SetFont( GetFontHandle( FontName ), FontHeight ); }

	/// Load a single line of text to internal buffers and calculate individual character sizes/positions
	scriptmethod bool LoadTextStringWithFont( const LString& TextString, clFontFace* Font, int FontHeight, LTextEncoding Encoding );

	/// Load a single line of text to internal buffers and calculate individual character sizes/positions
	scriptmethod bool LoadTextString( const LString& TextString, LTextEncoding Encoding ) { return LoadTextStringWithFont( TextString, FCurrentFont, FCurrentFontHeight, Encoding ); }

	/// Compute the width of a previously loaded text string
	scriptmethod int  CalculateLineWidth() const;

	/// Compute the height of a previously loaded text string
	scriptmethod int  CalculateLineHeight() const;

	/// Calculate bounding box and the base line for the loaded text string (in pixels)
	noexport scriptmethod void CalculateLineParameters( int* Width, int* MinY, int* MaxY, int* BaseLine ) const;

	/// Calculate bounding box and the base line for the loaded text string (in normalized screen coordinates)
	noexport scriptmethod void CalculateLineParametersFloat( int* Width, int* MinY, int* MaxY, int* BaseLine, float* TextWidth, float* TextHeight ) const;

	/**
	   \brief Render a single line of text using the specified font

	   Converts the text line to the glyph run. Changes the state of internal caches by loading new glyphs.

	   \param TextString - source text string
	   \param Font       - used font
	   \param FontHeight - font height
	   \param StartX     - starting pixel for the text line
	   \param Y          - base vertical position for the text
	   \param Color      - RGBA color for symbols
	   \param Encoding   - Encoding_UTF8 or Encoding_UCS2
	   \param Out        - output bitmap
	*/
	scriptmethod void RenderLineOnBitmap( const LString& TextString, clFontFace* Font, int FontHeight, int StartX, int Y, const vec4& Color, LTextEncoding Encoding, bool LeftToRight, clBitmap* Out );
	scriptmethod size_t RenderLineInVertexAttribs( const LVector2& Origin, const LString& TextString, clFontFace* Font, int FontHeight, int StartX, int Y, const vec4& Color, LTextEncoding Encoding, bool LeftToRight, std::vector<sVAContainer>& Out );

	/// Render the line using selected font
	scriptmethod void RenderLine( const LString& TextString, int X, int Y, const LVector4& Color, LTextEncoding Encoding, bool LeftToRight, clBitmap* Out );

	/// Create appropriate bitmap and render text line using selected font
	scriptmethod clBitmap* RenderText( const LString& TextString, const LVector4& Color, LTextEncoding Encoding, bool LeftToRight );

	/// Create appropriate bitmap and render text line using specified font
	scriptmethod clBitmap* RenderTextWithFont( const LString& TextString, clFontFace* Font, int FontHeight, const LVector4& Color, LTextEncoding Encoding, bool LeftToRight );

	/// Force font file loading to avoid late binding
	scriptmethod bool PreloadFont( const LString& FileName ) { return ( LoadFontFile( FileName ) == 0 ); }

	/// Get a handle to font. Loads required files automatically and changes intenals buffers. Cannot be const
	scriptmethod clFontFace* GetFontHandle( const LString& FileName );

	/// Set current mask/bitmap mode
	scriptmethod void SetMaskMode( bool RenderMaskOnly ) { FMaskMode = RenderMaskOnly; }

	/// Get current mask mode
	scriptmethod bool GetMaskMode() const { return FMaskMode; }

private:

#pragma region Text rendering setup

	/// Render alpha-blended pixels or only the alpha channel
	bool FMaskMode;

	/// Selected font height
	int FCurrentFontHeight;

	/// Current font face
	clFontFace* FCurrentFont;

#pragma endregion

#pragma region FreeType internals

	/// Internal flag to check the FreeType state
	bool FInitialized;

	/// Local instance of the library (for thread-safe execution)
	FT_Library FLibrary;

	/// Cache manager
	FTC_Manager FManager;

	/// Glyph cache
	FTC_ImageCache FImageCache;

	/// Character map cache
	FTC_CMapCache FCMapCache;

	/// Load FreeType dll and initialize local library instance with caches
	void InitFreeType();

	/// Decrement reference count to FreeType dll and destroy local library instance
	void StopFreeType();

	/**
	   \brief Internal representation of a single freetype character

	   Copied from UTF-8/ASCII strings by setting the 'FChar' field
	*/
	struct sFTChar
	{
		/// UCS2(4) character, suitable for FreeType
		FT_UInt FChar;

		/// Handle for the font face
		FT_Face FFace;

		/// Internal character index
		FT_UInt FIndex;

		/// Handle for the rendered glyph
		FT_Glyph FGlyph;

		/// Fixed-point character advance and character size
		FT_F26Dot6 FAdvance, FWidth;

		/// Cache node for this glyph
		FTC_Node FCacheNode;

		/// Default parameters
		sFTChar() : FChar( 0 ), FFace( NULL ), FIndex( ( FT_UInt )( -1 ) ), FGlyph( NULL ), FAdvance( 0 ), FWidth( 0 ), FCacheNode( NULL ) { }
	};

	/// Adjust current glyph's advance using information about adjacent symbols
	void Kern( sFTChar& Left, const sFTChar& Right );

	/// Get the width and advance information for the char
	void SetAdvance( sFTChar& Char );

	/// Release cache instances for string characters
	void FreeString();

	/// List of buffers with loaded font files. Map is used to prevent multiple file reads
	std::map<LString, void*> FAllocatedFonts;

	/// List of initialized font face handles
	std::map<LString, FT_Face> FFontFaceHandles;

	/// List of available font faces
	std::vector<clFontFace*> FFontFaces;

	/// Internal face requester for FreeType (loads fonts from memory blocks)
	static FT_Error FreeType_Face_Requester( FTC_FaceID FaceID, FT_Library Library, FT_Pointer RequestData, FT_Face* TheFace );

	/// Get the char index in selected font. TODO: handle non-default charmaps for MacOS
	FT_UInt  GetCharIndex( clFontFace* Font, FT_UInt Char );

	/// Get the rendered glyph for Char using specified font and font height
	FT_Glyph GetGlyph( clFontFace* Font, int Height, FT_UInt Char, FT_UInt LoadFlags, FTC_Node* CNode );

	/// Construct a new face with specified dimension
	FT_Face  GetSizedFace( clFontFace* Font, int Height );

	/// Load font file and store it in internal list for later use
	FT_Error LoadFontFile( const LString& FileName );

	/// Draw the single character on the image
	void DrawGlyphOnBitmap( clBitmap* Out, FT_Bitmap* Bitmap, int X0, int Y0, const LVector4& Color ) const;

	/// Inserty a glyph bitmap into the glyphs cache
	void UpdateGlyphsCache( FT_Bitmap* Bitmap, const sFTChar& Char, int Size, sCacheEntry* Entry );

#pragma endregion

#pragma region UTF8 decoding

/// These macros are local and are undefined at the end of this region
#define UTF8_LINE_END (0)
#define UTF8_DECODE_ERROR (-1)

	/// Buffer with the current string
	std::vector<sFTChar> FString;

	/// Fill the FString buffer (decode utf8 to FT_Uints)
	void DecodeUTF8( const char* InStr )
	{
		FIndex = 0;
		FBuffer = InStr;
		FLength = ( int )strlen( InStr );

		FString.clear();

		int R = DecodeNextUTF8Char();

		while ( ( R != UTF8_LINE_END ) && ( R != UTF8_DECODE_ERROR ) )
		{
			sFTChar Ch;
			Ch.FFace      = NULL;
			Ch.FGlyph     = NULL;
			Ch.FCacheNode = NULL;
			Ch.FIndex   = ( FT_UInt )( -1 );
			Ch.FAdvance = 0;
			Ch.FWidth   = 0;
			Ch.FChar    = R;

			FString.push_back( Ch );

			R = DecodeNextUTF8Char();
		}
	}

	/// Current character index
	int  FIndex;

	/// Source buffer length
	int  FLength;

	/// Source buffer pointer
	const char* FBuffer;

	/// Current byte
	int  FByte;

	/// Gets the next byte. Returns UTF8_LINE_END if there are no more bytes.
	inline int NextUTF8() { return ( FIndex >= FLength ) ? UTF8_LINE_END : ( FBuffer[FIndex++] & 0xFF ); }

	/// Gets the low 6 bits of the next continuation byte. Returns UTF8_DECODE_ERROR if it is not a contination byte.
	inline int ContUTF8() { int c = NextUTF8(); return ( ( c & 0xC0 ) == 0x80 ) ? ( c & 0x3F ) : UTF8_DECODE_ERROR; }

	/*
	    strict UTF-8 decoder for continuations not more than 4 bytes

	    Code     | Cont |  Min  |    Max
	    ==================================
	    0xxxxxxx |   0  |     0 |     127
	    110xxxxx |   1  |   128 |    2047
	    1110xxxx |   2  |  2048 |   65535 \ {55296..57343}
	    11110xxx |   3  | 65536 | 1114111
	*/

	/// Extract the next character (between 0 and 1114111) or return UTF8_END/UTF8_ERROR
	int DecodeNextUTF8Char()
	{
		// the first byte of the character and the result
		int c, r;

		if ( FIndex >= FLength ) { return FIndex == FLength ? UTF8_LINE_END : UTF8_DECODE_ERROR; }

		c = NextUTF8();

		// Zero continuation (0 to 127)
		if ( ( c & 0x80 ) == 0 ) { return c; }

		// First contination (128 to 2047)
		if ( ( c & 0xE0 ) == 0xC0 )
		{
			int c1 = ContUTF8();

			if ( c1 < 0 ) { return UTF8_DECODE_ERROR; }

			r = ( ( c & 0x1F ) << 6 ) | c1;
			return r >= 128 ? r : UTF8_DECODE_ERROR;
		}

		// Second continuation (2048 to 55295 and 57344 to 65535)
		if ( ( c & 0xF0 ) == 0xE0 )
		{
			int c1 = ContUTF8();
			int c2 = ContUTF8();

			if ( c1 < 0 || c2 < 0 ) { return UTF8_DECODE_ERROR; }

			r = ( ( c & 0x0F ) << 12 ) | ( c1 << 6 ) | c2;
			return r >= 2048 && ( r < 55296 || r > 57343 ) ? r : UTF8_DECODE_ERROR;
		}

		// Third continuation (65536 to 1114111)
		if ( ( c & 0xF8 ) == 0xF0 )
		{
			int c1 = ContUTF8();
			int c2 = ContUTF8();
			int c3 = ContUTF8();

			if ( c1 < 0 || c2 < 0 || c3 < 0 ) { return UTF8_DECODE_ERROR; }

			r = ( ( c & 0x0F ) << 18 ) | ( c1 << 12 ) | ( c2 << 6 ) | c3;
			return r >= 65536 && r <= 1114111 ? r : UTF8_DECODE_ERROR;
		}

		return UTF8_DECODE_ERROR;
	}

#undef UTF8_ERROR
#undef UTF8_END

#pragma endregion
};

class scriptfinal clGlyphsCache: public iObject
{
public:
	clGlyphsCache();
	virtual ~clGlyphsCache();
	//
	// iObject interface
	//
	virtual void    AfterConstruction();
	//
	// clGlyphsManager
	//
	scriptmethod sCacheEntry    InsertGlyph( clBitmap* Bitmap, uint64_t GlyphKey );
	scriptmethod bool           HasGlyph( uint64_t GlyphKey, sCacheEntry* Entry );
	scriptmethod size_t         GetTotalPages() const;
	scriptmethod clBitmap*      GetPageBitmap(size_t Idx) const;
	scriptmethod iTexture*      GetPageTexture(size_t Idx) const;
private:
	clPtr<clImageCache>               FCache;
	std::map< uint64_t, sCacheEntry > FGlyphsMap;
};

#endif
