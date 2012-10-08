package com.linderdaum.engine;

import com.android.vending.licensing.LicenseChecker;
import com.android.vending.licensing.LicenseCheckerCallback;
import com.android.vending.licensing.ServerManagedPolicy;
import com.android.vending.licensing.AESObfuscator;

import android.util.Log;

public class LinderdaumLicenseChecker implements LicenseCheckerCallback
{
	public static final String TAG = "LinderdaumLicenseChecker";

	public void allow()
	{
		if ( LinderdaumEngineActivity.m_LEngine.isFinishing() )
		{
			// Don't update UI if Activity is finishing.
			return;
		}

		Log.d( TAG, "allow()" );
	}

	public void dontAllow()
	{
     	if ( LinderdaumEngineActivity.m_LEngine.isFinishing() )
		{
			// Don't update UI if Activity is finishing.
			return;
		}

		Log.d( TAG, "dontAllow()" );

		LinderdaumJNILib.OpenURL( "market://details?id="+LinderdaumEngineActivity.m_LEngine.getApplication().getPackageName() );

		LinderdaumEngineActivity.m_LEngine.finish();
	}

	@Override
	public void applicationError(ApplicationErrorCode errorCode)
	{
		Log.d( TAG, "applicationError: " + errorCode );
	}
}
