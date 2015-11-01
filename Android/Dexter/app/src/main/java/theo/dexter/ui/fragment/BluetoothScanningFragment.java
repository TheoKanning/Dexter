package theo.dexter.ui.fragment;


import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;

import butterknife.Bind;
import butterknife.ButterKnife;
import theo.dexter.DexterApplication;
import theo.dexter.R;
import theo.dexter.bluetooth.BluetoothScanner;
import theo.dexter.ui.activity.PlayActivity;

/**
 * Fragment to choose from paired bluetooth devices
 */
public class BluetoothScanningFragment extends BaseFragment implements BluetoothScanner.OnBluetoothDeviceDiscoveredListener, AdapterView.OnItemClickListener {

    private static final int REQUEST_ENABLE_BT = 1;

    @Bind(R.id.device_list)
    ListView deviceList;

    private ArrayAdapter<String> adapter;
    private ArrayList<BluetoothDevice> devices;

    private BluetoothScanner bluetoothScanner;

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

        bluetoothScanner = new BluetoothScanner(getContext());

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
        //btService.stopScan();

        devices = new ArrayList<>();
        adapter = new ArrayAdapter<>(getContext(), android.R.layout.simple_list_item_1, new ArrayList<String>());
        deviceList.setAdapter(adapter);
        deviceList.setOnItemClickListener(this);

        bluetoothScanner.startScan(this);

        //btService.startScan(this);
    }


    @Override
    public void OnBluetoothDeviceDiscovered(BluetoothDevice device) {
        devices.add(device);
        adapter.add(device.getName() + "\n" + device.getAddress());
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        bluetoothScanner.stopScan();

        BluetoothDevice selected = devices.get(position);

        Intent i = new Intent(getActivity(), PlayActivity.class);
        i.putExtra(ControlFragment.ADDRESS_EXTRA, selected.getAddress());
        startActivity(i);
    }

    @Override
    public int getTitleResourceId() {
        return R.string.fragment_bluetooth_title;
    }
}
