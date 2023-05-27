package net.kcci.HomeIot;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.fragment.app.Fragment;


import java.text.SimpleDateFormat;
import java.util.Timer;
import java.util.TimerTask;

public class Fragment3Telnet extends Fragment {
    MainActivity mainActivity;
    ClientThread clientThread;
    TextView textView;
    ToggleButton toggleButtonStart;
    WebView webView;
    SimpleDateFormat dataFormat = new SimpleDateFormat("yy.MM.dd HH:mm:ss");
    Timer timer;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment3telnet, container, false);
        Button buttonSend = view.findViewById(R.id.buttonSend);

        textView = view.findViewById(R.id.textView);
        mainActivity = (MainActivity) getActivity();
        webView = view.findViewById(R.id.webview);
        webView.setWebViewClient(new WebViewClient());
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setUseWideViewPort(true); // 1. 스케일 모드 설정
        webSettings.setLoadWithOverviewMode(true); // 2. 스케일 모드 설정
        webSettings.setUseWideViewPort(true); // WebView의 폭을 화면 크기에 맞게 조정합니다.


        webView.loadUrl("http://10.10.141.36");

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (clientThread == null) {
                    clientThread = new ClientThread();
                }
                if (clientThread != null && ClientThread.socket != null) {
                    String strData = "[ALLMSG]OFF\n";
                    clientThread.sendData(strData);
                } else {
                    Toast.makeText(getActivity(), "login 확인", Toast.LENGTH_SHORT).show();
                }
            }
        });
        return view;
    }

    void recvDataProcess(String data) {


        String[] splitLists = data.toString().split("\\[|]|@|\\n");

        if (splitLists[2].equals("ON")) {
            timer = new Timer();
            long startTime = System.currentTimeMillis(); // 시작 시간을 기록합니다.
            timer.scheduleAtFixedRate(new TimerTask() {
                public void run() {
                    // 매 초마다 호출되는 코드
                    long elapsedTime = System.currentTimeMillis() - startTime;
                    long elapsedSeconds = elapsedTime / 1000;
                    getActivity().runOnUiThread(new Runnable() {
                        public void run() {
                            textView.setText("경과시간: " + elapsedSeconds + " seconds");
                        }
                    });
                }
            }, 0, 1000);
        }
        else if(splitLists[2].equals("OFF")){
            stopTimer();
        }
    }

    public void stopTimer() {
        if (timer != null) {
            timer.cancel(); // 타이머를 취소합니다.
            timer = null;
        }
    }
}

