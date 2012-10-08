/**
 * \file LinderdaumEngineActivity.java
 * \brief Linderdaum Engine activity for Android
 * \version 0.5.99c
 * \date 04/05/2011
 * \author Sergey Kosarevsky, 2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

package com.linderdaum.engine;

import android.util.Log;
import android.content.Context;
import android.content.res.AssetManager;

import java.io.*;

class LinderdaumDeployer
{
	public static final String TAG = "LinderdaumDeployer";

	private long APKInstallTime;
	private AssetManager assetManager;

	private static final int CopyBufferSize     = 128000;
	private static final int BufferedStreamSize = 256000;

	public LinderdaumDeployer( Context context )
	{
		APKInstallTime = new File( context.getPackageResourcePath() ).lastModified();
		assetManager = context.getAssets();
	}

	public void CopyAssets( String TargetDir, String Root )
	{
		String Prefix = "";

		if ( Root != "" ) { Prefix = Root + "/"; }

		String[] files = null;

		try
		{
			files = assetManager.list( Root );
		}
		catch ( IOException e )
		{
			Log.e( TAG, e.getMessage() );
		}

		if ( files != null && files.length > 0 )
		{
			Log.w( TAG, "Processing folder " + Root );

			// process folder
			File F = new File( TargetDir, Root );
			F.mkdirs();

			for ( int i = 0; i < files.length; i++ )
			{
				CopyAssets( TargetDir, Prefix + files[i] );
			}
		}
		else
		{
			// copy file
			String TargetName = TargetDir + "/" + Root;

			File OutFile = new File( TargetName );

			if ( OutFile.exists() && OutFile.lastModified() > APKInstallTime )
			{
				Log.w( TAG, "Skipping " + Root );
				return;
			}

			try
			{
				Log.w( TAG, "Copying " + Root + " to " + TargetName );
				InputStream in = assetManager.open( Root, assetManager.ACCESS_STREAMING );
				OutputStream out = new BufferedOutputStream( new FileOutputStream( TargetName ), BufferedStreamSize );
				CopyFile( in, out );
				in.close();
				out.flush();
				out.close();
			}
			catch ( Exception e )
			{
				Log.e( TAG, e.getMessage() );
			}
		}
	}

	private static void CopyFile( InputStream in, OutputStream out ) throws IOException
	{
		byte[] buffer = new byte[ CopyBufferSize ];
		int read;

		while ( ( read = in.read( buffer ) ) != -1 )
		{
			out.write( buffer, 0, read );
		}
	}
}
