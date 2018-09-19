package theo.dexter.ui.activity;

import android.os.Bundle;

import theo.dexter.R;
import theo.dexter.ui.fragment.ControlFragment;

public class PlayActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);

        ControlFragment fragment = new ControlFragment();
        fragment.setArguments(getIntent().getExtras());
        setFragment(fragment, true);
    }
}
