package theo.dexter.bluetooth

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Handler

/**
 * Class to handle all bluetooth scanning
 *
 * @author Theo Kanning
 */
class BluetoothScanner(private val context: Context) {

    private val btAdapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

    private val handler: Handler = Handler()

    private var listener: OnBluetoothDeviceDiscoveredListener? = null

    private val endScan: Runnable

    private val receiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            val action = intent.action

            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND == action) {
                val device = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                if (device.name != null) {
                    listener?.onBluetoothDeviceDiscovered(device)
                }
            }
        }
    }

    interface OnBluetoothDeviceDiscoveredListener {
        fun onBluetoothDeviceDiscovered(device: BluetoothDevice)
    }

    init {

        // Register the BroadcastReceiver
        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        context.applicationContext.registerReceiver(receiver, filter)
        endScan = Runnable {
            btAdapter.cancelDiscovery()
            context.applicationContext.unregisterReceiver(receiver)
        }
    }

    fun startScan(listener: OnBluetoothDeviceDiscoveredListener) {
        this.listener = listener
        btAdapter.startDiscovery()
        for (device in btAdapter.bondedDevices) {
            listener.onBluetoothDeviceDiscovered(device)
        }

        handler.postDelayed(endScan, SCAN_DURATION.toLong())
    }

    fun stopScan() {
        btAdapter.cancelDiscovery()
        handler.removeCallbacks(endScan)
    }

    companion object {
        private val SCAN_DURATION = 10000 //10s
    }
}

