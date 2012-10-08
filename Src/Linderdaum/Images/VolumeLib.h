/**
   Useful volume processing routines

   (C) Viktor Latypov, 2008-2011

   Use freely, just mention the original author.

   Not the part of Linderdaum Engine, so the coding convention is different and there are no dependencies on the Engine here
*/

#ifndef __VolumeProcessingLib__h__included__
#define __VolumeProcessingLib__h__included__

/// Tricky templated functions frequently convert float to int
// in a 'dead' branch of code like if (sizeof(T) < 4) ...
/// Of course, this MUST be done by partial specialization to avoid this warning
#pragma warning(disable:4244)

// The same for this warning about constant boolean expressions.
// I didn't want to mess with partial specializations
#pragma warning(disable:4127)

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef int VOLUME_PTR_SCALAR; // for 32-bit environment
typedef void* VOLUME_PTR;

// I/O callbacks
typedef int ( *VL_ReadSeekCallback )( void* DataSource, size_t Offset );
typedef size_t ( *VL_ReadCallback )( void* DataSource, VOLUME_PTR block, size_t Offset );

typedef int ( *VL_WriteSeekCallback )( void* DataSource, size_t Offset );
typedef size_t ( *VL_WriteCallback )( void* DataSource, VOLUME_PTR block, size_t Offset );

class InStream;
class OutStream;

OutStream* create_file_writer( const char* fname );
InStream*  create_file_reader( const char* fname );

void create_empty_file( const char* fname );

void WriteBMPHeader( OutStream* s, int W, int H, int BPP );

/**
   Template method converting the input float or double type to scalar(short/char) OutType
**/
template <class OutType, class InType>
OutType quantizeTo( InType val )
{
	int maxValue = static_cast<OutType>( ( 2 << ( sizeof( OutType ) * 8 - 1 ) ) - 1 );

	if ( val > static_cast<InType>( +1.0 ) ) { val = static_cast<InType>( +1.0 ); }

	if ( val < static_cast<InType>( -1.0 ) ) { val = static_cast<InType>( -1.0 ); }

	int col = static_cast<int>( ( ( val + 1.0f ) / 2.0f ) * static_cast<InType>( maxValue ) );

	if ( col > maxValue ) { col = maxValue; }

	if ( col < 0 ) { col = 0; }

	return static_cast<OutType>( col );
}

/**
   Simple fixed-dimension vector with per-component addition/multiplication

   [should be ] used for precalculated gradient maps (LVector3 with 8-bit components) and other stuff

   Copy constructor and assignment operator is provided.
   This class does not provide any arithmetical operations.

   Template parameters specify the component type and the dimension of the vector
**/
template <class ComponentT, int Dim>
class VolumeVector
{
public:
	/**
	   Default empty constructor
	**/
	VolumeVector()
	{
	}

	/**
	   Copy constructor
	**/
	VolumeVector( const VolumeVector<ComponentT, Dim>& src )
	{
		AssignVector( src );
	}

	/**
	   Read/Write access to values[] array
	**/
	ComponentT& operator [] ( int idx )
	{
		return Values[idx];
	}

	/**
	   Read-only access to values[] array
	**/
	const ComponentT& operator [] ( int idx ) const
	{
		return Values[idx];
	}

	/**
	   Per-component comparison operator
	**/
	bool operator == ( const VolumeVector<ComponentT, Dim>& src )
	{
		for ( int i = 0 ; i < Dim ; i++ )
		{
			if ( Values[i] != src[i] ) { return false; }
		}

		return true;
	}

	/**
	   Assignment operator
	**/
	VolumeVector<ComponentT, Dim>& operator = ( const VolumeVector<ComponentT, Dim>& src )
	{
		AssignVector( src );
		return *this;
	}

	/**
	   Squared euclidean length of the vector
	**/
	ComponentT LengthSqr() const
	{
		ComponentT s = static_cast<ComponentT>( 0.0f );

		for ( int i = 0 ; i < Dim ; i++ )
		{
			s += ( Values[i] ) * ( Values[i] );
		}

		return s;
	}

	/**
	   Normalization function. Returns the length of the vector
	**/
	ComponentT Normalize()
	{
		double s = static_cast<ComponentT>( sqrt( static_cast<double>( LengthSqr() ) ) );

		if ( s == 0 ) { return s; }

		for ( int i = 0 ; i < Dim ; i++ )
		{
			Values[i] = static_cast<ComponentT>( static_cast<double>( Values[i] ) / static_cast<double>( s ) );
		}

		return static_cast<ComponentT>( s );
	}
private:
	/// actual value container - static array
	ComponentT Values[Dim];

	/**
	   Per-component copy routine
	**/
	void AssignVector( const VolumeVector<ComponentT, Dim>& src )
	{
		// TODO : direct memcpy ?
		for ( int i = 0 ; i < Dim ; i++ )
		{
			Values[i] = src[i];
		}
	}
};

/**
   Single slice of volume
   Can hold the actual data or just a pointer to the user supplied data
**/
class VolumeSlice
{
public:
	/**
	   Positions the cursor to the beginning of the slice
	**/
	void MoveToFirstVoxel()
	{
		CurrentVoxelOfs = 0; //Ptr = TheData;
	}

	/**
	   Template method to put the next voxel to the Slice
	   using CurrentVoxelOfs as the current voxel index

	   @param value
	      actual voxel value to be stored
	**/
	template <class VoxelT> void PutNextVoxel( const VoxelT& value )
	{
		// TODO : static template polymorphism should help to remove this 'if'
		//        right now we just trust the compiler - it should remove this redundant check
		if ( sizeof( VoxelT ) < 3 )
		{
			// objects smaller than 3 bytes are copied directly
			reinterpret_cast<VoxelT*>( TheData )[CurrentVoxelOfs] = value;
		}
		else
		{
			// objects bigger than 2 bytes might not have the copy constructor so we copy them byte-by-byte
			// well, this is strange, since the previous case _will_ generate a compile_time error...
			memcpy( &( reinterpret_cast<VoxelT*>( TheData )[CurrentVoxelOfs] ), &value, sizeof( VoxelT ) );
		}

		// proceed to next voxel
		CurrentVoxelOfs++;
	}

	/**
	   Default constructor for the VolumeSlice class
	   Creates (0,0)-sized slice with default 1 byte per pixel data, the data is managed by the caller
	**/
	VolumeSlice()
	{
		TheData = 0;
		ZeroRow = 0;
		SetSliceParams( 0, 0, 1, false, 0 );
	}

	/**
	   Constructor for the VolumeSlice class
	   @param W
	      X size of the slice
	   @param H
	      Y size of the slice
	   @param bpp
	      Bytes Per Voxel in this slice
	   @param ownsData
	      if true, then this object manages the TheData array and frees it in destructor
	      if false, then the 'data' pointer is managed by the caller
	   @param data
	      external data, supplied by the user
	      if NULL, then no data is assigned
	**/
	VolumeSlice( int W, int H, int bpp, bool ownsData, VOLUME_PTR data )
	{
		TheData = 0;
		ZeroRow = 0;
		SetSliceParams( W, H, bpp, ownsData, data );
	}

	/**
	   Sets slice parameters
	   This method is called from constructor or by the user if the reallocation of slice data is required
	   @param W
	      X size of the slice
	   @param H
	      Y size of the slice
	   @param bpp
	      Bytes Per Voxel in this slice
	   @param ownsData
	      if true, then this object manages the TheData array and frees it in destructor
	      if false, then the 'data' pointer is managed by the caller
	   @param data
	      external data, supplied by the user. it is stored in the TheData array and freed automatically if the 'ownsData' flag is set
	      if NULL, then no data is assigned
	**/
	void SetSliceParams( int W, int H, int bpp, bool ownsData, VOLUME_PTR data )
	{
		Width  = W;
		Height = H;
		BPP    = bpp;

		if ( ZeroRow != 0 )
		{
			// clear the proxy line
			delete [] reinterpret_cast<unsigned char*>( ZeroRow );
		}

		ZeroRow = 0;

		if ( TheData != NULL )
		{
			if ( IsOwner )
			{
				// clear old data
				delete[] reinterpret_cast<unsigned char*>( TheData );
			}
		}

		TheData = data;
		IsOwner = ownsData;

		if ( IsOwner )
		{
			TheData = ( VOLUME_PTR )( new unsigned char[W*H*bpp] );

			if ( data != 0 )
			{
				// copy source ?
//					memcpy(data, TheData, W*H*bpp);
			}
		}
	}

	/**
	   Clears the slice with zero values
	   No checks are performed - the W/H/BPP should be valid for this slice
	**/
	void FillWithZeroes();

	/**
	   Returns the pointer to subregion of the line
	**/
	VOLUME_PTR GetSubRowPtr( int Y, int X )
	{
		VOLUME_PTR_SCALAR Ofs = ( Y * Width + X ) * BPP;

		return reinterpret_cast<VOLUME_PTR>( &( reinterpret_cast<unsigned char*>( TheData ) )[Ofs] );
	}

