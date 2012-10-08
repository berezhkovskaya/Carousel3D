package com.linderdaum.engine.<!PROJECT_NAME!>;

import com.linderdaum.engine.LinderdaumEngineActivity;
import com.linderdaum.engine.LinderdaumJNILib;

import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MenuInflater;
import android.os.Bundle;
import android.util.Log;
import android.widget.FrameLayout;

public class <!PROJECT_NAME!>Activity extends LinderdaumEngineActivity
{
	public static final String TAG = "<!PROJECT_NAME!>Activity";

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// use super.CheckLicense() here
	}

	@Override
	public View Deployed()
	{
		// add your views here

		return super.Deployed();
	} 
}
