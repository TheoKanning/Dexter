package theo.dexter.ui.activity;

import android.os.Bundle;

import theo.dexter.R;
import theo.dexter.ui.fragment.ControlFragment;

public class PlayActivity extends BaseActivity {

    public static final String EXTRA_ADDRESS = "address";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);

        setFragment(new ControlFragment(), true);
    }

}