	/**
	   Returns the pointer to specified line number
	   No boundary checks is done. This method is used in voxel-to-voxel filters
	   which do not require voxel's neighbourhood
	**/
	VOLUME_PTR GetRowPtr( int Y )
	{
		int Ofs = Y * Width * BPP;
		return reinterpret_cast<VOLUME_PTR>( &( reinterpret_cast<unsigned char*>( TheData ) )[Ofs] );
	}

	/**
	   Returns the pointer to the specified line number.
	   If the line number is outside of [0..H-1] range, then the 'proxy' zero line is returned
	   This method is used in filters which use the neighbourhood of the voxel
	**/
	VOLUME_PTR GetSafeRowPtr( int Y );

	/**
	   Split slice into [numX x numY] blocks
	   @param Slices
	      This array (VolumeSlice*[]) contains numX*numY slices to be filled
	   @param numX
	      Number of X-blocks
	   @param numY
	      Number of Y-blocks
	**/
	void SplitSlice( VolumeSlice** Slices, int numX, int numY );

	/**
	   Non-virtual destructor since there are no derived SliceClasses
	   Clears the data if it is required
	**/
	~VolumeSlice()
	{
		if ( IsOwner )
		{
			delete [] ( reinterpret_cast<unsigned char*>( TheData ) );
		}
	}

	/**
	   Returns current slice width
	**/
	inline int GetWidth() const { return Width; }

	/**
	   Returns current slice height
	**/
	inline int GetHeight() const { return Height; }

	/**
	   Returns current slice BytesPerPixel
	**/
	inline int GetBPP() const { return BPP; }

	/**
	   Gets current data pointer
	**/
	inline VOLUME_PTR GetData() const { return TheData; }

	/**
	   Simple 'TheData' pointer assignment
	   @param data
	      user-supplied block of data
	**/
	inline void SetData( VOLUME_PTR data ) { TheData = data; }
private:
	/// Is this slice the owner of the data or just the pointer holder
	bool IsOwner;

	/// Slice width
	int Width;
	/// Slice height
	int Height;
	/// Slice Bytes Per Pixel
	int BPP;

	/// Data pointer (user-supplied or in-place allocated)
	VOLUME_PTR TheData;

	/// 'Proxy' zero row
	VOLUME_PTR ZeroRow;
private:
	/// Index of current voxel being filled
	VOLUME_PTR_SCALAR CurrentVoxelOfs;
};

// size of the slice cache
#define MAX_SLICES_IN_ITERATION 10

/**
   Abstract interface for volume reading
   At the moment only linear (slice-byt-slice) volume layout is supported.
   Later the fundamental GetSlicePtr() routing can hide any possible layout from the end-user
**/
class VolumeSlicer
{
public:
	/**
	   Default iterator's constructor
	   Assigns volume parameters and allocates space for slice cache
	**/
	VolumeSlicer( int W, int H, int D, int bpp )
	{
		Width  = W;
		Height = H;
		Depth  = D;
		BPP    = bpp;

		ZeroSlice = 0;

		for ( int i = 0 ; i < MAX_SLICES_IN_ITERATION ; i++ )
		{
			slices[i].SetSliceParams( W, H, BPP, true, 0 );
			sliceCoords[i] = -1000;
		}

		currentSliceInCache = 0;
	}

	/**
	   Assigns input stream
	   If the volume is stored in multiple files then this method might not be used
	**/
	void SetInStream( InStream* is ) { input = is; }

	/**
	   Fundamental operation wich hides the global layout of the volume
	   providing user a simple interface to access each slice

	   This method also checks if Z is out of [0..D-1] range.
	   If Z is out of volume then the 'proxy' zero-filled slice is returned

	   Also it maintains the cache of accessed slices to avoid unneccessary extraction of data
	   If the Z value was used it is found in the sliceCoords[] array and SlicePtr is extracted
	   from slices[] array. The cache is a fixe array of size MAX_SLICES_IN_ITERATION
	   (currently 10, so no filter with z-size of the kernel more than 10 is possible)

	   Using the 'proxy' zero slice helps to avoid boundary checking in inner cycles
	**/
	VolumeSlice* GetZSlicePtr( int Z );

	/**
	   Get source volume bytes per pixel
	**/
	inline int GetBPP()    const { return BPP;    }
	/**
	   Get source volume width
	**/
	inline int GetWidth()  const { return Width;  }
	/**
	   Get source volume height
	**/
	inline int GetHeight() const { return Height; }
	/**
	   Get source volume depth
	**/
	inline int GetDepth()  const { return Depth;  }

	/**
	   Deletes slice cache and proxy zero slice
	**/
	virtual ~VolumeSlicer();

private:
	/// input stream, if needed
	InStream* input;
	/// parameters of the source array
	int Width, Depth, Height, BPP;

	/// empty slice used to prevent boundary checking in inner cycles
	VolumeSlice* ZeroSlice;

	// the stack of slices, do NOT request more than here !
	// TODO : use file mapping, not reading...
	int sliceCoords[MAX_SLICES_IN_ITERATION];
	VolumeSlice slices[MAX_SLICES_IN_ITERATION];

	/// index of the currently used slice in cache
	int currentSliceInCache;

	/**
	   Direct access to the data, unsafe and used internally
	   Z should be in [0..D-1] range
	   This function copies/read the data from actual source to slice cache
	**/
	VolumeSlice* GetSliceByCoord( int Z );
};


/**
   Used in filtering processes
   Implemented as a subvolume or as an array of pointers to actual data samples
   It also helps to avoid boundary checking

   We assume that the internal loop runs over X coordinate,
   so the Values[] array is arranged accordingly
**/
class VoxelNeighbourhoodIterator
{
	// currently depends on slice iterator
	// so there are serious performance issues for nontrivially stored volumes

public:
	/**
	   Constructor
	   Assigns heightbourhood dimensions and source volume iterator (slicer)
	   Calls AllocateValues() method
	**/
	VoxelNeighbourhoodIterator( VolumeSlicer* slicer, int xs, int ys, int zs )
	{
		Slicer = slicer;

		// sizes
		XSize = xs;
		YSize = ys;
		ZSize = zs;

		// half-sized
		XSizeD = XSize / 2;
		YSizeD = YSize / 2;
		ZSizeD = ZSize / 2;

		AllocateValues();
	}

	/**
	   Destructor
	   Cleans the temporary Values[] array
	**/
	virtual ~VoxelNeighbourhoodIterator();

	/**
	   Resets iteration process to the first voxel in range
	**/
	void MoveToFirstVoxel();
	/**
	   Moves to specified voxel and fetches the neighbourhood
	**/
	void MoveToVoxel( int i, int j, int k );

	/**
	**/
	void SetIterationRange( int minx, int miny, int minz, int maxx, int maxy, int maxz );

	/**
	   Utility functions helping to reduce the number of fetched voxels
	   MoveToX/Y/Z methods update the Values[] array in the most efficient way by swapping some fragments of the neighbourhood
	**/
	bool MoveToNextXVoxel();
	/**
	   See MoveToNextX
	**/
	bool MoveToNextYVoxel();
	/**
	   See MoveToNextX
	**/
	bool MoveToNextZVoxel();

	/**
	   Get volume width (X)
	**/
	inline int GetWidth() const  { return Width;  }
	/**
	   Get volume height (Y)
	**/
	inline int GetHeight() const { return Height; }
	/**
	   Get volume depth (Z)
	**/
	inline int GetDepth() const  { return Depth;  }
	/**
	   Get volume bytes per voxel
	**/
	inline int GetBPP() const    { return BPP; }

	/**
	   Get X Size of the neighbourhood
	**/
	inline int GetXSize() const  { return XSize; }
	/**
	   Get Y Size of the neighbourhood
	**/
	inline int GetYSize() const  { return YSize; }
	/**
	   Get Z Size of the neighbourhood
	**/
	inline int GetZSize() const  { return ZSize; }

	/**
	   Get current X coordinate of the iterator
	**/
	inline int GetCurX() const   { return CurX; }
	/**
	   Get current Y coordinate of the iterator
	**/
	inline int GetCurY() const   { return CurY; }
	/**
	   Get current Z coordinate of the iterator
	**/
	inline int GetCurZ() const   { return CurZ; }

	/**
	   debug dump of the values[] array
	**/
	template <class VoxelT> void DumpNeighbourhood() const
	{
		cout << "==========" << endl;

		for ( int ofs = 0 ; ofs < XSize * YSize * ZSize ; ofs++ )
		{
			if ( sizeof( VoxelT ) == 12 )
			{
				cout << "(" << ( reinterpret_cast<VolumeVector<float, 3>*>( Values ) )[ofs][0] << "," << ( reinterpret_cast<VolumeVector<float, 3>*>( Values ) )[ofs][1] << "," << ( reinterpret_cast<VolumeVector<float, 3>*>( Values ) )[ofs][2] << ")" << endl;

				cout << "(" << *reinterpret_cast<int*>( &( reinterpret_cast<VolumeVector<float, 3>*>( Values ) )[ofs][0] ) << "," << *reinterpret_cast<int*>( &( reinterpret_cast<VolumeVector<float, 3>*>( Values ) )[ofs][1] ) << "," << *reinterpret_cast<int*>( &( reinterpret_cast<VolumeVector<float, 3>*>( Values ) )[ofs][2] ) << ")" << endl;
			}
			else
			{
//	            cout << "v[" << ofs << "] = " << static_cast<int>((reinterpret_cast<VoxelT *>(Values))[ofs]) << endl;
			}
		}

		/*
		         int i,j,k;

		         for (k = -ZSizeD ; k < ZSizeD+1 ; k++)
		         {
		            for (j = -YSizeD ; j < YSizeD+1 ; j++)
		            {
		               // sample-by-sample copy
		               for (i = -XSizeD ; i < XSizeD+1 ; i++)
		               {
		                  VoxelT val = (reinterpret_cast<VoxelT*>(Values))[(((k + ZSizeD)*YSize+(j + YSizeD))*XSize + (i + XSizeD))];

		                  // int ?
		                  cout << "(" << i << "," << j << "," << k << ") = " << static_cast<int>(val) << endl;
		               }
		            }
		         }
		*/
	}

