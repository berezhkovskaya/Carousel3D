/**
 * \file LinderdaumEngineActivity.java
 * \brief Linderdaum Engine activity for Android
 * \version 0.5.99c
 * \date 04/05/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

package com.linderdaum.engine;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Bundle;
import android.os.Environment;
import android.os.Vibrator;
import android.database.Cursor;
import android.util.Log;
import android.net.Uri;
import android.view.View;
import android.view.Display;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.provider.Settings.Secure;
import android.widget.FrameLayout;
import android.provider.MediaStore.Images.Media;

import com.android.vending.licensing.LicenseChecker;
import com.android.vending.licensing.LicenseCheckerCallback;
import com.android.vending.licensing.ServerManagedPolicy;
import com.android.vending.licensing.AESObfuscator;

// bringing in some ads
import com.google.ads.Ad;
import com.google.ads.AdRequest;
import com.google.ads.AdListener;
import com.google.ads.AdSize;
import com.google.ads.AdView;

import java.io.*;

public class LinderdaumEngineActivity extends Activity implements AdListener
{
	public static final String TAG = "LinderdaumEngineActivity";

	public static LinderdaumEngineActivity m_LEngine = null;
	public LinderdaumSensors m_Sensors = null;
	public LinderdaumGLView m_View = null;
	public Vibrator m_Vibrator = null;
	public static LinderdaumDeployer m_Deployer = null;
	public static ProgressDialog LoadingDialog = null;
	public static String m_FilesDir;
	public AdView m_AdView = null;
	private String m_ExternalStoragePrefix;
	private boolean m_UnpackResources = false;

	private BroadcastReceiver m_ScreenReceiver = null;

	public String GetExternalStoragePrefix()
	{
		return m_ExternalStoragePrefix;
//		return Environment.getExternalStorageDirectory().getPath() + "/external_sd/Android/data/" + getApplication().getPackageName();
//		return Environment.getDownloadCacheDirectory().getPath() + "/" + getApplication().getPackageName();
	}

	protected void SetUnpackResources()
	{
		m_UnpackResources = true;
	}

	private class RendererSpawner implements Runnable
	{
		public void run()
		{
			String ExternalCacheDir = LinderdaumEngineActivity.m_LEngine.GetExternalStoragePrefix() + "/cache";

			LinderdaumJNILib.SetDirs( ExternalCacheDir, LinderdaumEngineActivity.m_FilesDir );
			LinderdaumJNILib.SetAPKName( LinderdaumEngineActivity.m_LEngine.getApplicationInfo().sourceDir );

			m_Sensors = new LinderdaumSensors();
			m_Vibrator = (Vibrator)getSystemService( Context.VIBRATOR_SERVICE );
			m_LEngine.m_View = new LinderdaumGLView( getApplication() );
			m_LEngine.setContentView( m_LEngine.Deployed() );

			LoadingDialog.dismiss();
			LoadingDialog = null;
		}
	}

	public class ScreenReceiver extends BroadcastReceiver
	{
		public boolean wasScreenOn = true;
	 
		@Override
		public void onReceive(Context context, Intent intent)
		{
			if ( intent.getAction().equals(Intent.ACTION_SCREEN_OFF) )
			{
				Log.e( TAG, "ACTION_SCREEN_OFF" );

				wasScreenOn = false;
			}
			else if ( intent.getAction().equals(Intent.ACTION_SCREEN_ON) )
			{
				Log.e( TAG, "ACTION_SCREEN_ON" );

				wasScreenOn = true;
			}
		}
	}

/////// AdMob ////////
	@Override
	public void onDismissScreen(Ad ad)
	{
		Log.e( TAG, "onDismissScreen" );
	}
	@Override
	public void onFailedToReceiveAd(Ad ad, AdRequest.ErrorCode error)
	{
		Log.e( TAG, "onFailedToReceiveAd" );
	}
	@Override
	public void onLeaveApplication(Ad ad) 
	{
		Log.e( TAG, "onLeaveApplication" );
	}
	@Override
	public void onPresentScreen(Ad ad)
	{
		Log.e( TAG, "onPresentScreen" );
	}
	@Override
	public void onReceiveAd(Ad ad)
	{
		Log.e( TAG, "onReceiveAd" );
	}
////////////////////

	private class DeployThread extends Thread
	{
		public void run()
		{
			if ( m_UnpackResources )
			{
				m_Deployer.CopyAssets( m_FilesDir, "CommonMedia.tar" );
				m_Deployer.CopyAssets( m_FilesDir, "Data" );
			}

			m_LEngine.runOnUiThread( new RendererSpawner() );
		}
	}

	@Override protected void onCreate( Bundle icicle )
	{
		super.onCreate( icicle );

		getWindow().addFlags( WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN );
		getWindow().addFlags( WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS );
		getWindow().addFlags( WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON   );

		ShowProgressDialog();

		m_ExternalStoragePrefix = Environment.getExternalStorageDirectory().getPath() + "/external_sd/Android/data/" + getApplication().getPackageName();

		String state = Environment.getExternalStorageState();

		if ( !Environment.MEDIA_MOUNTED.equals( state ) || Environment.MEDIA_MOUNTED_READ_ONLY.equals( state ) )
		{
//			ShowErrorDialog( "Unable to access SD card. Mount SD card and retry." );
//			m_ExternalStoragePrefix = Environment.getDownloadCacheDirectory().getPath() + "/" + getApplication().getPackageName();
//			m_ExternalStoragePrefix = this.getFilesDir().getPath() + "/" + getApplication().getPackageName();
			m_ExternalStoragePrefix = this.getDir( getApplication().getPackageName(), MODE_PRIVATE ).getPath();
		}

		m_LEngine  = this;
		m_FilesDir = GetExternalStoragePrefix() + "/files";

		new File( GetExternalStoragePrefix() + "/cache" ).mkdirs();
		new File( GetExternalStoragePrefix() + "/files" ).mkdirs();

		try
		{
			ComponentName CompName = new ComponentName( this, this.getClass() );
			Signature Signatures[] = this.getPackageManager().getPackageInfo( CompName.getPackageName(), PackageManager.GET_SIGNATURES ).signatures;
			LinderdaumJNILib.SetKey( Signatures[0].toCharsString() );
		}
		catch (android.content.pm.PackageManager.NameNotFoundException e)
		{
		}

		// initialize receiver
		IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);
		filter.addAction( Intent.ACTION_SCREEN_OFF );
		m_ScreenReceiver = new ScreenReceiver();
		registerReceiver( m_ScreenReceiver, filter );
		//

		Intent CommandLine = this.getIntent();
		
		Uri uri = CommandLine.getData();

		LinderdaumJNILib.SetCommandLine( (uri != null) ? getPath( uri ) : "" );

		m_Deployer = new LinderdaumDeployer( getApplication() );

		new DeployThread().start();
	}

	protected View Deployed()
	{
		return m_View;
	}

	@Override protected void onDestroy()
	{
		super.onDestroy();

		unregisterReceiver( m_ScreenReceiver );
	}

	private void ShowProgressDialog()
	{
		LoadingDialog = new ProgressDialog( this );
		LoadingDialog.setMessage( "Loading. Please wait..." );
		LoadingDialog.setIndeterminate( true );
		LoadingDialog.setProgressStyle( ProgressDialog.STYLE_SPINNER );
		LoadingDialog.setCancelable( false );
		LoadingDialog.show();
	}

	public void ShowErrorDialog( String Message )
	{
		AlertDialog.Builder builder = new AlertDialog.Builder( this );
		builder.setMessage( Message );
		builder.setNegativeButton( "OK", new DialogInterface.OnClickListener()
		{
			@Override
			public void onClick( DialogInterface arg0, int arg1 )
			{
				finish();
			}
		} );
		builder.setCancelable( false );
		AlertDialog alertDialog = builder.create();
		alertDialog.show();
	}

	public void ShowMessageDialog( String Message )
	{
		AlertDialog.Builder builder = new AlertDialog.Builder( this );
		builder.setMessage( Message );
		builder.setNegativeButton( "OK", null );
		builder.setCancelable( true );
		AlertDialog alertDialog = builder.create();
		alertDialog.show();
	}

	private final int L_MOTION_MOVE = 0;
	private final int L_MOTION_UP   = 1;
	private final int L_MOTION_DOWN = 2;

	@Override public boolean onTouchEvent( MotionEvent event )
	{
		// drop everything
		SendMotion( -1, 0, 0, false, L_MOTION_MOVE );

		int nEvent     = event.getAction() & MotionEvent.ACTION_MASK;
		int nIndex     = event.getActionIndex();
		int nPointerID = event.getPointerId( (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT );

		try
		{
			int x = ( int ) event.getX();
			int y = ( int ) event.getY();

			if ( nEvent == MotionEvent.ACTION_DOWN )
			{
				MoveMouse( x, y );
				SendKey( LinderdaumJNILib.LK_LBUTTON, true );
				for ( int i = 0; i != event.getPointerCount(); i++ )
				{
					SendMotion( event.getPointerId(i), (int)event.getX(i), (int)event.getY(i), true, L_MOTION_DOWN );
				}
			}

			if ( nEvent == MotionEvent.ACTION_UP || nEvent == MotionEvent.ACTION_CANCEL )
			{
				MoveMouse( x, y );
				SendKey( LinderdaumJNILib.LK_LBUTTON, false );
				SendMotion( -2, 0, 0, false, L_MOTION_UP );
				return nEvent <= MotionEvent.ACTION_MOVE;
			}

			// secondary pointers go up or down
			int maskedEvent = event.getActionMasked();

			if ( maskedEvent == MotionEvent.ACTION_POINTER_DOWN )
			{
				MoveMouse( x, y );
				for ( int i = 0; i != event.getPointerCount(); i++ )
				{
					SendMotion( event.getPointerId(i), (int)event.getX(i), (int)event.getY(i), true, L_MOTION_DOWN );
				}
			}

			if ( maskedEvent == MotionEvent.ACTION_POINTER_UP )
			{
				MoveMouse( x, y );
				for ( int i = 0; i != event.getPointerCount(); i++ )
				{
					SendMotion( event.getPointerId(i), (int)event.getX(i), (int)event.getY(i), i != nPointerID, L_MOTION_UP );
				}
				SendMotion( nPointerID, (int)event.getX(nPointerID), (int)event.getY(nPointerID), false, L_MOTION_MOVE );
			}

			if ( nEvent == MotionEvent.ACTION_MOVE )
			{
				MoveMouse( x, y );

				for ( int i = 0; i != event.getPointerCount(); i++ )
				{
					SendMotion( event.getPointerId(i), (int)event.getX(i), (int)event.getY(i), true, L_MOTION_MOVE );
				}
			}

		} // try
		catch ( java.lang.IllegalArgumentException e )
		{
		}

		SendMotion( -2, 0, 0, false, L_MOTION_MOVE );
		return nEvent <= MotionEvent.ACTION_MOVE;
	}

	@Override protected void onPause()
	{
		super.onPause();

		if ( m_View != null )
		{
			m_View.onPause();
			NativePause();
		}
	}

	@Override protected void onResume()
	{
		super.onResume();

		if ( m_View != null )
		{
			m_View.onResume();
			NativeResume();
		}
	}

	@Override
	public boolean onKeyDown( int keyCode, KeyEvent event )
	{
		int EngineKey = LinderdaumJNILib.TranslateKey( keyCode );

		if ( EngineKey != 0 )
		{
			SendKey( EngineKey, true );
			return true;
		}

		return super.onKeyDown( keyCode, event );
	}

	@Override
	public boolean onKeyUp( int keyCode, KeyEvent event )
	{
		int EngineKey = LinderdaumJNILib.TranslateKey( keyCode );

		if ( EngineKey != 0 )
		{
			SendKey( EngineKey, false );
			return true;
		}

		return super.onKeyUp( keyCode, event );
	}

	@Override
	public void onBackPressed()
	{
		//ExitEngine();
	}

	private LicenseCheckerCallback mLicenseCheckerCallback;
	private LicenseChecker mChecker;

	protected void CheckLicense( String BASE64_PUBLIC_KEY, byte[] SALT )
	{
		String deviceId = Secure.getString( getContentResolver(), Secure.ANDROID_ID );

		// Construct the LicenseCheckerCallback. The library calls this when done.
		mLicenseCheckerCallback = new LinderdaumLicenseChecker();

		// Construct the LicenseChecker with a Policy.
		mChecker = new LicenseChecker(
							this, new ServerManagedPolicy(this,
							new AESObfuscator( SALT, getPackageName(), deviceId) ),
							BASE64_PUBLIC_KEY
						);

		mChecker.checkAccess( mLicenseCheckerCallback );
	}

	protected FrameLayout AddAdMobBanners( String ApplicationID )
	{
		FrameLayout layout = new FrameLayout( this );

		layout.addView( m_View );

		Display display = getWindowManager().getDefaultDisplay(); 

		// Create the adView
		boolean IsLarge = ( display.getWidth() >= 1000 ) && ( display.getHeight() >= 600 );

		Log.i( TAG, "Device: " + android.os.Build.DEVICE );
		Log.i( TAG, "Model:  " + android.os.Build.MODEL  );

		if ( android.os.Build.MODEL.equals( "Galaxy Nexus" ) ) IsLarge = false;

		m_AdView = new AdView( this, IsLarge ? AdSize.IAB_LEADERBOARD : AdSize.BANNER, ApplicationID );
		m_AdView.setBackgroundColor(0x55000000);

		layout.addView( m_AdView );
    
		// Initiate a generic request to load it with an ad
		AdRequest request = new AdRequest();

		m_AdView.setAdListener( this );
		m_AdView.loadAd( request );            

		return layout;
	}

	public String getPath(Uri uri)
	{
		String selectedImagePath;
		//1:MEDIA GALLERY --- query from MediaStore.Images.Media.DATA
		String[] projection = { android.provider.MediaStore.Images.Media.DATA };

		Cursor cursor = managedQuery(uri, projection, null, null, null);

		if (cursor != null)
		{
			int column_index = cursor.getColumnIndexOrThrow(android.provider.MediaStore.Images.Media.DATA);
			cursor.moveToFirst();
			selectedImagePath = cursor.getString(column_index);
		}
		else
		{
			selectedImagePath = null;
		}

		if (selectedImagePath == null)
		{
			//2:OI FILE Manager --- call method: uri.getPath()
			selectedImagePath = uri.getPath();
		}
		return selectedImagePath;
	}

	protected void onActivityResult(final int requestCode, final int resultCode, final Intent i)
	{
		super.onActivityResult( requestCode, resultCode, i );

		if ( requestCode == LinderdaumJNILib.SELECT_PICTURE_CALLBACK && i != null && resultCode == RESULT_OK )
		{
			Uri uri = i.getData();

			SendSelectedImage( (uri != null) ? getPath( uri ) : "" );
		}
	}

	public void ShowAdsUI()
	{
		Log.i( TAG, "ShowAdsUI()" );

		runOnUiThread( new Runnable()
							{
								public void run()
								{
									Log.i( TAG, "run()" );

									if ( LinderdaumEngineActivity.m_LEngine.m_AdView != null )
									{
										Log.i( TAG, "setVisibility( View.VISIBLE )" );
										LinderdaumEngineActivity.m_LEngine.m_AdView.setVisibility( View.VISIBLE );
										LinderdaumEngineActivity.m_LEngine.m_AdView.loadAd( new AdRequest() );
									}
								}
							} );
	}

	public void HideAdsUI()
	{
		Log.i( TAG, "HideAdsUI()" );

		runOnUiThread( new Runnable()
							{
								public void run()
								{
									Log.i( TAG, "run()" );

									if ( LinderdaumEngineActivity.m_LEngine.m_AdView != null )
									{
										Log.i( TAG, "setVisibility( View.INVISIBLE )" );
										LinderdaumEngineActivity.m_LEngine.m_AdView.setVisibility( View.GONE );
									}
								}
							} );
	}

	public void Restart( Class ActivityToRestartClass )
	{
		Log.i( TAG, "Restarting..." );
		Intent i = new Intent(this, LinderdaumRestartActivity.class);
		i.putExtra( LinderdaumRestartActivity.ENGINE_PID,   android.os.Process.myPid() );
		i.putExtra( LinderdaumRestartActivity.ENGINE_CLASS, ActivityToRestartClass.getName() );
		startActivity(i);
		finish();
	}

	public native static void ExitEngine();
	public native static void SendKey( int keycode, boolean Pressed );
	public native static void SendMotion( int PointerID, int x, int y, boolean Pressed, int Flag );
	public native static void MoveMouse( int x, int y );
	public native static void NativePause();
   public native static void NativeResume();
	public native static void SendOptionsMenu( int MenuID );
	public native static void SendSelectedImage( String imageFilePath );
}
