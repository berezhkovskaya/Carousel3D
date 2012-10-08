/**
   Useful volume processing routines

   (C) Viktor Latypov, 2008-2011

   Use freely, just mention the original author.

   Not the part of Linderdaum Engine, so the coding convention is different and there are no dependencies on the Engine here
*/

#include "VolumeLib.h"

#include <stdio.h>

/*
   This code is really simple, but the number of used variables is too big

   We just extract eight voxels with coords (floor/ceil for OldX/OldY/OldZ)
   and trilinearly interpolate between them to get the final voxel

   The template parameter VoxelT assumes that there are appropriate arithmetical
   operators for this type

   Tri-cubic interpolation is similar, but we have to fetch a lot more voxels
*/

/*
   CurrentSlice is the feedback variable
*/

// VolumeWriter *writer = new LinearVolumeWriter(NewW, NewH, NewD, sizeof(VoxelT));


// rescaling with no filtering

template <class VoxelT> VoxelT GetLineSample( VOLUME_PTR Data, int idx )
{
	return ( reinterpret_cast<VoxelT*>( Data ) )[idx];
}

/*
   Weighted sum of two values
*/
template <class T> T SafeLinearInterpolate( double w1, T x1, double w2, T x2 )
{
	// this will do for the scalar data, but not vectors ...
	double v = static_cast<double>( x1 ) * w1 + static_cast<double>( x2 ) * w2;

	return static_cast<T>( v );
}


// TODO : convert 8-bit to 16-bit template specializations
template <class InT, class OutT> OutT ResampleValue( InT in )
{
	// HACK :
	if ( sizeof( InT ) == 2 && sizeof( OutT ) == 1 )
	{
		// resample unsigned short to unsigned char
		unsigned int* ThePtr = reinterpret_cast<unsigned int*>( &in );
		unsigned int OutValue = *ThePtr / 256;
		OutT* OutPtr = reinterpret_cast<OutT*>( &OutValue );
		return *OutPtr;
	}

	// FIXME : this is NOT correct !
	OutT out = static_cast<OutT>( in );

	return out;
}

template <class VoxelT, class VoxelOutT>
void RescaleVolumeBlock( VolumeSlicer* inputVolume,
                         VolumeWriter* writer,
                         int OldW, int OldH, int OldD,
                         int NewW, int NewH, int NewD, int StartZ, int FinishZ,
                         int* CurrentSlice )
{
	double XScale = static_cast<double>( OldW ) / static_cast<double>( NewW );
	double YScale = static_cast<double>( OldH ) / static_cast<double>( NewH );
	double ZScale = static_cast<double>( OldD ) / static_cast<double>( NewD );

	*CurrentSlice = StartZ;

	// create temporary slice (ownsData = true)
	VolumeSlice NewZSlice( NewW, NewH, sizeof( VoxelOutT ), true, 0 );

	// version with filtering, slice-by-slice (in selected region)
	for ( int NewZ = StartZ ; NewZ < FinishZ ; NewZ++ )
	{
		// reset write process
		NewZSlice.MoveToFirstVoxel();

		double OldZ = static_cast<double>( NewZ ) * ZScale;

		double MinOldZ = ::floor( OldZ );

		// get pointers to two slices used in interpolation
		VolumeSlice* MinZOldSlice = inputVolume->GetZSlicePtr( static_cast<int>( MinOldZ ) );

		for ( int NewY = 0 ; NewY < NewH ; NewY++ )
		{
			double OldY = static_cast<double>( NewY ) * YScale;

			double MinOldY = ::floor( OldY );

			// get pointers to four distinct lines with samples
			VOLUME_PTR MinYMinZOldLine = MinZOldSlice->GetRowPtr( static_cast<int>( MinOldY ) );

			for ( int NewX = 0 ; NewX < NewW ; NewX++ )
			{
				double OldX = static_cast<double>( NewX ) * XScale;

				double MinOldX = ::floor( OldX );

				// fetch one voxel from OldVolume
				VoxelOutT ResultVal = ResampleValue<VoxelT, VoxelOutT>( GetLineSample<VoxelT>( MinYMinZOldLine, static_cast<int>( MinOldX ) ) );

				NewZSlice.PutNextVoxel<VoxelOutT>( ResultVal );
			}
		}

		// write this slice to disk
		writer->WriteSliceZ( NewZ, &NewZSlice );

		( *CurrentSlice )++;
	}
}

VolumeSlice* VolumeSlicer::GetZSlicePtr( int Z )
{
	// try the cache
	VolumeSlice* theSlice = GetSliceByCoord( Z );

	if ( theSlice == 0 )
	{
		// read it !

		// TODO : support layouts for arrays

		size_t Ofs = Width * Height * BPP * Z;

		input->SeekTo( Ofs );

		theSlice = &slices[currentSliceInCache];
		sliceCoords[currentSliceInCache] = Z;
		currentSliceInCache++;

		if ( currentSliceInCache >= MAX_SLICES_IN_ITERATION ) { currentSliceInCache = 0; }

		input->ReadBytes( theSlice->GetData(), Width * Height * BPP );
		// TODO : use memory-mapping for files
		// here we should just pass the pointer to 'theSlice' using SetData
		// 'theSlice' should not be the owner of the data
	}

	return theSlice;
}

VolumeSlice* VolumeSlicer::GetSliceByCoord( int Z )
{
	if ( Z < 0 || Z >= Depth )
	{
		// out of bounds !
		if ( ZeroSlice == 0 )
		{
			// allocate zero slice
			ZeroSlice = new VolumeSlice( Width, Height, BPP, true, 0 );
			// TODO : fill it with zeros...
			ZeroSlice->FillWithZeroes();
		}

		return ZeroSlice;
	}

	for ( int i = 0 ; i < MAX_SLICES_IN_ITERATION ; i++ )
	{
		if ( sliceCoords[i] == Z )
		{
			return &slices[i];
		}
	}

	return 0;
}

