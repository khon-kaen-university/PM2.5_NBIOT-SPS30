#include "sps30.h"
#include "AIS_NB_BC95.h"

#define SP30_COMMS I2C_COMMS

String apnName = "devkit.nb";
String serverIP = "{IP Server}";
String serverPort = "{Port}";
String nodeName = "{node number}";

AIS_NB_BC95 AISnb;
SPS30 sps30;

uint8_t ret;
struct sps_values val;
const long interval = 10000;
unsigned long previousMillis = 0;
long cnt = 0;
String payload;

void setup() {
  Serial.begin(9600);
  
  AISnb.debug = true;
  AISnb.setupDevice(serverPort);
  String ip1 = AISnb.getDeviceIP();  
  delay(1000);
  pingRESP pingR = AISnb.pingIP(serverIP);
  previousMillis = millis();
  
  sps30.begin(SP30_COMMS);
  sps30.start();
  sps30.I2C_expect();
}

void loop() {
  ret = sps30.GetValues(&val);

  Serial.print(F("PM 1.0 : "));
  Serial.print(val.MassPM1);
  Serial.print(F("\t,PM 2.5 : "));
  Serial.print(val.MassPM2);
  Serial.print(F("\t,PM 10 : "));
  Serial.print(val.MassPM10);
  Serial.print(F("\n"));
  delay(5000);

  payload = "[{\"id\":"+nodeName+",\"pm1\":"+val.MassPM1+",\"pm2\":"+val.MassPM2+",\"pm10\":"+val.MassPM10+"}]";
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
    {
      UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, payload);
      previousMillis = currentMillis;
    }
  UDPReceive resp = AISnb.waitResponse();
}
