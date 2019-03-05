package theo.dexter.control

class ControlCalculator {

    companion object {
        const val accelThreshold = 1.0 // minimum acceleration to give a response
        const val accelMax = 5.0 // truncate any acceleration beyond this
        const val linearMax = 0.125 // maximum linear velocity (m/s)
        const val angularMax = 1 // maximum angular velocity (rad/sec)
    }

    /**
     * Calculates linear and angular velocity commands from x and y accel data. Each accel value
     * is constrained for safety, then multiplied to be within the desired speed range.
     */
    fun calculateControlCommand(xAccel: Double, yAccel: Double): ControlCommand {

        val xConstrained = constrain(threshold(xAccel))
        val yConstrained = -constrain(threshold(yAccel)) // negative yAccel means tilting forward

        val linear = round(yConstrained / accelMax * linearMax)
        val angular = round(xConstrained / accelMax * angularMax)

        return ControlCommand(linear, angular)
    }

    /**
     * Constrains an accel values to be within safe range
     */
    private fun constrain(number: Double): Double {
        return Math.min(Math.max(number, -accelMax), accelMax)
    }

    /**
     * Sets small accel values to zero and subtracts the threshold from larger values.
     */
    private fun threshold(accel: Double): Double =
            if ((-accelThreshold < accel) && (accel < accelThreshold)) {
                0.0
            } else if (accel > 0) {
                accel - accelThreshold
            } else {
                accel + accelThreshold
            }

    /**
     * Rounds to three decimal places
     */
    private fun round(speed: Double) = Math.round(speed * 1000.0) / 1000.0
}
