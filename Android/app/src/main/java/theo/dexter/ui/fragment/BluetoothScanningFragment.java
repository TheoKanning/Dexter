package theo.dexter.ui.fragment;


import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import javax.inject.Inject;

import butterknife.ButterKnife;
import theo.dexter.DexterApplication;
import theo.dexter.R;
import theo.dexter.bluetooth.BluetoothScanner;
import theo.dexter.ui.activity.PlayActivity;

/**
 * Fragment to choose from paired bluetooth devices
 */
public class BluetoothScanningFragment extends BaseFragment implements BluetoothScanner.OnBluetoothDeviceDiscoveredListener {

    private static final int REQUEST_ENABLE_BT = 1;

    @Inject
    BluetoothScanner bluetoothScanner;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_bluetooth, container, false);
        ((DexterApplication) getActivity().getApplication()).getComponent().inject(this);
        ButterKnife.bind(this, view);

        //Turn on Bluetooth
        if (!BluetoothAdapter.getDefaultAdapter().isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        scan();
    }

    /**
     * Clears all data and begins new scan
     */
    private void scan() {
        bluetoothScanner.startScan(this);
    }


    @Override
    public void onBluetoothDeviceDiscovered(BluetoothDevice device) {

        if (device.getName().equals("Dexter")) {
            bluetoothScanner.stopScan();

            Intent i = new Intent(getActivity(), PlayActivity.class);
            i.putExtra(ControlFragment.ADDRESS_EXTRA, device.getAddress());
            startActivity(i);
        }
    }
}
