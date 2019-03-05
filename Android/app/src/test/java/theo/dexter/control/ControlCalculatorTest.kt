package theo.dexter.control

import org.junit.Assert.assertEquals

import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized

@RunWith(Parameterized::class)
class ControlCalculatorTest(val xAccel: Double,
                            val yAccel: Double,
                            val linear: Double,
                            val angular: Double,
                            val description: String) {

    private val delta = 0.001

    companion object {
        @JvmStatic
        @Parameterized.Parameters(name = "{index}: {4}")
        fun data(): Collection<Array<Any>> {
            return listOf(
                    arrayOf(0.0, 0.0, 0.0, 0.0, "Zero acceleration equals zero speed"),
                    arrayOf(0.5, -0.5, 0.0, 0.0, "Under threshold equals zero speed"),
                    arrayOf(1.0, -1.0, 0.0, 0.0, "At threshold equals zero speed"),
                    arrayOf(2.5, -2.0, 0.025, 0.3, "Moderate speed"),
                    arrayOf(-3.5, 3.0, -0.05, -0.5, "Moderate speed"),
                    arrayOf(8.0, -6.0, 0.125, 1, "Above max acceleration get truncated")
            )
        }
    }

    @Test
    fun test() {
        val result = ControlCalculator().calculateControlCommand(xAccel, yAccel)
        assertEquals(description, linear, result.linearVelocity, delta)
        assertEquals(description, angular, result.angularVelocity, delta)
    }
}
