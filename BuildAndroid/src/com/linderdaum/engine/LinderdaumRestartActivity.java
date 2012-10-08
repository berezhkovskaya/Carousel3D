/**
 * \file LinderdaumRestartActivity.java
 * \brief Engine restart hook
 * \version 0.6.04
 * \date 01/11/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

package com.linderdaum.engine;

import android.app.Activity;
import android.util.Log;
import android.content.Intent;
import android.os.*;

public class LinderdaumRestartActivity extends Activity
{
	public static final String TAG = "LinderdaumRestartActivity";

	public static final String ENGINE_PID   = "EnginePID";
	public static final String ENGINE_CLASS = "EngineClass";

	protected int    EnginePID   = 0;
	protected String EngineClass = "";

	private class KillThread extends Thread
	{
		private static final int TIMEOUT_BEFORE = 1500;
		private static final int TIMEOUT_AFTER  = 500;

		@Override
		public void run()
		{
			Log.i( TAG, "About to restart..." );

			try
			{
				Thread.sleep( TIMEOUT_BEFORE );

				android.os.Process.killProcess( EnginePID );

				Thread.sleep( TIMEOUT_AFTER );
			}
			catch (Exception e)
			{
			}

			Log.i( TAG, "Restarting: "+EngineClass );

			try
			{
				Class c = Class.forName( EngineClass );

				finish();

				startActivity( new Intent( LinderdaumRestartActivity.this, c ) );
			}
			catch (Exception e)
			{
			}

			finish();
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		Bundle extras = getIntent().getExtras();

		if ( !extras.containsKey(ENGINE_PID)   ) finish();
		if ( !extras.containsKey(ENGINE_CLASS) ) finish();

		EnginePID   = extras.getInt(ENGINE_PID);
		EngineClass = extras.getString(ENGINE_CLASS);

		new KillThread().start();
	}
}
