package theo.dexter.ui.activity;

import android.os.Bundle;

import theo.dexter.R;
import theo.dexter.ui.fragment.ControlFragment;

public class MainActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setFragment(new ControlFragment(), true);
    }
}
