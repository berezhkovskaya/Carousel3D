#include "LFFT.h"

#include <math.h>

/**
Copyright
    Copyright(C) 1997,2001 Takuya OOURA (email: ooura@kurims.kyoto-u.ac.jp, web: http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html).
    You may use, copy, modify this code for any purpose and
    without fee. You may distribute this ORIGINAL package.

Modifications (LV):

   - removed explicit memory management for temporary archives ('t' can be allocated outside for 2d/3d transforms)
*/

/// Low-level functions
#ifdef __cplusplus
//extern "C"
//{
#endif

/// Size of 't' buffer for 2D transform
int gettsize2( int n1, int n2 );

/// Size of 't' buffer for 3D transform
int gettsize3( int n1, int n2, int n3 );

/// Complex DFT (isgn = 1 for forward and -1 for inverse)
void cdft( int n, int isgn, float* a, int* ip, float* w );

/// Real DFT (isgn = 1 for forward and -1 for inverse)
void rdft( int n, int isgn, float* a, int* ip, float* w );

/// Complex 2D DFT
void cdft2d( int n1, int n2, int isgn, float** a, float* t, int* ip, float* w );

/// Real 2D DFT
void rdft2d( int n1, int n2, int isgn, float** a, float* t, int* ip, float* w );

/// Value rearrangement for 2D transform
void rdft2dsort( int n1, int n2, int isgn, float** a );

/// Complex 3D DFT
void cdft3d( int n1, int n2, int n3, int isgn, float** *a, float* t, int* ip, float* w );

/// Read 3D DFT
void rdft3d( int n1, int n2, int n3, int isgn, float** *a, float* t, int* ip, float* w );

/// Value rearrangement for 3D transform
void rdft3dsort( int n1, int n2, int n3, int isgn, float** *a );

#ifdef __cplusplus
// }
#endif


template <class T> T MAX( T a, T b ) { return ( a > b ) ? a : b; }
template <class T> T MAX3( T a, T b, T c ) { return MAX( a, MAX( b, c ) ); }

void LFFT::Prepare1D( int an1 )
{
	Dim = 1;

	n1 = an1;

	ip = new int[2 + ( int ) sqrt( n1 + 0.5 )];
	w  = new float[3 * n1 / 2];

	ip[0] = 0;
}

void LFFT::AssignPointersFromBitmap( void* Bmp, int W, int H, int D )
{
	( void )D;
//	float *a1D, **a2D, ***a3D;

	if ( Dim == 2 )
	{
		a2D = new float*[sizeof( float* ) * H];

		for ( int y = 0 ; y < H ; y++ )
		{
			a2D[y] = &( ( float* )Bmp )[y * W];
		}
	}
}

void LFFT::Prepare2D( int an1, int an2 )
{
	Dim = 2;

	n1 = an1;
	n2 = an2;

	t = new float[ gettsize2( n1, n2 ) ];

///   a = alloc_2d_float(n1, n2);

	int n = MAX( n1, n2 / 2 );
	ip = new int[2 + ( int ) sqrt( n + 0.5 )];

	n = MAX( n1, n2 ) * 3 / 2;
	w = new float[n];

	/// Trigger the ip table recalculation in the first call of dft2d()
	ip[0] = 0;
}

void LFFT::Prepare3D( int an1, int an2, int an3 )
{
	Dim = 3;

	n1 = an1;
	n2 = an2;
	n3 = an3;

	t = new float[gettsize3( n1, n2, n3 )];

///   a = alloc_3d_float(n1, n2, n3);

	int n = MAX3( n1, n2, n3 / 2 );
	ip = new int[2 + ( int ) sqrt( n + 0.5 )];

	n = MAX3( n1, n2, n3 ) * 3 / 2;
	w = new float[n];

	/// Trigger the ip table recalculation in the first call of dft2d()
	ip[0] = 0;
}

void LFFT::FreeArray()
{
	// free arrays
	if ( Dim == 1 )
	{
		delete[] a1D;
		a1D = 0;
	}

	if ( Dim == 2 )
	{
		delete[] a2D[0];
		delete[] a2D;
		a2D = 0;
	}

	if ( Dim == 3 )
	{
		delete[] a3D[0][0];
		delete[] a3D[0];
		delete[] a3D;
		a3D = 0;
	}

	OwnsData = false;
}

void LFFT::AllocArray( bool Complex )
{
	OwnsData = true;

	int Mult = Complex ? 2 : 1;

	if ( Dim == 1 )
	{
		a1D = new float[Mult * n1];
	}

	if ( Dim == 2 )
	{
		a2D = new float*[n1];
		a2D[0] = new float[n1 * n2 * Mult];

		for ( int j = 1; j < n1; j++ )
		{
			a2D[j] = a2D[j - 1] + ( n2 * Mult );
		}
	}

	if ( Dim == 3 )
	{
		a3D = new float** [n1];

		a3D[0] = new float*[n1 * n2];

		for ( int j = 1; j < n1; j++ )
		{
			a3D[j] = a3D[j - 1] + n2;
		}

		a3D[0][0] = new float [n1 * n2 * n3 * Mult];

		for ( int j = 1; j < n1 * n2; j++ )
		{
			a3D[0][j] = a3D[0][j - 1] + ( n3 * Mult );
		}
	}
}

void LFFT::Free()
{
	delete[] w;
	delete[] ip;

	if ( Dim == 3 || Dim == 2 )
	{
		delete[] t;
///      free_3d_float(a);
	}

	if ( OwnsData )
	{
		FreeArray();
	}

	w = t = 0;
	ip = 0;
}

void LFFT::Transform( bool Inverse, bool Complex )
{
	int Sgn = Inverse ? -1 : 1;

	if ( Dim == 1 )
	{
		int maxN1 = Complex ? 2 * n1 : n1;
		Complex ? cdft( 2 * n1, Sgn, a1D, ip, w ) : rdft( n1, Sgn, a1D, ip, w );

		if ( Inverse )
		{
			float D = ( Complex ? 1.0f : 2.0f ) / ( float )n1;

			for ( int j1 = 0; j1 < maxN1 ; j1++ )
			{
				a1D[j1] *= D;
			}
		}
	}

	if ( Dim == 2 )
	{
		int maxN2 = Complex ? 2 * n2 : n2;
		Complex ? cdft2d( n1, 2 * n2, Sgn, a2D, t, ip, w ) :    rdft2d( n1, n2, Sgn, a2D, t, ip, w );

		if ( Inverse )
		{
			float D = ( ( Complex ? 1.0f : 2.0f ) / ( float )n1 ) / ( float )n2;

			for ( int j1 = 0; j1 < n1 ; j1++ )
			{
				for ( int j2 = 0; j2 < maxN2 ; j2++ )
				{
					a2D[j1][j2] *= D;
				}
			}
		}
	}

	if ( Dim == 3 )
	{
		int maxN3 = Complex ? 2 * n3 : n3;
		Complex ? cdft3d( n1, n2, 2 * n3, Sgn, a3D, t, ip, w ) : rdft3d( n1, n2, n3, Sgn, a3D, t, ip, w );

		if ( Inverse )
		{
			float D = ( ( ( Complex ? 1.0f : 2.0f ) / ( float )n1 ) / ( float )n2 ) / ( float )n3;

			for ( int j1 = 0; j1 < n1 ; j1++ )
			{
				for ( int j2 = 0; j2 < n2 ; j2++ )
				{
					for ( int j3 = 0; j3 < maxN3 ; j3++ )
					{
						a3D[j1][j2][j3] *= D;
					}
				}
			}
		}
	}
}


/**
With some modifications, this code is

    Copyright(C) 1997,2001 Takuya OOURA (email: ooura@kurims.kyoto-u.ac.jp, web: http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html).
    You may use, copy, modify this code for any purpose and
    without fee. You may distribute this ORIGINAL package.
*/

///////////// 1D

/*
Fast Fourier/Cosine/Sine Transform
    dimension   :one
    data length :power of 2
    decimation  :frequency
    radix       :split-radix
    data        :inplace
    table       :use
functions
    cdft: Complex Discrete Fourier Transform
    rdft: Real Discrete Fourier Transform
    ddct: Discrete Cosine Transform
    ddst: Discrete Sine Transform
    dfct: Cosine Transform of RDFT (Real Symmetric DFT)
    dfst: Sine Transform of RDFT (Real Anti-symmetric DFT)
function prototypes
    void cdft(int, int, float *, int *, float *);
    void rdft(int, int, float *, int *, float *);
    void ddct(int, int, float *, int *, float *);
    void ddst(int, int, float *, int *, float *);
    void dfct(int, float *, float *, int *, float *);
    void dfst(int, float *, float *, int *, float *);
macro definitions
    USE_CDFT_PTHREADS : default=not defined
        CDFT_THREADS_BEGIN_N  : must be >= 512, default=8192
        CDFT_4THREADS_BEGIN_N : must be >= 512, default=65536
    USE_CDFT_WINTHREADS : default=not defined
        CDFT_THREADS_BEGIN_N  : must be >= 512, default=32768
        CDFT_4THREADS_BEGIN_N : must be >= 512, default=524288


-------- Complex DFT (Discrete Fourier Transform) --------
    [definition]
        <case1>
            X[k] = sum_j=0^n-1 x[j]*exp(2*pi*i*j*k/n), 0<=k<n
        <case2>
            X[k] = sum_j=0^n-1 x[j]*exp(-2*pi*i*j*k/n), 0<=k<n
        (notes: sum_j=0^n-1 is a summation from j=0 to n-1)
    [usage]
        <case1>
            ip[0] = 0; // first time only
            cdft(2*n, 1, a, ip, w);
        <case2>
            ip[0] = 0; // first time only
            cdft(2*n, -1, a, ip, w);
    [parameters]
        2*n            :data length (int)
                        n >= 1, n = power of 2
        a[0...2*n-1]   :input/output data (float *)
                        input data
                            a[2*j] = Re(x[j]),
                            a[2*j+1] = Im(x[j]), 0<=j<n
                        output data
                            a[2*k] = Re(X[k]),
                            a[2*k+1] = Im(X[k]), 0<=k<n
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n)
                        strictly,
                        length of ip >=
                            2+(1<<(int)(log(n+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n/2-1]   :cos/sin table (float *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of
            cdft(2*n, -1, a, ip, w);
        is
            cdft(2*n, 1, a, ip, w);
            for (j = 0; j <= 2 * n - 1; j++) {
                a[j] *= 1.0 / n;
            }
        .


-------- Real DFT / Inverse of Real DFT --------
    [definition]
        <case1> RDFT
            R[k] = sum_j=0^n-1 a[j]*cos(2*pi*j*k/n), 0<=k<=n/2
            I[k] = sum_j=0^n-1 a[j]*sin(2*pi*j*k/n), 0<k<n/2
        <case2> IRDFT (excluding scale)
            a[k] = (R[0] + R[n/2]*cos(pi*k))/2 +
                   sum_j=1^n/2-1 R[j]*cos(2*pi*j*k/n) +
                   sum_j=1^n/2-1 I[j]*sin(2*pi*j*k/n), 0<=k<n
    [usage]
        <case1>
            ip[0] = 0; // first time only
            rdft(n, 1, a, ip, w);
        <case2>
            ip[0] = 0; // first time only
            rdft(n, -1, a, ip, w);
    [parameters]
        n              :data length (int)
                        n >= 2, n = power of 2
        a[0...n-1]     :input/output data (float *)
                        <case1>
                            output data
                                a[2*k] = R[k], 0<=k<n/2
                                a[2*k+1] = I[k], 0<k<n/2
                                a[1] = R[n/2]
                        <case2>
                            input data
                                a[2*j] = R[j], 0<=j<n/2
                                a[2*j+1] = I[j], 0<j<n/2
                                a[1] = R[n/2]
        ip[0...*]      :work area for bit reversal (int *)
                        length of ip >= 2+sqrt(n/2)
                        strictly,
                        length of ip >=
                            2+(1<<(int)(log(n/2+0.5)/log(2))/2).
                        ip[0],ip[1] are pointers of the cos/sin table.
        w[0...n/2-1]   :cos/sin table (float *)
                        w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of
            rdft(n, 1, a, ip, w);
        is
            rdft(n, -1, a, ip, w);
            for (j = 0; j <= n - 1; j++) {
                a[j] *= 2.0 / n;
            }
        .

Appendix :
    The cos/sin table is recalculated when the larger table required.
    w[] and ip[] are compatible with all routines.
*/


void makewt( int nw, int* ip, float* w );
void makect( int nc, int* ip, float* c );
void cftfsub( int n, float* a, int* ip, int nw, float* w );
void cftbsub( int n, float* a, int* ip, int nw, float* w );
void rftfsub( int n, float* a, int nc, float* c );
void rftbsub( int n, float* a, int nc, float* c );
void makeipt( int nw, int* ip );

void cdft( int n, int isgn, float* a, int* ip, float* w )
{
	int nw;

	nw = ip[0];

	if ( n > ( nw << 2 ) )
	{
		nw = n >> 2;
		makewt( nw, ip, w );
	}

	if ( isgn >= 0 )
	{
		cftfsub( n, a, ip, nw, w );
	}
	else
	{
		cftbsub( n, a, ip, nw, w );
	}
}


void rdft( int n, int isgn, float* a, int* ip, float* w )
{
	int nw, nc;
	float xi;

	nw = ip[0];

	if ( n > ( nw << 2 ) )
	{
		nw = n >> 2;
		makewt( nw, ip, w );
	}

	nc = ip[1];

	if ( n > ( nc << 2 ) )
	{
		nc = n >> 2;
		makect( nc, ip, w + nw );
	}

	if ( isgn >= 0 )
	{
		if ( n > 4 )
		{
			cftfsub( n, a, ip, nw, w );
			rftfsub( n, a, nc, w + nw );
		}
		else if ( n == 4 )
		{
			cftfsub( n, a, ip, nw, w );
		}

		xi = a[0] - a[1];
		a[0] += a[1];
		a[1] = xi;
	}
	else
	{
		a[1] = 0.5f * ( a[0] - a[1] );
		a[0] -= a[1];

		if ( n > 4 )
		{
			rftbsub( n, a, nc, w + nw );
			cftbsub( n, a, ip, nw, w );
		}
		else if ( n == 4 )
		{
			cftbsub( n, a, ip, nw, w );
		}
	}
}

/* -------- initializing routines -------- */

void makewt( int nw, int* ip, float* w )
{
	int j, nwh, nw0, nw1;
	float delta, wn4r, wk1r, wk1i, wk3r, wk3i;

	ip[0] = nw;
	ip[1] = 1;

	if ( nw > 2 )
	{
		nwh = nw >> 1;
		delta = atanf( 1.0f ) / nwh;
		wn4r = cosf( delta * nwh );
		w[0] = 1;
		w[1] = wn4r;

		if ( nwh == 4 )
		{
			w[2] = cosf( delta * 2.0f );
			w[3] = sinf( delta * 2.0f );
		}
		else if ( nwh > 4 )
		{
			makeipt( nw, ip );
			w[2] = 0.5f / cosf( delta * 2.0f );
			w[3] = 0.5f / cosf( delta * 6.0f );

			for ( j = 4; j < nwh; j += 4 )
			{
				w[j] = cosf( delta * j );
				w[j + 1] = sinf( delta * ( float )j );
				w[j + 2] = cosf( 3 * delta * ( float )j );
				w[j + 3] = -sinf( 3 * delta * ( float )j );
			}
		}

		nw0 = 0;

		while ( nwh > 2 )
		{
			nw1 = nw0 + nwh;
			nwh >>= 1;
			w[nw1] = 1;
			w[nw1 + 1] = wn4r;

			if ( nwh == 4 )
			{
				wk1r = w[nw0 + 4];
				wk1i = w[nw0 + 5];
				w[nw1 + 2] = wk1r;
				w[nw1 + 3] = wk1i;
			}
			else if ( nwh > 4 )
			{
				wk1r = w[nw0 + 4];
				wk3r = w[nw0 + 6];
				w[nw1 + 2] = 0.5f / wk1r;
				w[nw1 + 3] = 0.5f / wk3r;

				for ( j = 4; j < nwh; j += 4 )
				{
					wk1r = w[nw0 + 2 * j];
					wk1i = w[nw0 + 2 * j + 1];
					wk3r = w[nw0 + 2 * j + 2];
					wk3i = w[nw0 + 2 * j + 3];
					w[nw1 + j] = wk1r;
					w[nw1 + j + 1] = wk1i;
					w[nw1 + j + 2] = wk3r;
					w[nw1 + j + 3] = wk3i;
				}
			}

			nw0 = nw1;
		}
	}
}


