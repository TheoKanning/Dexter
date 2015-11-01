package theo.dexter;

import android.app.Application;

import theo.dexter.dagger.DaggerDexterComponent;
import theo.dexter.dagger.DexterComponent;
import theo.dexter.dagger.DexterModule;

public class DexterApplication extends Application {

    private DexterComponent component;

    @Override
    public void onCreate() {
        super.onCreate();

        component = DaggerDexterComponent.builder()
                .dexterModule(new DexterModule(this))
                .build();
        component.inject(this);

    }

    public DexterComponent getComponent() {
        return component;
    }
}