VolumeSlicer::~VolumeSlicer()
{
	if ( ZeroSlice != 0 )
	{
		delete ZeroSlice;
	}
}

// Neighbourhood iterator

VoxelNeighbourhoodIterator::~VoxelNeighbourhoodIterator()
{
	delete [] reinterpret_cast<unsigned char*>( Values );

	delete [] SelectedSlices;

	for ( int i = 0 ; i < YSize ; i++ )
	{
		delete [] ( SelectedLines[i] );
	}

	delete [] SelectedLines;
}

/*
int VoxelNeighbourhoodIterator::GetSelectedLineAddr(int z, int y)
{
   return SelectedLines[(z + ZSizeD)*YSize + (y + YSizeD)];
}
*/

void VoxelNeighbourhoodIterator::SetCurrentVoxel( int i, int j, int k )
{
	// select slices/lines
	for ( int z = -ZSizeD ; z < ZSizeD + 1 ; z++ )
	{
		VolumeSlice* SlicePtr = Slicer->GetZSlicePtr( k + z );
		SelectedSlices[z + ZSizeD] = SlicePtr;

		for ( int y = -YSizeD ; y < YSizeD + 1 ; y++ )
		{
			VOLUME_PTR LinePtr = SelectedSlices[z + ZSizeD]->GetSafeRowPtr( j + y );
			SelectedLines[z + ZSizeD][y + YSizeD] = LinePtr;
			// 32-bit unsafe !
//			SelectedLines[(z + ZSizeD)*YSize + (y + YSizeD)] = reinterpret_cast<int>(LinePtr);
		}
	}

	CurX = i;
	CurY = j;
	CurZ = k;
}

void VoxelNeighbourhoodIterator::AllocateValues()
{
	BPP    = Slicer->GetBPP();
	Width  = Slicer->GetWidth();
	Height = Slicer->GetHeight();
	Depth  = Slicer->GetDepth();

	int size = ( XSize + 1 ) * ( YSize + 1 ) * ( ZSize + 1 ) * BPP;
	//
	Values = new unsigned char[size];

	// line/slice cache
	SelectedSlices = new VolumeSlice*[ZSize+1];

	SelectedLines  = new VOLUME_PTR*[YSize+1];

	for ( int i = 0 ; i < YSize + 1 ; i++ )
	{
		SelectedLines[i] = new VOLUME_PTR[YSize];
	}

//	SelectedLines = new int[(ZSize+1)*(YSize+1)*sizeof(int)];

	SetIterationRange( 0, 0, 0, Width - 1, Height - 1, Depth - 1 );
}

// temporary workaround : we just fetch all the samples around from selected slices
void VoxelNeighbourhoodIterator::FetchNeighbourhood()
{
	VOLUME_PTR ValuesPtr = Values;

	int i, j, k, p;

	unsigned char ByteValue = 0;
	VOLUME_PTR VoxelAddr;
//   VOLUME_PTR_SCALAR DestAddr;
	VOLUME_PTR_SCALAR DestOfs;

	for ( k = -ZSizeD ; k < ZSizeD + 1 ; k++ )
	{
		for ( j = -YSizeD ; j < YSizeD + 1 ; j++ )
		{
			VOLUME_PTR SelLinePtr = SelectedLines[k + ZSizeD][j + YSizeD];
//			VOLUME_PTR SelLinePtr = SelectedLines[(k + ZSizeD)*YSize + (j + YSizeD)];
//			VOLUME_PTR SelLinePtr = reinterpret_cast<VOLUME_PTR>(SelectedLines[(k + ZSizeD)*YSize + (j + YSizeD)]);

			DestOfs = ( ( ( k + ZSizeD ) * XSize + ( j + YSizeD ) ) * YSize ) * BPP;

			// TODO : for extremely large volumes we should cache even lines without loading the whole slice !
			if ( CurX < XSizeD || CurX >= Width - XSizeD )
			{
				// byte-by-byte copy
				for ( i = -XSizeD ; i < XSizeD + 1 ; i++ )
				{
					for ( p = 0 ; p < BPP ; p++ )
					{
						// TODO : use increments instead of direct calculation
						VoxelAddr = &reinterpret_cast<unsigned char*>( SelLinePtr )[CurX + ( i )*BPP + p];

						// TODO : remove this from the inner loop, do the recalculation of i_start and i_end !
						if ( CurX + i < 0 || CurX + i > Width - 1 )
						{
							ByteValue = 0;
						}
						else
						{
							ByteValue = *( reinterpret_cast<unsigned char*>( VoxelAddr ) );
						}

						// TODO : use increments instead of direct calculation
						/*
						                  DestAddr = ValuesPtr+DestOfs+(i+XSizeD)*BPP+p;
						                  *reinterpret_cast<unsigned char *>(DestAddr) = ByteValue;
						*/
						reinterpret_cast<unsigned char*>( ValuesPtr )[DestOfs+( i+XSizeD )*BPP+p] = ByteValue;
					}
				}
			}
			else
			{
				// block copy
				// copy block (XSize*BPP) from CurrentLine + (CurX-XSizeD)*
				/*
				            memcpy(reinterpret_cast<VOLUME_PTR>(ValuesPtr + DestOfs),
				                   reinterpret_cast<VOLUME_PTR>(SelLinePtr + (CurX - XSizeD)*BPP), XSize*BPP);
				*/
				memcpy( &reinterpret_cast<unsigned char*>( ValuesPtr )[DestOfs],
				        &reinterpret_cast<unsigned char*>( SelLinePtr )[( CurX - XSizeD )*BPP], XSize * BPP );
				// TODO : use increments instead of direct calculation
			}
		}
	}
}

