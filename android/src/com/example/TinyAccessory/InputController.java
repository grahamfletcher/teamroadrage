package com.example.TinyAccessory;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;

import com.example.TinyAccessory.R;

import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.media.MediaPlayer;
import android.widget.Toast;
import android.widget.ImageView;
import android.content.Context;
import android.content.res.Resources;

public class InputController extends AccessoryController {
	private TextView mLightView;
	private TextView mLightRawView;
	private TextView mTemperature;
	
	private ImageView micon;
	private ProgressBar progbar;
	private int progstatus = 0;
	private boolean test = false;
	ArrayList<SwitchDisplayer> mSwitchDisplayers;
	private final DecimalFormat mLightValueFormatter = new DecimalFormat("##.#");
	private final DecimalFormat mTemperatureFormatter = new DecimalFormat(
			"###" + (char)0x00B0);

	InputController(TinyAccessoryActivity hostActivity) {
		super(hostActivity);
		mTemperature = (TextView) findViewById(R.id.tempValue);
		mLightView = (TextView) findViewById(R.id.lightPercentValue);
		progbar = (ProgressBar) findViewById(R.id.progressBar1);
		micon = (ImageView) findViewById(R.id.imageView1);
		
	}

	protected void onAccesssoryAttached() {
		mSwitchDisplayers = new ArrayList<SwitchDisplayer>();
		for (int i = 0; i < 4; ++i) {
			SwitchDisplayer sd = new SwitchDisplayer(i);
			mSwitchDisplayers.add(sd);
		}
	}

	
	public void setTemperature(int temperatureFromArduino) {
		
		
		int officonid, rainiconid, flakeiconid;
		/*
		 * Arduino board contains a 6 channel (8 channels on the Mini and Nano,
		 * 16 on the Mega), 10-bit analog to digital converter. This means that
		 * it will map input voltages between 0 and 5 volts into integer values
		 * between 0 and 1023. This yields a resolution between readings of: 5
		 * volts / 1024 units or, .0049 volts (4.9 mV) per unit.
		 */
		//double voltagemv = temperatureFromArduino * 4.9;
		/*
		 * The change in voltage is scaled to a temperature coefficient of 10.0
		 * mV/degC (typical) for the MCP9700/9700A and 19.5 mV/degC (typical)
         * for the MCP9701/9701A. The out- put voltage at 0 degC is also scaled
         * to 500 mV (typical) and 400 mV (typical) for the MCP9700/9700A and
		 * MCP9701/9701A, respectively. VOUT = TC¥TA+V0degC
		 */
		//double kVoltageAtZeroCmv = 400;
		//double kTemperatureCoefficientmvperC = 19.5;
		//double ambientTemperatureC = ((double) voltagemv - kVoltageAtZeroCmv)
		//		/ kTemperatureCoefficientmvperC;
		//double temperatureF = (9.0 / 5.0) * ambientTemperatureC + 32.0;
		
		final Drawable rainicon;
		final Drawable flakeicon;
		final Drawable officon;
		
        MediaPlayer mp = MediaPlayer.create(mHostActivity.getBaseContext(), R.raw.sound);
		
		rainiconid = R.drawable.rain_icon;
		flakeiconid = R.drawable.flake_icon;
		officonid = R.drawable.off_icon;
		
				
		rainicon = mHostActivity.getResources().getDrawable(rainiconid);
		flakeicon = mHostActivity.getResources().getDrawable(flakeiconid);
		officon = mHostActivity.getResources().getDrawable(officonid);
		
		double temperatureC = (temperatureFromArduino*.2222) - 61.11;
		mTemperature.setText(mTemperatureFormatter.format(temperatureC));
		
		Resources res = getResources();
		Rect bounds = progbar.getProgressDrawable().getBounds();
		progbar.setProgressDrawable(res.getDrawable( R.drawable.my_progress));
		progbar.getProgressDrawable().setBounds(bounds);
		//micon.setScaleType(ImageView.ScaleType.FIT_CENTER);

		if(progstatus < 100){
		progstatus++;

		if(test){
			micon.setImageDrawable(rainicon);
		}else{
			micon.setImageDrawable(flakeicon);
		}

		} else{
			progstatus = 0;
			test = !test;
            mp.start();

		}

		//micon.setScaleType(ImageView.ScaleType.FIT_CENTER);
		progbar.setProgress(progstatus);
       
	}
	

	public void setLightValue(int lightValueFromArduino) {
		//mLightRawView.setText(String.valueOf(lightValueFromArduino));
		//mLightView.setText(mLightValueFormatter
				//.format((100.0 * (double) lightValueFromArduino / 1024.0)));
	}