void makeipt( int nw, int* ip )
{
	int j, l, m, m2, p, q;

	ip[2] = 0;
	ip[3] = 16;
	m = 2;

	for ( l = nw; l > 32; l >>= 2 )
	{
		m2 = m << 1;
		q = m2 << 3;

		for ( j = m; j < m2; j++ )
		{
			p = ip[j] << 2;
			ip[m + j] = p;
			ip[m2 + j] = p + q;
		}

		m = m2;
	}
}


void makect( int nc, int* ip, float* c )
{
	int j, nch;
	float delta;

	ip[1] = nc;

	if ( nc > 1 )
	{
		nch = nc >> 1;
		delta = atanf( 1.0 ) / nch;
		c[0] = cosf( delta * nch );
		c[nch] = 0.5f * c[0];

		for ( j = 1; j < nch; j++ )
		{
			c[j] = 0.5f * cosf( delta * j );
			c[nc - j] = 0.5f * sinf( delta * j );
		}
	}
}


/* -------- child routines -------- */

void bitrv2( int n, int* ip, float* a );
void bitrv216( float* a );
void bitrv208( float* a );
void cftf1st( int n, float* a, float* w );
void cftrec4( int n, float* a, int nw, float* w );
void cftleaf( int n, int isplt, float* a, int nw, float* w );
void cftfx41( int n, float* a, int nw, float* w );
void cftf161( float* a, float* w );
void cftf081( float* a, float* w );
void cftf040( float* a );
void cftb040( float* a );
void cftx020( float* a );
void bitrv2conj( int n, int* ip, float* a );
void bitrv216neg( float* a );
void bitrv208neg( float* a );
void cftb1st( int n, float* a, float* w );

void cftfsub( int n, float* a, int* ip, int nw, float* w )
{
	if ( n > 8 )
	{
		if ( n > 32 )
		{
			cftf1st( n, a, &w[nw - ( n >> 2 )] );

			if ( n > 512 )
			{
				cftrec4( n, a, nw, w );
			}
			else if ( n > 128 )
			{
				cftleaf( n, 1, a, nw, w );
			}
			else
			{
				cftfx41( n, a, nw, w );
			}

			bitrv2( n, ip, a );
		}
		else if ( n == 32 )
		{
			cftf161( a, &w[nw - 8] );
			bitrv216( a );
		}
		else
		{
			cftf081( a, w );
			bitrv208( a );
		}
	}
	else if ( n == 8 )
	{
		cftf040( a );
	}
	else if ( n == 4 )
	{
		cftx020( a );
	}
}


void cftbsub( int n, float* a, int* ip, int nw, float* w )
{
	if ( n > 8 )
	{
		if ( n > 32 )
		{
			cftb1st( n, a, &w[nw - ( n >> 2 )] );

			if ( n > 512 )
			{
				cftrec4( n, a, nw, w );
			}
			else if ( n > 128 )
			{
				cftleaf( n, 1, a, nw, w );
			}
			else
			{
				cftfx41( n, a, nw, w );
			}

			bitrv2conj( n, ip, a );
		}
		else if ( n == 32 )
		{
			cftf161( a, &w[nw - 8] );
			bitrv216neg( a );
		}
		else
		{
			cftf081( a, w );
			bitrv208neg( a );
		}
	}
	else if ( n == 8 )
	{
		cftb040( a );
	}
	else if ( n == 4 )
	{
		cftx020( a );
	}
}


void bitrv2( int n, int* ip, float* a )
{
	int j, j1, k, k1, l, m, nh, nm;
	float xr, xi, yr, yi;

	m = 1;

	for ( l = n >> 2; l > 8; l >>= 2 )
	{
		m <<= 1;
	}

	nh = n >> 1;
	nm = 4 * m;

	if ( l == 8 )
	{
		for ( k = 0; k < m; k++ )
		{
			for ( j = 0; j < k; j++ )
			{
				j1 = 4 * j + 2 * ip[m + k];
				k1 = 4 * k + 2 * ip[m + j];
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 -= nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nh;
				k1 += 2;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 += nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += 2;
				k1 += nh;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 -= nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nh;
				k1 -= 2;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 += nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
			}

			k1 = 4 * k + 2 * ip[m + k];
			j1 = k1 + 2;
			k1 += nh;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 += nm;
			k1 += 2 * nm;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 += nm;
			k1 -= nm;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 -= 2;
			k1 -= nh;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 += nh + 2;
			k1 += nh + 2;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 -= nh - nm;
			k1 += 2 * nm - 2;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
		}
	}
	else
	{
		for ( k = 0; k < m; k++ )
		{
			for ( j = 0; j < k; j++ )
			{
				j1 = 4 * j + ip[m + k];
				k1 = 4 * k + ip[m + j];
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nh;
				k1 += 2;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += 2;
				k1 += nh;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nh;
				k1 -= 2;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= nm;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
			}

			k1 = 4 * k + ip[m + k];
			j1 = k1 + 2;
			k1 += nh;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 += nm;
			k1 += nm;
			xr = a[j1];
			xi = a[j1 + 1];
			yr = a[k1];
			yi = a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
		}
	}
}


void bitrv2conj( int n, int* ip, float* a )
{
	int j, j1, k, k1, l, m, nh, nm;
	float xr, xi, yr, yi;

	m = 1;

	for ( l = n >> 2; l > 8; l >>= 2 )
	{
		m <<= 1;
	}

	nh = n >> 1;
	nm = 4 * m;

	if ( l == 8 )
	{
		for ( k = 0; k < m; k++ )
		{
			for ( j = 0; j < k; j++ )
			{
				j1 = 4 * j + 2 * ip[m + k];
				k1 = 4 * k + 2 * ip[m + j];
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 -= nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nh;
				k1 += 2;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 += nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += 2;
				k1 += nh;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 -= nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nh;
				k1 -= 2;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 += nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= 2 * nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
			}

			k1 = 4 * k + 2 * ip[m + k];
			j1 = k1 + 2;
			k1 += nh;
			a[j1 - 1] = -a[j1 - 1];
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			a[k1 + 3] = -a[k1 + 3];
			j1 += nm;
			k1 += 2 * nm;
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 += nm;
			k1 -= nm;
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 -= 2;
			k1 -= nh;
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 += nh + 2;
			k1 += nh + 2;
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			j1 -= nh - nm;
			k1 += 2 * nm - 2;
			a[j1 - 1] = -a[j1 - 1];
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			a[k1 + 3] = -a[k1 + 3];
		}
	}
	else
	{
		for ( k = 0; k < m; k++ )
		{
			for ( j = 0; j < k; j++ )
			{
				j1 = 4 * j + ip[m + k];
				k1 = 4 * k + ip[m + j];
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nh;
				k1 += 2;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += 2;
				k1 += nh;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 += nm;
				k1 += nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nh;
				k1 -= 2;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				j1 -= nm;
				k1 -= nm;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
			}

			k1 = 4 * k + ip[m + k];
			j1 = k1 + 2;
			k1 += nh;
			a[j1 - 1] = -a[j1 - 1];
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			a[k1 + 3] = -a[k1 + 3];
			j1 += nm;
			k1 += nm;
			a[j1 - 1] = -a[j1 - 1];
			xr = a[j1];
			xi = -a[j1 + 1];
			yr = a[k1];
			yi = -a[k1 + 1];
			a[j1] = yr;
			a[j1 + 1] = yi;
			a[k1] = xr;
			a[k1 + 1] = xi;
			a[k1 + 3] = -a[k1 + 3];
		}
	}
}


void bitrv216( float* a )
{
	float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i,
	      x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i,
	      x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;

	x1r = a[2];
	x1i = a[3];
	x2r = a[4];
	x2i = a[5];
	x3r = a[6];
	x3i = a[7];
	x4r = a[8];
	x4i = a[9];
	x5r = a[10];
	x5i = a[11];
	x7r = a[14];
	x7i = a[15];
	x8r = a[16];
	x8i = a[17];
	x10r = a[20];
	x10i = a[21];
	x11r = a[22];
	x11i = a[23];
	x12r = a[24];
	x12i = a[25];
	x13r = a[26];
	x13i = a[27];
	x14r = a[28];
	x14i = a[29];
	a[2] = x8r;
	a[3] = x8i;
	a[4] = x4r;
	a[5] = x4i;
	a[6] = x12r;
	a[7] = x12i;
	a[8] = x2r;
	a[9] = x2i;
	a[10] = x10r;
	a[11] = x10i;
	a[14] = x14r;
	a[15] = x14i;
	a[16] = x1r;
	a[17] = x1i;
	a[20] = x5r;
	a[21] = x5i;
	a[22] = x13r;
	a[23] = x13i;
	a[24] = x3r;
	a[25] = x3i;
	a[26] = x11r;
	a[27] = x11i;
	a[28] = x7r;
	a[29] = x7i;
}


void bitrv216neg( float* a )
{
	float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i,
	      x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i,
	      x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i,
	      x13r, x13i, x14r, x14i, x15r, x15i;

	x1r = a[2];
	x1i = a[3];
	x2r = a[4];
	x2i = a[5];
	x3r = a[6];
	x3i = a[7];
	x4r = a[8];
	x4i = a[9];
	x5r = a[10];
	x5i = a[11];
	x6r = a[12];
	x6i = a[13];
	x7r = a[14];
	x7i = a[15];
	x8r = a[16];
	x8i = a[17];
	x9r = a[18];
	x9i = a[19];
	x10r = a[20];
	x10i = a[21];
	x11r = a[22];
	x11i = a[23];
	x12r = a[24];
	x12i = a[25];
	x13r = a[26];
	x13i = a[27];
	x14r = a[28];
	x14i = a[29];
	x15r = a[30];
	x15i = a[31];
	a[2] = x15r;
	a[3] = x15i;
	a[4] = x7r;
	a[5] = x7i;
	a[6] = x11r;
	a[7] = x11i;
	a[8] = x3r;
	a[9] = x3i;
	a[10] = x13r;
	a[11] = x13i;
	a[12] = x5r;
	a[13] = x5i;
	a[14] = x9r;
	a[15] = x9i;
	a[16] = x1r;
	a[17] = x1i;
	a[18] = x14r;
	a[19] = x14i;
	a[20] = x6r;
	a[21] = x6i;
	a[22] = x10r;
	a[23] = x10i;
	a[24] = x2r;
	a[25] = x2i;
	a[26] = x12r;
	a[27] = x12i;
	a[28] = x4r;
	a[29] = x4i;
	a[30] = x8r;
	a[31] = x8i;
}


void bitrv208( float* a )
{
	float x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;

	x1r = a[2];
	x1i = a[3];
	x3r = a[6];
	x3i = a[7];
	x4r = a[8];
	x4i = a[9];
	x6r = a[12];
	x6i = a[13];
	a[2] = x4r;
	a[3] = x4i;
	a[6] = x6r;
	a[7] = x6i;
	a[8] = x1r;
	a[9] = x1i;
	a[12] = x3r;
	a[13] = x3i;
}


void bitrv208neg( float* a )
{
	float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i,
	      x5r, x5i, x6r, x6i, x7r, x7i;

	x1r = a[2];
	x1i = a[3];
	x2r = a[4];
	x2i = a[5];
	x3r = a[6];
	x3i = a[7];
	x4r = a[8];
	x4i = a[9];
	x5r = a[10];
	x5i = a[11];
	x6r = a[12];
	x6i = a[13];
	x7r = a[14];
	x7i = a[15];
	a[2] = x7r;
	a[3] = x7i;
	a[4] = x3r;
	a[5] = x3i;
	a[6] = x5r;
	a[7] = x5i;
	a[8] = x1r;
	a[9] = x1i;
	a[10] = x6r;
	a[11] = x6i;
	a[12] = x2r;
	a[13] = x2i;
	a[14] = x4r;
	a[15] = x4i;
}