bool VoxelNeighbourhoodIterator::MoveToNextXVoxel()
{
	if ( CurX >= MaxX )
	{
		CurX = MinX;
		CurY++;

		if ( CurY >= MaxY )
		{
			CurY = MinY;
			CurZ++;

			if ( CurZ >= MaxZ )
			{
				return false;
			}
		}

		// refresh line and slice pointers
		MoveToVoxel( CurX, CurY, CurZ );
	}
	else
	{
		// just increment X
		CurX++;
	}

	// finally fetch the voxels
	FetchNeighbourhood();

	return true;
}

bool VoxelNeighbourhoodIterator::MoveToNextYVoxel()
{
	if ( CurY >= MaxY )
	{
		CurY = MinY;
		CurZ++;

		if ( CurZ >= MaxZ )
		{
			return false;
		}
	}
	else
	{
		// just increment Y
		CurY++;
	}

	// refresh line and slice pointers
	MoveToVoxel( CurX, CurY, CurZ );

	FetchNeighbourhood();

	return true;
}

bool VoxelNeighbourhoodIterator::MoveToNextZVoxel()
{
	// just increment Z
	CurZ++;

	if ( CurZ >= MaxZ )
	{
		return false;
	}

	MoveToVoxel( CurX, CurY, CurZ );

	FetchNeighbourhood();

	return true;
}

void VoxelNeighbourhoodIterator::SetIterationRange( int minx, int miny, int minz, int maxx, int maxy, int maxz )
{
	MinX = minx;
	MinY = miny;
	MinZ = minz;
	MaxX = maxx;
	MaxY = maxy;
	MaxZ = maxz;
}

void VoxelNeighbourhoodIterator::MoveToVoxel( int i, int j, int k )
{
	SetCurrentVoxel( i, j, k );
	FetchNeighbourhood();
}

void VoxelNeighbourhoodIterator::MoveToFirstVoxel()
{
	// Iteration starts at (MinX,MinY,MinZ) and finishes at (MaxX,MaxY,MaxZ)
	SetCurrentVoxel( MinX, MinY, MinZ );
	FetchNeighbourhood();
}

bool VoxelNeighbourhoodIterator::IsLastVoxel() const
{
	if ( CurZ >= MaxZ ) { return true; }

	return false;
}

void SeriesVolumeWriter::WriteSliceZ( int Z, VolumeSlice* Slice )
{
	char buf[1024];
	buf[NameBase.length()+0] = '0';
	buf[NameBase.length()+1] = '0';
	buf[NameBase.length()+2] = '0';
	buf[NameBase.length()+3] = '0';
#ifndef _MSC_VER
	sprintf( buf, string( NameBase + "%04d.slice" + string( WriteBMP ? string( ".bmp" ) : string( "" ) ) ).c_str(), Z );
#else
	sprintf_s( buf, 1024, string( NameBase + "%04d.slice" + string( WriteBMP ? string( ".bmp" ) : string( "" ) ) ).c_str(), Z );
#endif
	create_empty_file( buf );
	OutStream* outStream = create_file_writer( buf );
	outStream->SeekTo( 0 );

	if ( WriteBMP )
	{
		WriteBMPHeader( outStream, Slice->GetWidth(), Slice->GetHeight(), 32/*Slice->GetBPP()*8*/ );

		// convert to greyscale RGBA first
		if ( values == 0 )
		{
			// lazy allocation of 'values'
			values = new unsigned char[Slice->GetWidth()*Slice->GetHeight()*4];
		}

		for ( int ofs = 0 ; ofs < Slice->GetHeight()*Slice->GetWidth() ; ofs++ )
		{
			// 8-bit raw
			if ( Slice->GetBPP() == 1 )
			{
				unsigned char vv = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs];
				values[ofs*4+0] = vv;
				values[ofs*4+1] = vv;
				values[ofs*4+2] = vv;
				values[ofs*4+3] = 0;
			}

			// 16-bit raw
			if ( Slice->GetBPP() == 2 )
			{
				// downsampling from unsigned short
				unsigned char vv = static_cast<unsigned char>( reinterpret_cast<unsigned short*>( Slice->GetData() )[ofs] >> 8 );
				values[ofs*4+0] = vv;
				values[ofs*4+1] = vv;
				values[ofs*4+2] = vv;
				values[ofs*4+3] = 0;
			}

			// 24-bit RGB (quantized gradients or just RGB data)
			if ( Slice->GetBPP() == 3 )
			{
				// direct copying
				unsigned char _r = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*3+0];
				unsigned char _g = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*3+1];
				unsigned char _b = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*3+2];
				values[ofs*4+0] = _r;
				values[ofs*4+1] = _g;
				values[ofs*4+2] = _b;
				values[ofs*4+3] = 0;
			}

			// floating-point gradients
			if ( Slice->GetBPP() == 3 * 4 )
			{
				// convert values from [-1,1] to [0..255]
				VolumeVector<float, 3> *gr = &reinterpret_cast<VolumeVector<float, 3>*>( Slice->GetData() )[ofs];

				for ( int i = 0 ; i < 3 ; i++ ) { values[ofs*4 + i] = quantize8( ( *gr )[i] ); }

				values[ofs*4 + 3] = 0;
			}

			// 32-bit RGB (quantized gradients(+magnitude) or just RGB data)
			if ( Slice->GetBPP() == 4 )
			{
				// direct copying
				values[ofs*4+0] = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*4+0];
				values[ofs*4+1] = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*4+1];
				values[ofs*4+2] = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*4+2];
				values[ofs*4+3] = reinterpret_cast<unsigned char*>( Slice->GetData() )[ofs*4+3];
			}

			// floating-point gradients with magnitudes
			if ( Slice->GetBPP() == 4 * 4 )
			{
				// convert values from [-1,1] to [0..255]
				VolumeVector<float, 4> *gr = &reinterpret_cast<VolumeVector<float, 4>*>( Slice->GetData() )[ofs];

				for ( int i = 0 ; i < 3 ; i++ ) { values[ofs*4 + i] = quantize8( ( *gr )[i] ); }

				// convert fourth component directly to byte
				values[ofs*4 + 3] = static_cast<unsigned char>( ( *gr )[3] );
			}

			// double floating point gradients
		}

		outStream->WriteBytes( values, 4 * Slice->GetHeight()*Slice->GetWidth() );
	}
	else
	{
		outStream->WriteBytes( Slice->GetData(), Slice->GetBPP()*Slice->GetHeight()*Slice->GetWidth() );
	}

	delete outStream;
}

