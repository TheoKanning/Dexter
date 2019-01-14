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
import theo.dexter.control.ControlCalculator;
import theo.dexter.control.ControlCommand;

/**
 * Fragment for steering car
 */
public class ControlFragment extends BaseFragment implements SensorEventListener, BluetoothConnection.BluetoothConnectionListener{

    private static final String TAG = "ControlFragment";
    public static final String ADDRESS_EXTRA = "ControlFragment.AddressExtra";

    private static final String COMMAND_LINEAR = "L";
    private static final String COMMAND_ANGULAR = "A";

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

    private BluetoothConnection bluetoothConnection;

    private SensorManager sensorManager;
    private Sensor accel;
    private ControlCalculator calculator = new ControlCalculator();

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
        String address = arguments.getString(ADDRESS_EXTRA);

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

        float xRaw, yRaw;        // RAW-value from Accelerometer sensor

        WindowManager windowMgr = (WindowManager) getActivity().getSystemService(Context.WINDOW_SERVICE);
        int rotationIndex = windowMgr.getDefaultDisplay().getRotation();
        if (rotationIndex == 1 || rotationIndex == 3) {            // detect 90 or 270 degree rotation
            xRaw = -e.values[1];
            yRaw = e.values[0];
        } else {
            xRaw = e.values[0];
            yRaw = e.values[1];
        }

        ControlCommand command = calculator.calculateControlCommand(xRaw, yRaw);

        String cmdSendL = String.valueOf(COMMAND_LINEAR + command.getLinearVelocity() + "\r\n");
        String cmdSendR = String.valueOf(COMMAND_ANGULAR + command.getAngularVelocity() + "\r\n");

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