void cftf1st( int n, float* a, float* w )
{
	int j, j0, j1, j2, j3, k, m, mh;
	float wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i,
	      wd1r, wd1i, wd3r, wd3i;
	float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i,
	      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;

	mh = n >> 3;
	m = 2 * mh;
	j1 = m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[0] + a[j2];
	x0i = a[1] + a[j2 + 1];
	x1r = a[0] - a[j2];
	x1i = a[1] - a[j2 + 1];
	x2r = a[j1] + a[j3];
	x2i = a[j1 + 1] + a[j3 + 1];
	x3r = a[j1] - a[j3];
	x3i = a[j1 + 1] - a[j3 + 1];
	a[0] = x0r + x2r;
	a[1] = x0i + x2i;
	a[j1] = x0r - x2r;
	a[j1 + 1] = x0i - x2i;
	a[j2] = x1r - x3i;
	a[j2 + 1] = x1i + x3r;
	a[j3] = x1r + x3i;
	a[j3 + 1] = x1i - x3r;
	wn4r = w[1];
	csc1 = w[2];
	csc3 = w[3];
	wd1r = 1;
	wd1i = 0;
	wd3r = 1;
	wd3i = 0;
	k = 0;

	for ( j = 2; j < mh - 2; j += 4 )
	{
		k += 4;
		wk1r = csc1 * ( wd1r + w[k] );
		wk1i = csc1 * ( wd1i + w[k + 1] );
		wk3r = csc3 * ( wd3r + w[k + 2] );
		wk3i = csc3 * ( wd3i + w[k + 3] );
		wd1r = w[k];
		wd1i = w[k + 1];
		wd3r = w[k + 2];
		wd3i = w[k + 3];
		j1 = j + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j] + a[j2];
		x0i = a[j + 1] + a[j2 + 1];
		x1r = a[j] - a[j2];
		x1i = a[j + 1] - a[j2 + 1];
		y0r = a[j + 2] + a[j2 + 2];
		y0i = a[j + 3] + a[j2 + 3];
		y1r = a[j + 2] - a[j2 + 2];
		y1i = a[j + 3] - a[j2 + 3];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		y2r = a[j1 + 2] + a[j3 + 2];
		y2i = a[j1 + 3] + a[j3 + 3];
		y3r = a[j1 + 2] - a[j3 + 2];
		y3i = a[j1 + 3] - a[j3 + 3];
		a[j] = x0r + x2r;
		a[j + 1] = x0i + x2i;
		a[j + 2] = y0r + y2r;
		a[j + 3] = y0i + y2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		a[j1 + 2] = y0r - y2r;
		a[j1 + 3] = y0i - y2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2] = wk1r * x0r - wk1i * x0i;
		a[j2 + 1] = wk1r * x0i + wk1i * x0r;
		x0r = y1r - y3i;
		x0i = y1i + y3r;
		a[j2 + 2] = wd1r * x0r - wd1i * x0i;
		a[j2 + 3] = wd1r * x0i + wd1i * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3] = wk3r * x0r + wk3i * x0i;
		a[j3 + 1] = wk3r * x0i - wk3i * x0r;
		x0r = y1r + y3i;
		x0i = y1i - y3r;
		a[j3 + 2] = wd3r * x0r + wd3i * x0i;
		a[j3 + 3] = wd3r * x0i - wd3i * x0r;
		j0 = m - j;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0] + a[j2];
		x0i = a[j0 + 1] + a[j2 + 1];
		x1r = a[j0] - a[j2];
		x1i = a[j0 + 1] - a[j2 + 1];
		y0r = a[j0 - 2] + a[j2 - 2];
		y0i = a[j0 - 1] + a[j2 - 1];
		y1r = a[j0 - 2] - a[j2 - 2];
		y1i = a[j0 - 1] - a[j2 - 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		y2r = a[j1 - 2] + a[j3 - 2];
		y2i = a[j1 - 1] + a[j3 - 1];
		y3r = a[j1 - 2] - a[j3 - 2];
		y3i = a[j1 - 1] - a[j3 - 1];
		a[j0] = x0r + x2r;
		a[j0 + 1] = x0i + x2i;
		a[j0 - 2] = y0r + y2r;
		a[j0 - 1] = y0i + y2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		a[j1 - 2] = y0r - y2r;
		a[j1 - 1] = y0i - y2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2] = wk1i * x0r - wk1r * x0i;
		a[j2 + 1] = wk1i * x0i + wk1r * x0r;
		x0r = y1r - y3i;
		x0i = y1i + y3r;
		a[j2 - 2] = wd1i * x0r - wd1r * x0i;
		a[j2 - 1] = wd1i * x0i + wd1r * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3] = wk3i * x0r + wk3r * x0i;
		a[j3 + 1] = wk3i * x0i - wk3r * x0r;
		x0r = y1r + y3i;
		x0i = y1i - y3r;
		a[j3 - 2] = wd3i * x0r + wd3r * x0i;
		a[j3 - 1] = wd3i * x0i - wd3r * x0r;
	}

	wk1r = csc1 * ( wd1r + wn4r );
	wk1i = csc1 * ( wd1i + wn4r );
	wk3r = csc3 * ( wd3r - wn4r );
	wk3i = csc3 * ( wd3i - wn4r );
	j0 = mh;
	j1 = j0 + m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[j0 - 2] + a[j2 - 2];
	x0i = a[j0 - 1] + a[j2 - 1];
	x1r = a[j0 - 2] - a[j2 - 2];
	x1i = a[j0 - 1] - a[j2 - 1];
	x2r = a[j1 - 2] + a[j3 - 2];
	x2i = a[j1 - 1] + a[j3 - 1];
	x3r = a[j1 - 2] - a[j3 - 2];
	x3i = a[j1 - 1] - a[j3 - 1];
	a[j0 - 2] = x0r + x2r;
	a[j0 - 1] = x0i + x2i;
	a[j1 - 2] = x0r - x2r;
	a[j1 - 1] = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	a[j2 - 2] = wk1r * x0r - wk1i * x0i;
	a[j2 - 1] = wk1r * x0i + wk1i * x0r;
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	a[j3 - 2] = wk3r * x0r + wk3i * x0i;
	a[j3 - 1] = wk3r * x0i - wk3i * x0r;
	x0r = a[j0] + a[j2];
	x0i = a[j0 + 1] + a[j2 + 1];
	x1r = a[j0] - a[j2];
	x1i = a[j0 + 1] - a[j2 + 1];
	x2r = a[j1] + a[j3];
	x2i = a[j1 + 1] + a[j3 + 1];
	x3r = a[j1] - a[j3];
	x3i = a[j1 + 1] - a[j3 + 1];
	a[j0] = x0r + x2r;
	a[j0 + 1] = x0i + x2i;
	a[j1] = x0r - x2r;
	a[j1 + 1] = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	a[j2] = wn4r * ( x0r - x0i );
	a[j2 + 1] = wn4r * ( x0i + x0r );
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	a[j3] = -wn4r * ( x0r + x0i );
	a[j3 + 1] = -wn4r * ( x0i - x0r );
	x0r = a[j0 + 2] + a[j2 + 2];
	x0i = a[j0 + 3] + a[j2 + 3];
	x1r = a[j0 + 2] - a[j2 + 2];
	x1i = a[j0 + 3] - a[j2 + 3];
	x2r = a[j1 + 2] + a[j3 + 2];
	x2i = a[j1 + 3] + a[j3 + 3];
	x3r = a[j1 + 2] - a[j3 + 2];
	x3i = a[j1 + 3] - a[j3 + 3];
	a[j0 + 2] = x0r + x2r;
	a[j0 + 3] = x0i + x2i;
	a[j1 + 2] = x0r - x2r;
	a[j1 + 3] = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	a[j2 + 2] = wk1i * x0r - wk1r * x0i;
	a[j2 + 3] = wk1i * x0i + wk1r * x0r;
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	a[j3 + 2] = wk3i * x0r + wk3r * x0i;
	a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}


void cftb1st( int n, float* a, float* w )
{
	int j, j0, j1, j2, j3, k, m, mh;
	float wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i,
	      wd1r, wd1i, wd3r, wd3i;
	float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i,
	      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;

	mh = n >> 3;
	m = 2 * mh;
	j1 = m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[0] + a[j2];
	x0i = -a[1] - a[j2 + 1];
	x1r = a[0] - a[j2];
	x1i = -a[1] + a[j2 + 1];
	x2r = a[j1] + a[j3];
	x2i = a[j1 + 1] + a[j3 + 1];
	x3r = a[j1] - a[j3];
	x3i = a[j1 + 1] - a[j3 + 1];
	a[0] = x0r + x2r;
	a[1] = x0i - x2i;
	a[j1] = x0r - x2r;
	a[j1 + 1] = x0i + x2i;
	a[j2] = x1r + x3i;
	a[j2 + 1] = x1i + x3r;
	a[j3] = x1r - x3i;
	a[j3 + 1] = x1i - x3r;
	wn4r = w[1];
	csc1 = w[2];
	csc3 = w[3];
	wd1r = 1;
	wd1i = 0;
	wd3r = 1;
	wd3i = 0;
	k = 0;

	for ( j = 2; j < mh - 2; j += 4 )
	{
		k += 4;
		wk1r = csc1 * ( wd1r + w[k] );
		wk1i = csc1 * ( wd1i + w[k + 1] );
		wk3r = csc3 * ( wd3r + w[k + 2] );
		wk3i = csc3 * ( wd3i + w[k + 3] );
		wd1r = w[k];
		wd1i = w[k + 1];
		wd3r = w[k + 2];
		wd3i = w[k + 3];
		j1 = j + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j] + a[j2];
		x0i = -a[j + 1] - a[j2 + 1];
		x1r = a[j] - a[j2];
		x1i = -a[j + 1] + a[j2 + 1];
		y0r = a[j + 2] + a[j2 + 2];
		y0i = -a[j + 3] - a[j2 + 3];
		y1r = a[j + 2] - a[j2 + 2];
		y1i = -a[j + 3] + a[j2 + 3];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		y2r = a[j1 + 2] + a[j3 + 2];
		y2i = a[j1 + 3] + a[j3 + 3];
		y3r = a[j1 + 2] - a[j3 + 2];
		y3i = a[j1 + 3] - a[j3 + 3];
		a[j] = x0r + x2r;
		a[j + 1] = x0i - x2i;
		a[j + 2] = y0r + y2r;
		a[j + 3] = y0i - y2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i + x2i;
		a[j1 + 2] = y0r - y2r;
		a[j1 + 3] = y0i + y2i;
		x0r = x1r + x3i;
		x0i = x1i + x3r;
		a[j2] = wk1r * x0r - wk1i * x0i;
		a[j2 + 1] = wk1r * x0i + wk1i * x0r;
		x0r = y1r + y3i;
		x0i = y1i + y3r;
		a[j2 + 2] = wd1r * x0r - wd1i * x0i;
		a[j2 + 3] = wd1r * x0i + wd1i * x0r;
		x0r = x1r - x3i;
		x0i = x1i - x3r;
		a[j3] = wk3r * x0r + wk3i * x0i;
		a[j3 + 1] = wk3r * x0i - wk3i * x0r;
		x0r = y1r - y3i;
		x0i = y1i - y3r;
		a[j3 + 2] = wd3r * x0r + wd3i * x0i;
		a[j3 + 3] = wd3r * x0i - wd3i * x0r;
		j0 = m - j;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0] + a[j2];
		x0i = -a[j0 + 1] - a[j2 + 1];
		x1r = a[j0] - a[j2];
		x1i = -a[j0 + 1] + a[j2 + 1];
		y0r = a[j0 - 2] + a[j2 - 2];
		y0i = -a[j0 - 1] - a[j2 - 1];
		y1r = a[j0 - 2] - a[j2 - 2];
		y1i = -a[j0 - 1] + a[j2 - 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		y2r = a[j1 - 2] + a[j3 - 2];
		y2i = a[j1 - 1] + a[j3 - 1];
		y3r = a[j1 - 2] - a[j3 - 2];
		y3i = a[j1 - 1] - a[j3 - 1];
		a[j0] = x0r + x2r;
		a[j0 + 1] = x0i - x2i;
		a[j0 - 2] = y0r + y2r;
		a[j0 - 1] = y0i - y2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i + x2i;
		a[j1 - 2] = y0r - y2r;
		a[j1 - 1] = y0i + y2i;
		x0r = x1r + x3i;
		x0i = x1i + x3r;
		a[j2] = wk1i * x0r - wk1r * x0i;
		a[j2 + 1] = wk1i * x0i + wk1r * x0r;
		x0r = y1r + y3i;
		x0i = y1i + y3r;
		a[j2 - 2] = wd1i * x0r - wd1r * x0i;
		a[j2 - 1] = wd1i * x0i + wd1r * x0r;
		x0r = x1r - x3i;
		x0i = x1i - x3r;
		a[j3] = wk3i * x0r + wk3r * x0i;
		a[j3 + 1] = wk3i * x0i - wk3r * x0r;
		x0r = y1r - y3i;
		x0i = y1i - y3r;
		a[j3 - 2] = wd3i * x0r + wd3r * x0i;
		a[j3 - 1] = wd3i * x0i - wd3r * x0r;
	}

	wk1r = csc1 * ( wd1r + wn4r );
	wk1i = csc1 * ( wd1i + wn4r );
	wk3r = csc3 * ( wd3r - wn4r );
	wk3i = csc3 * ( wd3i - wn4r );
	j0 = mh;
	j1 = j0 + m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[j0 - 2] + a[j2 - 2];
	x0i = -a[j0 - 1] - a[j2 - 1];
	x1r = a[j0 - 2] - a[j2 - 2];
	x1i = -a[j0 - 1] + a[j2 - 1];
	x2r = a[j1 - 2] + a[j3 - 2];
	x2i = a[j1 - 1] + a[j3 - 1];
	x3r = a[j1 - 2] - a[j3 - 2];
	x3i = a[j1 - 1] - a[j3 - 1];
	a[j0 - 2] = x0r + x2r;
	a[j0 - 1] = x0i - x2i;
	a[j1 - 2] = x0r - x2r;
	a[j1 - 1] = x0i + x2i;
	x0r = x1r + x3i;
	x0i = x1i + x3r;
	a[j2 - 2] = wk1r * x0r - wk1i * x0i;
	a[j2 - 1] = wk1r * x0i + wk1i * x0r;
	x0r = x1r - x3i;
	x0i = x1i - x3r;
	a[j3 - 2] = wk3r * x0r + wk3i * x0i;
	a[j3 - 1] = wk3r * x0i - wk3i * x0r;
	x0r = a[j0] + a[j2];
	x0i = -a[j0 + 1] - a[j2 + 1];
	x1r = a[j0] - a[j2];
	x1i = -a[j0 + 1] + a[j2 + 1];
	x2r = a[j1] + a[j3];
	x2i = a[j1 + 1] + a[j3 + 1];
	x3r = a[j1] - a[j3];
	x3i = a[j1 + 1] - a[j3 + 1];
	a[j0] = x0r + x2r;
	a[j0 + 1] = x0i - x2i;
	a[j1] = x0r - x2r;
	a[j1 + 1] = x0i + x2i;
	x0r = x1r + x3i;
	x0i = x1i + x3r;
	a[j2] = wn4r * ( x0r - x0i );
	a[j2 + 1] = wn4r * ( x0i + x0r );
	x0r = x1r - x3i;
	x0i = x1i - x3r;
	a[j3] = -wn4r * ( x0r + x0i );
	a[j3 + 1] = -wn4r * ( x0i - x0r );
	x0r = a[j0 + 2] + a[j2 + 2];
	x0i = -a[j0 + 3] - a[j2 + 3];
	x1r = a[j0 + 2] - a[j2 + 2];
	x1i = -a[j0 + 3] + a[j2 + 3];
	x2r = a[j1 + 2] + a[j3 + 2];
	x2i = a[j1 + 3] + a[j3 + 3];
	x3r = a[j1 + 2] - a[j3 + 2];
	x3i = a[j1 + 3] - a[j3 + 3];
	a[j0 + 2] = x0r + x2r;
	a[j0 + 3] = x0i - x2i;
	a[j1 + 2] = x0r - x2r;
	a[j1 + 3] = x0i + x2i;
	x0r = x1r + x3i;
	x0i = x1i + x3r;
	a[j2 + 2] = wk1i * x0r - wk1r * x0i;
	a[j2 + 3] = wk1i * x0i + wk1r * x0r;
	x0r = x1r - x3i;
	x0i = x1i - x3r;
	a[j3 + 2] = wk3i * x0r + wk3r * x0i;
	a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

int cfttree( int n, int j, int k, float* a, int nw, float* w );
void cftmdl1( int n, float* a, float* w );
void cftmdl2( int n, float* a, float* w );

void cftrec4( int n, float* a, int nw, float* w )
{
	int isplt, j, k, m;

	m = n;

	while ( m > 512 )
	{
		m >>= 2;
		cftmdl1( m, &a[n - m], &w[nw - ( m >> 1 )] );
	}

	cftleaf( m, 1, &a[n - m], nw, w );
	k = 0;

	for ( j = n - m; j > 0; j -= m )
	{
		k++;
		isplt = cfttree( m, j, k, a, nw, w );
		cftleaf( m, isplt, &a[j - m], nw, w );
	}
}


int cfttree( int n, int j, int k, float* a, int nw, float* w )
{
	int i, isplt, m;

	if ( ( k & 3 ) != 0 )
	{
		isplt = k & 1;

		if ( isplt != 0 )
		{
			cftmdl1( n, &a[j - n], &w[nw - ( n >> 1 )] );
		}
		else
		{
			cftmdl2( n, &a[j - n], &w[nw - n] );
		}
	}
	else
	{
		m = n;

		for ( i = k; ( i & 3 ) == 0; i >>= 2 )
		{
			m <<= 2;
		}

		isplt = i & 1;

		if ( isplt != 0 )
		{
			while ( m > 128 )
			{
				cftmdl1( m, &a[j - m], &w[nw - ( m >> 1 )] );
				m >>= 2;
			}
		}
		else
		{
			while ( m > 128 )
			{
				cftmdl2( m, &a[j - m], &w[nw - m] );
				m >>= 2;
			}
		}
	}

	return isplt;
}