void VolumeSlice::FillWithZeroes()
{
	if ( TheData != NULL )
	{
		memset( TheData, 0, Width * Height * BPP );
	}
}

void VolumeSlice::SplitSlice( VolumeSlice** Slices, int numX, int numY )
{
	int SliceIdx;

	int i, j, x, y;

	int NewW = this->Width / numX;
	int NewH = this->Height / numY;

	VolumeSlice* DestSlice;

	for ( j = 0 ; j < numY ; j++ )
	{
		for ( y = 0 ; y < NewH ; y++ )
		{
			VOLUME_PTR SourceLinePtr = GetRowPtr( j * NewH + y );

			for ( i = 0 ; i < numX ; i++ )
			{
				SliceIdx = numX * j + i;
				DestSlice = Slices[SliceIdx];

				for ( x = 0 ; x < NewW ; x++ )
				{
					int Ofs = ( i * NewW + x ) * BPP;
					unsigned char* DirectLinePtr = reinterpret_cast<unsigned char*>( SourceLinePtr );

					VOLUME_PTR SourceSubLine = reinterpret_cast<VOLUME_PTR>( &DirectLinePtr[Ofs] );

					// copy source line to selected slice
					memcpy( DestSlice->GetRowPtr( j ), SourceSubLine, NewW * BPP );
				}
			}
		}
	}
}

VOLUME_PTR VolumeSlice::GetSafeRowPtr( int Y )
{
	if ( Y < 0 || Y >= Height )
	{
		if ( ZeroRow == 0 )
		{
			ZeroRow = new unsigned char[Width*BPP];
			// TODO : fill with zeros
			memset( ZeroRow, 0, Width * BPP );
		}

		return ZeroRow;
	}

	VOLUME_PTR_SCALAR Ofs  = Y * Width * BPP;
	return &( reinterpret_cast<unsigned char*>( TheData )[Ofs] );
}

void MemOutStream::WriteBytes( VOLUME_PTR data, size_t count )
{
	memcpy( reinterpret_cast<VOLUME_PTR>( &reinterpret_cast<unsigned char*>( m_Data )[m_Pos] ), data, count );
}

void MemInStream::ReadBytes( VOLUME_PTR data, size_t count )
{
	memcpy( data, reinterpret_cast<VOLUME_PTR>( &reinterpret_cast<unsigned char*>( m_Data )[m_Pos] ), count );
}


void LinearVolumeWriter::WriteSliceZ( int Z, VolumeSlice* Slice )
{
	size_t blockSize = Slice->GetWidth() * Slice->GetHeight() * Slice->GetBPP();

	size_t blockOffset = blockSize * static_cast<size_t>( Z );

	// TODO : cache position !
	output->SeekTo( blockOffset );

	output->WriteBytes( Slice->GetData(), blockSize );
}

VolumeWriter::~VolumeWriter()
{
	if ( DeleteOnExit )
	{
		if ( output != 0 )
		{
			delete output;
		}
	}
}


typedef VolumeVector<unsigned char, 3> Vec3Char;
typedef VolumeVector<unsigned char, 4> Vec4Char;
typedef VolumeVector<float, 3> Vec3Float;
typedef VolumeVector<float, 4> Vec4Float;

// rescaling
extern "C"
void VL_RescaleVolume( void* Slicer, void* Writer,
                       int NewW, int NewH, int NewD, int NewBPP, int StartZ, int FinishZ,
                       int* CurrentSlice )
{
	VolumeSlicer* inputVolume = reinterpret_cast<VolumeSlicer*>( Slicer );
	VolumeWriter* writer = reinterpret_cast<VolumeWriter*>( Writer );

	int OldW = inputVolume->GetWidth();
	int OldH = inputVolume->GetHeight();
	int OldD = inputVolume->GetHeight();
	int OldBPP = inputVolume->GetBPP();

#define __RESCALE_CALL(SrcType,DestType) \
RescaleVolumeBlock<SrcType ,DestType >(inputVolume,writer,OldW,OldH,OldD,NewW,NewH,NewD,StartZ,FinishZ,CurrentSlice)

	// TODO : replace uchar by Int2Type<OldBPP,NewBPP>::Type - this should be hardcoded, of course
	if ( OldBPP == 1 )
	{
		if ( NewBPP == 1 ) { __RESCALE_CALL( unsigned char, unsigned char ); }

		if ( NewBPP == 2 ) { __RESCALE_CALL( unsigned char, unsigned short ); }
	}

	if ( OldBPP == 2 )
	{
		if ( NewBPP == 1 ) { __RESCALE_CALL( unsigned short, unsigned char ); }

		if ( NewBPP == 2 ) { __RESCALE_CALL( unsigned short, unsigned short ); }
	}

	if ( OldBPP == 3 )
	{
		// 3-vector of chars is always converted to 3-vector of chars
		__RESCALE_CALL( Vec3Char, Vec3Char );
	}

	if ( OldBPP == 4 )
	{
		// 4-vector of chars or just long are converted to 4-vector of chars
		__RESCALE_CALL( Vec4Char, Vec4Char );
	}

	if ( OldBPP == 12 )
	{
		// 3-vector of floats is always converted to 3-vector of floats
		__RESCALE_CALL( Vec3Float, Vec3Float );
	}

	if ( OldBPP == 16 )
	{
		// 4-vector of floats is always converted to 4-vector of floats
		__RESCALE_CALL( Vec4Float, Vec4Float );
	}

#undef __RESCALE_CALL
}

