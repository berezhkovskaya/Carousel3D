#ifndef __lfft__h__included__
#define __lfft__h__included__

/**
   \brief Utility class to hold the required temporary buffers for the 1D/2D/3D DFFT using Takuya Ooura's implementation

   This is the reference CPU implementation, for real-time 2D transforms one must use GPU-based FFT.

   The reason for this class is simple: it is an adapter for internal image (clBitmap) processing
*/
class LFFT
{
public:
	LFFT(): Dim( 0 ), OwnsData( false ) {}

	~LFFT() { Free(); }

	void Transform( bool Inverse, bool Complex );

	/// Make the forward or inverse Real DFT
	inline void TransformReal( bool Inverse ) { Transform( Inverse, false ); }

	/// Make the forward or inverse Complex DFT
	inline void TransformComplex( bool Inverse ) { Transform( Inverse, true ); }

#pragma region Utility constructors for fast initialization

	LFFT( int an1 )                    { Prepare1D( an1 ); }
	LFFT( int an1, int an2 )           { Prepare2D( an1, an2 ); }
	LFFT( int an1, int an2, int an3 )  { Prepare3D( an1, an2, an3 ); }

#pragma endregion

#pragma region Buffer management

	/// Prepare one-dimensional transform
	void Prepare1D( int an1 );

	/// Prepare two-dimensional transform
	void Prepare2D( int an1, int an2 );

	/// Prepare three-dimensional transform
	void Prepare3D( int an1, int an2, int an3 );

	/// Free all buffers
	void Free();

#pragma endregion

	/// Assign bitmap and store internal pointers
	void AssignPointersFromBitmap( void* Bmp, int W, int H, int D );

	void AllocArray( bool Complex );

	void FreeArray();

public:

	/// Do we have the data in aND arrays or only pointers
	bool OwnsData;

	/// Source buffer sizes
	int n1, n2, n3;

	/// Internal pointer arrays for 1D/2D/3D transforms
	float* a1D, **a2D, *** a3D;

private:

	/// Number of dimensions
	int Dim;

	/// Temporary buffer
	int* ip;

	/// Another temporary buffer
	float* t;

	/// And another buffer
	float* w;
};

#endif
