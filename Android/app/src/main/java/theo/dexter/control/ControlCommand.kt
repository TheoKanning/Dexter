package theo.dexter.control

class ControlCommand(val linearVelocity: Double, // linear velocity in m/s
                     val angularVelocity: Double) { // angular velocity in rad/sec

    override fun toString(): String {
        return "L$linearVelocity\n\r" +
                "A$angularVelocity\n\r"
    }
}