	/**
	   Get the specified voxel from neighbourhood
	   (i,j,k) triple specifies the voxel in local coordinates
	**/
	template <class VoxelT> VoxelT GetVoxel( int i, int j, int k ) const
	{
		int Ofs = ( ( k * XSize + j ) * YSize + i );
//			cout << static_cast<int>(reinterpret_cast<unsigned char *>(Values)[(k*XSize+j)*YSize+i]) << endl;
		return ( reinterpret_cast<VoxelT*>( Values ) )[Ofs];
	}

	/**
	   Convolves current neighbourhood with the kernel in 'coeffs'
	   here OutT is some scalar type so we can up-cast it to double to gain maximum accuracy
	**/
	template <class VoxelT, class FilterCoeffT> VoxelT MakeScalarConvolution( FilterCoeffT* coeffs ) const
	{
		FilterCoeffT sum = 0;

		for ( int i = 0 ; i < XSize * YSize * ZSize ; i++ )
		{
			sum += coeffs[i] * static_cast<FilterCoeffT>( ( reinterpret_cast<VoxelT*>( Values ) )[i] );
		}

		// we do NOT check overflow yet/
		// should be something like
		//   if (sum > TypeParameters<VoxelT>::MaximumValue) sum = TypeParameters<VoxelT>::MaximumValue

		return static_cast<VoxelT>( sum );
	}

	/**
	   Makes a per-component convolution for vector-valued volumes
	**/
	template <class ComponentT, class FilterCoeffT, int Dim> void MakeVectorConvolution( VolumeVector<ComponentT, Dim>& vec, FilterCoeffT* coeffs ) const
	{
		ComponentT* ValueArray = reinterpret_cast<ComponentT*>( Values );

		for ( int j = 0 ; j < Dim ; j++ )
		{
			vec[j] = static_cast<ComponentT>( 0.0f );

			for ( int i = 0 ; i < XSize * YSize * ZSize ; i++ )
			{
				// i-th vector, j-th component
//					cout << "val[" << i << "]" << val << endl;
				vec[j] += coeffs[i] * ValueArray[i*Dim+j];
			}
		}

		return;
	}

	/**
	   Is the iteration finished ?
	**/
	bool IsLastVoxel() const;
public:
	/// raw storage for the neighbourhood
	VOLUME_PTR Values;
	/// dimensions of the neighbourhood
	int XSize, YSize, ZSize;
	/// half-sizes
	int XSizeD, YSizeD, ZSizeD;
	/// we do not want to call Slicer's method each time we need it
	int Width, Depth, Height, BPP;

	/// current voxel indices
	int CurX, CurY, CurZ;
	/// minimum values of the iteration range
	int MinX, MinY, MinZ;
	/// maximum values of the iteration range
	int MaxX, MaxY, MaxZ;

	/// cached pointers to currently used slices
	VolumeSlice* *SelectedSlices;
	/// cached pointers to currently selected lines
	VOLUME_PTR**  SelectedLines;
private:
	/// Souce volume iterator
	VolumeSlicer* Slicer;

	/**
	   Allocates values[] array
	**/
	void AllocateValues();

	/**
	   Updates line/slice pointers
	**/
	void SetCurrentVoxel( int i, int j, int k );
	/**
	   Gets voxel values, using previously fetched ones
	**/
	void FetchNeighbourhood();
};

class VolumeHistogram
{
public:
	VolumeHistogram( int Len, int bytesPP, VOLUME_PTR Data, bool ownsData = false )
		: OwnsData( ownsData ), TheData( Data ), BytesPP( bytesPP ), DataLength( Len )
	{
		TheData = Data;

		if ( OwnsData )
		{
			if ( TheData == 0 )
			{
				TheData = new unsigned char[Len*BytesPP];
			}
		}
	}

	inline VOLUME_PTR GetData() const
	{
		return TheData;
	}

	virtual ~VolumeHistogram()
	{
		if ( OwnsData )
		{
			if ( TheData != 0 )
			{
				delete [] reinterpret_cast<unsigned char*>( TheData );
			}
		}
	}

	template<class SampleType>
	void FindMinMax( SampleType& minValue, SampleType& maxValue )
	{
		maxValue = static_cast<SampleType>( 0.0f );
		minValue = ( 2 << ( sizeof( SampleType ) * 8 - 1 ) ) - 1;

		SampleType val;

		for ( size_t i = 0 ; i < DataLength ; i++ )
		{
			val = reinterpret_cast<SampleType*>( TheData )[i];

			if ( maxValue <= val ) { maxValue = val; }

			if ( minValue >= val ) { minValue = val; }
		}
	}

	template<class SampleType>
	void NormalizeSamples()
	{
		double TypeMax = 10000000.0f;

		if ( sizeof( SampleType ) < 5 )
		{
			// short or byte or int32
			TypeMax = static_cast<double>( ( 2 << ( sizeof( SampleType ) * 8 - 1 ) ) - 1 );
		}

		SampleType maximumValue, minimumValue;
		FindMinMax<SampleType>( maximumValue, minimumValue );

		// 1. find maximum value
		for ( size_t i = 0 ; i < DataLength ; i++ )
		{
			if ( maximumValue < reinterpret_cast<SampleType*>( TheData )[i] )
			{
				maximumValue = reinterpret_cast<SampleType*>( TheData )[i];
			}
		}

		if ( maximumValue == static_cast<SampleType>( 0 ) )
		{
			maximumValue = static_cast<SampleType>( 1 );
		}

		// 2. divide everything by this value
		SampleType oldVal, newVal;

		for ( size_t i = 0 ; i < DataLength ; i++ )
		{
			oldVal = reinterpret_cast<SampleType*>( TheData )[i];
			newVal = static_cast<SampleType>( TypeMax * ( static_cast<double>( oldVal ) / static_cast<double>( maximumValue ) ) );
			reinterpret_cast<SampleType*>( TheData )[i] = newVal;
		}
	}

	template<class SampleType>
	void ConvertToLogScale()
	{
	}
protected:
	int BytesPP;
	VOLUME_PTR TheData;
	size_t DataLength;
	bool OwnsData;
};

class VolumeHistogram1D : public VolumeHistogram
{
public:
	VolumeHistogram1D( int Len, int bytesPP, VOLUME_PTR Data, bool ownsData = false )
		: VolumeHistogram( Len, bytesPP, Data, ownsData )
	{
	}

	// collect density values
	template<class SampleType, class VoxelT>
	void CollectSamples( VolumeSlicer* Slicer, int StartZ, int FinishZ, int* CurrentSlice )
	{
		int W = Slicer->GetWidth();
		int H = Slicer->GetHeight();

		VolumeSlice* slice;

		int X, Y, Z;

		for ( Z = StartZ ; Z < FinishZ ; Z++ )
		{
			slice = Slicer->GetZSlicePtr( Z );

			for ( Y = 0 ; Y < H ; Y++ )
			{
				VOLUME_PTR line_ptr = slice->GetRowPtr( Y );

				for ( X = 0 ; X < W ; X++ )
				{
					( reinterpret_cast<SampleType*>( TheData )[reinterpret_cast<VoxelT*>( line_ptr )[X]] )++;
				}
			}

			// update progress
			( *CurrentSlice )++;
		}
	}

	virtual ~VolumeHistogram1D()
	{
	}
};

class VolumeHistogram2D : public VolumeHistogram
{
public:
	VolumeHistogram2D( int W, int H, int bytesPP, VOLUME_PTR Data, bool ownsData = false )
		: VolumeHistogram( W* H, bytesPP, Data, ownsData )
	{
		Width = W;
		Height = H;
	}
	/*
	      // collect (gradient,density) pairs
	      template<class SampleType, class VoxelT>
	      void CollectSamples(VolumeSlicer *Slicer, int StartZ, int FinishZ, int *CurrentSlice)
	      {
	         (void)CurrentSlice;
	         (void)StartZ;
	         (void)FinishZ;
	         int W = Slicer->GetWidth();
	         int H = Slicer->GetHeight();
	      }
	*/
	template<class SampleType>
	void RenderRGBABitmap( unsigned char* BMP )
	{
		int X, Y;
		unsigned char* ptr = BMP;
		unsigned char* DataPtr = reinterpret_cast<unsigned char*>( TheData );

		for ( Y = 0 ; Y < Height ; Y++ )
		{
			for ( X = 0 ; X < Width ; X++ )
			{
				*ptr++ = *DataPtr;
				*ptr++ = *DataPtr;
				*ptr++ = *DataPtr;
				*ptr++ = 0;
				DataPtr++;
			}
		}
	}