void cftf161( float* a, float* w );
void cftf162( float* a, float* w );
void cftf081( float* a, float* w );
void cftf082( float* a, float* w );

void cftleaf( int n, int isplt, float* a, int nw, float* w )
{

	if ( n == 512 )
	{
		cftmdl1( 128, a, &w[nw - 64] );
		cftf161( a, &w[nw - 8] );
		cftf162( &a[32], &w[nw - 32] );
		cftf161( &a[64], &w[nw - 8] );
		cftf161( &a[96], &w[nw - 8] );
		cftmdl2( 128, &a[128], &w[nw - 128] );
		cftf161( &a[128], &w[nw - 8] );
		cftf162( &a[160], &w[nw - 32] );
		cftf161( &a[192], &w[nw - 8] );
		cftf162( &a[224], &w[nw - 32] );
		cftmdl1( 128, &a[256], &w[nw - 64] );
		cftf161( &a[256], &w[nw - 8] );
		cftf162( &a[288], &w[nw - 32] );
		cftf161( &a[320], &w[nw - 8] );
		cftf161( &a[352], &w[nw - 8] );

		if ( isplt != 0 )
		{
			cftmdl1( 128, &a[384], &w[nw - 64] );
			cftf161( &a[480], &w[nw - 8] );
		}
		else
		{
			cftmdl2( 128, &a[384], &w[nw - 128] );
			cftf162( &a[480], &w[nw - 32] );
		}

		cftf161( &a[384], &w[nw - 8] );
		cftf162( &a[416], &w[nw - 32] );
		cftf161( &a[448], &w[nw - 8] );
	}
	else
	{
		cftmdl1( 64, a, &w[nw - 32] );
		cftf081( a, &w[nw - 8] );
		cftf082( &a[16], &w[nw - 8] );
		cftf081( &a[32], &w[nw - 8] );
		cftf081( &a[48], &w[nw - 8] );
		cftmdl2( 64, &a[64], &w[nw - 64] );
		cftf081( &a[64], &w[nw - 8] );
		cftf082( &a[80], &w[nw - 8] );
		cftf081( &a[96], &w[nw - 8] );
		cftf082( &a[112], &w[nw - 8] );
		cftmdl1( 64, &a[128], &w[nw - 32] );
		cftf081( &a[128], &w[nw - 8] );
		cftf082( &a[144], &w[nw - 8] );
		cftf081( &a[160], &w[nw - 8] );
		cftf081( &a[176], &w[nw - 8] );

		if ( isplt != 0 )
		{
			cftmdl1( 64, &a[192], &w[nw - 32] );
			cftf081( &a[240], &w[nw - 8] );
		}
		else
		{
			cftmdl2( 64, &a[192], &w[nw - 64] );
			cftf082( &a[240], &w[nw - 8] );
		}

		cftf081( &a[192], &w[nw - 8] );
		cftf082( &a[208], &w[nw - 8] );
		cftf081( &a[224], &w[nw - 8] );
	}
}


void cftmdl1( int n, float* a, float* w )
{
	int j, j0, j1, j2, j3, k, m, mh;
	float wn4r, wk1r, wk1i, wk3r, wk3i;
	float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

	mh = n >> 3;
	m = 2 * mh;
	j1 = m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[0] + a[j2];
	x0i = a[1] + a[j2 + 1];
	x1r = a[0] - a[j2];
	x1i = a[1] - a[j2 + 1];
	x2r = a[j1] + a[j3];
	x2i = a[j1 + 1] + a[j3 + 1];
	x3r = a[j1] - a[j3];
	x3i = a[j1 + 1] - a[j3 + 1];
	a[0] = x0r + x2r;
	a[1] = x0i + x2i;
	a[j1] = x0r - x2r;
	a[j1 + 1] = x0i - x2i;
	a[j2] = x1r - x3i;
	a[j2 + 1] = x1i + x3r;
	a[j3] = x1r + x3i;
	a[j3 + 1] = x1i - x3r;
	wn4r = w[1];
	k = 0;

	for ( j = 2; j < mh; j += 2 )
	{
		k += 4;
		wk1r = w[k];
		wk1i = w[k + 1];
		wk3r = w[k + 2];
		wk3i = w[k + 3];
		j1 = j + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j] + a[j2];
		x0i = a[j + 1] + a[j2 + 1];
		x1r = a[j] - a[j2];
		x1i = a[j + 1] - a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[j] = x0r + x2r;
		a[j + 1] = x0i + x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2] = wk1r * x0r - wk1i * x0i;
		a[j2 + 1] = wk1r * x0i + wk1i * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3] = wk3r * x0r + wk3i * x0i;
		a[j3 + 1] = wk3r * x0i - wk3i * x0r;
		j0 = m - j;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0] + a[j2];
		x0i = a[j0 + 1] + a[j2 + 1];
		x1r = a[j0] - a[j2];
		x1i = a[j0 + 1] - a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[j0] = x0r + x2r;
		a[j0 + 1] = x0i + x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2] = wk1i * x0r - wk1r * x0i;
		a[j2 + 1] = wk1i * x0i + wk1r * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3] = wk3i * x0r + wk3r * x0i;
		a[j3 + 1] = wk3i * x0i - wk3r * x0r;
	}

	j0 = mh;
	j1 = j0 + m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[j0] + a[j2];
	x0i = a[j0 + 1] + a[j2 + 1];
	x1r = a[j0] - a[j2];
	x1i = a[j0 + 1] - a[j2 + 1];
	x2r = a[j1] + a[j3];
	x2i = a[j1 + 1] + a[j3 + 1];
	x3r = a[j1] - a[j3];
	x3i = a[j1 + 1] - a[j3 + 1];
	a[j0] = x0r + x2r;
	a[j0 + 1] = x0i + x2i;
	a[j1] = x0r - x2r;
	a[j1 + 1] = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	a[j2] = wn4r * ( x0r - x0i );
	a[j2 + 1] = wn4r * ( x0i + x0r );
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	a[j3] = -wn4r * ( x0r + x0i );
	a[j3 + 1] = -wn4r * ( x0i - x0r );
}


void cftmdl2( int n, float* a, float* w )
{
	int j, j0, j1, j2, j3, k, kr, m, mh;
	float wn4r, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
	float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;

	mh = n >> 3;
	m = 2 * mh;
	wn4r = w[1];
	j1 = m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[0] - a[j2 + 1];
	x0i = a[1] + a[j2];
	x1r = a[0] + a[j2 + 1];
	x1i = a[1] - a[j2];
	x2r = a[j1] - a[j3 + 1];
	x2i = a[j1 + 1] + a[j3];
	x3r = a[j1] + a[j3 + 1];
	x3i = a[j1 + 1] - a[j3];
	y0r = wn4r * ( x2r - x2i );
	y0i = wn4r * ( x2i + x2r );
	a[0] = x0r + y0r;
	a[1] = x0i + y0i;
	a[j1] = x0r - y0r;
	a[j1 + 1] = x0i - y0i;
	y0r = wn4r * ( x3r - x3i );
	y0i = wn4r * ( x3i + x3r );
	a[j2] = x1r - y0i;
	a[j2 + 1] = x1i + y0r;
	a[j3] = x1r + y0i;
	a[j3 + 1] = x1i - y0r;
	k = 0;
	kr = 2 * m;

	for ( j = 2; j < mh; j += 2 )
	{
		k += 4;
		wk1r = w[k];
		wk1i = w[k + 1];
		wk3r = w[k + 2];
		wk3i = w[k + 3];
		kr -= 4;
		wd1i = w[kr];
		wd1r = w[kr + 1];
		wd3i = w[kr + 2];
		wd3r = w[kr + 3];
		j1 = j + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j] - a[j2 + 1];
		x0i = a[j + 1] + a[j2];
		x1r = a[j] + a[j2 + 1];
		x1i = a[j + 1] - a[j2];
		x2r = a[j1] - a[j3 + 1];
		x2i = a[j1 + 1] + a[j3];
		x3r = a[j1] + a[j3 + 1];
		x3i = a[j1 + 1] - a[j3];
		y0r = wk1r * x0r - wk1i * x0i;
		y0i = wk1r * x0i + wk1i * x0r;
		y2r = wd1r * x2r - wd1i * x2i;
		y2i = wd1r * x2i + wd1i * x2r;
		a[j] = y0r + y2r;
		a[j + 1] = y0i + y2i;
		a[j1] = y0r - y2r;
		a[j1 + 1] = y0i - y2i;
		y0r = wk3r * x1r + wk3i * x1i;
		y0i = wk3r * x1i - wk3i * x1r;
		y2r = wd3r * x3r + wd3i * x3i;
		y2i = wd3r * x3i - wd3i * x3r;
		a[j2] = y0r + y2r;
		a[j2 + 1] = y0i + y2i;
		a[j3] = y0r - y2r;
		a[j3 + 1] = y0i - y2i;
		j0 = m - j;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0] - a[j2 + 1];
		x0i = a[j0 + 1] + a[j2];
		x1r = a[j0] + a[j2 + 1];
		x1i = a[j0 + 1] - a[j2];
		x2r = a[j1] - a[j3 + 1];
		x2i = a[j1 + 1] + a[j3];
		x3r = a[j1] + a[j3 + 1];
		x3i = a[j1 + 1] - a[j3];
		y0r = wd1i * x0r - wd1r * x0i;
		y0i = wd1i * x0i + wd1r * x0r;
		y2r = wk1i * x2r - wk1r * x2i;
		y2i = wk1i * x2i + wk1r * x2r;
		a[j0] = y0r + y2r;
		a[j0 + 1] = y0i + y2i;
		a[j1] = y0r - y2r;
		a[j1 + 1] = y0i - y2i;
		y0r = wd3i * x1r + wd3r * x1i;
		y0i = wd3i * x1i - wd3r * x1r;
		y2r = wk3i * x3r + wk3r * x3i;
		y2i = wk3i * x3i - wk3r * x3r;
		a[j2] = y0r + y2r;
		a[j2 + 1] = y0i + y2i;
		a[j3] = y0r - y2r;
		a[j3 + 1] = y0i - y2i;
	}

	wk1r = w[m];
	wk1i = w[m + 1];
	j0 = mh;
	j1 = j0 + m;
	j2 = j1 + m;
	j3 = j2 + m;
	x0r = a[j0] - a[j2 + 1];
	x0i = a[j0 + 1] + a[j2];
	x1r = a[j0] + a[j2 + 1];
	x1i = a[j0 + 1] - a[j2];
	x2r = a[j1] - a[j3 + 1];
	x2i = a[j1 + 1] + a[j3];
	x3r = a[j1] + a[j3 + 1];
	x3i = a[j1 + 1] - a[j3];
	y0r = wk1r * x0r - wk1i * x0i;
	y0i = wk1r * x0i + wk1i * x0r;
	y2r = wk1i * x2r - wk1r * x2i;
	y2i = wk1i * x2i + wk1r * x2r;
	a[j0] = y0r + y2r;
	a[j0 + 1] = y0i + y2i;
	a[j1] = y0r - y2r;
	a[j1 + 1] = y0i - y2i;
	y0r = wk1i * x1r - wk1r * x1i;
	y0i = wk1i * x1i + wk1r * x1r;
	y2r = wk1r * x3r - wk1i * x3i;
	y2i = wk1r * x3i + wk1i * x3r;
	a[j2] = y0r - y2r;
	a[j2 + 1] = y0i - y2i;
	a[j3] = y0r + y2r;
	a[j3 + 1] = y0i + y2i;
}


void cftfx41( int n, float* a, int nw, float* w )
{
	void cftf161( float * a, float * w );
	void cftf162( float * a, float * w );
	void cftf081( float * a, float * w );
	void cftf082( float * a, float * w );

	if ( n == 128 )
	{
		cftf161( a, &w[nw - 8] );
		cftf162( &a[32], &w[nw - 32] );
		cftf161( &a[64], &w[nw - 8] );
		cftf161( &a[96], &w[nw - 8] );
	}
	else
	{
		cftf081( a, &w[nw - 8] );
		cftf082( &a[16], &w[nw - 8] );
		cftf081( &a[32], &w[nw - 8] );
		cftf081( &a[48], &w[nw - 8] );
	}
}


void cftf161( float* a, float* w )
{
	float wn4r, wk1r, wk1i,
	      x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i,
	      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
	      y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i,
	      y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i,
	      y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;

	wn4r = w[1];
	wk1r = w[2];
	wk1i = w[3];
	x0r = a[0] + a[16];
	x0i = a[1] + a[17];
	x1r = a[0] - a[16];
	x1i = a[1] - a[17];
	x2r = a[8] + a[24];
	x2i = a[9] + a[25];
	x3r = a[8] - a[24];
	x3i = a[9] - a[25];
	y0r = x0r + x2r;
	y0i = x0i + x2i;
	y4r = x0r - x2r;
	y4i = x0i - x2i;
	y8r = x1r - x3i;
	y8i = x1i + x3r;
	y12r = x1r + x3i;
	y12i = x1i - x3r;
	x0r = a[2] + a[18];
	x0i = a[3] + a[19];
	x1r = a[2] - a[18];
	x1i = a[3] - a[19];
	x2r = a[10] + a[26];
	x2i = a[11] + a[27];
	x3r = a[10] - a[26];
	x3i = a[11] - a[27];
	y1r = x0r + x2r;
	y1i = x0i + x2i;
	y5r = x0r - x2r;
	y5i = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	y9r = wk1r * x0r - wk1i * x0i;
	y9i = wk1r * x0i + wk1i * x0r;
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	y13r = wk1i * x0r - wk1r * x0i;
	y13i = wk1i * x0i + wk1r * x0r;
	x0r = a[4] + a[20];
	x0i = a[5] + a[21];
	x1r = a[4] - a[20];
	x1i = a[5] - a[21];
	x2r = a[12] + a[28];
	x2i = a[13] + a[29];
	x3r = a[12] - a[28];
	x3i = a[13] - a[29];
	y2r = x0r + x2r;
	y2i = x0i + x2i;
	y6r = x0r - x2r;
	y6i = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	y10r = wn4r * ( x0r - x0i );
	y10i = wn4r * ( x0i + x0r );
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	y14r = wn4r * ( x0r + x0i );
	y14i = wn4r * ( x0i - x0r );
	x0r = a[6] + a[22];
	x0i = a[7] + a[23];
	x1r = a[6] - a[22];
	x1i = a[7] - a[23];
	x2r = a[14] + a[30];
	x2i = a[15] + a[31];
	x3r = a[14] - a[30];
	x3i = a[15] - a[31];
	y3r = x0r + x2r;
	y3i = x0i + x2i;
	y7r = x0r - x2r;
	y7i = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	y11r = wk1i * x0r - wk1r * x0i;
	y11i = wk1i * x0i + wk1r * x0r;
	x0r = x1r + x3i;
	x0i = x1i - x3r;
	y15r = wk1r * x0r - wk1i * x0i;
	y15i = wk1r * x0i + wk1i * x0r;
	x0r = y12r - y14r;
	x0i = y12i - y14i;
	x1r = y12r + y14r;
	x1i = y12i + y14i;
	x2r = y13r - y15r;
	x2i = y13i - y15i;
	x3r = y13r + y15r;
	x3i = y13i + y15i;
	a[24] = x0r + x2r;
	a[25] = x0i + x2i;
	a[26] = x0r - x2r;
	a[27] = x0i - x2i;
	a[28] = x1r - x3i;
	a[29] = x1i + x3r;
	a[30] = x1r + x3i;
	a[31] = x1i - x3r;
	x0r = y8r + y10r;
	x0i = y8i + y10i;
	x1r = y8r - y10r;
	x1i = y8i - y10i;
	x2r = y9r + y11r;
	x2i = y9i + y11i;
	x3r = y9r - y11r;
	x3i = y9i - y11i;
	a[16] = x0r + x2r;
	a[17] = x0i + x2i;
	a[18] = x0r - x2r;
	a[19] = x0i - x2i;
	a[20] = x1r - x3i;
	a[21] = x1i + x3r;
	a[22] = x1r + x3i;
	a[23] = x1i - x3r;
	x0r = y5r - y7i;
	x0i = y5i + y7r;
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	x0r = y5r + y7i;
	x0i = y5i - y7r;
	x3r = wn4r * ( x0r - x0i );
	x3i = wn4r * ( x0i + x0r );
	x0r = y4r - y6i;
	x0i = y4i + y6r;
	x1r = y4r + y6i;
	x1i = y4i - y6r;
	a[8] = x0r + x2r;
	a[9] = x0i + x2i;
	a[10] = x0r - x2r;
	a[11] = x0i - x2i;
	a[12] = x1r - x3i;
	a[13] = x1i + x3r;
	a[14] = x1r + x3i;
	a[15] = x1i - x3r;
	x0r = y0r + y2r;
	x0i = y0i + y2i;
	x1r = y0r - y2r;
	x1i = y0i - y2i;
	x2r = y1r + y3r;
	x2i = y1i + y3i;
	x3r = y1r - y3r;
	x3i = y1i - y3i;
	a[0] = x0r + x2r;
	a[1] = x0i + x2i;
	a[2] = x0r - x2r;
	a[3] = x0i - x2i;
	a[4] = x1r - x3i;
	a[5] = x1i + x3r;
	a[6] = x1r + x3i;
	a[7] = x1i - x3r;
}


