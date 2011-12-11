package com.example.TinyAccessory;

import com.example.TinyAccessory.R;

import android.view.ViewGroup;

public class OutputController extends AccessoryController {

	OutputController(TinyAccessoryActivity hostActivity, boolean vertical) {
		super(hostActivity);
	}

	protected void onAccesssoryAttached() {
		setupPwmOutController(0, R.id.pwmout1);

	}

	private void setupPwmOutController(int pwmoutIndex, int viewId) {
		PwmOutController sc = new PwmOutController(mHostActivity, pwmoutIndex);
		sc.attachToView((ViewGroup) findViewById(viewId));
	}

}
