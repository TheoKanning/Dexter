package theo.dexter.dagger;

import android.app.Application;

import javax.inject.Singleton;

import dagger.Component;
import theo.dexter.ui.fragment.BluetoothScanningFragment;
import theo.dexter.ui.fragment.ControlFragment;

@Singleton
@Component(modules = DexterModule.class)
public interface DexterComponent {
    //activities

    //application
    void inject(Application application);

    //fragments
    void inject(BluetoothScanningFragment bluetoothScanningFragment);

    void inject(ControlFragment controlFragment);
}
