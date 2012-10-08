/**
 * \file LinderdaumSensors.java
 * \brief Linderdaum Engine sensors library for Android
 * \version 0.6.06
 * \date 04/01/2012
 * \author Sergey Kosarevsky, 2011-2012
 * \author support@linderdaum.com http://www.linderdaum.com
 */

package com.linderdaum.engine;

import android.os.Bundle;
import android.content.Context;
import android.util.Log;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.app.Activity;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;

import java.io.*;

public class LinderdaumSensors
{
	private class LinearAccelerometerListener implements SensorEventListener
	{
		private float gravity[] = new float[3];
		private float linear_acceleration[] = new float[3];

		@Override
		public void onSensorChanged( SensorEvent event )
		{
			// alpha is calculated as t / (t + dT)
			// with t, the low-pass filter's time-constant
			// and dT, the event delivery rate

			final float alpha = 0.8f;

			gravity[0] = alpha * gravity[0] + (1.0f - alpha) * event.values[0];
			gravity[1] = alpha * gravity[1] + (1.0f - alpha) * event.values[1];
			gravity[2] = alpha * gravity[2] + (1.0f - alpha) * event.values[2];

			linear_acceleration[0] = event.values[0] - gravity[0];
			linear_acceleration[1] = event.values[1] - gravity[1];
			linear_acceleration[2] = event.values[2] - gravity[2];

			UpdateLinearAcceleration( linear_acceleration[0], linear_acceleration[1], linear_acceleration[2] );
		}

		@Override
		public void onAccuracyChanged( Sensor sensor, int accuracy )
		{
		}
	}
	private class GyroscopeListener implements SensorEventListener
	{
		@Override
		public void onSensorChanged( SensorEvent event )
		{
			UpdateVelocity( event.values[0], event.values[1], event.values[2] );
		}

		@Override
		public void onAccuracyChanged( Sensor sensor, int accuracy )
		{
		}
	}
	private class GPSListener implements LocationListener
	{
		@Override
		public void 	onLocationChanged(Location l)
		{
			UpdateGPS( l.getAltitude(), l.getLatitude(), l.getLongitude(), l.getSpeed(), l.getBearing(), l.getTime() );
		}
		@Override
		public void 	onProviderDisabled(String provider)
		{
		}
		@Override
		public void 	onProviderEnabled(String provider)
		{
		}
		@Override
		public void 	onStatusChanged(String provider, int status, Bundle extras)
		{
		}
	}

	public static final String TAG = "LinderdaumSensors";

	private SensorManager   m_SensorManager;
	private LocationManager m_LocationManager;

	// linear accelerometer
	private boolean                     m_LinearAccelerationActive = false;
	private Sensor                      m_LinearAccelerationSensor;
	private LinearAccelerometerListener m_LinearAccelerometerListener = new LinearAccelerometerListener();

	// gyroscope
	private boolean           m_GyroscopeActive = false;
	private Sensor            m_GyroscopeSensor;
	private GyroscopeListener m_GyroscopeListener = new GyroscopeListener();

	// GPS
	private boolean     m_GPSActive = false;
	private GPSListener m_GPSListener = new GPSListener();

	LinderdaumSensors()
	{
		m_SensorManager = (SensorManager)LinderdaumEngineActivity.m_LEngine.getSystemService( Context.SENSOR_SERVICE );
		m_LocationManager = (LocationManager)LinderdaumEngineActivity.m_LEngine.getSystemService(Context.LOCATION_SERVICE);

		m_LinearAccelerationSensor = m_SensorManager.getDefaultSensor( Sensor.TYPE_ACCELEROMETER );
		m_GyroscopeSensor          = m_SensorManager.getDefaultSensor( Sensor.TYPE_GYROSCOPE     );
	}

	void EnableLinearAccelerometer(boolean Active)
	{
		if ( m_LinearAccelerationActive == Active ) return;

		if ( Active )
		{
			m_SensorManager.registerListener( m_LinearAccelerometerListener, m_LinearAccelerationSensor, SensorManager.SENSOR_DELAY_GAME );
		}
		else
		{
			m_SensorManager.unregisterListener( m_LinearAccelerometerListener );
		}

		m_LinearAccelerationActive = Active;
	}

	void EnableGyroscope(boolean Active)
	{
		if ( m_GyroscopeActive == Active ) return;

		if ( Active )
		{
			m_SensorManager.registerListener( m_GyroscopeListener, m_GyroscopeSensor, SensorManager.SENSOR_DELAY_GAME );
		}
		else
		{
			m_SensorManager.unregisterListener( m_GyroscopeListener );
		}

		m_GyroscopeActive = Active;
	}

	void EnableGPS(boolean Active)
	{
		if ( m_GPSActive == Active ) return;

		if ( Active )
		{
			Location l = m_LocationManager.getLastKnownLocation("");

			if ( l != null ) UpdateGPS( l.getAltitude(), l.getLatitude(), l.getLongitude(), l.getSpeed(), l.getBearing(), l.getTime() );

			LinderdaumEngineActivity.m_LEngine.runOnUiThread( new Runnable()
								{
									public void run()
									{
										m_LocationManager.requestLocationUpdates( LocationManager.NETWORK_PROVIDER, 0, 0, m_GPSListener );
										m_LocationManager.requestLocationUpdates( LocationManager.GPS_PROVIDER, 0, 0, m_GPSListener );
									}
								} );
		}
		else
		{
			LinderdaumEngineActivity.m_LEngine.runOnUiThread( new Runnable()
								{
									public void run()
									{
										m_LocationManager.removeUpdates( m_GPSListener );
									}
								} );
			
		}

		m_GPSActive = Active;
	}

	private native void UpdateLinearAcceleration( float ax, float ay, float az );
	private native void UpdateVelocity( float vx, float vy, float vz );
	private native void UpdateGPS( double Altitude, double Latitude, double Longitude, double Speed, double Bearing, double TimeStamp );
}
