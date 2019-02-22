package theo.dexter.ui.fragment


import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.os.Bundle
import android.support.v4.app.Fragment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.Toast

import theo.dexter.R
import theo.dexter.bluetooth.BluetoothConnection
import theo.dexter.bluetooth.BluetoothScanner
import theo.dexter.control.ControlCalculator
import kotlinx.android.synthetic.main.fragment_control.*

/**
 * Fragment for steering Dexter
 */
class ControlFragment : Fragment(), SensorEventListener, BluetoothConnection.BluetoothConnectionListener, BluetoothScanner.OnBluetoothDeviceDiscoveredListener {

    private lateinit var bluetoothScanner: BluetoothScanner

    private var bluetoothConnection: BluetoothConnection? = null

    private lateinit var sensorManager: SensorManager
    private var accel: Sensor? = null
    private val calculator = ControlCalculator()

    private var parked = true

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_control, container, false)

        bluetoothScanner = BluetoothScanner(context!!)

        sensorManager = activity!!.getSystemService(Context.SENSOR_SERVICE) as SensorManager
        accel = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)

        return view
    }

    override fun onResume() {
        super.onResume()
        stopDriving()

        if (!BluetoothAdapter.getDefaultAdapter().isEnabled) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT)
        }

        sensorManager.registerListener(this, accel, SensorManager.SENSOR_DELAY_NORMAL)
        bluetoothScanner.startScan(this)

        start.setOnClickListener { stopDriving() }
        stop.setOnClickListener { startDriving() }
    }

    override fun onPause() {
        super.onPause()
        sensorManager.unregisterListener(this)
        bluetoothConnection?.disconnect()
    }

    override fun onSensorChanged(e: SensorEvent) {

        if (bluetoothConnection != null && !bluetoothConnection!!.isConnected) {
            Log.d(TAG, "BluetoothConnection.isConnected returned false")
            return
        }

        if (parked) {
            return
        }

        val xRaw: Float
        val yRaw: Float        // RAW-value from Accelerometer sensor

        val windowMgr = activity!!.getSystemService(Context.WINDOW_SERVICE) as WindowManager
        val rotationIndex = windowMgr.defaultDisplay.rotation
        if (rotationIndex == 1 || rotationIndex == 3) {            // detect 90 or 270 degree rotation
            xRaw = -e.values[1]
            yRaw = e.values[0]
        } else {
            xRaw = e.values[0]
            yRaw = e.values[1]
        }

        val command = calculator.calculateControlCommand(xRaw.toDouble(), yRaw.toDouble())

        val cmdSendL = COMMAND_LINEAR + command.linearVelocity + "\r\n"
        val cmdSendR = COMMAND_ANGULAR + command.angularVelocity + "\r\n"

        bluetoothConnection?.write(cmdSendL + cmdSendR)
        Log.d(TAG, cmdSendL + cmdSendR)
    }

    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {

    }

    override fun onConnect() {
        Toast.makeText(context, "Connected!", Toast.LENGTH_SHORT).show()
        Log.i(TAG, "Connected")
    }

    override fun onDisconnect() {
        Toast.makeText(context, "Disconnected...", Toast.LENGTH_SHORT).show()
        Log.i(TAG, "Disconnected")
    }

    override fun onBluetoothDeviceDiscovered(device: BluetoothDevice) {
        if (device.name == "Dexter") {
            bluetoothScanner.stopScan()
            bluetoothConnection = BluetoothConnection(device, this)
        }
    }

    private fun startDriving() {
        start.visibility = View.GONE
        stop.visibility = View.VISIBLE
        parked = false
    }

    private fun stopDriving() {
        start.visibility = View.VISIBLE
        stop.visibility = View.GONE
        parked = true
    }

    companion object {

        private val TAG = "ControlFragment"
        private val REQUEST_ENABLE_BT = 1

        private val COMMAND_LINEAR = "L"
        private val COMMAND_ANGULAR = "A"
    }
}
