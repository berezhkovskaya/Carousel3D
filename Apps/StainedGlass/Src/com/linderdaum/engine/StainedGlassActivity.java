package com.linderdaum.engine.stainedglass;

import com.linderdaum.engine.LinderdaumEngineActivity;
import com.linderdaum.engine.LinderdaumJNILib;

import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MenuInflater;
import android.os.Bundle;
import android.util.Log;
import android.widget.FrameLayout;

public class StainedGlassActivity extends LinderdaumEngineActivity
{
	public static final String TAG = "StainedGlassActivity";

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
	}

	@Override
	public View Deployed()
	{
		// add your views here

		return super.Deployed();
	} 

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
//	   MenuInflater inflater = getMenuInflater();
//	   inflater.inflate( R.menu.game_menu, menu );
   	return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch ( item.getItemId() )
		{
		default:
			return super.onOptionsItemSelected(item);
		}
	}
}