	virtual ~VolumeHistogram2D()
	{
	}
private:
	int Width, Height;
};

/**
   Abstract volume writing interface
   Basic functionality is contained in WriteSliceZ() method
   The prevailing (and at the time the only) volume layout is the linear slice list indexed by z-axis values
**/
class VolumeWriter
{
public:
	/**
	   Default constructor
	**/
	VolumeWriter() { output = 0; DeleteOnExit = false; }

	inline void SetOutStream( OutStream* os, bool deleteOnExit = false ) { output = os; DeleteOnExit = deleteOnExit;}

	/**
	   Writes the z-axis slice to the volume
	**/
	virtual void WriteSliceZ( int Z, VolumeSlice* Slice ) = 0;

	/**
	   Virtual destructor which optionally (depending on the DeleteOnExit flag) deletes OutputStream (output)
	**/
	virtual ~VolumeWriter();
protected:
	OutStream* output;
	/// Flag indicating that the output stream should be deleted on exit
	bool DeleteOnExit;
};


/**
   Linear volume writer which stores the volume as a sequence of z-indexed slices
**/
class LinearVolumeWriter : public VolumeWriter
{
public:
	LinearVolumeWriter() {}

	/**
	   Actual slice writing method
	   Uses Slice->GetData() to acquire data and writes it to the OutputStream
	**/
	virtual void WriteSliceZ( int Z, VolumeSlice* Slice );

	virtual ~LinearVolumeWriter() {}
};

/**
   For debugging purposes the output of some algorithm can be directed to mutiple destinations
   CompositeVolumeWriter stores an array of VolumeWriter objects and each call to WriteSliceZ()
   automatically calls WriteSliceZ() method for each of the stored writers
**/
class CompositeVolumeWriter : public VolumeWriter
{
public:
	CompositeVolumeWriter( bool ownsWriters = false )
	{
		OwnsWriters = ownsWriters;
	}

	virtual ~CompositeVolumeWriter()
	{
		if ( OwnsWriters )
		{
			DeleteWriters();
		}

		ClearWriters();
	}

	/**
	   Redirects writing of the slice to each writer in the writers[] array
	**/
	virtual void WriteSliceZ( int Z, VolumeSlice* Slice )
	{
		for ( size_t i = 0 ; i != writers.size() ; i++ )
		{
			writers[i]->WriteSliceZ( Z, Slice );
		}
	}

	/**
	   Adds another volume writer to the list
	**/
	void AddWriter( VolumeWriter* w )
	{
		writers.push_back( w );
	}

	/**
	  Clear the writers[] array without deleting each writer
	  Used if the caller manages each writer himself
	**/
	void ClearWriters()
	{
		writers.clear();
	}

	/**
	   Deletes each writer in the writers[] array
	   Used when the writers[] array is managed by this class
	**/
	void DeleteWriters()
	{
		for ( size_t i = 0 ; i != writers.size() ; i++ )
		{
			delete writers[i];
		}
	}
private:
	/// writers[] array contains references to actual writers
	std::vector<VolumeWriter*> writers;
	/// boolean flag indicating wheter the elements of writers[] array should be deleted in destructor
	bool OwnsWriters;
};


class VolumeStream
{
public:
	virtual size_t GetSize() { return 0; }

	virtual void SeekTo( size_t Offset ) = 0;

	virtual ~VolumeStream()
	{
	}
};

class InStream : public VolumeStream
{
public:
	InStream()
	{
	}
	/*
	      // map read-only bytes
	      virtual VOLUME_PTR MapBytes_Read(size_t Offset, size_t Length) = 0;
	*/
	virtual void ReadBytes( VOLUME_PTR Buffer, size_t Size ) = 0;
	/*
	      virtual void UnmapBytes() = 0;
	*/
};

/*
   Hopefully, reentrant and thread-safe output stream abstraction
*/
class OutStream : public VolumeStream
{
public:
	OutStream()
	{
	}

	virtual void WriteBytes( VOLUME_PTR data, size_t count ) = 0;

	virtual ~OutStream()
	{
	}
};

// basic in-mem stream
class MemStream
{
public:
	MemStream( size_t size, VOLUME_PTR data_ptr, bool OwnsData )
		: m_Size( size ), m_Data( data_ptr ), m_OwnsData( OwnsData )
	{
		m_Pos = 0;

		if ( m_OwnsData )
		{
			if ( data_ptr == 0 )
			{
				// allocate it
				m_Data = new unsigned char[size];
			}
		}
	}

	virtual ~MemStream()
	{
		if ( m_OwnsData )
		{
			if ( m_Data != 0 )
			{
				delete [] reinterpret_cast<unsigned char*>( m_Data );
			}
		}
	}
protected:
	size_t m_Size;
	size_t m_Pos;
	VOLUME_PTR m_Data;
	bool m_OwnsData;
};

// direct reading from memory block
class MemInStream : public InStream, public MemStream
{
public:
	MemInStream( size_t size, VOLUME_PTR data_ptr, bool OwnsData ) : MemStream( size, data_ptr, OwnsData )
	{
	}

	virtual ~MemInStream()
	{
	}

	virtual void ReadBytes( VOLUME_PTR data, size_t count );

	virtual void SeekTo( size_t Offset ) { m_Pos = Offset; }
	virtual size_t GetSize() { return m_Size; }
};

// direct writing to memory block
class MemOutStream : public OutStream, public MemStream
{
public:
	MemOutStream( size_t size, VOLUME_PTR data_ptr, bool OwnsData ) : MemStream( size, data_ptr, OwnsData )
	{
	}

	virtual ~MemOutStream()
	{
	}

	virtual void WriteBytes( VOLUME_PTR data, size_t count );

	virtual void SeekTo( size_t Offset ) { m_Pos = Offset; }
	virtual size_t GetSize() { return m_Size; }
};

/**
   For debugging purposes slices of the output volume
   can be written as a series of files (one file for each slice)
   If the WriteBMP option is specified then the .BMP file heade is automatically
   added to each file. Later the user can view the files in any of the graphics editing software
   Each output file in the WriteBMP mode is automatically converted to 24 or 32 bits
   (see the WriteSliceZ method description)
**/
class SeriesVolumeWriter : public VolumeWriter
{
public:
	/**
	   Constructor
	   @param writeBMP
	      if true, then .BMP file header is automatically written to the beginning of each file
	   @param name_base
	      the beginning symbols of the name of each file
	      the file name format is <name_base><_><slice number(Z)>[.bmp or .raw]
	**/
	SeriesVolumeWriter( bool writeBMP, const string& name_base )
	{
		WriteBMP = writeBMP;
		NameBase = name_base;
		values = 0;
	}

	/**
	   Cleans up all used resources
	**/
	virtual ~SeriesVolumeWriter()
	{
		if ( values != 0 )
		{
			delete [] values;
		}
	}

	/**
	   Writes the slice performing necessary quantization/resampling.
	   If the input volume has 8 bits per voxel, then the output is a greyscale image.
	   If the input volume has 16 bits per voxel then each value is resampled to [0..255] (8bit) range and
	   turned into the 24-bit greyscale image
	   If the input voxels are the 3- or 4-vectors of 8-bit samples then they are written as 24 or 32 bit color images
	   If the input is a floating-point vector volume then each floating point value is quantized
	   from [-1,1] to [0..255] range and the output is 24/32 bit image
	**/
	virtual void WriteSliceZ( int Z, VolumeSlice* Slice );
private:
	/// flag indicating the need to write .bmp file header
	bool WriteBMP;
	/// file name base
	string NameBase;
	/// temporary array for slice data storage
	unsigned char* values;
};

/**
   Template method used in WriteSliceZ() procedure
   to resample floating point values from [-1,1] range to [0..255]

   Remember, that static_cast<T>((2 << (sizeof(T)*8))-1) is the maximum value for unsigned OutType
**/
inline unsigned char quantize8( float val )
{
	// clamp values
	if ( val > +1.0 ) { val = +1.0; }

	if ( val < -1.0 ) { val = -1.0; }

	// convert to [0..255] range
	// saturate once more
	int col = static_cast<int>( ( ( val + 1.0f ) / 2.0f ) * 255.0f );

	if ( col < 0 ) { col = 0; }

	if ( col > 255 ) { col = 255; }

	return static_cast<unsigned char>( col );
}


template<class OutType, class InType>
OutType resampleTo( InType val )
{
	float maxInVal  = static_cast<float>( ( 2 << ( sizeof( InType ) * 8 ) ) - 1 );
	float maxOutVal = static_cast<float>( ( 2 << ( sizeof( OutType ) * 8 ) ) - 1 );
	float inVal = static_cast<float>( val );

	return static_cast<OutType>( ( inVal / maxInVal ) * maxOutVal );
}

// information gathering