#if defined(_WIN32)

#include <windows.h>

class WinInStream : public InStream
{
public:
	WinInStream( const char* fname )
	{
		_TheFile = CreateFileA( fname,
		                        GENERIC_READ,
		                        FILE_SHARE_READ,
		                        NULL,
		                        OPEN_EXISTING,
		                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
		                        NULL );

//			if (_TheFile == INVALID_HANDLE_VALUE) ...

		_Size =::GetFileSize( _TheFile, NULL );
	}

	virtual size_t GetSize() { return _Size; }

	virtual void SeekTo( size_t Offset )
	{
		// TODO : handle 64-bit/128-bit offsets
//			LONG highOfs = *reinterpret_cast<LONG*>(&Offset);
		SetFilePointer( _TheFile, static_cast<LONG>( Offset ), NULL/*&highOfs*/, FILE_BEGIN );
	}

	virtual void ReadBytes( VOLUME_PTR Buffer, size_t Size )
	{
		DWORD bytesRead;
		ReadFile( _TheFile, Buffer, static_cast<DWORD>( Size ), &bytesRead, NULL );
	}

private:
	HANDLE _TheFile;
	size_t _Size;
};


/*
   Shared output
*/
class WinOutStream : public OutStream
{
public:
	WinOutStream( const char* fname )
	{
		TheFile = CreateFile( fname,
		                      GENERIC_WRITE,
		                      FILE_SHARE_WRITE, // shared writing !
		                      NULL,
		                      OPEN_EXISTING /*CREATE_NEVER*/ /*ALWAYS*/,    // OPEN_ALWAYS - for append
		                      FILE_ATTRIBUTE_NORMAL,
		                      NULL );

//			if (TheFile == INVALID_HANDLE_VALUE) ...
	}

	virtual void SeekTo( size_t Offset )
	{
//			LONG highOfs = *reinterpret_cast<LONG*>(&Offset);
		SetFilePointer( TheFile, static_cast<LONG>( Offset ), NULL/*&highOfs*/, FILE_BEGIN );
	}

	virtual void WriteBytes( VOLUME_PTR data, size_t count )
	{
		DWORD written;
		WriteFile( TheFile, data, static_cast<LONG>( count ), &written, NULL );
		//
	}

	virtual ~WinOutStream()
	{
		CloseHandle( TheFile );
	}
private:
	HANDLE TheFile;
};

#include <windows.h>

#ifdef __GNUC__
#define GCC_PACK(n) __attribute__((packed,aligned(n)))
#endif

#ifdef _MSC_VER
#define GCC_PACK(n) __declspec(align(n))
#endif

#endif // OS_LINUX

OutStream* create_file_writer( const char* fname )
{
#if defined(_WIN32)
	return new WinOutStream( fname );
#else
	return NULL;
#endif
}

InStream*  create_file_reader( const char* fname )
{
#if defined(_WIN32)
	return new WinInStream( fname );
#else
	return NULL;
#endif
}

void create_empty_file( const char* fname )
{
	FILE* f;
#ifdef _MSC_VER
	fopen_s( &f, fname, "wb" );
#else
	f = fopen( fname, "wb" );
#endif
	fclose( f );
}

void WriteBMPHeader( OutStream* s, int W, int H, int BPP )
{
#if defined(_WIN32)

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
	struct GCC_PACK( 1 ) BMPFHeader_s
	{
	   WORD bfType;
	   DWORD bfSize;
	   WORD bfReserved1;
	   WORD bfReserved2;
	   DWORD bfOffBits;
	};

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
	struct GCC_PACK( 1 ) BMPIHeader_s
	{
	   DWORD biSize;
	   LONG biWidth;
	   LONG biHeight;
	   WORD biPlanes;
	   WORD biBitCount;
	   DWORD  biCompression;
	   DWORD  biSizeImage;
	   LONG biXPelsPerMeter;
	   LONG biYPelsPerMeter;
	   DWORD  biClrUsed;
	   DWORD  biClrImportant;
	};
	// end

	BMPFHeader_s bfh;
	BMPIHeader_s bih;

	int remainder = W % 4;

	// setup header parameters
	bfh.bfType = 0x4d42; // "BM"
	bfh.bfOffBits = sizeof( BMPFHeader_s ) + sizeof( BMPIHeader_s );
	bfh.bfSize = sizeof( BMPFHeader_s ) + sizeof( BMPIHeader_s ) + ( BPP / 8 ) * W * H + H * remainder;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;

	bih.biWidth = W;
	bih.biHeight = H;
	bih.biSize = sizeof( BMPIHeader_s );
	bih.biPlanes = 1;
	bih.biBitCount = static_cast<WORD>( BPP );
	bih.biCompression = 0;
	bih.biSizeImage = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	bih.biXPelsPerMeter = bih.biYPelsPerMeter = 0;

//	int ss1 = sizeof(BMPFHeader_s);
//	int ss = sizeof(bfh);
//	int sz = sizeof(bih);

	// write header
	s->WriteBytes( reinterpret_cast<void*>( &bfh ), sizeof( bfh ) );
	s->WriteBytes( reinterpret_cast<void*>( &bih ), sizeof( bih ) );

#endif // OS_LINUX
}

