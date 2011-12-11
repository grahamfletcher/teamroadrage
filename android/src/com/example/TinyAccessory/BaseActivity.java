package com.example.TinyAccessory;

import com.example.TinyAccessory.R;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

public class BaseActivity extends TinyAccessoryActivity {

	private InputController mInputController;

	public BaseActivity() {
		super();
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (mAccessory != null) {
			showControls();
		} else {
			hideControls();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add("Simulate");
		menu.add("Quit");
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (item.getTitle() == "Simulate") {
			showControls();
		} else if (item.getTitle() == "Quit") {
			finish();
			System.exit(0);
		}
		return true;
	}

	protected void enableControls(boolean enable) {
		if (enable) {
			showControls();
		} else {
			hideControls();
		}
	}

	protected void hideControls() {
		setContentView(R.layout.no_device);
		mInputController = null;
	}

	protected void showControls() {
		setContentView(R.layout.main);

		mInputController = new InputController(this);
		mInputController.accessoryAttached();
	}

	protected void handleLightMessage(LightMsg l) {
		if (mInputController != null) {
			mInputController.setLightValue(l.getLight());
		}
	}

	protected void handleSwitchMessage(SwitchMsg o) {
		if (mInputController != null) {
			byte sw = o.getSw();
			if (sw >= 0 && sw < 4) {
				mInputController.switchStateChanged(sw, o.getState() != 0);
			}
		}
	}
	
	protected void handleArduinoMessage(ArduinoMsg a) {
		if (mInputController != null) {
			
			byte[] t = a.getArduino(); //Gets the temp byte array passed from the TinyAccessoryActivity.java
			mInputController.arduinoChanged(t); //Passes it to mInputController which is an instance of InputController. 
												//Look at the arduinoChanged method in InputController.java
		}
	}
	
	protected void handleTemperatureMessage(TemperatureMsg t) {
		if (mInputController != null) {
			mInputController.setTemperature(t.getTemperature());
		}
	}

}