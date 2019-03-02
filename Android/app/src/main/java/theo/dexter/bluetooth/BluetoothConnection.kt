package theo.dexter.bluetooth

import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.os.Handler
import android.util.Log

import java.io.IOException
import java.io.InputStream
import java.nio.charset.Charset
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

    private var connectedThread : ConnectedThread? = null

    private var connected = false

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
                bluetoothSocket!!.connect()
                bluetoothConnectionListener.onConnect()
                Log.d(TAG, "Connected")
            } catch (connectException: IOException) {
                // Unable to connect; close the bluetoothSocket and get out
                Log.e(TAG, "Unable to connect to device", connectException)
                cancel()

                return
            }
            connected = true
            connectedThread = ConnectedThread(bluetoothSocket)
            connectedThread?.start()
        }

        /** Will cancel an in-progress connection, and close the bluetoothSocket  */
        fun cancel() {
            try {
                connected = false
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
            bluetoothSocket?.outputStream?.flush()
        }

        fun isConnected() : Boolean {
            return bluetoothSocket?.isConnected == true
        }
    }

    private inner class ConnectedThread(bluetoothSocket: BluetoothSocket) : Thread() {
        val inputStream : InputStream = bluetoothSocket.inputStream
        val buffer = ByteArray(1024)

        override fun run() {
            while (connected) {
                inputStream.read(buffer)
                //Log.d(TAG, "Received Message ${String(buffer, Charset.defaultCharset())}")
            }
        }

    }

    companion object {

        private val TAG = BluetoothConnection::class.java.simpleName

        private val MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
    }
}
