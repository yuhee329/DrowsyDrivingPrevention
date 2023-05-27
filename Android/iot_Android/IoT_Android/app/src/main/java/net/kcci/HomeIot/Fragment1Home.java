package net.kcci.HomeIot;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;

import androidx.fragment.app.Fragment;


public class Fragment1Home extends Fragment {
    MainActivity mainActivity;
    ImageButton imageButtonLamp;
    ImageButton imageButtonPlug;
    boolean imageButtonLampCheck;
    boolean imageButtonPlugCheck;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment1home, container, false);
        imageButtonLamp = view.findViewById(R.id.imageButtonLamp);
        imageButtonPlug = view.findViewById(R.id.imageButtonPlug);
        mainActivity = (MainActivity)getActivity();
        imageButtonLamp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(ClientThread.socket != null) {
                    if (imageButtonLampCheck)
                        mainActivity.clientThread.sendData(ClientThread.cortexm4 + "LAMPOFF\n");
                    else
                        mainActivity.clientThread.sendData(ClientThread.cortexm4 + "LAMPON\n");
                }
            }
        });
        imageButtonPlug = view.findViewById(R.id.imageButtonPlug);
        imageButtonPlug.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(ClientThread.socket != null) {
                    if (imageButtonPlugCheck)
                        mainActivity.clientThread.sendData(ClientThread.cortexm4 + "PLUGOFF\n");
                    else
                        mainActivity.clientThread.sendData(ClientThread.cortexm4 + "PLUGON\n");
                }
            }
        });
        return view;
    }
    void recvDataProcess(String strRecvData)
    {
        String[] splitLists = strRecvData.toString().split("\\[|]|@|\\n") ;
        for(int i=0; i< splitLists.length; i++)
            Log.d("recvDataProcess"," i: " + i + ", value: " +splitLists[i]);
/*        if(splitLists[2].equals("LAMPON")) {
            imageButtonLamp.setImageResource(R.drawable.lamp_on);
            imageButtonLampCheck = true;
        } else if(splitLists[2].equals("LAMPOFF")) {
            imageButtonLamp.setImageResource(R.drawable.lamp_off);
            imageButtonLampCheck = false;
        } else if(splitLists[2].equals("PLUGON")) {
            imageButtonPlug.setImageResource(R.drawable.plug_on);
            imageButtonPlugCheck = true;
        } else if(splitLists[2].equals("PLUGOFF")) {
            imageButtonPlug.setImageResource(R.drawable.plug_off);
            imageButtonPlugCheck = false;
        }
*/
        buttonUpdate(splitLists[2]);
        if(splitLists.length == 3) {
            String[] insertArgs = new String[3];
            insertArgs[0] = splitLists[1];          //"KSH_ARD"
            int index = splitLists[2].indexOf('O');
            String tempStr = splitLists[2].substring(0,index);
            insertArgs[1] = tempStr;                //"LAMP"
            insertArgs[2] = splitLists[2].substring(index);
            Log.d("database", " 0 : " + insertArgs[0] + " 1 : " + insertArgs[1] + " 2 : " + insertArgs[2] );
            mainActivity.database.updateRecordParam(mainActivity.SQLdb, insertArgs);
        }
    }
    @Override
    public void onStart() {
        super.onStart();
        String rstStr[] = mainActivity.database.executeRawQueryParam(mainActivity.SQLdb, "LAMP");
        buttonUpdate(rstStr[3] + rstStr[4]);        //LAMP + ON
        rstStr = mainActivity.database.executeRawQueryParam(mainActivity.SQLdb, "PLUG");
        buttonUpdate(rstStr[3] + rstStr[4]);        //PLUG + ON
//      Log.d("MainActivity", rstStr[0] + " ," + rstStr[1] + " ," + rstStr[2] + " ," + rstStr[3] + " ," + rstStr[4]);
    }
    public void buttonUpdate(String cmd)
    {
        Log.d("MainActivity ", cmd);
        if (cmd.equals("LAMPON")) {
            imageButtonLamp.setImageResource(R.drawable.lamp_on);
            imageButtonLamp.setBackgroundColor(Color.GREEN);
            imageButtonLampCheck = true;
        } else if (cmd.equals("LAMPOFF")) {
            imageButtonLamp.setImageResource(R.drawable.lamp_off);
            imageButtonLamp.setBackgroundColor(Color.LTGRAY);
            imageButtonLampCheck = false;
        } else if (cmd.equals("PLUGON")) {
            imageButtonPlug.setImageResource(R.drawable.plug_on);
            imageButtonPlug.setBackgroundColor(Color.GREEN);
            imageButtonPlugCheck = true;
        } else if (cmd.equals("PLUGOFF")) {
            imageButtonPlug.setImageResource(R.drawable.plug_off);
            imageButtonPlug.setBackgroundColor(Color.LTGRAY);
            imageButtonPlugCheck = false;
        }
    }
}
