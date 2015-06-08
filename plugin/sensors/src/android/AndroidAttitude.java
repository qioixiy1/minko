package minko.plugin.sensors;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.Matrix;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import org.libsdl.app.*;

/**
 * Provides head tracking information from the device IMU. 
 */
public class AndroidAttitude
{
	private Activity _sdlActivity;
	private final Context _context;
	private Looper _sensorLooper;
	private SensorManager _sensorManager;
	private SensorEventListener _sensorEventListener;
	private volatile boolean _tracking;

	// Native functions
	public native void minkoNativeOnAttitudeEvent(float[] rotationMatrix, float[] quaternion);

	public AndroidAttitude(Activity sdlActivity)
	{
		_sdlActivity = sdlActivity;
		_context = SDLActivity.getContext();
	}

	public void startTracking()
	{
		if (_tracking) {
			return;
		}

		_sensorEventListener = new SensorEventListener()
		{
			public void onSensorChanged(SensorEvent event) {
				AndroidAttitude.this.processSensorEvent(event);
			}

			public void onAccuracyChanged(Sensor sensor, int accuracy)
			{
			}
		};
		
		Thread sensorThread = new Thread(new Runnable()
		{
			public void run() {
				Looper.prepare();

				_sensorLooper = Looper.myLooper();
				Handler handler = new Handler();

				_sensorManager = (SensorManager)_context.getSystemService("sensor");

				Sensor sensor = _sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
				
				if(sensor == null)
					sensor = _sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

				_sensorManager.registerListener(_sensorEventListener, sensor, 0, handler);

				Looper.loop();
			}
		});

		sensorThread.start();
		_tracking = true;
	}

	public void stopTracking()
	{
		if (!_tracking) {
			return;
		}

		SensorManager _sensorManager = (SensorManager)_context.getSystemService("sensor");

		_sensorManager.unregisterListener(_sensorEventListener);
		_sensorEventListener = null;

		_sensorLooper.quit();
		_sensorLooper = null;
		_tracking = false;
	}

	private synchronized void processSensorEvent(SensorEvent event)
	{		
		float[] rotationVector = { -event.values[1], event.values[0], event.values[2] };

		float[] quaternion = new float[4];
		float[] rotationMatrix = new float[16];

		_sensorManager.getQuaternionFromVector(quaternion, rotationVector);
		_sensorManager.getRotationMatrixFromVector(rotationMatrix, rotationVector);

		minkoNativeOnAttitudeEvent(rotationMatrix, quaternion);
	}
}