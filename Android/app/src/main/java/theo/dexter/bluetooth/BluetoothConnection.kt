package theo.dexter.bluetooth

import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.os.Handler
import android.util.Log

import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.util.UUID

/**
 * Class for handling connection to a single device
 * Largely taken from google documentation
 * http://developer.android.com/guide/topics/connectivity/bluetooth.html
 *
 * @author Theo Kanning
 */
class BluetoothConnection(device: BluetoothDevice, private val bluetoothConnectionListener: BluetoothConnectionListener) {

    private val bluetoothHandler: Handler = Handler()

    private val connectThread: ConnectThread

    private var connectedThread: ConnectedThread? = null

    var isConnected = false
        private set

    interface BluetoothConnectionListener {
        fun onConnect()
        fun onDisconnect()
    }

    init {
        this.connectThread = ConnectThread(device)
        bluetoothHandler.post(connectThread)
    }

    fun write(message: String) {
        connectedThread?.write(message)
    }

    fun disconnect() {
        connectThread.cancel()
    }

    private inner class ConnectThread(bluetoothDevice: BluetoothDevice) : Thread() {
        private val bluetoothSocket: BluetoothSocket?

        init {
            // Use a temporary object that is later assigned to bluetoothSocket,
            // because bluetoothSocket is final
            var tmp: BluetoothSocket? = null

            Log.d(TAG, "Initializing ConnectThread, address = " + bluetoothDevice.address)

            // Get a BluetoothSocket to connect with the given BluetoothDevice
            try {
                tmp = bluetoothDevice.createRfcommSocketToServiceRecord(MY_UUID)
            } catch (e: IOException) {
                Log.e(TAG, "Could not create socket", e)
            }

            bluetoothSocket = tmp
        }

        override fun run() {

            try {
                // Connect the device through the bluetoothSocket. This will block
                // until it succeeds or throws an exception
                bluetoothSocket!!.connect()
                bluetoothConnectionListener.onConnect()
            } catch (connectException: IOException) {
                // Unable to connect; close the bluetoothSocket and get out
                Log.e(TAG, "Unable to connect to device", connectException)
                try {
                    bluetoothSocket!!.close()
                } catch (closeException: IOException) {
                }

                return
            }

            isConnected = true

            // Do work to manage the connection (in a separate thread)
            connectedThread = ConnectedThread(bluetoothSocket)
        }

        /** Will cancel an in-progress connection, and close the bluetoothSocket  */
        fun cancel() {
            try {
                connectedThread?.cancel()
                isConnected = false
                bluetoothConnectionListener.onDisconnect()
            } catch (e: IOException) {
            }

        }
    }

    private inner class ConnectedThread(private val bluetoothSocket: BluetoothSocket) : Thread() {
        private val inputStream: InputStream = bluetoothSocket.inputStream
        private val outputStream: OutputStream = bluetoothSocket.outputStream
        private val buffer: ByteArray = ByteArray(1024)

        override fun run() {
            var bytes: Int // bytes returned from read()

            // Keep listening to the InputStream until an exception occurs
            while (true) {
                try {
                    // Read from the InputStream
                    bytes = inputStream.read(buffer)
                    // Send the obtained bytes to the UI activity
                    bluetoothHandler.obtainMessage(MESSAGE_RECEIVED, bytes, -1, buffer)
                            .sendToTarget()
                } catch (e: IOException) {
                    break
                }

            }
        }

        /* Call this from the main activity to send data to the remote device */
        fun write(message: String) {
            try {
                outputStream.write(message.toByteArray())
            } catch (e: IOException) {
                Log.e(TAG, "Error writing message", e)
            }
        }

        /* Call this from the main activity to shutdown the connection */
        fun cancel() {
            try {
                bluetoothSocket.close()
            } catch (e: IOException) {
            }

        }
    }

    companion object {

        private val TAG = BluetoothConnection::class.java.simpleName

        private val MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

        private val MESSAGE_RECEIVED = 1
    }
}