void cftf162( float* a, float* w )
{
	float wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i,
	      x0r, x0i, x1r, x1i, x2r, x2i,
	      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
	      y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i,
	      y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i,
	      y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;

	wn4r = w[1];
	wk1r = w[4];
	wk1i = w[5];
	wk3r = w[6];
	wk3i = -w[7];
	wk2r = w[8];
	wk2i = w[9];
	x1r = a[0] - a[17];
	x1i = a[1] + a[16];
	x0r = a[8] - a[25];
	x0i = a[9] + a[24];
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	y0r = x1r + x2r;
	y0i = x1i + x2i;
	y4r = x1r - x2r;
	y4i = x1i - x2i;
	x1r = a[0] + a[17];
	x1i = a[1] - a[16];
	x0r = a[8] + a[25];
	x0i = a[9] - a[24];
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	y8r = x1r - x2i;
	y8i = x1i + x2r;
	y12r = x1r + x2i;
	y12i = x1i - x2r;
	x0r = a[2] - a[19];
	x0i = a[3] + a[18];
	x1r = wk1r * x0r - wk1i * x0i;
	x1i = wk1r * x0i + wk1i * x0r;
	x0r = a[10] - a[27];
	x0i = a[11] + a[26];
	x2r = wk3i * x0r - wk3r * x0i;
	x2i = wk3i * x0i + wk3r * x0r;
	y1r = x1r + x2r;
	y1i = x1i + x2i;
	y5r = x1r - x2r;
	y5i = x1i - x2i;
	x0r = a[2] + a[19];
	x0i = a[3] - a[18];
	x1r = wk3r * x0r - wk3i * x0i;
	x1i = wk3r * x0i + wk3i * x0r;
	x0r = a[10] + a[27];
	x0i = a[11] - a[26];
	x2r = wk1r * x0r + wk1i * x0i;
	x2i = wk1r * x0i - wk1i * x0r;
	y9r = x1r - x2r;
	y9i = x1i - x2i;
	y13r = x1r + x2r;
	y13i = x1i + x2i;
	x0r = a[4] - a[21];
	x0i = a[5] + a[20];
	x1r = wk2r * x0r - wk2i * x0i;
	x1i = wk2r * x0i + wk2i * x0r;
	x0r = a[12] - a[29];
	x0i = a[13] + a[28];
	x2r = wk2i * x0r - wk2r * x0i;
	x2i = wk2i * x0i + wk2r * x0r;
	y2r = x1r + x2r;
	y2i = x1i + x2i;
	y6r = x1r - x2r;
	y6i = x1i - x2i;
	x0r = a[4] + a[21];
	x0i = a[5] - a[20];
	x1r = wk2i * x0r - wk2r * x0i;
	x1i = wk2i * x0i + wk2r * x0r;
	x0r = a[12] + a[29];
	x0i = a[13] - a[28];
	x2r = wk2r * x0r - wk2i * x0i;
	x2i = wk2r * x0i + wk2i * x0r;
	y10r = x1r - x2r;
	y10i = x1i - x2i;
	y14r = x1r + x2r;
	y14i = x1i + x2i;
	x0r = a[6] - a[23];
	x0i = a[7] + a[22];
	x1r = wk3r * x0r - wk3i * x0i;
	x1i = wk3r * x0i + wk3i * x0r;
	x0r = a[14] - a[31];
	x0i = a[15] + a[30];
	x2r = wk1i * x0r - wk1r * x0i;
	x2i = wk1i * x0i + wk1r * x0r;
	y3r = x1r + x2r;
	y3i = x1i + x2i;
	y7r = x1r - x2r;
	y7i = x1i - x2i;
	x0r = a[6] + a[23];
	x0i = a[7] - a[22];
	x1r = wk1i * x0r + wk1r * x0i;
	x1i = wk1i * x0i - wk1r * x0r;
	x0r = a[14] + a[31];
	x0i = a[15] - a[30];
	x2r = wk3i * x0r - wk3r * x0i;
	x2i = wk3i * x0i + wk3r * x0r;
	y11r = x1r + x2r;
	y11i = x1i + x2i;
	y15r = x1r - x2r;
	y15i = x1i - x2i;
	x1r = y0r + y2r;
	x1i = y0i + y2i;
	x2r = y1r + y3r;
	x2i = y1i + y3i;
	a[0] = x1r + x2r;
	a[1] = x1i + x2i;
	a[2] = x1r - x2r;
	a[3] = x1i - x2i;
	x1r = y0r - y2r;
	x1i = y0i - y2i;
	x2r = y1r - y3r;
	x2i = y1i - y3i;
	a[4] = x1r - x2i;
	a[5] = x1i + x2r;
	a[6] = x1r + x2i;
	a[7] = x1i - x2r;
	x1r = y4r - y6i;
	x1i = y4i + y6r;
	x0r = y5r - y7i;
	x0i = y5i + y7r;
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	a[8] = x1r + x2r;
	a[9] = x1i + x2i;
	a[10] = x1r - x2r;
	a[11] = x1i - x2i;
	x1r = y4r + y6i;
	x1i = y4i - y6r;
	x0r = y5r + y7i;
	x0i = y5i - y7r;
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	a[12] = x1r - x2i;
	a[13] = x1i + x2r;
	a[14] = x1r + x2i;
	a[15] = x1i - x2r;
	x1r = y8r + y10r;
	x1i = y8i + y10i;
	x2r = y9r - y11r;
	x2i = y9i - y11i;
	a[16] = x1r + x2r;
	a[17] = x1i + x2i;
	a[18] = x1r - x2r;
	a[19] = x1i - x2i;
	x1r = y8r - y10r;
	x1i = y8i - y10i;
	x2r = y9r + y11r;
	x2i = y9i + y11i;
	a[20] = x1r - x2i;
	a[21] = x1i + x2r;
	a[22] = x1r + x2i;
	a[23] = x1i - x2r;
	x1r = y12r - y14i;
	x1i = y12i + y14r;
	x0r = y13r + y15i;
	x0i = y13i - y15r;
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	a[24] = x1r + x2r;
	a[25] = x1i + x2i;
	a[26] = x1r - x2r;
	a[27] = x1i - x2i;
	x1r = y12r + y14i;
	x1i = y12i - y14r;
	x0r = y13r - y15i;
	x0i = y13i + y15r;
	x2r = wn4r * ( x0r - x0i );
	x2i = wn4r * ( x0i + x0r );
	a[28] = x1r - x2i;
	a[29] = x1i + x2r;
	a[30] = x1r + x2i;
	a[31] = x1i - x2r;
}


void cftf081( float* a, float* w )
{
	float wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i,
	      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
	      y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

	wn4r = w[1];
	x0r = a[0] + a[8];
	x0i = a[1] + a[9];
	x1r = a[0] - a[8];
	x1i = a[1] - a[9];
	x2r = a[4] + a[12];
	x2i = a[5] + a[13];
	x3r = a[4] - a[12];
	x3i = a[5] - a[13];
	y0r = x0r + x2r;
	y0i = x0i + x2i;
	y2r = x0r - x2r;
	y2i = x0i - x2i;
	y1r = x1r - x3i;
	y1i = x1i + x3r;
	y3r = x1r + x3i;
	y3i = x1i - x3r;
	x0r = a[2] + a[10];
	x0i = a[3] + a[11];
	x1r = a[2] - a[10];
	x1i = a[3] - a[11];
	x2r = a[6] + a[14];
	x2i = a[7] + a[15];
	x3r = a[6] - a[14];
	x3i = a[7] - a[15];
	y4r = x0r + x2r;
	y4i = x0i + x2i;
	y6r = x0r - x2r;
	y6i = x0i - x2i;
	x0r = x1r - x3i;
	x0i = x1i + x3r;
	x2r = x1r + x3i;
	x2i = x1i - x3r;
	y5r = wn4r * ( x0r - x0i );
	y5i = wn4r * ( x0r + x0i );
	y7r = wn4r * ( x2r - x2i );
	y7i = wn4r * ( x2r + x2i );
	a[8] = y1r + y5r;
	a[9] = y1i + y5i;
	a[10] = y1r - y5r;
	a[11] = y1i - y5i;
	a[12] = y3r - y7i;
	a[13] = y3i + y7r;
	a[14] = y3r + y7i;
	a[15] = y3i - y7r;
	a[0] = y0r + y4r;
	a[1] = y0i + y4i;
	a[2] = y0r - y4r;
	a[3] = y0i - y4i;
	a[4] = y2r - y6i;
	a[5] = y2i + y6r;
	a[6] = y2r + y6i;
	a[7] = y2i - y6r;
}


void cftf082( float* a, float* w )
{
	float wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i,
	      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
	      y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

	wn4r = w[1];
	wk1r = w[2];
	wk1i = w[3];
	y0r = a[0] - a[9];
	y0i = a[1] + a[8];
	y1r = a[0] + a[9];
	y1i = a[1] - a[8];
	x0r = a[4] - a[13];
	x0i = a[5] + a[12];
	y2r = wn4r * ( x0r - x0i );
	y2i = wn4r * ( x0i + x0r );
	x0r = a[4] + a[13];
	x0i = a[5] - a[12];
	y3r = wn4r * ( x0r - x0i );
	y3i = wn4r * ( x0i + x0r );
	x0r = a[2] - a[11];
	x0i = a[3] + a[10];
	y4r = wk1r * x0r - wk1i * x0i;
	y4i = wk1r * x0i + wk1i * x0r;
	x0r = a[2] + a[11];
	x0i = a[3] - a[10];
	y5r = wk1i * x0r - wk1r * x0i;
	y5i = wk1i * x0i + wk1r * x0r;
	x0r = a[6] - a[15];
	x0i = a[7] + a[14];
	y6r = wk1i * x0r - wk1r * x0i;
	y6i = wk1i * x0i + wk1r * x0r;
	x0r = a[6] + a[15];
	x0i = a[7] - a[14];
	y7r = wk1r * x0r - wk1i * x0i;
	y7i = wk1r * x0i + wk1i * x0r;
	x0r = y0r + y2r;
	x0i = y0i + y2i;
	x1r = y4r + y6r;
	x1i = y4i + y6i;
	a[0] = x0r + x1r;
	a[1] = x0i + x1i;
	a[2] = x0r - x1r;
	a[3] = x0i - x1i;
	x0r = y0r - y2r;
	x0i = y0i - y2i;
	x1r = y4r - y6r;
	x1i = y4i - y6i;
	a[4] = x0r - x1i;
	a[5] = x0i + x1r;
	a[6] = x0r + x1i;
	a[7] = x0i - x1r;
	x0r = y1r - y3i;
	x0i = y1i + y3r;
	x1r = y5r - y7r;
	x1i = y5i - y7i;
	a[8] = x0r + x1r;
	a[9] = x0i + x1i;
	a[10] = x0r - x1r;
	a[11] = x0i - x1i;
	x0r = y1r + y3i;
	x0i = y1i - y3r;
	x1r = y5r + y7r;
	x1i = y5i + y7i;
	a[12] = x0r - x1i;
	a[13] = x0i + x1r;
	a[14] = x0r + x1i;
	a[15] = x0i - x1r;
}


void cftf040( float* a )
{
	float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

	x0r = a[0] + a[4];
	x0i = a[1] + a[5];
	x1r = a[0] - a[4];
	x1i = a[1] - a[5];
	x2r = a[2] + a[6];
	x2i = a[3] + a[7];
	x3r = a[2] - a[6];
	x3i = a[3] - a[7];
	a[0] = x0r + x2r;
	a[1] = x0i + x2i;
	a[2] = x1r - x3i;
	a[3] = x1i + x3r;
	a[4] = x0r - x2r;
	a[5] = x0i - x2i;
	a[6] = x1r + x3i;
	a[7] = x1i - x3r;
}


void cftb040( float* a )
{
	float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

	x0r = a[0] + a[4];
	x0i = a[1] + a[5];
	x1r = a[0] - a[4];
	x1i = a[1] - a[5];
	x2r = a[2] + a[6];
	x2i = a[3] + a[7];
	x3r = a[2] - a[6];
	x3i = a[3] - a[7];
	a[0] = x0r + x2r;
	a[1] = x0i + x2i;
	a[2] = x1r + x3i;
	a[3] = x1i - x3r;
	a[4] = x0r - x2r;
	a[5] = x0i - x2i;
	a[6] = x1r - x3i;
	a[7] = x1i + x3r;
}


void cftx020( float* a )
{
	float x0r, x0i;

	x0r = a[0] - a[2];
	x0i = a[1] - a[3];
	a[0] += a[2];
	a[1] += a[3];
	a[2] = x0r;
	a[3] = x0i;
}


void rftfsub( int n, float* a, int nc, float* c )
{
	int j, k, kk, ks, m;
	float wkr, wki, xr, xi, yr, yi;

	m = n >> 1;
	ks = 2 * nc / m;
	kk = 0;

	for ( j = 2; j < m; j += 2 )
	{
		k = n - j;
		kk += ks;
		wkr = 0.5f - c[nc - kk];
		wki = c[kk];
		xr = a[j] - a[k];
		xi = a[j + 1] + a[k + 1];
		yr = wkr * xr - wki * xi;
		yi = wkr * xi + wki * xr;
		a[j] -= yr;
		a[j + 1] -= yi;
		a[k] += yr;
		a[k + 1] -= yi;
	}
}


