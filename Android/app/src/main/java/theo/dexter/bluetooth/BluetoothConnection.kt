package theo.dexter.bluetooth

import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.os.Handler
import android.util.Log

import java.io.IOException
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

    interface BluetoothConnectionListener {
        fun onConnect()
        fun onDisconnect()
    }

    init {
        connectThread = ConnectThread(device)
        bluetoothHandler.post(connectThread)
    }

    fun write(message: String) {
        connectThread.write(message)
    }

    fun disconnect() {
        connectThread.cancel()
    }

    fun isConnected() : Boolean {
        return connectThread.isConnected()
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

            Log.d(TAG, "ConnectThread initialized")

            bluetoothSocket = tmp
        }

        override fun run() {

            try {
                // Connect the device through the bluetoothSocket. This will block
                // until it succeeds or throws an exception
                Log.d(TAG, "Connecting")
                bluetoothSocket!!.connect() // todo this is somehow blocking the UI
                bluetoothConnectionListener.onConnect()
                Log.d(TAG, "Connected")
            } catch (connectException: IOException) {
                // Unable to connect; close the bluetoothSocket and get out
                Log.e(TAG, "Unable to connect to device", connectException)
                cancel()

                return
            }
        }

        /** Will cancel an in-progress connection, and close the bluetoothSocket  */
        fun cancel() {
            try {
                bluetoothSocket?.outputStream?.close()
                bluetoothSocket?.inputStream?.close()
                bluetoothSocket?.close()
                bluetoothConnectionListener.onDisconnect()
            } catch (closeException: IOException) {
                Log.e(TAG, "Unable to close BluetoothSocket", closeException)
            }

        }

        fun write(message: String) {
            bluetoothSocket?.outputStream?.write(message.toByteArray())
        }

        fun isConnected() : Boolean {
            return bluetoothSocket?.isConnected == true
        }
    }

    companion object {

        private val TAG = BluetoothConnection::class.java.simpleName

        private val MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
    }
}