// 1. Find minimum/maximum value in the specified slice range
template <class VoxelT>
void FindMinMaxScalarValue( VolumeSlicer* Slicer, int StartZ, int FinishZ, VoxelT* minValue, VoxelT* maxValue, int* CurrentSlice )
{
	int X, Y, Z;
	*CurrentSlice = 0;

	for ( Z = StartZ ; Z < FinishZ ; Z++ )
	{
		VolumeSlice* Slice = Slicer->GetZSlicePtr( Z );

		VOLUME_PTR SlicePtr = Slice->GetData();

		int Ofs = 0;

		VoxelT* value;

		for ( Y = 0 ; Y < Slicer->GetHeight() ; Y++ )
		{
			for ( X = 0 ; X < Slicer->GetWidth() ; X++ )
			{
				//
				value = &reinterpret_cast<VoxelT*>( SlicePtr )[Ofs];

				if ( *value > *maxValue ) { *maxValue = *value; }

				if ( *value < *minValue ) { *minValue = *value; }

				Ofs++;
			}
		}

		( *CurrentSlice )++;
	}
}

// 2. Make a scalar convolution (maximum accuracy floating point arithmetics)
template <class VoxelT, class FilterCoeffsT>
void MakeScalarVolumeConvolution( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VolumeWriter* output, int StartZ, int FinishZ, int Size, FilterCoeffsT* filter, int* CurrentSlice )
{
	( void )D;
	( void )BPP;
//	VolumeSlicer Slicer(W,H,D,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, Size, Size, Size );

	VolumeSlice NewZSlice( iter.GetWidth(), iter.GetHeight(), sizeof( VoxelT ), true, 0 );

	*CurrentSlice = 0;

	for ( int Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		// reset voxel iteration
		iter.MoveToVoxel( 0, 0, Z );
		iter.SetIterationRange( 0, 0, Z, W - 1, H - 1, Z + 1 );

		// iterate neighbourhoods for each voxel in the slice
		while ( !iter.IsLastVoxel() )
		{
			// convolve the neighbourhood and store value into the output
			NewZSlice.PutNextVoxel<VoxelT>( iter.MakeScalarConvolution<VoxelT, FilterCoeffsT>( filter ) );

			iter.MoveToNextXVoxel();
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		( *CurrentSlice )++;
	}
}

// 2a. Make a scalar convolution (maximum accuracy floating point arithmetics) with in-place quantization
template <class VoxelT, class VoxelOutT, class FilterCoeffsT>
void MakeScalarVolumeConvolution_Quantize( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VolumeWriter* output, int StartZ, int FinishZ, int Size, FilterCoeffsT* filter, int* CurrentSlice )
{
//	VolumeSlicer Slicer(W,H,D,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, Size, Size, Size );

	VolumeSlice NewZSlice( iter.GetWidth(), iter.GetHeight(), sizeof( VoxelOutT ), true, 0 );

	*CurrentSlice = 0;

	VoxelOutT valueOut;

	for ( int Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		// reset voxel iteration
		iter.MoveToVoxel( 0, 0, Z );
		iter.SetIterationRange( 0, 0, Z, W - 1, H - 1, Z + 1 );

		// iterate neighbourhoods for each voxel in the slice
		while ( !iter.IsLastVoxel() )
		{
			// make quantization
			valueOut = quantizeTo<VoxelOutT, VoxelT>( iter.MakeScalarConvolution<VoxelT, FilterCoeffsT>( filter ) );
			// convolve the neighbourhood and store value into the output
			NewZSlice.PutNextVoxel<VoxelOutT>( valueOut );

			iter.MoveToNextXVoxel();
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		( *CurrentSlice )++;
	}
}



// 3. Make a vector convolution
template <class ComponentT, int Dim, class FilterCoeffsT>
void MakeVectorVolumeConvolution( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VolumeWriter* output, int StartZ, int FinishZ, int Size, FilterCoeffsT* filter, int* CurrentSlice )
{
	( void )BPP;
	( void )D;
//	VolumeSlicer Slicer(W,H,D,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, Size, Size, Size );

	VolumeSlice NewZSlice( iter.GetWidth(), iter.GetHeight(), sizeof( ComponentT )*Dim, true, 0 );

	*CurrentSlice = 0;

	VolumeVector<ComponentT, Dim> vec;

	for ( int Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		// reset voxel iteration
		iter.MoveToVoxel( 0, 0, Z );
		iter.SetIterationRange( 0, 0, Z, W - 1, H - 1, Z + 1 );

		// iterate neighbourhoods for each voxel in the slice
		while ( !iter.IsLastVoxel() )
		{
//			cout << "[i,j,k] = " << iter.GetCurX() << " " << iter.GetCurY() << " " << iter.GetCurZ() << endl;

			// convolve the neighbourhood and store value into the output
			iter.MakeVectorConvolution<ComponentT, FilterCoeffsT, Dim>( vec, filter );
			/*
			         if (vec[0] != 0)
			         {
			            cout << "conv_vox = " << vec[0] << "," << vec[1] << "," << vec[2] << endl;
			         }
			*/
			NewZSlice.PutNextVoxel<VolumeVector<ComponentT, Dim> >( vec );

			iter.MoveToNextXVoxel();
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		( *CurrentSlice )++;
		cout << "FilterSlice : " << *CurrentSlice << endl;
	}
}

// 3a. Make a vector convolution with in-place quantization
template <class ComponentT, int Dim, class ComponentOutT, class FilterCoeffsT>
void MakeVectorVolumeConvolution_Quantize( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VolumeWriter* output, int StartZ, int FinishZ, int Size, FilterCoeffsT* filter, int* CurrentSlice )
{
	( void )BPP;
	( void )D;
//	VolumeSlicer Slicer(W,H,D,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, Size, Size, Size );

	VolumeSlice NewZSlice( iter.GetWidth(), iter.GetHeight(), sizeof( ComponentOutT )*Dim, true, 0 );

	*CurrentSlice = 0;

	VolumeVector<ComponentOutT, Dim> outVec;
	VolumeVector<ComponentT, Dim> vec;
	int n;

	unsigned char* valPtr;
	float* inPtr;

	for ( int Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		// reset voxel iteration
		iter.MoveToVoxel( 0, 0, Z );
		iter.SetIterationRange( 0, 0, Z, W - 1, H - 1, Z + 1 );

		// iterate neighbourhoods for each voxel in the slice
		while ( !iter.IsLastVoxel() )
		{
			// convolve the neighbourhood and store value into the output
			iter.MakeVectorConvolution<ComponentT, FilterCoeffsT, Dim>( vec, filter );

			// make per-component quantization
			for ( n = 0 ; n < 3 ; n++ ) { outVec[n] = quantizeTo<ComponentOutT, ComponentT>( vec[n] ); }

			if ( Dim == 4 )
			{
				// do not quantize the last component - just convert it ; TODO : make Clamp<> function !
				// outVec[3] = ClampValue<ComponentOutT, ComponentT>(vec[3]);
				outVec[3] = static_cast<ComponentOutT>( vec[3] );

				if ( sizeof( ComponentOutT ) == 1 )
				{
					// treat output as byte : clamp float to [0..255];
					inPtr = reinterpret_cast<float*>( &( vec[3] ) );

					if ( *inPtr > 255.0 ) { *inPtr = 255.0; }

					if ( *inPtr < 0 )
					{
						cout << "strange !" << endl;
						*inPtr = 0.0;
					}

					valPtr = reinterpret_cast<unsigned char*>( &( outVec[3] ) );
					*valPtr = static_cast<unsigned char>( *inPtr );
				}
			}

			NewZSlice.PutNextVoxel<VolumeVector<ComponentOutT, Dim> >( outVec );

			iter.MoveToNextXVoxel();
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		( *CurrentSlice )++;
		cout << "FilterSlice : " << *CurrentSlice << endl;
	}
}

// 4. Calculate gradient map (Sobel's filter)
template <class VoxelT, class VecComponentT, int Dim>
void CalculateGradients_Sobel( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VolumeWriter* output, int StartZ, int FinishZ, int* CurrentSlice )
{
	( void )BPP;
	( void )D;

	VoxelNeighbourhoodIterator iter( Slicer, 3, 3, 3 );

	int SobelFilterWeights[][3][3][3] =
	{
		// x-dir
		{	{{ -1, -3, -1}, { -3, -6, -3}, { -1, -3, -1}},
			{{ 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0}},
			{{ 1,  3,  1}, { 3,  6,  3}, { 1,  3,  1}}
		},
		// y-dir
		{	{{ -1, -3, -1}, { 0,  0,  0}, { 1,  3,  1}},
			{{ -3, -6, -3}, { 0,  0,  0}, { 3,  6,  3}},
			{{ -1, -3, -1}, { 0,  0,  0}, { 1,  3,  1}}
		},
		// z-dir
		{	{{ -1,  0,  1}, { -3,  0,  3}, { -1,  0,  1}},
			{{ -3,  0,  3}, { -6,  0,  6}, { -3,  0,  3}},
			{{ -1,  0,  1}, { -3,  0,  3}, { -1,  0,  1}}
		}
	};

	int i, j, k, n;

//	int ss = sizeof(SobelFilterWeights);
	int* directSobelPtr[3];

	for ( n = 0 ; n < 3 ; n++ )
	{
		directSobelPtr[n] = reinterpret_cast<int*>( &SobelFilterWeights[n] );
	}

	int NewBPP = sizeof( VolumeVector<VecComponentT, Dim> );
	VolumeSlice NewZSlice( W, H, NewBPP, true, 0 );

	*CurrentSlice = 0;

	// 1/sqrt(3) is the constant which is multiplied at each gradient
	VecComponentT normalizer = static_cast<VecComponentT>( 1.0 / sqrt( static_cast<double>( 3.0f ) ) );

	int Z;

	for ( Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		iter.SetIterationRange( 0, 0, Z, W - 1, H - 1, Z + 1 );

		// reset voxel iteration
		iter.MoveToVoxel( 0, 0, Z );

		// iterate neighbourhoods for each voxel in the slice
		VecComponentT value;
		VolumeVector<VecComponentT, 3> gradient;
		VolumeVector<VecComponentT, Dim> *gradient_d = reinterpret_cast<VolumeVector<VecComponentT, Dim>*>( &gradient );

		VecComponentT magnitude;

		while ( !iter.IsLastVoxel() )
		{
//			cout << "[i,j,k] = " << iter.GetCurX() << " " << iter.GetCurY() << " " << iter.GetCurZ() << endl;

			gradient[0] = static_cast<VecComponentT>( 0.0f );
			gradient[1] = static_cast<VecComponentT>( 0.0f );
			gradient[2] = static_cast<VecComponentT>( 0.0f );

			for ( i = 0 ; i < 3 ; i++ )
			{
				for ( j = 0 ; j < 3 ; j++ )
				{
					for ( k = 0 ; k < 3 ; k++ )
					{
						value = static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( i, j, k ) );

						for ( n = 0 ; n < 3 ; n++ )
						{
							gradient[n] += value * static_cast<VecComponentT>( SobelFilterWeights[n][i][j][k] );
						}
					}
				}
			}

			/*
			         // FIXME : find appropriate order in SobelWeights array
			         // this version works a lot faster !
			         for (n = 0 ; n < 3 ; n++)
			         {
			            for (int i = 0 ; i < 27 ; i++)
			            {
			               gradient[n] += reinterpret_cast<VoxelT*>(iter.Values)[i*3+n] * static_cast<VecComponentT>(directSobelPtr[n][i]);
			            }
			         }
			*/

			/*
			         if (gradient[0] != 0 || gradient[1] != 0 || gradient[2] != 0)
			         {
			            cout << "voxel[" << iter.GetCurX() << "," << iter.GetCurY() << "," << iter.GetCurZ() << "]" << endl;
			            cout << "gr = " << gradient[0] << "," << gradient[1] << "," << gradient[2] << endl;
			         }
			*/

			// Normalize() returns the magnitude
			magnitude = gradient.Normalize();

			if ( Dim == 4 )
			{
				( *gradient_d )[3] = magnitude * normalizer;
			}

			// store the value into the output volume (3 or 4 components)
			NewZSlice.PutNextVoxel<VolumeVector<VecComponentT, Dim> >( *gradient_d );

			// continue iteration
			iter.MoveToNextXVoxel();
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		cout << "Slice : " << *CurrentSlice << endl;
		( *CurrentSlice )++;
	}
}


// 5. Calculate gradient map (Central differences)
template <class VoxelT, class VecComponentT, int Dim>
void CalculateGradients_CD( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VolumeWriter* output, int StartZ, int FinishZ, int* CurrentSlice )
{
//	VolumeSlicer Slicer(W,H,D,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, 3, 3, 3 );

	int NewBPP = sizeof( VolumeVector<VecComponentT, Dim> );
	VolumeSlice NewZSlice( W, H, NewBPP, true, 0 );

	*CurrentSlice = 0;

	for ( int Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		iter.SetIterationRange( 0, 0, Z, W - 1, H - 1, Z + 1 );

		// reset voxel iteration
		iter.MoveToVoxel( 0, 0, Z );

		// iterate neighbourhoods for each voxel in the slice
		VecComponentT value;
		VolumeVector<VecComponentT, 3> gradient;
		VolumeVector<VecComponentT, Dim> *gradient_d = reinterpret_cast<VolumeVector<VecComponentT, Dim>*>( &gradient );

		VecComponentT magnitude;

		while ( !iter.IsLastVoxel() )
		{
			gradient[0] = ( static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 2, 1, 1 ) ) - static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 0, 1, 1 ) ) ) / 2.0;
			gradient[1] = ( static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 2, 1 ) ) - static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 0, 1 ) ) ) / 2.0;
			gradient[2] = ( static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 1, 2 ) ) - static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 1, 0 ) ) ) / 2.0;

			magnitude = gradient.Normalize();

			if ( Dim == 4 )
			{
				( *gradient_d )[3] = magnitude;
			}

			// store the value into the output volume
			NewZSlice.PutNextVoxel<VolumeVector<VecComponentT, Dim> >( *gradient_d );

			// continue iteration
			iter.MoveToNextXVoxel();
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		cout << "Slice : " << *CurrentSlice << endl;
		( *CurrentSlice )++;
	}
}