void rftbsub( int n, float* a, int nc, float* c )
{
	int j, k, kk, ks, m;
	float wkr, wki, xr, xi, yr, yi;

	m = n >> 1;
	ks = 2 * nc / m;
	kk = 0;

	for ( j = 2; j < m; j += 2 )
	{
		k = n - j;
		kk += ks;
		wkr = 0.5f - c[nc - kk];
		wki = c[kk];
		xr = a[j] - a[k];
		xi = a[j + 1] + a[k + 1];
		yr = wkr * xr + wki * xi;
		yi = wkr * xi - wki * xr;
		a[j] -= yr;
		a[j + 1] -= yi;
		a[k] += yr;
		a[k + 1] -= yi;
	}
}


///////////// 2D

/*
Fast Fourier/Cosine/Sine Transform
    dimension   :two
    data length :power of 2
    decimation  :frequency
    radix       :split-radix, row-column
    data        :inplace
    table       :use
functions
    cdft2d: Complex Discrete Fourier Transform
    rdft2d: Real Discrete Fourier Transform
    ddct2d: Discrete Cosine Transform
    ddst2d: Discrete Sine Transform
function prototypes
    void cdft2d(int, int, int, float **, float *, int *, float *);
    void rdft2d(int, int, int, float **, float *, int *, float *);
    void rdft2dsort(int, int, int, float **);
    void ddct2d(int, int, int, float **, float *, int *, float *);
    void ddst2d(int, int, int, float **, float *, int *, float *);
necessary package
    fftsg.c  : 1D-FFT package


-------- Complex DFT (Discrete Fourier Transform) --------
    [definition]
        <case1>
            X[k1][k2] = sum_j1=0^n1-1 sum_j2=0^n2-1 x[j1][j2] *
                            exp(2*pi*i*j1*k1/n1) *
                            exp(2*pi*i*j2*k2/n2), 0<=k1<n1, 0<=k2<n2
        <case2>
            X[k1][k2] = sum_j1=0^n1-1 sum_j2=0^n2-1 x[j1][j2] *
                            exp(-2*pi*i*j1*k1/n1) *
                            exp(-2*pi*i*j2*k2/n2), 0<=k1<n1, 0<=k2<n2
        (notes: sum_j=0^n-1 is a summation from j=0 to n-1)
    [usage]
        <case1>
            ip[0] = 0; // first time only
            cdft2d(n1, 2*n2, 1, a, t, ip, w);
        <case2>
            ip[0] = 0; // first time only
            cdft2d(n1, 2*n2, -1, a, t, ip, w);
    [parameters]
        n1     :data length (int)
                n1 >= 1, n1 = power of 2
        2*n2   :data length (int)
                n2 >= 1, n2 = power of 2
        a[0...n1-1][0...2*n2-1]
               :input/output data (float **)
                input data
                    a[j1][2*j2] = Re(x[j1][j2]),
                    a[j1][2*j2+1] = Im(x[j1][j2]),
                    0<=j1<n1, 0<=j2<n2
                output data
                    a[k1][2*k2] = Re(X[k1][k2]),
                    a[k1][2*k2+1] = Im(X[k1][k2]),
                    0<=k1<n1, 0<=k2<n2
        t[0...*]
               :work area (float *)
                length of t >= 8*n1,                   if single thread,
                length of t >= 8*n1*FFT2D_MAX_THREADS, if multi threads,
                t is dynamically allocated, if t == NULL.
        ip[0...*]
               :work area for bit reversal (int *)
                length of ip >= 2+sqrt(n)
                (n = max(n1, n2))
                ip[0],ip[1] are pointers of the cos/sin table.
        w[0...*]
               :cos/sin table (float *)
                length of w >= max(n1/2, n2/2)
                w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of
            cdft2d(n1, 2*n2, -1, a, t, ip, w);
        is
            cdft2d(n1, 2*n2, 1, a, t, ip, w);
            for (j1 = 0; j1 <= n1 - 1; j1++) {
                for (j2 = 0; j2 <= 2 * n2 - 1; j2++) {
                    a[j1][j2] *= 1.0 / n1 / n2;
                }
            }
        .


-------- Real DFT / Inverse of Real DFT --------
    [definition]
        <case1> RDFT
            R[k1][k2] = sum_j1=0^n1-1 sum_j2=0^n2-1 a[j1][j2] *
                            cos(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2),
                            0<=k1<n1, 0<=k2<n2
            I[k1][k2] = sum_j1=0^n1-1 sum_j2=0^n2-1 a[j1][j2] *
                            sin(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2),
                            0<=k1<n1, 0<=k2<n2
        <case2> IRDFT (excluding scale)
            a[k1][k2] = (1/2) * sum_j1=0^n1-1 sum_j2=0^n2-1
                            (R[j1][j2] *
                            cos(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2) +
                            I[j1][j2] *
                            sin(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2)),
                            0<=k1<n1, 0<=k2<n2
        (notes: R[n1-k1][n2-k2] = R[k1][k2],
                I[n1-k1][n2-k2] = -I[k1][k2],
                R[n1-k1][0] = R[k1][0],
                I[n1-k1][0] = -I[k1][0],
                R[0][n2-k2] = R[0][k2],
                I[0][n2-k2] = -I[0][k2],
                0<k1<n1, 0<k2<n2)
    [usage]
        <case1>
            ip[0] = 0; // first time only
            rdft2d(n1, n2, 1, a, t, ip, w);
        <case2>
            ip[0] = 0; // first time only
            rdft2d(n1, n2, -1, a, t, ip, w);
    [parameters]
        n1     :data length (int)
                n1 >= 2, n1 = power of 2
        n2     :data length (int)
                n2 >= 2, n2 = power of 2
        a[0...n1-1][0...n2-1]
               :input/output data (float **)
                <case1>
                    output data
                        a[k1][2*k2] = R[k1][k2] = R[n1-k1][n2-k2],
                        a[k1][2*k2+1] = I[k1][k2] = -I[n1-k1][n2-k2],
                            0<k1<n1, 0<k2<n2/2,
                        a[0][2*k2] = R[0][k2] = R[0][n2-k2],
                        a[0][2*k2+1] = I[0][k2] = -I[0][n2-k2],
                            0<k2<n2/2,
                        a[k1][0] = R[k1][0] = R[n1-k1][0],
                        a[k1][1] = I[k1][0] = -I[n1-k1][0],
                        a[n1-k1][1] = R[k1][n2/2] = R[n1-k1][n2/2],
                        a[n1-k1][0] = -I[k1][n2/2] = I[n1-k1][n2/2],
                            0<k1<n1/2,
                        a[0][0] = R[0][0],
                        a[0][1] = R[0][n2/2],
                        a[n1/2][0] = R[n1/2][0],
                        a[n1/2][1] = R[n1/2][n2/2]
                <case2>
                    input data
                        a[j1][2*j2] = R[j1][j2] = R[n1-j1][n2-j2],
                        a[j1][2*j2+1] = I[j1][j2] = -I[n1-j1][n2-j2],
                            0<j1<n1, 0<j2<n2/2,
                        a[0][2*j2] = R[0][j2] = R[0][n2-j2],
                        a[0][2*j2+1] = I[0][j2] = -I[0][n2-j2],
                            0<j2<n2/2,
                        a[j1][0] = R[j1][0] = R[n1-j1][0],
                        a[j1][1] = I[j1][0] = -I[n1-j1][0],
                        a[n1-j1][1] = R[j1][n2/2] = R[n1-j1][n2/2],
                        a[n1-j1][0] = -I[j1][n2/2] = I[n1-j1][n2/2],
                            0<j1<n1/2,
                        a[0][0] = R[0][0],
                        a[0][1] = R[0][n2/2],
                        a[n1/2][0] = R[n1/2][0],
                        a[n1/2][1] = R[n1/2][n2/2]
                ---- output ordering ----
                    rdft2d(n1, n2, 1, a, t, ip, w);
                    rdft2dsort(n1, n2, 1, a);
                    // stored data is a[0...n1-1][0...n2+1]:
                    // a[k1][2*k2] = R[k1][k2],
                    // a[k1][2*k2+1] = I[k1][k2],
                    // 0<=k1<n1, 0<=k2<=n2/2.
                    // the stored data is larger than the input data!
                ---- input ordering ----
                    rdft2dsort(n1, n2, -1, a);
                    rdft2d(n1, n2, -1, a, t, ip, w);
        t[0...*]
               :work area (float *)
                length of t >= 8*n1,                   if single thread,
                length of t >= 8*n1*FFT2D_MAX_THREADS, if multi threads,
                t is dynamically allocated, if t == NULL.
        ip[0...*]
               :work area for bit reversal (int *)
                length of ip >= 2+sqrt(n)
                (n = max(n1, n2/2))
                ip[0],ip[1] are pointers of the cos/sin table.
        w[0...*]
               :cos/sin table (float *)
                length of w >= max(n1/2, n2/4) + n2/4
                w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of
            rdft2d(n1, n2, 1, a, t, ip, w);
        is
            rdft2d(n1, n2, -1, a, t, ip, w);
            for (j1 = 0; j1 <= n1 - 1; j1++) {
                for (j2 = 0; j2 <= n2 - 1; j2++) {
                    a[j1][j2] *= 2.0 / n1 / n2;
                }
            }
        .
*/

int gettsize2( int n1, int n2 )
{
	int nt = 8 * n1;

	if ( n2 == 4 )
	{
		nt >>= 1;
	}
	else if ( n2 < 4 )
	{
		nt >>= 2;
	}

	return nt;
}

void makewt( int nw, int* ip, float* w );
void makect( int nc, int* ip, float* c );
void cdft( int n, int isgn, float* a, int* ip, float* w );
void rdft( int n, int isgn, float* a, int* ip, float* w );
void cdft2d_sub( int n1, int n2, int isgn, float** a, float* t, int* ip, float* w );
void rdft2d_sub( int n1, int n2, int isgn, float** a );

void cdft2d( int n1, int n2, int isgn, float** a, float* t, int* ip, float* w )
{
	int n, i;

	n = n1 << 1;

	if ( n < n2 )
	{
		n = n2;
	}

	if ( n > ( ip[0] << 2 ) )
	{
		makewt( n >> 2, ip, w );
	}

	for ( i = 0; i < n1; i++ )
	{
		cdft( n2, isgn, a[i], ip, w );
	}

	cdft2d_sub( n1, n2, isgn, a, t, ip, w );
}

void rdft2d( int n1, int n2, int isgn, float** a, float* t, int* ip, float* w )
{
	int n, nw, nc, i;

	n = n1 << 1;

	if ( n < n2 )
	{
		n = n2;
	}

	nw = ip[0];

	if ( n > ( nw << 2 ) )
	{
		nw = n >> 2;
		makewt( nw, ip, w );
	}

	nc = ip[1];

	if ( n2 > ( nc << 2 ) )
	{
		nc = n2 >> 2;
		makect( nc, ip, w + nw );
	}

	if ( isgn < 0 )
	{
		rdft2d_sub( n1, n2, isgn, a );
		cdft2d_sub( n1, n2, isgn, a, t, ip, w );
	}

	for ( i = 0; i < n1; i++ )
	{
		rdft( n2, isgn, a[i], ip, w );
	}

	if ( isgn >= 0 )
	{
		cdft2d_sub( n1, n2, isgn, a, t, ip, w );
		rdft2d_sub( n1, n2, isgn, a );
	}
}


void rdft2dsort( int n1, int n2, int isgn, float** a )
{
	int n1h, i;
	float x, y;

	n1h = n1 >> 1;

	if ( isgn < 0 )
	{
		for ( i = n1h + 1; i < n1; i++ )
		{
			a[i][0] = a[i][n2 + 1];
			a[i][1] = a[i][n2];
		}

		a[0][1] = a[0][n2];
		a[n1h][1] = a[n1h][n2];
	}
	else
	{
		for ( i = n1h + 1; i < n1; i++ )
		{
			y = a[i][0];
			x = a[i][1];
			a[i][n2] = x;
			a[i][n2 + 1] = y;
			a[n1 - i][n2] = x;
			a[n1 - i][n2 + 1] = -y;
			a[i][0] = a[n1 - i][0];
			a[i][1] = -a[n1 - i][1];
		}

		a[0][n2] = a[0][1];
		a[0][n2 + 1] = 0;
		a[0][1] = 0;
		a[n1h][n2] = a[n1h][1];
		a[n1h][n2 + 1] = 0;
		a[n1h][1] = 0;
	}
}

/* -------- child routines -------- */


void cdft2d_sub( int n1, int n2, int isgn, float** a, float* t,
                 int* ip, float* w )
{
	int i, j;

	if ( n2 > 4 )
	{
		for ( j = 0; j < n2; j += 8 )
		{
			for ( i = 0; i < n1; i++ )
			{
				t[2 * i] = a[i][j];
				t[2 * i + 1] = a[i][j + 1];
				t[2 * n1 + 2 * i] = a[i][j + 2];
				t[2 * n1 + 2 * i + 1] = a[i][j + 3];
				t[4 * n1 + 2 * i] = a[i][j + 4];
				t[4 * n1 + 2 * i + 1] = a[i][j + 5];
				t[6 * n1 + 2 * i] = a[i][j + 6];
				t[6 * n1 + 2 * i + 1] = a[i][j + 7];
			}

			cdft( 2 * n1, isgn, t, ip, w );
			cdft( 2 * n1, isgn, &t[2 * n1], ip, w );
			cdft( 2 * n1, isgn, &t[4 * n1], ip, w );
			cdft( 2 * n1, isgn, &t[6 * n1], ip, w );

			for ( i = 0; i < n1; i++ )
			{
				a[i][j] = t[2 * i];
				a[i][j + 1] = t[2 * i + 1];
				a[i][j + 2] = t[2 * n1 + 2 * i];
				a[i][j + 3] = t[2 * n1 + 2 * i + 1];
				a[i][j + 4] = t[4 * n1 + 2 * i];
				a[i][j + 5] = t[4 * n1 + 2 * i + 1];
				a[i][j + 6] = t[6 * n1 + 2 * i];
				a[i][j + 7] = t[6 * n1 + 2 * i + 1];
			}
		}
	}
	else if ( n2 == 4 )
	{
		for ( i = 0; i < n1; i++ )
		{
			t[2 * i] = a[i][0];
			t[2 * i + 1] = a[i][1];
			t[2 * n1 + 2 * i] = a[i][2];
			t[2 * n1 + 2 * i + 1] = a[i][3];
		}

		cdft( 2 * n1, isgn, t, ip, w );
		cdft( 2 * n1, isgn, &t[2 * n1], ip, w );

		for ( i = 0; i < n1; i++ )
		{
			a[i][0] = t[2 * i];
			a[i][1] = t[2 * i + 1];
			a[i][2] = t[2 * n1 + 2 * i];
			a[i][3] = t[2 * n1 + 2 * i + 1];
		}
	}
	else if ( n2 == 2 )
	{
		for ( i = 0; i < n1; i++ )
		{
			t[2 * i] = a[i][0];
			t[2 * i + 1] = a[i][1];
		}

		cdft( 2 * n1, isgn, t, ip, w );

		for ( i = 0; i < n1; i++ )
		{
			a[i][0] = t[2 * i];
			a[i][1] = t[2 * i + 1];
		}
	}
}