#include <stdlib.h>

// closest power of twon which is less than val
int __ClosestPowerOfTwo( double val )
{
	return  static_cast<int>( pow( 2.0, ::floor( log( val ) / log( static_cast<double>( 2 ) ) ) ) );
}



extern "C"
void VL_SuggestSize( int OldW, int OldH, int OldD, int OldBPP, int NewBPP, double MaxSize, int* OptW, int* OptH, int* OptD )
{
	double SourceSize = static_cast<double>( OldW ) * static_cast<double>( OldH ) * static_cast<double>( OldD ) * static_cast<double>( OldBPP );

	double FormatRatio = static_cast<double>( OldBPP ) / static_cast<double>( NewBPP );

	// extract cubic root
	double DownscaleDim = pow( ( SourceSize / MaxSize ) / FormatRatio, 1.0 / 3.0 );

	// optimal values for new dimensions
	*OptW = __ClosestPowerOfTwo( static_cast<double>( OldW ) / DownscaleDim );
	*OptH = __ClosestPowerOfTwo( static_cast<double>( OldH ) / DownscaleDim );
	*OptD = __ClosestPowerOfTwo( static_cast<double>( OldD ) / DownscaleDim );

	// no rescaling sometimes
	if ( *OptW > OldW ) { *OptW = ( int )OldW; }

	if ( *OptH > OldH ) { *OptH = ( int )OldH; }

	if ( *OptD > OldD ) { *OptD = ( int )OldD; }
}

void VL_CalcGrads( void* inData, void* outBuffer, int W, int H, int D, int BPP, int GW, int GH, int GD, int GBPP )
{
	void* input = VL_CreateMemoryInputStream( W * H * D * BPP, inData, 0 );
	void* inputSlicer = VL_CreateVolumeSlicer( W, H, D, BPP );
	VL_SetInputStreamForSlicer( inputSlicer, input );

	void* inMemGradientWriter = VL_CreateLinearVolumeWriter();

	// output stream is deleted automatically
	VL_SetOutputStreamForWriter( inMemGradientWriter, VL_CreateMemoryOutputStream( GW * GH * GD * GBPP, outBuffer, 0 ), 1 );

	int grad_progress;

	if ( BPP == 1 )
	{
		VL_CalculateGradients_Sobel8_UChar_Rescale( inputSlicer, W, H, D, BPP, GW, GH, GD, 4, inMemGradientWriter, 0, GD, &grad_progress );
	}

	if ( BPP == 2 )
	{
		VL_CalculateGradients_Sobel16_UChar_Rescale( inputSlicer, W, H, D, BPP, GW, GH, GD, 4, inMemGradientWriter, 0, GD, &grad_progress );
	}

	// cleanup
	VL_DeleteVolumeSlicer( inputSlicer );
	VL_DeleteVolumeWriter( inMemGradientWriter );
	VL_DeleteStream( input );
}

void VL_RescaleMemoryVolume( void* inBuffer, void* outBuffer, int OldW, int OldH, int OldD, int OldBPP, int NewW, int NewH, int NewD, int NewBPP )
{
	//
	void* writer = VL_CreateLinearVolumeWriter();
	VL_SetOutputStreamForWriter( writer, VL_CreateMemoryOutputStream( NewW * NewH * NewD * NewBPP, outBuffer, 0 ), 1 );

	void* input = VL_CreateMemoryInputStream( OldW * OldH * OldD * OldBPP, inBuffer, 0 );
	void* slicer = VL_CreateVolumeSlicer( OldW, OldH, OldD, OldBPP );

	VL_SetInputStreamForSlicer( slicer, input );

	int CurrentSlice;
	VL_RescaleVolume( slicer, writer, NewW, NewH, NewD, NewBPP, 0, NewD, &CurrentSlice );

	VL_DeleteVolumeSlicer( slicer );
	VL_DeleteVolumeWriter( writer );
	VL_DeleteStream( input );
}

// i/o adaptation
extern "C"
void* VL_CreateMemoryInputStream( size_t Size, VOLUME_PTR Data, int OwnsData )
{
	return reinterpret_cast<void*>( new MemInStream( Size, Data, OwnsData != 0 ) );
}

extern "C"
void* VL_CreateMemoryOutputStream( size_t Size, VOLUME_PTR Data, int OwnsData )
{
	return reinterpret_cast<void*>( new MemOutStream( Size, Data, OwnsData != 0 ) );
}

extern "C"
void  VL_DeleteStream( void* TheStream )
{
	delete reinterpret_cast<VolumeStream*>( TheStream );
}

// StdIO (based on win32api)
extern "C"
void* VL_CreateFileOutputStream( const char* fname )
{
	// win32-specific
	return create_file_writer( fname );
}

extern "C"
void* VL_CreateFileInputStream( const char* fname )
{
	// win32-specific
	return create_file_reader( fname );
}

// creation of empty file
extern "C"
void  VL_CreateEmptyFile( const char* fname )
{
	// win32-specific
	create_empty_file( fname );
}

// VolumeWriters
extern "C"
void* VL_CreateLinearVolumeWriter()
{
	return new LinearVolumeWriter();
}

extern "C"
void* VL_CreateCompositeWriter( int OwnsWriters )
{
	return reinterpret_cast<void*>( new CompositeVolumeWriter( OwnsWriters != 0 ) );
}

