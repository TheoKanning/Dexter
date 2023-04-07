package theo.dexter.ui.fragment


import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import androidx.core.app.ActivityCompat

import theo.dexter.R
import theo.dexter.bluetooth.BluetoothConnection
import theo.dexter.bluetooth.BluetoothScanner
import theo.dexter.control.ControlCalculator
import kotlinx.android.synthetic.main.fragment_control.*

/**
 * Fragment for steering Dexter
 */
class ControlFragment : Fragment(), BluetoothConnection.BluetoothConnectionListener, BluetoothScanner.DexterScanListener {

    private lateinit var bluetoothScanner: BluetoothScanner

    private var bluetoothConnection: BluetoothConnection? = null

    private lateinit var sensorManager: SensorManager
    private lateinit var accel: Sensor

    private val calculator = ControlCalculator()

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_control, container, false)

        bluetoothScanner = BluetoothScanner(requireContext())

        sensorManager = requireActivity().getSystemService(Context.SENSOR_SERVICE) as SensorManager
        accel = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)

        return view
    }

    override fun onResume() {
        super.onResume()

        if (!BluetoothAdapter.getDefaultAdapter().isEnabled) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT)
        }

        startSearching()

        start.setOnClickListener { startDriving() }
        stop.setOnClickListener { stopDriving() }
    }

    override fun onPause() {
        super.onPause()
        stopDriving()
        bluetoothConnection?.disconnect()
    }

    private val sensorListener = object: SensorEventListener {
        override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {}

        override fun onSensorChanged(event: SensorEvent) {
            if (bluetoothConnection?.isConnected() != true) {
                Log.d(TAG, "BluetoothConnection.isConnected returned false")
                return
            }

            val xRaw: Float
            val yRaw: Float        // RAW-value from Accelerometer sensor

            val windowMgr = activity!!.getSystemService(Context.WINDOW_SERVICE) as WindowManager
            val rotationIndex = windowMgr.defaultDisplay.rotation
            if (rotationIndex == 1 || rotationIndex == 3) {            // detect 90 or 270 degree rotation
                xRaw = -event.values[1]
                yRaw = event.values[0]
            } else {
                xRaw = event.values[0]
                yRaw = event.values[1]
            }

            val command = calculator.calculateControlCommand(xRaw.toDouble(), yRaw.toDouble())

            bluetoothConnection?.write(command.toString())
            Log.d(TAG, command.toString())
        }
    }

    override fun onConnect() {
        drivingLayout.visibility = View.GONE
        pausedLayout.visibility = View.VISIBLE
        searchingLayout.visibility = View.GONE
        Log.i(TAG, "Connected")
    }

    override fun onDisconnect() {
        Log.i(TAG, "Disconnected")
    }

    override fun onDexterFound(device: BluetoothDevice) {
        Log.d(TAG, "Dexter found, connecting...")
        bluetoothConnection = BluetoothConnection(device, this)
    }

    private fun startDriving() {
        drivingLayout.visibility = View.VISIBLE
        pausedLayout.visibility = View.GONE
        searchingLayout.visibility = View.GONE
        sensorManager.registerListener(sensorListener, accel, 100000)
    }

    private fun stopDriving() {
        drivingLayout.visibility = View.GONE
        pausedLayout.visibility = View.VISIBLE
        searchingLayout.visibility = View.GONE
        sensorManager.unregisterListener(sensorListener)
    }

    private fun startSearching() {
        bluetoothScanner.findDexter(this)
        drivingLayout.visibility = View.GONE
        pausedLayout.visibility = View.GONE
        searchingLayout.visibility = View.VISIBLE
    }

    companion object {
        private val TAG = "ControlFragment"
        private val REQUEST_ENABLE_BT = 1
    }
}
