package theo.dexter.ui.activity

import android.os.Bundle
import android.support.v7.app.AppCompatActivity

import theo.dexter.R
import theo.dexter.ui.fragment.ControlFragment

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        supportFragmentManager
                .beginTransaction()
                .add(R.id.fragment_container, ControlFragment())
                .commit()
    }
}
