#include <WiFiEsp.h>
#include <SoftwareSerial.h>
#include <MsTimer2.h>
#include <Arduino.h>
#include <stdio.h>

#define AP_SSID "iot0"
#define AP_PASS "iot00000"
#define SERVER_NAME "10.10.141.64"
#define SERVER_PORT 6000
#define LOGID "Driver"
#define PASSWD "PASSWD"

#define WIFIRX 6  //6:RX-->ESP8266 TX
#define WIFITX 7  //7:TX -->ESP8266 RX
#define trig 9
#define echo 10

#define CMD_SIZE 50
#define ARR_CNT 5
#define CARNAME "GENESIS"

//Wifi
WiFiEspClient client;
SoftwareSerial wifiSerial(WIFIRX, WIFITX);
char sendId[10] = "YHCI_SQL";
char sendBuf[CMD_SIZE];
char getSensorId[10];
int recvBuf;
long duration, cm;
bool offFlag = 0;


//timer
bool timerIsrFlag = false;
unsigned int secCount;
int sensorTime;
int cnt;

//vibration sensor
int vib_sensor = 12;
int vib_Data;
bool tpFlag = 0;



void setup() {
  Serial.begin(19200);
  wifi_Setup();
  pinMode(vib_sensor, INPUT);  //센서핀 입력
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  MsTimer2::set(1000, timerIsr);  // 1000ms period
  MsTimer2::start();
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
void loop() {

  send_TA();
  if (client.available()) {
    socketEvent();
  }
  if (timerIsrFlag)  //timerIsrFlag가 1일 때 작동.
  {
    timerIsrFlag = false;
    if (!(secCount % 3))  //3초에 한번씩 실행
    {
      digitalWrite(trig, LOW);  //초음파 센서로 거리측정
      delayMicroseconds(2);
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);
      duration = pulseIn(echo, HIGH);
      cm = microsecondsToCentimeters(duration);
      sprintf(sendBuf, "[%s]SENSOR@%s@%d\n", sendId, CARNAME, cm);  //서버로 센서값 전송
      client.write(sendBuf, strlen(sendBuf));                       //예시 : [YHCI_SQL]SENSOR@GENSIS@10
      client.flush();

      if (cm > 7 && offFlag == 0) {  //7cm 이상 거리가 늘어나면 cnt 증가
        cnt++;
        if (cnt >= 5) {                      //cnt가 5회 이상일 경우
          sprintf(sendBuf, "[ALLMSG]ON\n");  //서버로 신호를 보내고 졸음 방지센서 작동
          client.write(sendBuf, strlen(sendBuf));
          client.flush();
          offFlag = 1;
          cnt = 0;
        }
      }
    }
  }
}

void socketEvent() {
  int i = 0;
  char* pToken;
  char* pArray[ARR_CNT] = { 0 };
  char recvBuf[CMD_SIZE] = { 0 };
  int len;

  sendBuf[0] = '\0';
  len = client.readBytesUntil('\n', recvBuf, CMD_SIZE);
  client.flush();
  Serial.print("recv : ");
  Serial.print(recvBuf);

  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL) {
    pArray[i] = pToken;
    if (++i >= ARR_CNT)
      break;
    pToken = strtok(NULL, "[@]");
  }

  if (pArray[1] != NULL) {
    if (strcmp(pArray[1], "OFF") == 0) {  // 서버에서 받은 메시지가 "OFF"인 경우
      offFlag = 0;
      tpFlag = 0;  // offFlag를 0으로 설정하여 실행을 멈춤
    }
    return;
  }
}

void timerIsr() {
  timerIsrFlag = true;
  secCount++;
}

void wifi_Setup() {
  wifiSerial.begin(19200);
  wifi_Init();
  server_Connect();
}

void wifi_Init() {
  {
    WiFi.init(&wifiSerial);
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present");
    }
  }

  while (WiFi.begin(AP_SSID, AP_PASS) != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(AP_SSID);
  }
}
int server_Connect() {

  Serial.println("Starting connection to server...");

  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    Serial.println("Connect to server");
    client.print("[" LOGID ":" PASSWD "]");
  } else {
    Serial.println("server connection failure");
  }
}

void send_TA() {

  vib_Data = digitalRead(vib_sensor);  //진동센서 측정

  if ((vib_Data == 1) && tpFlag == 0) {  //진동감지 될 경우 TA 신호 전송
    sprintf(sendBuf, "[ALLMSG]TA\n");
    client.write(sendBuf, strlen(sendBuf));
    sprintf(sendBuf, "[FCM]Trafficaccident\n");  //[FCM]으로 서버로 전송
    client.write(sendBuf, strlen(sendBuf));      //서버로부터 안드로이드로 푸시메시지 발송
    client.flush();
    offFlag = 1;
    tpFlag = 1;
  }
}