void rdft2d_sub( int n1, int n2, int isgn, float** a )
{
	int n1h, i, j;
	float xi;

	( void )n2;

	n1h = n1 >> 1;

	if ( isgn < 0 )
	{
		for ( i = 1; i < n1h; i++ )
		{
			j = n1 - i;
			xi = a[i][0] - a[j][0];
			a[i][0] += a[j][0];
			a[j][0] = xi;
			xi = a[j][1] - a[i][1];
			a[i][1] += a[j][1];
			a[j][1] = xi;
		}
	}
	else
	{
		for ( i = 1; i < n1h; i++ )
		{
			j = n1 - i;
			a[j][0] = 0.5f * ( a[i][0] - a[j][0] );
			a[i][0] -= a[j][0];
			a[j][1] = 0.5f * ( a[i][1] + a[j][1] );
			a[i][1] -= a[j][1];
		}
	}
}


///////////// 3D

/*
Fast Fourier/Cosine/Sine Transform
    dimension   :three
    data length :power of 2
    decimation  :frequency
    radix       :split-radix, row-column
    data        :inplace
    table       :use
functions
    cdft3d: Complex Discrete Fourier Transform
    rdft3d: Real Discrete Fourier Transform
    ddct3d: Discrete Cosine Transform
    ddst3d: Discrete Sine Transform
function prototypes
    void cdft3d(int, int, int, int, float ***, float *, int *, float *);
    void rdft3d(int, int, int, int, float ***, float *, int *, float *);
    void rdft3dsort(int, int, int, int, float ***);
    void ddct3d(int, int, int, int, float ***, float *, int *, float *);
    void ddst3d(int, int, int, int, float ***, float *, int *, float *);
necessary package
    fftsg.c  : 1D-FFT package
macro definitions
    USE_FFT3D_PTHREADS : default=not defined
        FFT3D_MAX_THREADS     : must be 2^N, default=4
        FFT3D_THREADS_BEGIN_N : default=65536
    USE_FFT3D_WINTHREADS : default=not defined
        FFT3D_MAX_THREADS     : must be 2^N, default=4
        FFT3D_THREADS_BEGIN_N : default=131072


-------- Complex DFT (Discrete Fourier Transform) --------
    [definition]
        <case1>
            X[k1][k2][k3] = sum_j1=0^n1-1 sum_j2=0^n2-1 sum_j3=0^n3-1
                                x[j1][j2][j3] *
                                exp(2*pi*i*j1*k1/n1) *
                                exp(2*pi*i*j2*k2/n2) *
                                exp(2*pi*i*j3*k3/n3),
                                0<=k1<n1, 0<=k2<n2, 0<=k3<n3
        <case2>
            X[k1][k2][k3] = sum_j1=0^n1-1 sum_j2=0^n2-1 sum_j3=0^n3-1
                                x[j1][j2][j3] *
                                exp(-2*pi*i*j1*k1/n1) *
                                exp(-2*pi*i*j2*k2/n2) *
                                exp(-2*pi*i*j3*k3/n3),
                                0<=k1<n1, 0<=k2<n2, 0<=k3<n3
        (notes: sum_j=0^n-1 is a summation from j=0 to n-1)
    [usage]
        <case1>
            ip[0] = 0; // first time only
            cdft3d(n1, n2, 2*n3, 1, a, t, ip, w);
        <case2>
            ip[0] = 0; // first time only
            cdft3d(n1, n2, 2*n3, -1, a, t, ip, w);
    [parameters]
        n1     :data length (int)
                n1 >= 1, n1 = power of 2
        n2     :data length (int)
                n2 >= 1, n2 = power of 2
        2*n3   :data length (int)
                n3 >= 1, n3 = power of 2
        a[0...n1-1][0...n2-1][0...2*n3-1]
               :input/output data (float ***)
                input data
                    a[j1][j2][2*j3] = Re(x[j1][j2][j3]),
                    a[j1][j2][2*j3+1] = Im(x[j1][j2][j3]),
                    0<=j1<n1, 0<=j2<n2, 0<=j3<n3
                output data
                    a[k1][k2][2*k3] = Re(X[k1][k2][k3]),
                    a[k1][k2][2*k3+1] = Im(X[k1][k2][k3]),
                    0<=k1<n1, 0<=k2<n2, 0<=k3<n3
        t[0...*]
               :work area (float *)
                length of t >= max(8*n1, 8*n2),        if single thread,
                length of t >= max(8*n1, 8*n2)*FFT3D_MAX_THREADS,
                                                       if multi threads,
                t is dynamically allocated, if t == NULL.
        ip[0...*]
               :work area for bit reversal (int *)
                length of ip >= 2+sqrt(n)
                (n = max(n1, n2, n3))
                ip[0],ip[1] are pointers of the cos/sin table.
        w[0...*]
               :cos/sin table (float *)
                length of w >= max(n1/2, n2/2, n3/2)
                w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of
            cdft3d(n1, n2, 2*n3, -1, a, t, ip, w);
        is
            cdft3d(n1, n2, 2*n3, 1, a, t, ip, w);
            for (j1 = 0; j1 <= n1 - 1; j1++) {
                for (j2 = 0; j2 <= n2 - 1; j2++) {
                    for (j3 = 0; j3 <= 2 * n3 - 1; j3++) {
                        a[j1][j2][j3] *= 1.0 / n1 / n2 / n3;
                    }
                }
            }
        .


-------- Real DFT / Inverse of Real DFT --------
    [definition]
        <case1> RDFT
            R[k1][k2][k3] = sum_j1=0^n1-1 sum_j2=0^n2-1 sum_j3=0^n3-1
                                a[j1][j2][j3] *
                                cos(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2 +
                                    2*pi*j3*k3/n3),
                                0<=k1<n1, 0<=k2<n2, 0<=k3<n3
            I[k1][k2][k3] = sum_j1=0^n1-1 sum_j2=0^n2-1 sum_j3=0^n3-1
                                a[j1][j2][j3] *
                                sin(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2 +
                                    2*pi*j3*k3/n3),
                                0<=k1<n1, 0<=k2<n2, 0<=k3<n3
        <case2> IRDFT (excluding scale)
            a[k1][k2][k3] = (1/2) * sum_j1=0^n1-1 sum_j2=0^n2-1 sum_j3=0^n3-1
                                (R[j1][j2][j3] *
                                cos(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2 +
                                    2*pi*j3*k3/n3) +
                                I[j1][j2][j3] *
                                sin(2*pi*j1*k1/n1 + 2*pi*j2*k2/n2 +
                                    2*pi*j3*k3/n3)),
                                0<=k1<n1, 0<=k2<n2, 0<=k3<n3
        (notes: R[(n1-k1)%n1][(n2-k2)%n2][(n3-k3)%n3] = R[k1][k2][k3],
                I[(n1-k1)%n1][(n2-k2)%n2][(n3-k3)%n3] = -I[k1][k2][k3],
                0<=k1<n1, 0<=k2<n2, 0<=k3<n3)
    [usage]
        <case1>
            ip[0] = 0; // first time only
            rdft3d(n1, n2, n3, 1, a, t, ip, w);
        <case2>
            ip[0] = 0; // first time only
            rdft3d(n1, n2, n3, -1, a, t, ip, w);
    [parameters]
        n1     :data length (int)
                n1 >= 2, n1 = power of 2
        n2     :data length (int)
                n2 >= 2, n2 = power of 2
        n3     :data length (int)
                n3 >= 2, n3 = power of 2
        a[0...n1-1][0...n2-1][0...n3-1]
               :input/output data (float ***)
                <case1>
                    output data
                        a[k1][k2][2*k3] = R[k1][k2][k3]
                                        = R[(n1-k1)%n1][(n2-k2)%n2][n3-k3],
                        a[k1][k2][2*k3+1] = I[k1][k2][k3]
                                          = -I[(n1-k1)%n1][(n2-k2)%n2][n3-k3],
                            0<=k1<n1, 0<=k2<n2, 0<k3<n3/2,
                            (n%m : n mod m),
                        a[k1][k2][0] = R[k1][k2][0]
                                     = R[(n1-k1)%n1][n2-k2][0],
                        a[k1][k2][1] = I[k1][k2][0]
                                     = -I[(n1-k1)%n1][n2-k2][0],
                        a[k1][n2-k2][1] = R[(n1-k1)%n1][k2][n3/2]
                                        = R[k1][n2-k2][n3/2],
                        a[k1][n2-k2][0] = -I[(n1-k1)%n1][k2][n3/2]
                                        = I[k1][n2-k2][n3/2],
                            0<=k1<n1, 0<k2<n2/2,
                        a[k1][0][0] = R[k1][0][0]
                                    = R[n1-k1][0][0],
                        a[k1][0][1] = I[k1][0][0]
                                    = -I[n1-k1][0][0],
                        a[k1][n2/2][0] = R[k1][n2/2][0]
                                       = R[n1-k1][n2/2][0],
                        a[k1][n2/2][1] = I[k1][n2/2][0]
                                       = -I[n1-k1][n2/2][0],
                        a[n1-k1][0][1] = R[k1][0][n3/2]
                                       = R[n1-k1][0][n3/2],
                        a[n1-k1][0][0] = -I[k1][0][n3/2]
                                       = I[n1-k1][0][n3/2],
                        a[n1-k1][n2/2][1] = R[k1][n2/2][n3/2]
                                          = R[n1-k1][n2/2][n3/2],
                        a[n1-k1][n2/2][0] = -I[k1][n2/2][n3/2]
                                          = I[n1-k1][n2/2][n3/2],
                            0<k1<n1/2,
                        a[0][0][0] = R[0][0][0],
                        a[0][0][1] = R[0][0][n3/2],
                        a[0][n2/2][0] = R[0][n2/2][0],
                        a[0][n2/2][1] = R[0][n2/2][n3/2],
                        a[n1/2][0][0] = R[n1/2][0][0],
                        a[n1/2][0][1] = R[n1/2][0][n3/2],
                        a[n1/2][n2/2][0] = R[n1/2][n2/2][0],
                        a[n1/2][n2/2][1] = R[n1/2][n2/2][n3/2]
                <case2>
                    input data
                        a[j1][j2][2*j3] = R[j1][j2][j3]
                                        = R[(n1-j1)%n1][(n2-j2)%n2][n3-j3],
                        a[j1][j2][2*j3+1] = I[j1][j2][j3]
                                          = -I[(n1-j1)%n1][(n2-j2)%n2][n3-j3],
                            0<=j1<n1, 0<=j2<n2, 0<j3<n3/2,
                        a[j1][j2][0] = R[j1][j2][0]
                                     = R[(n1-j1)%n1][n2-j2][0],
                        a[j1][j2][1] = I[j1][j2][0]
                                     = -I[(n1-j1)%n1][n2-j2][0],
                        a[j1][n2-j2][1] = R[(n1-j1)%n1][j2][n3/2]
                                        = R[j1][n2-j2][n3/2],
                        a[j1][n2-j2][0] = -I[(n1-j1)%n1][j2][n3/2]
                                        = I[j1][n2-j2][n3/2],
                            0<=j1<n1, 0<j2<n2/2,
                        a[j1][0][0] = R[j1][0][0]
                                    = R[n1-j1][0][0],
                        a[j1][0][1] = I[j1][0][0]
                                    = -I[n1-j1][0][0],
                        a[j1][n2/2][0] = R[j1][n2/2][0]
                                       = R[n1-j1][n2/2][0],
                        a[j1][n2/2][1] = I[j1][n2/2][0]
                                       = -I[n1-j1][n2/2][0],
                        a[n1-j1][0][1] = R[j1][0][n3/2]
                                       = R[n1-j1][0][n3/2],
                        a[n1-j1][0][0] = -I[j1][0][n3/2]
                                       = I[n1-j1][0][n3/2],
                        a[n1-j1][n2/2][1] = R[j1][n2/2][n3/2]
                                          = R[n1-j1][n2/2][n3/2],
                        a[n1-j1][n2/2][0] = -I[j1][n2/2][n3/2]
                                          = I[n1-j1][n2/2][n3/2],
                            0<j1<n1/2,
                        a[0][0][0] = R[0][0][0],
                        a[0][0][1] = R[0][0][n3/2],
                        a[0][n2/2][0] = R[0][n2/2][0],
                        a[0][n2/2][1] = R[0][n2/2][n3/2],
                        a[n1/2][0][0] = R[n1/2][0][0],
                        a[n1/2][0][1] = R[n1/2][0][n3/2],
                        a[n1/2][n2/2][0] = R[n1/2][n2/2][0],
                        a[n1/2][n2/2][1] = R[n1/2][n2/2][n3/2]
                ---- output ordering ----
                    rdft3d(n1, n2, n3, 1, a, t, ip, w);
                    rdft3dsort(n1, n2, n3, 1, a);
                    // stored data is a[0...n1-1][0...n2-1][0...n3+1]:
                    // a[k1][k2][2*k3] = R[k1][k2][k3],
                    // a[k1][k2][2*k3+1] = I[k1][k2][k3],
                    // 0<=k1<n1, 0<=k2<n2, 0<=k3<=n3/2.
                    // the stored data is larger than the input data!
                ---- input ordering ----
                    rdft3dsort(n1, n2, n3, -1, a);
                    rdft3d(n1, n2, n3, -1, a, t, ip, w);
        t[0...*]
               :work area (float *)
                length of t >= max(8*n1, 8*n2),        if single thread,
                length of t >= max(8*n1, 8*n2)*FFT3D_MAX_THREADS,
                                                       if multi threads,
                t is dynamically allocated, if t == NULL.
        ip[0...*]
               :work area for bit reversal (int *)
                length of ip >= 2+sqrt(n)
                (n = max(n1, n2, n3/2))
                ip[0],ip[1] are pointers of the cos/sin table.
        w[0...*]
               :cos/sin table (float *)
                length of w >= max(n1/2, n2/2, n3/4) + n3/4
                w[],ip[] are initialized if ip[0] == 0.
    [remark]
        Inverse of
            rdft3d(n1, n2, n3, 1, a, t, ip, w);
        is
            rdft3d(n1, n2, n3, -1, a, t, ip, w);
            for (j1 = 0; j1 <= n1 - 1; j1++) {
                for (j2 = 0; j2 <= n2 - 1; j2++) {
                    for (j3 = 0; j3 <= n3 - 1; j3++) {
                        a[j1][j2][j3] *= 2.0 / n1 / n2 / n3;
                    }
                }
            }
        .
*/

void makewt( int nw, int* ip, float* w );
void makect( int nc, int* ip, float* c );
void cdft( int n, int isgn, float* a, int* ip, float* w );
void rdft( int n, int isgn, float* a, int* ip, float* w );
void xdft3da_sub( int n1, int n2, int n3, int icr, int isgn, float** *a, float* t, int* ip, float* w );
void cdft3db_sub( int n1, int n2, int n3, int isgn, float** *a, float* t, int* ip, float* w );
void rdft3d_sub( int n1, int n2, int n3, int isgn, float** *a );

int gettsize3( int n1, int n2, int n3 )
{
	int nt = n1;

	if ( nt < n2 )
	{
		nt = n2;
	}

	nt *= 8;

	if ( n3 == 4 )
	{
		nt >>= 1;
	}
	else if ( n3 < 4 )
	{
		nt >>= 2;
	}

	return nt;
}

void cdft3d( int n1, int n2, int n3, int isgn, float** *a, float* t, int* ip, float* w )
{
	int n = n1;

	if ( n < n2 ) { n = n2; }

	n <<= 1;

	if ( n < n3 ) { n = n3; }

	if ( n > ( ip[0] << 2 ) )
	{
		makewt( n >> 2, ip, w );
	}

	xdft3da_sub( n1, n2, n3, 0, isgn, a, t, ip, w );
	cdft3db_sub( n1, n2, n3, isgn, a, t, ip, w );
}