// 6. Calculate gradient map (Central differences) with downscaling and downsampling
template <class VoxelT, class VecComponentT, class VecOutComponentT, int Dim>
void CalculateGradients_CD_Rescale_Quantize( VolumeSlicer* Slicer, int OldW, int OldH, int OldD, int BPP, int NewW, int NewH, int NewD, VolumeWriter* output, int StartZ, int FinishZ, int* CurrentSlice )
{
//	VolumeSlicer Slicer(OldW,OldH,OldD,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, 3, 3, 3 );

	int NewBPP = sizeof( VolumeVector<VecOutComponentT, Dim> );
	VolumeSlice NewZSlice( NewW, NewH, NewBPP, true, 0 );

	double XScale = static_cast<double>( OldW ) / static_cast<double>( NewW );
	double YScale = static_cast<double>( OldH ) / static_cast<double>( NewH );
	double ZScale = static_cast<double>( OldD ) / static_cast<double>( NewD );

	int i;
	int x, y;

	*CurrentSlice = 0;

	int NewZ, NewY, NewX;

	double OldZ, OldY, OldX;
	double MinOldZ, MinOldY, MinOldX;

	for ( NewZ = StartZ ; NewZ < FinishZ ; NewZ++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		OldZ = static_cast<double>( NewZ ) * ZScale;
		MinOldZ = ::floor( OldZ );

//		VolumeSlice *SourceZSlice = Slicer.GetZSlicePtr(static_cast<int>(MinOldZ));

		iter.SetIterationRange( 0, 0, static_cast<int>( MinOldZ ), OldW - 1, OldH - 1, static_cast<int>( MinOldZ ) + 1 );

		// iterate neighbourhoods for each voxel in the slice
		VolumeVector<VecComponentT, 3> gradient;
		VolumeVector<VecOutComponentT, Dim> gradient_d;

		VecComponentT magnitude;

		for ( NewY = 0 ; NewY < NewH ; NewY++ )
		{
			OldY = static_cast<double>( NewY ) * YScale;
			MinOldY = ::floor( OldY );

			for ( NewX = 0 ; NewX < NewW ; NewX++ )
			{
				OldX = static_cast<double>( NewX ) * XScale;
				MinOldX = ::floor( OldX );

				// reset voxel iteration
				iter.MoveToVoxel( static_cast<int>( MinOldX ), static_cast<int>( MinOldY ), static_cast<int>( MinOldZ ) );

				gradient[0] = ( static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 2, 1, 1 ) ) - static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 0, 1, 1 ) ) ) / 2.0;
				gradient[1] = ( static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 2, 1 ) ) - static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 0, 1 ) ) ) / 2.0;
				gradient[2] = ( static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 1, 2 ) ) - static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( 1, 1, 0 ) ) ) / 2.0;

				magnitude = gradient.Normalize();

				// make per-component quantization
				for ( i = 0 ; i < 3 ; i++ ) { gradient_d[i] = quantizeTo<VecOutComponentT, VecComponentT>( gradient[i] ); }

				if ( Dim == 4 )
				{
					gradient_d[3] = static_cast<VecOutComponentT>( magnitude );
				}

				// store the value into the output volume
				NewZSlice.PutNextVoxel<VolumeVector<VecOutComponentT, Dim> >( gradient_d );
			}
		}

		// write output slice
		output->WriteSliceZ( NewZ, &NewZSlice );
		// update progress
		cout << "Slice : " << *CurrentSlice << endl;
		( *CurrentSlice )++;
	}
}