extern "C"
void  VL_AddWriterToComposite( void* composite, void* writer )
{
	reinterpret_cast<CompositeVolumeWriter*>( composite )->AddWriter( reinterpret_cast<VolumeWriter*>( writer ) );
}

// used for debugging on win32 : write volume as the series of .bmp images
extern "C"
void* VL_CreateSeriesVolumeWriter( const char* name_base )
{
	return new SeriesVolumeWriter( true, string( name_base ) );
}

// adaptation of external I/O routines
extern "C"
void* VL_CreateAdapterInputStream( void* DataSource, VL_ReadSeekCallback VL_ReadSeek_func, VL_ReadCallback VL_Read_func )
{
	return new InputStreamAdapter( DataSource, VL_ReadSeek_func, VL_Read_func );
}

extern "C"
void* VL_CreateAdapterOutputStream( void* DataSource, VL_WriteSeekCallback VL_WriteSeek_func, VL_WriteCallback VL_Write_func )
{
	return new OutputStreamAdapter( DataSource, VL_WriteSeek_func, VL_Write_func );
}

// attach output stream
extern "C"
void  VL_SetOutputStreamForWriter( void* writer, void* outStream, int DeleteStreamOnExit )
{
	reinterpret_cast<VolumeWriter*>( writer )->SetOutStream( reinterpret_cast<OutStream*>( outStream ), DeleteStreamOnExit != 0 );
}

// delete volume writer
extern "C"
void  VL_DeleteVolumeWriter( void* writer )
{
	delete reinterpret_cast<VolumeWriter*>( writer );
}

// create volume slicer (input entity encapsulating the volume layout)
extern "C"
void* VL_CreateVolumeSlicer( int W, int H, int D, int BPP )
{
	return new VolumeSlicer( W, H, D, BPP );
}

//
extern "C"
void  VL_DeleteVolumeSlicer( void* Slicer )
{
	delete [] reinterpret_cast<VolumeSlicer*>( Slicer );
}

extern "C"
void  VL_SetInputStreamForSlicer( void* Slicer, void* inStream )
{
	reinterpret_cast<VolumeSlicer*>( Slicer )->SetInStream( reinterpret_cast<InStream*>( inStream ) );
}


// filter preparation : simple template parameter specification for C++ functions

extern "C"
void VL_PrepareGaussianFilter_Double( int size, double sigma, double* filter )
{
	PrepareGaussianFilter<double>( size, sigma, filter );
}

extern "C"
void VL_PrepareGaussianFilter_Single( int size, float  sigma, float*  filter )
{
	PrepareGaussianFilter<float>( size, sigma, filter );
}

extern "C"
void VL_PrepareMedianFilter_Double( int size, double* filter )
{
	PrepareMedianFilter<double>( size, filter );
}

extern "C"
void VL_PrepareMedianFilter_Single( int size, float*  filter )
{
	PrepareMedianFilter<float>( size, filter );
}

// actual filtering
#define MAKE_PASS_FILTER_IMPL(FuncName, SampleType) \
extern "C"\
void FuncName(void* slicer, int W, int H, int D, int BPP, SampleType minVal, SampleType maxVal,void* writer, int StartZ, int FinishZ, int* CurrentSlice) \
{\
MakePassFilter<SampleType>(reinterpret_cast<VolumeSlicer*>(slicer),W,H,D,BPP,minVal,maxVal,reinterpret_cast<VolumeWriter*>(writer),StartZ,FinishZ,CurrentSlice);\
}

MAKE_PASS_FILTER_IMPL( VL_MakePassFilter8, unsigned char )
MAKE_PASS_FILTER_IMPL( VL_MakePassFilter16, unsigned short )
MAKE_PASS_FILTER_IMPL( VL_MakePassFilterSingle, float )
MAKE_PASS_FILTER_IMPL( VL_MakePassFilterDouble, double )

// scalar convolution
#define MAKE_SCALAR_CONV_FILTER_IMPL(FuncName, SampleType, FilterT) \
extern "C"\
void FuncName(void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, FilterT* filter, int* CurrentSlice) \
{\
MakeScalarVolumeConvolution<SampleType,FilterT>(reinterpret_cast<VolumeSlicer*>(slicer),W,H,D,BPP,reinterpret_cast<VolumeWriter*>(writer),StartZ,FinishZ,size,filter,CurrentSlice);\
}

MAKE_SCALAR_CONV_FILTER_IMPL( VL_MakeConvolution8_Single, unsigned char, float )
MAKE_SCALAR_CONV_FILTER_IMPL( VL_MakeConvolution16_Single, unsigned short, float )
MAKE_SCALAR_CONV_FILTER_IMPL( VL_MakeConvolutionSingle_Single, float, float )
MAKE_SCALAR_CONV_FILTER_IMPL( VL_MakeConvolutionDouble_Double, double, double )


// 3-d vector convolution (for gradients' filtering)
extern "C"
void VL_MakeVec3SingleConvolution_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float*  filter, int* CurrentSlice )
{
	MakeVectorVolumeConvolution<float, 3, float>( reinterpret_cast<VolumeSlicer*>( slicer ), W, H, D, BPP, reinterpret_cast<VolumeWriter*>( writer ), StartZ, FinishZ, size, filter, CurrentSlice );
}

extern "C"
void VL_MakeVec3DoubleConvolution_Double( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, double* filter, int* CurrentSlice )
{
	MakeVectorVolumeConvolution<double, 3, double>( reinterpret_cast<VolumeSlicer*>( slicer ), W, H, D, BPP, reinterpret_cast<VolumeWriter*>( writer ), StartZ, FinishZ, size, filter, CurrentSlice );
}

