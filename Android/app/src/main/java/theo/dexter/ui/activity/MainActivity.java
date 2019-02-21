package theo.dexter.ui.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import theo.dexter.R;
import theo.dexter.ui.fragment.ControlFragment;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getSupportFragmentManager()
                .beginTransaction()
                .add(R.id.fragment_container, new ControlFragment())
                .commit();
    }
}
