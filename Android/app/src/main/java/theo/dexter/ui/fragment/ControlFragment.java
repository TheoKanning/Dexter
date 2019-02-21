package theo.dexter.ui.fragment;


import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;
import theo.dexter.R;
import theo.dexter.bluetooth.BluetoothConnection;
import theo.dexter.bluetooth.BluetoothScanner;
import theo.dexter.control.ControlCalculator;
import theo.dexter.control.ControlCommand;

/**
 * Fragment for steering Dexter
 */
public class ControlFragment extends Fragment implements SensorEventListener, BluetoothConnection.BluetoothConnectionListener, BluetoothScanner.OnBluetoothDeviceDiscoveredListener {

    private static final String TAG = "ControlFragment";
    private static final int REQUEST_ENABLE_BT = 1;

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
        ButterKnife.bind(this, view);

        bluetoothScanner = new BluetoothScanner(getContext());

        sensorManager = (SensorManager) getActivity().getSystemService(Context.SENSOR_SERVICE);
        accel = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        park();

        if (!BluetoothAdapter.getDefaultAdapter().isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        sensorManager.registerListener(this, accel, SensorManager.SENSOR_DELAY_NORMAL);
        bluetoothScanner.startScan(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this);
        bluetoothConnection.disconnect();
    }

    @Override
    public void onSensorChanged(SensorEvent e) {

        if(bluetoothConnection != null && !bluetoothConnection.isConnected()){
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
        Log.i("ControlFragment", "Connected");
    }

    @Override
    public void onDisconnect() {
        Toast.makeText(getContext(), "Disconnected...", Toast.LENGTH_SHORT).show();
        Log.i("ControlFragment", "Disconnected");
    }

    @Override
    public void onBluetoothDeviceDiscovered(BluetoothDevice device) {
        if (device.getName().equals("Dexter")) {
            bluetoothScanner.stopScan();
            bluetoothConnection = new BluetoothConnection(device, this);
        }
    }
}
