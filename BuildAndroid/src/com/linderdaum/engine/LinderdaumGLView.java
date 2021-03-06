/**
 * \file LinderdaumEngineActivity.java
 * \brief Linderdaum Engine OpenGL ES 2.0 view for Android
 * \version 0.5.99c
 * \date 04/05/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

package com.linderdaum.engine;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.app.ProgressDialog;
import android.os.Looper;
import android.os.Environment;

import java.util.Locale;
import java.io.*;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class LinderdaumGLView extends GLSurfaceView
{
	public static final String TAG = "LinderdaumGLView";

	private static Context local_context = null;

	public LinderdaumGLView( Context context )
	{
		super( context );
		local_context = context;

		SetLocale();

		init( false, 16, 0 );
	}

	private static void SetLocale()
	{
		String Lang    = Locale.getDefault().getLanguage();

		Log.e( TAG, "Locale: " + Lang );
		//String Country = Locale.getDefault().getCountry();

		LinderdaumJNILib.SetLocaleName( Lang );
	}

	/*

	    public void ShowLoadingDialog()
	    {
	        LoadingDialog = new ProgressDialog(this);
	        LoadingDialog.setMessage("Loading. Please wait...");
	        LoadingDialog.setIndeterminate(true);
	        LoadingDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
	        LoadingDialog.show();
	    }
	*/

	private void init( boolean translucent, int depth, int stencil )
	{
		/* By default, GLSurfaceView() creates a RGB_565 opaque surface.
		 * If we want a translucent one, we should change the surface's
		 * format here, using PixelFormat.TRANSLUCENT for GL Surfaces
		 * is interpreted as any 32-bit surface with alpha by SurfaceFlinger.
		 */
		this.getHolder().setFormat( PixelFormat.RGB_565 );
		if ( translucent )
		{
			this.getHolder().setFormat( PixelFormat.TRANSLUCENT );
		}

		setEGLContextFactory( new ContextFactory() );

		/* We need to choose an EGLConfig that matches the format of
		 * our surface exactly. This is going to be done in our
		 * custom config chooser. See ConfigChooser class definition
		 * below.
		 */
		setEGLConfigChooser( translucent ?
		                     new ConfigChooser( 8, 8, 8, 8, depth, stencil ) :
		                     new ConfigChooser( 5, 6, 5, 0, depth, stencil ) );

		setRenderer( new Renderer() );
	}

	private static class ContextFactory implements GLSurfaceView.EGLContextFactory
	{
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
		public EGLContext createContext( EGL10 egl, EGLDisplay display, EGLConfig eglConfig )
		{
			Log.w( TAG, "creating OpenGL ES 2.0 context" );
			checkEglError( "Before eglCreateContext", egl );
			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext( display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list );
			checkEglError( "After eglCreateContext", egl );
			return context;
		}

		public void destroyContext( EGL10 egl, EGLDisplay display, EGLContext context )
		{
			egl.eglDestroyContext( display, context );
		}
	}

	private static void checkEglError( String prompt, EGL10 egl )
	{
		int error;

		while ( ( error = egl.eglGetError() ) != EGL10.EGL_SUCCESS )
		{
			Log.e( TAG, String.format( "%s: EGL error: 0x%x", prompt, error ) );
		}
	}

	private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
	{

		public ConfigChooser( int r, int g, int b, int a, int depth, int stencil )
		{
			mRedSize = r;
			mGreenSize = g;
			mBlueSize = b;
			mAlphaSize = a;
			mDepthSize = depth;
			mStencilSize = stencil;
		}

		/* This EGL config specification is used to specify 2.0 rendering.
		 * We use a minimum size of 4 bits for red/green/blue, but will
		 * perform actual matching in chooseConfig() below.
		 */
		private static int EGL_OPENGL_ES2_BIT = 4;
		private static int[] s_configAttribs2 =
		{
//			EGL10.EGL_RED_SIZE, 4,
//			EGL10.EGL_GREEN_SIZE, 4,
//			EGL10.EGL_BLUE_SIZE, 4,
//			EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//			EGL10.EGL_NONE
			EGL10.EGL_RED_SIZE, 5,
			EGL10.EGL_GREEN_SIZE, 6,
			EGL10.EGL_BLUE_SIZE, 5,
			EGL10.EGL_ALPHA_SIZE, 0,
			EGL10.EGL_DEPTH_SIZE, 16,
			EGL10.EGL_STENCIL_SIZE, 0,
			EGL10.EGL_SAMPLE_BUFFERS, 0,
			EGL10.EGL_SAMPLES, 0,
			EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL10.EGL_NONE, EGL10.EGL_NONE
		};

		public EGLConfig chooseConfig( EGL10 egl, EGLDisplay display )
		{
			/* Get the number of minimally matching EGL configurations
			 */
			int[] num_config = new int[1];
			egl.eglChooseConfig( display, s_configAttribs2, null, 0, num_config );

			int numConfigs = num_config[0];

			if ( numConfigs <= 0 )
			{
				throw new IllegalArgumentException( "No configs match configSpec" );
			}

			// Allocate then read the array of minimally matching EGL configs
			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig( display, s_configAttribs2, configs, numConfigs, num_config );

			// Now return the "best" one
			return chooseConfig( egl, display, configs );
		}

		public EGLConfig chooseConfig( EGL10 egl, EGLDisplay display,
		                               EGLConfig[] configs )
		{
			for ( EGLConfig config : configs )
			{
				int d = findConfigAttrib( egl, display, config, EGL10.EGL_DEPTH_SIZE,   0 );
				int s = findConfigAttrib( egl, display, config, EGL10.EGL_STENCIL_SIZE, 0 );

				// We need at least mDepthSize and mStencilSize bits
				if ( d < mDepthSize || s < mStencilSize )
				{
					continue;
				}

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib( egl, display, config, EGL10.EGL_RED_SIZE,   0 );
				int g = findConfigAttrib( egl, display, config, EGL10.EGL_GREEN_SIZE, 0 );
				int b = findConfigAttrib( egl, display, config, EGL10.EGL_BLUE_SIZE,  0 );
				int a = findConfigAttrib( egl, display, config, EGL10.EGL_ALPHA_SIZE, 0 );

				if ( r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize )
				{
					return config;
				}
			}

			return null;
		}

		private int findConfigAttrib( EGL10 egl, EGLDisplay display,
		                              EGLConfig config, int attribute, int defaultValue )
		{

			if ( egl.eglGetConfigAttrib( display, config, attribute, mValue ) )
			{
				return mValue[0];
			}

			return defaultValue;
		}

		// Subclasses can adjust these values:
		protected int mRedSize;
		protected int mGreenSize;
		protected int mBlueSize;
		protected int mAlphaSize;
		protected int mDepthSize;
		protected int mStencilSize;
		private int[] mValue = new int[1];
	}

	private static class Renderer implements GLSurfaceView.Renderer
	{
		public void onDrawFrame( GL10 gl )
		{
			LinderdaumJNILib.step();
		}

		public void onSurfaceChanged( GL10 gl, int width, int height )
		{
			DisplayMetrics dm = new DisplayMetrics();
			LinderdaumEngineActivity.m_LEngine.getWindowManager().getDefaultDisplay().getMetrics(dm);

			LinderdaumJNILib.SurfaceChanged( width, height, (int)dm.xdpi, (int)dm.xdpi );
		}

		public void onSurfaceCreated( GL10 gl, EGLConfig config )
		{
			LinderdaumJNILib.SurfaceCreated();
		}
	}
}