// 7. Calculate gradient map (Sobel's filter) with quantization and downscaling
template <class VoxelT, class VecComponentT, class VecOutComponentT, int Dim>
void CalculateGradients_Sobel_Rescale_Quantize( VolumeSlicer* Slicer, int OldW, int OldH, int OldD, int BPP, int NewW, int NewH, int NewD, VolumeWriter* output, int StartZ, int FinishZ, int* CurrentSlice )
{
	( void )( BPP );
//	VolumeSlicer Slicer(OldW,OldH,OldD,BPP);
//	Slicer.SetInStream(input);
	VoxelNeighbourhoodIterator iter( Slicer, 3, 3, 3 );

	double XScale = static_cast<double>( OldW ) / static_cast<double>( NewW );
	double YScale = static_cast<double>( OldH ) / static_cast<double>( NewH );
	double ZScale = static_cast<double>( OldD ) / static_cast<double>( NewD );

	int SobelFilterWeights[][3][3][3] =
	{
		// x-dir
		{	{{ -1, -3, -1}, { -3, -6, -3}, { -1, -3, -1}},
			{{ 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0}},
			{{ 1,  3,  1}, { 3,  6,  3}, { 1,  3,  1}}
		},
		// y-dir
		{	{{ -1, -3, -1}, { 0,  0,  0}, { 1,  3,  1}},
			{{ -3, -6, -3}, { 0,  0,  0}, { 3,  6,  3}},
			{{ -1, -3, -1}, { 0,  0,  0}, { 1,  3,  1}}
		},
		// z-dir
		{	{{ -1,  0,  1}, { -3,  0,  3}, { -1,  0,  1}},
			{{ -3,  0,  3}, { -6,  0,  6}, { -3,  0,  3}},
			{{ -1,  0,  1}, { -3,  0,  3}, { -1,  0,  1}}
		}
	};

	int i, j, k, n;

//	int ss = sizeof(SobelFilterWeights);
	int* directSobelPtr[3];

	for ( n = 0 ; n < 3 ; n++ )
	{
		directSobelPtr[n] = reinterpret_cast<int*>( &SobelFilterWeights[n] );
	}

	int NewBPP = sizeof( VolumeVector<VecOutComponentT, Dim> );
	VolumeSlice NewZSlice( NewW, NewH, NewBPP, true, 0 );

	*CurrentSlice = 0;

	int NewZ, NewY, NewX;

	double OldZ, OldY, OldX;
	double MinOldZ, MinOldY, MinOldX;

	for ( NewZ = StartZ ; NewZ < FinishZ ; NewZ++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		OldZ = static_cast<double>( NewZ ) * ZScale;
		MinOldZ = ::floor( OldZ );

		iter.SetIterationRange( 0, 0, static_cast<int>( MinOldZ ), OldW - 1, OldH - 1, static_cast<int>( MinOldZ ) + 1 );

		// iterate neighbourhoods for each voxel in the slice
		VecComponentT value;
		VolumeVector<VecComponentT, 3> gradient;
		VolumeVector<VecOutComponentT, Dim> gradient_d;
		VecComponentT magnitude;

		for ( NewY = 0 ; NewY < NewH ; NewY++ )
		{
			OldY = static_cast<double>( NewY ) * YScale;
			MinOldY = ::floor( OldY );

			for ( NewX = 0 ; NewX < NewW ; NewX++ )
			{
				OldX = static_cast<double>( NewX ) * XScale;
				MinOldX = ::floor( OldX );

				gradient[0] = static_cast<VecComponentT>( 0.0f );
				gradient[1] = static_cast<VecComponentT>( 0.0f );
				gradient[2] = static_cast<VecComponentT>( 0.0f );

				iter.MoveToVoxel( static_cast<int>( MinOldX ), static_cast<int>( MinOldY ), static_cast<int>( MinOldZ ) );

				for ( i = 0 ; i < 3 ; i++ )
				{
					for ( j = 0 ; j < 3 ; j++ )
					{
						for ( k = 0 ; k < 3 ; k++ )
						{
							value = static_cast<VecComponentT>( iter.GetVoxel<VoxelT>( i, j, k ) );

							for ( n = 0 ; n < 3 ; n++ )
							{
								gradient[n] += value * static_cast<VecComponentT>( SobelFilterWeights[n][i][j][k] );
							}
						}
					}
				}

				/*
				            // FIXME : find appropriate order in SobelWeights array
				            // this version works a lot faster !
				            for (n = 0 ; n < 3 ; n++)
				            {
				               for (int i = 0 ; i < 27 ; i++)
				               {
				                  gradient[n] += reinterpret_cast<VoxelT*>(iter.Values)[i*3+n] * static_cast<VecComponentT>(directSobelPtr[n][i]);
				               }
				            }
				*/
				magnitude = gradient.Normalize();

				// quantize output
				for ( i = 0 ; i < 3 ; i++ ) { gradient_d[i] = quantizeTo<VecOutComponentT, VecComponentT>( gradient[i] ); }

				if ( Dim == 4 )
				{
					gradient_d[3] = static_cast<VecOutComponentT>( magnitude );
				}

				// store the value into the output volume
				NewZSlice.PutNextVoxel<VolumeVector<VecOutComponentT, Dim> >( gradient_d );
			}
		}

		// write output slice
		output->WriteSliceZ( NewZ, &NewZSlice );

		// update progress
		cout << "Slice : " << *CurrentSlice << endl;
		( *CurrentSlice )++;
	}
}

// 8. Threshold (low/high pass filter)
template <class VoxelT>
void MakePassFilter( VolumeSlicer* Slicer, int W, int H, int D, int BPP, VoxelT minVal, VoxelT maxVal, VolumeWriter* output, int StartZ, int FinishZ, int* CurrentSlice )
{
	( void )BPP;
	( void )D;
//	VolumeSlicer Slicer(W,H,D,BPP);
//	Slicer.SetInStream(input);

	VolumeSlice NewZSlice( W, H, sizeof( VoxelT ), true, 0 );

	*CurrentSlice = 0;

	VoxelT outValue;

	for ( int Z = StartZ ; Z < FinishZ ; Z++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		VolumeSlice* CurZSlice = Slicer->GetZSlicePtr( Z );

		// iterate neighbourhoods for each voxel in the slice
		for ( int Y = 0 ; Y < H ; Y++ )
		{
			VOLUME_PTR line_ptr = CurZSlice->GetSafeRowPtr( Y );

			for ( int X = 0 ; X < W ; X++ )
			{
				outValue = reinterpret_cast<VoxelT*>( line_ptr )[X];

				if ( outValue < minVal ) { outValue = static_cast<VoxelT>( 0 ); }

				if ( outValue > maxVal ) { outValue = static_cast<VoxelT>( 0 ); }

				// convolve the neighbourhood and store value into the output
				NewZSlice.PutNextVoxel<VoxelT>( outValue );
			}
		}

		// write output slice
		output->WriteSliceZ( Z, &NewZSlice );

		// update progress
		cout << "Slice : " << *CurrentSlice << endl;
		( *CurrentSlice )++;
	}
}

#include <math.h>

// GRAD_FILTER_SIZE = 5
// SIGMA2 = SigmaSqr = 5.0

template <class FilterCoeffT>
void PrepareMedianFilter( int Size, FilterCoeffT* filter )
{
	int N = Size * Size * Size;

	// initialize kernel
	for ( int i = 0 ; i < N ; i++ )
	{
		filter[i] = static_cast<FilterCoeffT>( 1.0 ) / static_cast<FilterCoeffT>( N );
	}
}

// SigmaSquared and Size are the parameters
template <class FilterCoeffT>
void PrepareGaussianFilter( int Size, FilterCoeffT SigmaSqr, FilterCoeffT* filter )
{
	int Size2 = Size / 2;

	int i, j, k, ofs;
	FilterCoeffT val;
	FilterCoeffT sum = 0.0;

	ofs = 0;

	// 1. initialize kernel
	for ( k = -Size2 ; k <= Size2 ; k++ )
	{
		for ( j = -Size2 ; j <= Size2 ; j++ )
		{
			for ( i = -Size2 ; i <= Size2 ; i++ )
			{
				val = exp( -( static_cast<FilterCoeffT>( i * i + j * j + k * k ) ) / SigmaSqr );
				sum += val;

				filter[ofs++] = val;
			}
		}
	}

	// 2. normalize the kernel
	int N = Size * Size * Size;

	for ( ofs = 0 ; ofs < N; ofs++ ) { filter[ofs++] /= sum; }
}

