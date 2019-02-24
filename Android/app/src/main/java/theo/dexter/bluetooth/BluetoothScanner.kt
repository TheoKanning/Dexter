package theo.dexter.bluetooth

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Handler
import android.util.Log

/**
 * Class to handle all bluetooth scanning
 *
 * @author Theo Kanning
 */
class BluetoothScanner(private val context: Context) {

    private val btAdapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

    private val handler: Handler = Handler()

    private var listener: DexterScanListener? = null

    private val endScan: Runnable

    private val receiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            val action = intent.action

            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND == action) {
                val device = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                if (device.name == DEXTER_NAME) {
                    Log.d(TAG, "Scan found Dexter")
                    listener?.onDexterFound(device)
                    stopScan()
                }
            }
        }
    }

    interface DexterScanListener {
        fun onDexterFound(device: BluetoothDevice)
    }

    init {
        endScan = Runnable {
            stopScan()
        }
    }

    fun findDexter(listener: DexterScanListener) {
        this.listener = listener

        // search for bonded devices and return early if Dexter is found
        for (device in btAdapter.bondedDevices) {
            if (device.name == DEXTER_NAME) {
                Log.d(TAG, "Found Dexter in bonded devices")
                listener.onDexterFound(device)
                return
            }
        }

        // Dexter is not paired, start discovery
        startScan()
    }

    private fun startScan() {
        Log.d(TAG, "Starting bluetooth scan")
        // Register the BroadcastReceiver
        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        context.applicationContext.registerReceiver(receiver, filter)
        btAdapter.startDiscovery()

        handler.postDelayed(endScan, SCAN_DURATION.toLong())
    }

    private fun stopScan() {
        btAdapter.cancelDiscovery()
        context.applicationContext.unregisterReceiver(receiver)
        handler.removeCallbacks(endScan)
    }

    companion object {
        private const val TAG = "BluetoothScanner"
        private const val SCAN_DURATION = 10000 //10s
        private const val DEXTER_NAME = "Dexter"
    }
}

