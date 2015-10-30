package theo.dexter;

import android.app.Application;
import android.content.Intent;

import theo.dexter.bluetooth.BluetoothService;
import theo.dexter.dagger.DaggerDexterComponent;
import theo.dexter.dagger.DexterModule;
import theo.dexter.dagger.DexterComponent;

public class DexterApplication extends Application {

    private DexterComponent component;

    @Override
    public void onCreate() {
        super.onCreate();

        component = DaggerDexterComponent.builder()
                .dexterModule(new DexterModule(this))
                .build();
        component.inject(this);

        startService(new Intent(this, BluetoothService.class));
    }

    public DexterComponent getComponent() {
        return component;
    }
}
