package theo.dexter.dagger;

import android.content.Context;

import dagger.Module;

@Module
public class DexterModule {

    Context context;

    public DexterModule(Context context) {
        this.context = context;
    }

}
