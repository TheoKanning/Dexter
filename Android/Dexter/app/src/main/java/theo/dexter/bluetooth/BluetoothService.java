package theo.dexter.bluetooth;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.UUID;

/**
 * I don't think this needs to be in a service
 */
public class BluetoothService extends Service {

    public interface OnBluetoothDeviceDiscoveredListener{
        void OnBluetoothDeviceDiscovered(BluetoothDevice device);
    }

    private static final String TAG = "BluetoothService";
    private static final int SCAN_DURATION = 10000; //10s
    public final static int BL_NOT_AVAILABLE = 1;        	// Bluetooth is not available
    public final static int BL_INCORRECT_ADDRESS = 2;		// incorrect MAC-address
    public final static int BL_REQUEST_ENABLE = 3;			// request enable Bluetooth
    public final static int BL_SOCKET_FAILED = 4;			// socket error
    public final static int RECIEVE_MESSAGE = 5;			// receive message
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private Context context;

    private BluetoothAdapter btAdapter;
    private BluetoothDevice connectedDevice;
    private BluetoothSocket btSocket;
    private OutputStream outStream;
    private OnBluetoothDeviceDiscoveredListener listener;
    private Handler handler = new Handler();
    private IBinder binder = new BluetoothServiceBinder();

    @Override
    public void onCreate() {
        super.onCreate();

        // Register the BroadcastReceiver
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(receiver, filter); // Don't forget to unregister during onDestroy
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (!btAdapter.isEnabled()) {

        }

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    public class BluetoothServiceBinder extends Binder {
        public BluetoothService getService() {
            return BluetoothService.this;
        }
    }

    public static void bindToService(Context context, ServiceConnection connection) {
        Intent intent = new Intent(context, BluetoothService.class);
        context.bindService(intent, connection, Context.BIND_ABOVE_CLIENT);
    }

    public void startScan(OnBluetoothDeviceDiscoveredListener listener){
        this.listener = listener;
        btAdapter.startDiscovery();
        Toast.makeText(this, "Scan started", Toast.LENGTH_SHORT).show();

        Runnable endScan = new Runnable() {
            @Override
            public void run() {
                stopScan();
                Toast.makeText(BluetoothService.this, "Scan stopped", Toast.LENGTH_SHORT).show();
            }
        };
        handler.postDelayed(endScan, SCAN_DURATION);
    }

    public void stopScan(){
        btAdapter.cancelDiscovery();
    }

    private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException {
            try {
                final Method m = device.getClass().getMethod("createInsecureRfcommSocketToServiceRecord", new Class[] { UUID.class });
                return (BluetoothSocket) m.invoke(device, MY_UUID);
            } catch (Exception e) {
                Log.e(TAG, "Could not create Insecure RFComm Connection", e);
            }

        return  device.createRfcommSocketToServiceRecord(MY_UUID);
    }

    public boolean connect(String address){
        boolean connected = false;

        stopScan();

        if(!BluetoothAdapter.checkBluetoothAddress(address)){
            //mHandler.sendEmptyMessage(BL_INCORRECT_ADDRESS);
            return false;
        }
        else {

            BluetoothDevice device = btAdapter.getRemoteDevice(address);
            try {
                btSocket = createBluetoothSocket(device);
            } catch (IOException e1) {
                Log.e(TAG, "In BT_Connect() socket create failed: " + e1.getMessage());
                //mHandler.sendEmptyMessage(BL_SOCKET_FAILED);
                return false;
            }

            if (btAdapter.isDiscovering()) {
                btAdapter.cancelDiscovery();
            }

            Log.d(TAG, "...Connecting...");
            try {
                btSocket.connect();
                connectedDevice = device;
                Log.d(TAG, "...Connection ok...");
            } catch (IOException e) {
                try {
                    btSocket.close();
                } catch (IOException e2) {
                    Log.e(TAG, "In BT_Connect() unable to close socket during connection failure" + e2.getMessage());
                    //mHandler.sendEmptyMessage(BL_SOCKET_FAILED);
                    return false;
                }
            }

            // Create a data stream so we can talk to server.
            Log.d(TAG, "...Create Socket...");

            try {
                outStream = btSocket.getOutputStream();
                connected = true;
            } catch (IOException e) {
                Log.e(TAG, "In BT_Connect() output stream creation failed:" + e.getMessage());
                //mHandler.sendEmptyMessage(BL_SOCKET_FAILED);
                return false;
            }
            /*if(listen_InStream) {		// whether to create a thread for the incoming data
                mConnectedThread = new ConnectedThread();
                mConnectedThread.start();
            }*/
        }
        return connected;
    }

    public void disconnect(){
        try{
            outStream.close();
        } catch (IOException e){

        }
        outStream = null;

        try {
            btSocket.close();
        } catch (IOException e){

        }
        btSocket = null;
    }

    public void sendData(String message) {
        byte[] msgBuffer = message.getBytes();

        Log.i(TAG, "Send data: " + message);

        if (outStream != null) {
            try {
                outStream.write(msgBuffer);
            } catch (IOException e) {
                Log.e(TAG, "Eception occurred during write: " + e.getMessage());
                //mHandler.sendEmptyMessage(BL_SOCKET_FAILED);
                return;
            }
        } else Log.e(TAG, "Error Send data: outStream is Null");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(receiver);
    }


    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action) && listener != null) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if(device.getName() != null) {
                    listener.OnBluetoothDeviceDiscovered(device);
                }
            }
        }
    };
}