void rdft3d( int n1, int n2, int n3, int isgn, float** *a, float* t, int* ip, float* w )
{
	int nw, nc;

	int n = n1;

	if ( n < n2 ) { n = n2; }

	n <<= 1;

	if ( n < n3 ) { n = n3; }

	nw = ip[0];

	if ( n > ( nw << 2 ) )
	{
		nw = n >> 2;
		makewt( nw, ip, w );
	}

	nc = ip[1];

	if ( n3 > ( nc << 2 ) )
	{
		nc = n3 >> 2;
		makect( nc, ip, w + nw );
	}

	if ( isgn < 0 )
	{
		rdft3d_sub( n1, n2, n3, isgn, a );
		cdft3db_sub( n1, n2, n3, isgn, a, t, ip, w );
	}

	xdft3da_sub( n1, n2, n3, 1, isgn, a, t, ip, w );

	if ( isgn >= 0 )
	{
		cdft3db_sub( n1, n2, n3, isgn, a, t, ip, w );
		rdft3d_sub( n1, n2, n3, isgn, a );
	}
}

void rdft3dsort( int n1, int n2, int n3, int isgn, float** *a )
{
	int n1h, n2h, i, j;
	float x, y;

	n1h = n1 >> 1;
	n2h = n2 >> 1;

	if ( isgn < 0 )
	{
		for ( i = 0; i < n1; i++ )
		{
			for ( j = n2h + 1; j < n2; j++ )
			{
				a[i][j][0] = a[i][j][n3 + 1];
				a[i][j][1] = a[i][j][n3];
			}
		}

		for ( i = n1h + 1; i < n1; i++ )
		{
			a[i][0][0] = a[i][0][n3 + 1];
			a[i][0][1] = a[i][0][n3];
			a[i][n2h][0] = a[i][n2h][n3 + 1];
			a[i][n2h][1] = a[i][n2h][n3];
		}

		a[0][0][1] = a[0][0][n3];
		a[0][n2h][1] = a[0][n2h][n3];
		a[n1h][0][1] = a[n1h][0][n3];
		a[n1h][n2h][1] = a[n1h][n2h][n3];
	}
	else
	{
		for ( j = n2h + 1; j < n2; j++ )
		{
			y = a[0][j][0];
			x = a[0][j][1];
			a[0][j][n3] = x;
			a[0][j][n3 + 1] = y;
			a[0][n2 - j][n3] = x;
			a[0][n2 - j][n3 + 1] = -y;
			a[0][j][0] = a[0][n2 - j][0];
			a[0][j][1] = -a[0][n2 - j][1];
		}

		for ( i = 1; i < n1; i++ )
		{
			for ( j = n2h + 1; j < n2; j++ )
			{
				y = a[i][j][0];
				x = a[i][j][1];
				a[i][j][n3] = x;
				a[i][j][n3 + 1] = y;
				a[n1 - i][n2 - j][n3] = x;
				a[n1 - i][n2 - j][n3 + 1] = -y;
				a[i][j][0] = a[n1 - i][n2 - j][0];
				a[i][j][1] = -a[n1 - i][n2 - j][1];
			}
		}

		for ( i = n1h + 1; i < n1; i++ )
		{
			y = a[i][0][0];
			x = a[i][0][1];
			a[i][0][n3] = x;
			a[i][0][n3 + 1] = y;
			a[n1 - i][0][n3] = x;
			a[n1 - i][0][n3 + 1] = -y;
			a[i][0][0] = a[n1 - i][0][0];
			a[i][0][1] = -a[n1 - i][0][1];
			y = a[i][n2h][0];
			x = a[i][n2h][1];
			a[i][n2h][n3] = x;
			a[i][n2h][n3 + 1] = y;
			a[n1 - i][n2h][n3] = x;
			a[n1 - i][n2h][n3 + 1] = -y;
			a[i][n2h][0] = a[n1 - i][n2h][0];
			a[i][n2h][1] = -a[n1 - i][n2h][1];
		}

		a[0][0][n3] = a[0][0][1];
		a[0][0][n3 + 1] = 0;
		a[0][0][1] = 0;
		a[0][n2h][n3] = a[0][n2h][1];
		a[0][n2h][n3 + 1] = 0;
		a[0][n2h][1] = 0;
		a[n1h][0][n3] = a[n1h][0][1];
		a[n1h][0][n3 + 1] = 0;
		a[n1h][0][1] = 0;
		a[n1h][n2h][n3] = a[n1h][n2h][1];
		a[n1h][n2h][n3 + 1] = 0;
		a[n1h][n2h][1] = 0;
	}
}

/* -------- child routines -------- */


void xdft3da_sub( int n1, int n2, int n3, int icr, int isgn, float** *a, float* t, int* ip, float* w )
{
	int i, j, k;

	for ( i = 0; i < n1; i++ )
	{
		if ( icr == 0 )
		{
			for ( j = 0; j < n2; j++ )
			{
				cdft( n3, isgn, a[i][j], ip, w );
			}
		}
		else if ( isgn >= 0 )
		{
			for ( j = 0; j < n2; j++ )
			{
				rdft( n3, isgn, a[i][j], ip, w );
			}
		}

		if ( n3 > 4 )
		{
			for ( k = 0; k < n3; k += 8 )
			{
				for ( j = 0; j < n2; j++ )
				{
					t[2 * j] = a[i][j][k];
					t[2 * j + 1] = a[i][j][k + 1];
					t[2 * n2 + 2 * j] = a[i][j][k + 2];
					t[2 * n2 + 2 * j + 1] = a[i][j][k + 3];
					t[4 * n2 + 2 * j] = a[i][j][k + 4];
					t[4 * n2 + 2 * j + 1] = a[i][j][k + 5];
					t[6 * n2 + 2 * j] = a[i][j][k + 6];
					t[6 * n2 + 2 * j + 1] = a[i][j][k + 7];
				}

				cdft( 2 * n2, isgn, t, ip, w );
				cdft( 2 * n2, isgn, &t[2 * n2], ip, w );
				cdft( 2 * n2, isgn, &t[4 * n2], ip, w );
				cdft( 2 * n2, isgn, &t[6 * n2], ip, w );

				for ( j = 0; j < n2; j++ )
				{
					a[i][j][k] = t[2 * j];
					a[i][j][k + 1] = t[2 * j + 1];
					a[i][j][k + 2] = t[2 * n2 + 2 * j];
					a[i][j][k + 3] = t[2 * n2 + 2 * j + 1];
					a[i][j][k + 4] = t[4 * n2 + 2 * j];
					a[i][j][k + 5] = t[4 * n2 + 2 * j + 1];
					a[i][j][k + 6] = t[6 * n2 + 2 * j];
					a[i][j][k + 7] = t[6 * n2 + 2 * j + 1];
				}
			}
		}
		else if ( n3 == 4 )
		{
			for ( j = 0; j < n2; j++ )
			{
				t[2 * j] = a[i][j][0];
				t[2 * j + 1] = a[i][j][1];
				t[2 * n2 + 2 * j] = a[i][j][2];
				t[2 * n2 + 2 * j + 1] = a[i][j][3];
			}

			cdft( 2 * n2, isgn, t, ip, w );
			cdft( 2 * n2, isgn, &t[2 * n2], ip, w );

			for ( j = 0; j < n2; j++ )
			{
				a[i][j][0] = t[2 * j];
				a[i][j][1] = t[2 * j + 1];
				a[i][j][2] = t[2 * n2 + 2 * j];
				a[i][j][3] = t[2 * n2 + 2 * j + 1];
			}
		}
		else if ( n3 == 2 )
		{
			for ( j = 0; j < n2; j++ )
			{
				t[2 * j] = a[i][j][0];
				t[2 * j + 1] = a[i][j][1];
			}

			cdft( 2 * n2, isgn, t, ip, w );

			for ( j = 0; j < n2; j++ )
			{
				a[i][j][0] = t[2 * j];
				a[i][j][1] = t[2 * j + 1];
			}
		}

		if ( icr != 0 && isgn < 0 )
		{
			for ( j = 0; j < n2; j++ )
			{
				rdft( n3, isgn, a[i][j], ip, w );
			}
		}
	}
}


void cdft3db_sub( int n1, int n2, int n3, int isgn, float** *a, float* t, int* ip, float* w )
{
	int i, j, k;

	if ( n3 > 4 )
	{
		for ( j = 0; j < n2; j++ )
		{
			for ( k = 0; k < n3; k += 8 )
			{
				for ( i = 0; i < n1; i++ )
				{
					t[2 * i] = a[i][j][k];
					t[2 * i + 1] = a[i][j][k + 1];
					t[2 * n1 + 2 * i] = a[i][j][k + 2];
					t[2 * n1 + 2 * i + 1] = a[i][j][k + 3];
					t[4 * n1 + 2 * i] = a[i][j][k + 4];
					t[4 * n1 + 2 * i + 1] = a[i][j][k + 5];
					t[6 * n1 + 2 * i] = a[i][j][k + 6];
					t[6 * n1 + 2 * i + 1] = a[i][j][k + 7];
				}

				cdft( 2 * n1, isgn, t, ip, w );
				cdft( 2 * n1, isgn, &t[2 * n1], ip, w );
				cdft( 2 * n1, isgn, &t[4 * n1], ip, w );
				cdft( 2 * n1, isgn, &t[6 * n1], ip, w );

				for ( i = 0; i < n1; i++ )
				{
					a[i][j][k] = t[2 * i];
					a[i][j][k + 1] = t[2 * i + 1];
					a[i][j][k + 2] = t[2 * n1 + 2 * i];
					a[i][j][k + 3] = t[2 * n1 + 2 * i + 1];
					a[i][j][k + 4] = t[4 * n1 + 2 * i];
					a[i][j][k + 5] = t[4 * n1 + 2 * i + 1];
					a[i][j][k + 6] = t[6 * n1 + 2 * i];
					a[i][j][k + 7] = t[6 * n1 + 2 * i + 1];
				}
			}
		}
	}
	else if ( n3 == 4 )
	{
		for ( j = 0; j < n2; j++ )
		{
			for ( i = 0; i < n1; i++ )
			{
				t[2 * i] = a[i][j][0];
				t[2 * i + 1] = a[i][j][1];
				t[2 * n1 + 2 * i] = a[i][j][2];
				t[2 * n1 + 2 * i + 1] = a[i][j][3];
			}

			cdft( 2 * n1, isgn, t, ip, w );
			cdft( 2 * n1, isgn, &t[2 * n1], ip, w );

			for ( i = 0; i < n1; i++ )
			{
				a[i][j][0] = t[2 * i];
				a[i][j][1] = t[2 * i + 1];
				a[i][j][2] = t[2 * n1 + 2 * i];
				a[i][j][3] = t[2 * n1 + 2 * i + 1];
			}
		}
	}
	else if ( n3 == 2 )
	{
		for ( j = 0; j < n2; j++ )
		{
			for ( i = 0; i < n1; i++ )
			{
				t[2 * i] = a[i][j][0];
				t[2 * i + 1] = a[i][j][1];
			}

			cdft( 2 * n1, isgn, t, ip, w );

			for ( i = 0; i < n1; i++ )
			{
				a[i][j][0] = t[2 * i];
				a[i][j][1] = t[2 * i + 1];
			}
		}
	}
}

void rdft3d_sub( int n1, int n2, int n3, int isgn, float** *a )
{
	int n1h, n2h, i, j, k, l;
	float xi;

	( void )n3;

	n1h = n1 >> 1;
	n2h = n2 >> 1;

	if ( isgn < 0 )
	{
		for ( i = 1; i < n1h; i++ )
		{
			j = n1 - i;
			xi = a[i][0][0] - a[j][0][0];
			a[i][0][0] += a[j][0][0];
			a[j][0][0] = xi;
			xi = a[j][0][1] - a[i][0][1];
			a[i][0][1] += a[j][0][1];
			a[j][0][1] = xi;
			xi = a[i][n2h][0] - a[j][n2h][0];
			a[i][n2h][0] += a[j][n2h][0];
			a[j][n2h][0] = xi;
			xi = a[j][n2h][1] - a[i][n2h][1];
			a[i][n2h][1] += a[j][n2h][1];
			a[j][n2h][1] = xi;

			for ( k = 1; k < n2h; k++ )
			{
				l = n2 - k;
				xi = a[i][k][0] - a[j][l][0];
				a[i][k][0] += a[j][l][0];
				a[j][l][0] = xi;
				xi = a[j][l][1] - a[i][k][1];
				a[i][k][1] += a[j][l][1];
				a[j][l][1] = xi;
				xi = a[j][k][0] - a[i][l][0];
				a[j][k][0] += a[i][l][0];
				a[i][l][0] = xi;
				xi = a[i][l][1] - a[j][k][1];
				a[j][k][1] += a[i][l][1];
				a[i][l][1] = xi;
			}
		}

		for ( k = 1; k < n2h; k++ )
		{
			l = n2 - k;
			xi = a[0][k][0] - a[0][l][0];
			a[0][k][0] += a[0][l][0];
			a[0][l][0] = xi;
			xi = a[0][l][1] - a[0][k][1];
			a[0][k][1] += a[0][l][1];
			a[0][l][1] = xi;
			xi = a[n1h][k][0] - a[n1h][l][0];
			a[n1h][k][0] += a[n1h][l][0];
			a[n1h][l][0] = xi;
			xi = a[n1h][l][1] - a[n1h][k][1];
			a[n1h][k][1] += a[n1h][l][1];
			a[n1h][l][1] = xi;
		}
	}
	else
	{
		for ( i = 1; i < n1h; i++ )
		{
			j = n1 - i;
			a[j][0][0] = 0.5f * ( a[i][0][0] - a[j][0][0] );
			a[i][0][0] -= a[j][0][0];
			a[j][0][1] = 0.5f * ( a[i][0][1] + a[j][0][1] );
			a[i][0][1] -= a[j][0][1];
			a[j][n2h][0] = 0.5f * ( a[i][n2h][0] - a[j][n2h][0] );
			a[i][n2h][0] -= a[j][n2h][0];
			a[j][n2h][1] = 0.5f * ( a[i][n2h][1] + a[j][n2h][1] );
			a[i][n2h][1] -= a[j][n2h][1];

			for ( k = 1; k < n2h; k++ )
			{
				l = n2 - k;
				a[j][l][0] = 0.5f * ( a[i][k][0] - a[j][l][0] );
				a[i][k][0] -= a[j][l][0];
				a[j][l][1] = 0.5f * ( a[i][k][1] + a[j][l][1] );
				a[i][k][1] -= a[j][l][1];
				a[i][l][0] = 0.5f * ( a[j][k][0] - a[i][l][0] );
				a[j][k][0] -= a[i][l][0];
				a[i][l][1] = 0.5f * ( a[j][k][1] + a[i][l][1] );
				a[j][k][1] -= a[i][l][1];
			}
		}

		for ( k = 1; k < n2h; k++ )
		{
			l = n2 - k;
			a[0][l][0] = 0.5f * ( a[0][k][0] - a[0][l][0] );
			a[0][k][0] -= a[0][l][0];
			a[0][l][1] = 0.5f * ( a[0][k][1] + a[0][l][1] );
			a[0][k][1] -= a[0][l][1];
			a[n1h][l][0] = 0.5f * ( a[n1h][k][0] - a[n1h][l][0] );
			a[n1h][k][0] -= a[n1h][l][0];
			a[n1h][l][1] = 0.5f * ( a[n1h][k][1] + a[n1h][l][1] );
			a[n1h][k][1] -= a[n1h][l][1];
		}
	}
}
