package theo.dexter.ui.fragment;


import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import javax.inject.Inject;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;
import theo.dexter.DexterApplication;
import theo.dexter.R;
import theo.dexter.bluetooth.BluetoothConnection;
import theo.dexter.bluetooth.BluetoothScanner;

/**
 * Fragment for steering car
 */
public class ControlFragment extends BaseFragment implements SensorEventListener, BluetoothConnection.BluetoothConnectionListener{

    private static final String TAG = "ControlFragment";
    public static final String ADDRESS_EXTRA = "ControlFragment.AddressExtra";

    @Bind(R.id.control_drive)
    Button drive;

    @Bind(R.id.control_park)
    Button park;

    @OnClick(R.id.control_drive)
    public void drive(){
        drive.setVisibility(View.GONE);
        park.setVisibility(View.VISIBLE);
        parked = false;
    }

    @OnClick(R.id.control_park)
    public void park(){
        drive.setVisibility(View.VISIBLE);
        park.setVisibility(View.GONE);
        parked = true;
    }


    @Inject
    BluetoothScanner bluetoothScanner;

    private String address;
    private BluetoothConnection bluetoothConnection;

    private SensorManager sensorManager;
    private Sensor accel;

    private int xAxis = 0;
    private int yAxis = 0;
    private int motorLeft = 0;
    private int motorRight = 0;
    private int xMax = 7;
    private int yMax = 5;
    private int yThreshold = 50;
    private final int pwmMax = 255;
    private final int xR = 5;
    private final String commandLeft = "L";
    private final String commandRight = "R";

    private boolean parked = true;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_control, container, false);
        ((DexterApplication) getActivity().getApplication()).getComponent().inject(this);
        ButterKnife.bind(this, view);

        sensorManager = (SensorManager) getActivity().getSystemService(Context.SENSOR_SERVICE);
        accel = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        Bundle arguments = getArguments();
        address = arguments.getString(ADDRESS_EXTRA);

        if(address == null){
            Log.e(TAG, "No address received");
        } else {
            bluetoothConnection = new BluetoothConnection(bluetoothScanner.getDevices(address), this);
        }

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        park();
        sensorManager.registerListener(this, accel, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    public void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this);
        bluetoothConnection.disconnect();
    }

    @Override
    public int getTitleResourceId() {
        return R.string.fragment_control_title;
    }

    @Override
    public void onSensorChanged(SensorEvent e) {

        if(!bluetoothConnection.isConnected()){
            Log.d(TAG, "BluetoothConnection.isConnected returned false");
            return;
        }

        if(parked){
            return;
        }

        String directionL = "";
        String directionR = "";
        String cmdSendL, cmdSendR;
        float xRaw, yRaw;        // RAW-value from Accelerometer sensor

        WindowManager windowMgr = (WindowManager) getActivity().getSystemService(Context.WINDOW_SERVICE);
        int rotationIndex = windowMgr.getDefaultDisplay().getRotation();
        if (rotationIndex == 1 || rotationIndex == 3) {            // detect 90 or 270 degree rotation (îïðåäåëÿåì ïîâîðîò óñòðîéñòâà íà 90 èëè 270 ãðàäóñîâ)
            xRaw = -e.values[1];
            yRaw = e.values[0];
        } else {
            xRaw = e.values[0];
            yRaw = e.values[1];
        }

        xAxis = Math.round(xRaw * pwmMax / xR);
        yAxis = Math.round(yRaw * pwmMax / yMax);

        if (xAxis > pwmMax) xAxis = pwmMax;
        else if (xAxis < -pwmMax)
            xAxis = -pwmMax;        // negative - tilt right

        if (yAxis > pwmMax) yAxis = pwmMax;
        else if (yAxis < -pwmMax)
            yAxis = -pwmMax;        // negative - tilt forward
        else if (yAxis >= 0 && yAxis < yThreshold) yAxis = 0;
        else if (yAxis < 0 && yAxis > -yThreshold) yAxis = 0;

        if (xAxis > 0) {        // if tilt to left, slow down the left engine
            motorRight = yAxis;
            if (Math.abs(Math.round(xRaw)) > xR) {
                motorLeft = Math.round((xRaw - xR) * pwmMax / (xMax - xR));
                motorLeft = Math.round(-motorLeft * yAxis / pwmMax);
                //if(motorLeft < -pwmMax) motorLeft = -pwmMax;
            } else motorLeft = yAxis - yAxis * xAxis / pwmMax;
        } else if (xAxis < 0) {        // tilt to right (íàêëîí âïðàâî)
            motorLeft = yAxis;
            if (Math.abs(Math.round(xRaw)) > xR) {
                motorRight = Math.round((Math.abs(xRaw) - xR) * pwmMax / (xMax - xR));
                motorRight = Math.round(-motorRight * yAxis / pwmMax);
                //if(motorRight > -pwmMax) motorRight = -pwmMax;
            } else motorRight = yAxis - yAxis * Math.abs(xAxis) / pwmMax;
        } else if (xAxis == 0) {
            motorLeft = yAxis;
            motorRight = yAxis;
        }

        if (motorLeft > 0) {            // tilt to backward (íàêëîí íàçàä)
            directionL = "-";
        }
        if (motorRight > 0) {        // tilt to backward (íàêëîí íàçàä)
            directionR = "-";
        }
        motorLeft = Math.abs(motorLeft);
        motorRight = Math.abs(motorRight);

        if (motorLeft > pwmMax) motorLeft = pwmMax;
        if (motorRight > pwmMax) motorRight = pwmMax;

        cmdSendL = String.valueOf(commandLeft + directionL + motorLeft + "\r");
        cmdSendR = String.valueOf(commandRight + directionR + motorRight + "\r");

        bluetoothConnection.write(cmdSendL + cmdSendR);
        Log.d(TAG, cmdSendL + cmdSendR);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }



    @Override
    public void onMessageReceived(String message) {

    }

    @Override
    public void onConnect() {
        Toast.makeText(getContext(), "Connected!", Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onDisconnect() {

    }
}