	public void switchStateChanged(int switchIndex, boolean switchState) {
		if (switchIndex >= 0 && switchIndex < mSwitchDisplayers.size()) {
			SwitchDisplayer sd = mSwitchDisplayers.get(switchIndex);
			sd.onSwitchStateChange(switchState);
		}
	}

	public void arduinoChanged(byte[] t) {
		//mTemperature.setText("whore");
		//text1.setText("" + 133);
		/*text1.setText("" + t[0]); // txt is the name of the text field in the GUI, txt.setText just sets the field to whatever is passed to it.
		text2.setText("" + t[1]);
		text3.setText("" + t[2]);
		text4.setText("" + t[3]);
		text5.setText("" + t[4]);
		text6.setText("" + t[5]);
		*/
		
		
		int officonid, rainiconid, flakeiconid;
        float value1 = 0;
        int   value2= 0;
		final Drawable rainicon;
		final Drawable flakeicon;
		final Drawable officon;
		
		if(t[4] < 0){
			value1 = t[4]&0xFF;
		}else{
			value1 = t[4];
		}

        value1 = value1 / 10;
		
		if(t[5] < 0){
			value2 = t[5]&0xFF;
		}else{
			value2 = t[5];
		}
		
		rainiconid = R.drawable.rain_icon;
		flakeiconid = R.drawable.flake_icon;
		officonid = R.drawable.off_icon;
	
		rainicon = mHostActivity.getResources().getDrawable(rainiconid);
		flakeicon = mHostActivity.getResources().getDrawable(flakeiconid);
		officon = mHostActivity.getResources().getDrawable(officonid);
		
        MediaPlayer mp = MediaPlayer.create(mHostActivity.getBaseContext(), R.raw.sound);
		
		Resources res = getResources();
		Rect bounds = progbar.getProgressDrawable().getBounds();
		progbar.setProgressDrawable(res.getDrawable(R.drawable.my_progress));
		progbar.getProgressDrawable().setBounds(bounds);
		
        if(t[0] == 1 || t[0] == '1' ){ //Show flake icon when boolean is one
            micon.setImageDrawable(flakeicon);
        }else if(t[1] == 1 || t[1] == '1' ){ //Show rain icon when boolean is one
			micon.setImageDrawable(rainicon);
		}else{ //else set to officon
			micon.setImageDrawable(officon);
		}
		
        if(t[2] == 1 || t[2] == '1' ) { //Play music if the boolean is one
            mp.start();
        }
		
		// Set Progress Bar
		if(t[3] != 254){
		progbar.setProgress(t[3]); //setting the byte to int just in case!!
		}else{
			progbar.setProgress(0); 
		}
		
		if(value1 != 254){
		mTemperature.setText("" + value1); //Set the time headway
		}else{
		mTemperature.setText("--");
		}
		
		if(value2 != 254){
        mLightView.setText( "" + value2 );
        //mLightView.setText(""+ (int) (value2 * 0.0006213 *3600)); //0.0006213 miles to a meter, 3600 seconds in an hour
		}else{
			mLightView.setText("--");
		}
		
		// buf[0] = (bool); ice present
	     // buf[1] = (bool); rain present
	     // buf[2] = (bool); play alarm
	     // buf[3] = (int); percent for progress bar (already multiplied by 100)
	     // buf[4] = (float); time headway * 10 (divide this by 10 and then display)
	     // buf[5] = (int); lead vehicle velocity (it's a percent, already multiplied by 100)
		
	}
	

	public void onLightChange(int lightValue) {
		setLightValue(lightValue);
	}

	public void onTemperature(int temperature) {
		setTemperature(temperature);
	}
	
	public void onSwitchStateChange(int switchIndex, Boolean switchState) {
		switchStateChanged(switchIndex, switchState);
	}

	class SwitchDisplayer {
		private final ImageView mTargetView;
		private final Drawable mOnImage;
		private final Drawable mOffImage;

		SwitchDisplayer(int switchIndex) {
			int viewId, onImageId, offImageId;
			switch (switchIndex) {
			default:
				viewId = R.id.Button1;
				onImageId = R.drawable.indicator_button1_on_noglow;
				offImageId = R.drawable.indicator_button1_off_noglow;
				break;
			}
			mTargetView = (ImageView) findViewById(viewId);
			mOffImage = mHostActivity.getResources().getDrawable(offImageId);
			mOnImage = mHostActivity.getResources().getDrawable(onImageId);
		}

		public void onSwitchStateChange(Boolean switchState) {
			Drawable currentImage;
			if (!switchState) {
				currentImage = mOffImage;
			} else {
				currentImage = mOnImage;
			}
			mTargetView.setImageDrawable(currentImage);
		}

	}
}