/**
   Calculates EmptySpaceLeapingMap
   Each voxel of the map is a triple : (minValue/maxValue/radius of the neighbourhood - or zero)

   In a rendering process this map is used to determine the number of voxels to skip
**/
template <class VoxelT>
void CalculateESLMap( VolumeSlicer* Slicer, int W, int H, int D, int BPP, int Factor, VolumeWriter* output, int StartZ, int FinishZ, int* CurrentSlice )
{
	( void )( BPP );
	( void )D;

	// fool-protection
	if ( Factor < 1 ) { return; }

	int NewW = W / Factor;
	int NewH = H / Factor;
//	int NewD = D/Factor;

	VoxelNeighbourhoodIterator iter( Slicer, Factor + 1, Factor + 1, Factor + 1 );

	int NewBPP = 3/* 3 * BPP */; // quantize to 8 bit
	VolumeSlice NewZSlice( NewW, NewH, NewBPP, true, 0 );

	*CurrentSlice = 0;

	int NewX, NewY, NewZ;

	for ( NewZ = StartZ ; NewZ < FinishZ ; NewZ++ )
	{
		// reset slice writing
		NewZSlice.MoveToFirstVoxel();

		int Z = NewZ * Factor;

//		VolumeSlice *SourceZSlice = Slicer.GetZSlicePtr(static_cast<int>(MinOldZ));

		iter.SetIterationRange( 0, 0, static_cast<int>( Z ), W - 1, H - 1, static_cast<int>( Z ) + 1 );

		int i, j, k;
		int X, Y;
		VoxelT value, minValue, maxValue;
		VolumeVector<unsigned char, 3> outValue;
		int TheShift;

		for ( NewY = 0 ; NewY < NewH ; NewY++ )
		{
			Y = NewY * Factor;

			X = 0;

			for ( NewX = 0 ; NewX < NewW ; NewX++ )
			{
				// reset voxel iteration
				iter.MoveToVoxel( X + Factor / 2, Y + Factor / 2, Z + Factor / 2 );

//				minValue = ((2 << (sizeof(VoxelT))-1)-1); // hack ? It was a working line, with strange -1

				TheShift = sizeof( VoxelT ) - 1;
				minValue = ( 2 << TheShift ) - 1; // hack ?

				maxValue = 0;

				for ( i = 0 ; i < Factor ; i++ )
				{
					for ( j = 0 ; j < Factor ; j++ )
					{
						for ( k = 0 ; k < Factor ; k++ )
						{
							value = iter.GetVoxel<VoxelT>( i, j, k );

							if ( value > maxValue ) { maxValue = value; }

							if ( value < minValue ) { minValue = value; }
						}
					}
				}

//				NewZSlice.PutNextVoxel<VoxelT>(iter.GetVoxel<VoxelT>( Factor/2,  Factor/2,  Factor/2));

//				cout << "MinValue : " << static_cast<int>(minValue) << endl;
//				cout << "MaxValue : " << static_cast<int>(maxValue) << endl;

				// B
				outValue[0] = 0;

				if ( sizeof( VoxelT ) == 1 )
				{
					// G
					outValue[1] = minValue;
					// R
					outValue[2] = maxValue;
				}
				else
				{
					outValue[1] = resampleTo<unsigned char, VoxelT>( minValue );
					outValue[2] = resampleTo<unsigned char, VoxelT>( maxValue );
				}

				// store the value into the output volume
				NewZSlice.PutNextVoxel<VolumeVector<unsigned char, 3> >( outValue );


				X += Factor;
			}
		}

		// write output slice
		output->WriteSliceZ( NewZ, &NewZSlice );
		// update progress
		cout << "Slice : " << *CurrentSlice << endl;
		( *CurrentSlice )++;
	}
}

/*
// takes input volume and processes it using voxel iterator
VolumeFilter

VolumeGradientFilter

VolumeThresholdFilter

VolumeSobelEdgeFilter
*/


class InputStreamAdapter : public InStream
{
public:
	InputStreamAdapter( void* dataSource, VL_ReadSeekCallback read_seek_func, VL_ReadCallback read_func )
	{
		DataSource = dataSource;
		SeekFunc = read_seek_func;
		ReadFunc = read_func;
	}

	virtual void SeekTo( size_t Offset )
	{
		SeekFunc( DataSource, Offset );
	}

	virtual void ReadBytes( VOLUME_PTR Buffer, size_t Size )
	{
		ReadFunc( DataSource, Buffer, Size );
	}

	virtual ~InputStreamAdapter()
	{
	}
private:
	void*                DataSource;

	VL_ReadSeekCallback  SeekFunc;
	VL_ReadCallback      ReadFunc;
};

class OutputStreamAdapter : public OutStream
{
public:
	OutputStreamAdapter( void* dataSource, VL_WriteSeekCallback write_seek_func, VL_WriteCallback write_func )
	{
		DataSource = dataSource;
		SeekFunc   = write_seek_func;
		WriteFunc  = write_func;
	}

	virtual void SeekTo( size_t Offset )
	{
		SeekFunc( DataSource, Offset );
	}

	virtual void WriteBytes( VOLUME_PTR Buffer, size_t Size )
	{
		WriteFunc( DataSource, Buffer, Size );
	}

	virtual ~OutputStreamAdapter()
	{
	}
private:
	void*                DataSource;

	VL_WriteSeekCallback SeekFunc;
	VL_WriteCallback     WriteFunc;
};


#ifndef DLLIMPORT
#define DLLIMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

	DLLIMPORT void VL_RescaleVolume( void* Slicer, void* Writer, int NewW, int NewH, int NewD, int NewBPP, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_SuggestSize( int OldW, int OldH, int OldD, int OldBPP, int NewBPP, double MaxSize, int* OptW, int* OptH, int* OptD );
	DLLIMPORT void VL_CalcGrads( void* inData, void* outBuffer, int W, int H, int D, int BPP, int GW, int GH, int GD, int GBPP );
	DLLIMPORT void VL_RescaleMemoryVolume( void* inBuffer, void* outBuffer, int OldW, int OldH, int OldD, int OldBPP, int NewW, int NewH, int NewD, int NewBPP );
	DLLIMPORT void* VL_CreateMemoryInputStream( size_t Size, VOLUME_PTR Data, int OwnsData );
	DLLIMPORT void* VL_CreateMemoryOutputStream( size_t Size, VOLUME_PTR Data, int OwnsData );
	DLLIMPORT void VL_DeleteStream( void* TheStream );
	DLLIMPORT void* VL_CreateFileOutputStream( const char* fname );
	DLLIMPORT void* VL_CreateFileInputStream( const char* fname );
	DLLIMPORT void VL_CreateEmptyFile( const char* fname );
	DLLIMPORT void* VL_CreateLinearVolumeWriter();
	DLLIMPORT void* VL_CreateCompositeWriter( int OwnsWriters );
	DLLIMPORT void VL_AddWriterToComposite( void* composite, void* writer );
	DLLIMPORT void* VL_CreateSeriesVolumeWriter( const char* name_base );
	DLLIMPORT void* VL_CreateAdapterInputStream( void* DataSource, VL_ReadSeekCallback VL_ReadSeek_func, VL_ReadCallback VL_Read_func );
	DLLIMPORT void* VL_CreateAdapterOutputStream( void* DataSource, VL_WriteSeekCallback VL_WriteSeek_func, VL_WriteCallback VL_Write_func );
	DLLIMPORT void VL_SetOutputStreamForWriter( void* writer, void* outStream, int DeleteStreamOnExit );
	DLLIMPORT void VL_DeleteVolumeWriter( void* writer );
	DLLIMPORT void* VL_CreateVolumeSlicer( int W, int H, int D, int BPP );
	DLLIMPORT void VL_DeleteVolumeSlicer( void* Slicer );
	DLLIMPORT void VL_SetInputStreamForSlicer( void* Slicer, void* inStream );
	DLLIMPORT void VL_PrepareGaussianFilter_Double( int size, double sigma, double* filter );
	DLLIMPORT void VL_PrepareGaussianFilter_Single( int size, float  sigma, float* filter );
	DLLIMPORT void VL_PrepareMedianFilter_Double( int size, double* filter );
	DLLIMPORT void VL_PrepareMedianFilter_Single( int size, float* filter );
	DLLIMPORT void VL_CalculateGradients_Sobel8_Single( void* Slicer, int W, int H, int D, int BPP,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel16_Single( void* Slicer, int W, int H, int D, int BPP,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel8_Double( void* Slicer, int W, int H, int D, int BPP,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel16_Double( void* Slicer, int W, int H, int D, int BPP,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel8_Single_Rescale( void* Slicer, int W, int H, int D, int BPP, int NewW, int NewH, int NewD,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel16_Single_Rescale( void* Slicer, int W, int H, int D, int BPP, int NewW, int NewH, int NewD,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel8_UChar_Rescale( void* Slicer, int W, int H, int D, int BPP, int NewW, int NewH, int NewD,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateGradients_Sobel16_UChar_Rescale( void* Slicer, int W, int H, int D, int BPP, int NewW, int NewH, int NewD,  int Dim, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_MakePassFilter8( void* slicer, int W, int H, int D, int BPP, unsigned char minVal, unsigned char maxVal, void* writer, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_MakePassFilter16( void* slicer, int W, int H, int D, int BPP, unsigned short minVal, unsigned short maxVal, void* writer, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_MakePassFilterSingle( void* slicer, int W, int H, int D, int BPP, float minVal, float maxVal, void* writer, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_MakePassFilterDouble( void* slicer, int W, int H, int D, int BPP, double minVal, double maxVal, void* writer, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_MakeConvolution8_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float*  filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeConvolution16_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float* filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeConvolutionSingle_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float*  filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeConvolutionDouble_Double( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, double* filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeVec3SingleConvolution_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float*  filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeVec3DoubleConvolution_Double( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, double* filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeVec3SingleVolumeConvolution_Quantize8( void* Slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float* filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeVec4SingleConvolution_Single( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float*  filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeVec4DoubleConvolution_Double( void* slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, double* filter, int* CurrentSlice );
	DLLIMPORT void VL_MakeVec4SingleVolumeConvolution_Quantize8( void* Slicer, int W, int H, int D, int BPP, void* writer, int StartZ, int FinishZ, int size, float* filter, int* CurrentSlice );
	DLLIMPORT void VL_CalculateESLMap8( void* Slicer, int W, int H, int D, int BPP, int Factor, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT void VL_CalculateESLMap16( void* Slicer, int W, int H, int D, int BPP, int Factor, void* output, int StartZ, int FinishZ, int* CurrentSlice );
	DLLIMPORT int VL_GetLastErrorCode();

#ifdef __cplusplus
}
#endif

#endif