extern "C"
void VL_MakeVec3SingleVolumeConvolution_Quantize8( void* Slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float* filter, int* CurrentSlice )
{
	MakeVectorVolumeConvolution_Quantize<float, 3, unsigned char, float>( reinterpret_cast<VolumeSlicer*>( Slicer ), W, H, D, BPP, reinterpret_cast<VolumeWriter*>( writer ), StartZ, FinishZ, size, filter, CurrentSlice );
}

// 4-d vector convolution
extern "C"
void VL_MakeVec4SingleConvolution_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float*  filter, int* CurrentSlice )
{
	MakeVectorVolumeConvolution<float, 4, float>( reinterpret_cast<VolumeSlicer*>( slicer ), W, H, D, BPP, reinterpret_cast<VolumeWriter*>( writer ), StartZ, FinishZ, size, filter, CurrentSlice );
}

extern "C"
void VL_MakeVec4DoubleConvolution_Double( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, double* filter, int* CurrentSlice )
{
	MakeVectorVolumeConvolution<double, 4, double>( reinterpret_cast<VolumeSlicer*>( slicer ), W, H, D, BPP, reinterpret_cast<VolumeWriter*>( writer ), StartZ, FinishZ, size, filter, CurrentSlice );
}

extern "C"
void VL_MakeVec4SingleVolumeConvolution_Quantize8( void* Slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float* filter, int* CurrentSlice )
{
	MakeVectorVolumeConvolution_Quantize<float, 4, unsigned char, float>( reinterpret_cast<VolumeSlicer*>( Slicer ), W, H, D, BPP, reinterpret_cast<VolumeWriter*>( writer ), StartZ, FinishZ, size, filter, CurrentSlice );
}


#define CALC_GRADS_SOBEL_NOSCALE(FuncName,VoxelT,VecT) \
extern "C" \
void FuncName(void* Slicer, int W, int H, int D, int BPP, int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice)\
{\
   if (Dim == 3)\
   {\
      CalculateGradients_Sobel<VoxelT, VecT, 3>(reinterpret_cast<VolumeSlicer*>(Slicer),W,H,D,BPP,reinterpret_cast<VolumeWriter*>(output),StartZ,FinishZ,CurrentSlice);\
   }\
   else\
   {\
      CalculateGradients_Sobel<VoxelT, VecT, 4>(reinterpret_cast<VolumeSlicer*>(Slicer),W,H,D,BPP,reinterpret_cast<VolumeWriter*>(output),StartZ,FinishZ,CurrentSlice);\
   }\
}

CALC_GRADS_SOBEL_NOSCALE( VL_CalculateGradients_Sobel8_Single, unsigned char, float )
CALC_GRADS_SOBEL_NOSCALE( VL_CalculateGradients_Sobel16_Single, unsigned short, float )
CALC_GRADS_SOBEL_NOSCALE( VL_CalculateGradients_Sobel8_Double, unsigned char, double )
CALC_GRADS_SOBEL_NOSCALE( VL_CalculateGradients_Sobel16_Double, unsigned short, double )

// gradients calculation
#define CALC_GRADS_SOBEL_RESCALE(FuncName,VoxelT,VecT, VecOutT) \
extern "C" \
void FuncName(void* Slicer, int W, int H, int D, int BPP, int NewW, int NewH, int NewD, int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice)\
{\
   if (Dim == 3)\
   {\
      CalculateGradients_Sobel_Rescale_Quantize<VoxelT, VecT, VecOutT, 3>(reinterpret_cast<VolumeSlicer*>(Slicer),W,H,D,BPP,NewW,NewH,NewD,reinterpret_cast<VolumeWriter*>(output),StartZ,FinishZ,CurrentSlice);\
   }\
   else\
   {\
      CalculateGradients_Sobel_Rescale_Quantize<VoxelT, VecT, VecOutT, 4>(reinterpret_cast<VolumeSlicer*>(Slicer),W,H,D,BPP,NewW,NewH,NewD,reinterpret_cast<VolumeWriter*>(output),StartZ,FinishZ,CurrentSlice);\
   }\
}

CALC_GRADS_SOBEL_RESCALE( VL_CalculateGradients_Sobel8_Single_Rescale, unsigned char, float, float )
CALC_GRADS_SOBEL_RESCALE( VL_CalculateGradients_Sobel16_Single_Rescale, unsigned short, float, float )

CALC_GRADS_SOBEL_RESCALE( VL_CalculateGradients_Sobel8_UChar_Rescale, unsigned char, float, unsigned char )
CALC_GRADS_SOBEL_RESCALE( VL_CalculateGradients_Sobel16_UChar_Rescale, unsigned short, float, unsigned char )


extern "C" void VL_CalculateESLMap8( void* Slicer, int W, int H, int D, int BPP, int Factor, void* output, int StartZ, int FinishZ, int* CurrentSlice )
{
	CalculateESLMap<unsigned char>( reinterpret_cast<VolumeSlicer*>( Slicer ), W, H, D, BPP, Factor, reinterpret_cast<VolumeWriter*>( output ), StartZ, FinishZ, CurrentSlice );
}

extern "C" void VL_CalculateESLMap16( void* Slicer, int W, int H, int D, int BPP, int Factor, void* output, int StartZ, int FinishZ, int* CurrentSlice )
{
	CalculateESLMap<unsigned short>( reinterpret_cast<VolumeSlicer*>( Slicer ), W, H, D, BPP, Factor, reinterpret_cast<VolumeWriter*>( output ), StartZ, FinishZ, CurrentSlice );
}


extern "C"
int VL_GetLastErrorCode()
{
	return 0;
}

