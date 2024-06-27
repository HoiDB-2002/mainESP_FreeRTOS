// #include "Lib.h"

#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include "Adafruit_MPR121.h"
Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#define esp32_CORE0 0
#define esp32_CORE1 1
#define relayPin 12
#define sirenPin 13

// Thông tin WiFi
const char* ssid = "abcdef";
const char* password = "1231231231";

// Thông tin MQTT Broker
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_username = "testServerU";
const char* mqtt_password = "testServerP";

WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial mySerial(1);

// ---------------Khai báo các task---------------------
// Giao tiếp với server
// void taskCommunicateServer(void *pvParameters);

// Kết nối với mạng có dây
//void taskConnectEthernet(void *pvParameters);

// Kết nối với wifi
//void taskConnectWifi(void *pvParameters);

// Quét nút bấm
void taskScanKeyboard(void* pvParameters);

// Giao tiếp với Arduino và LCD
// void taskCommunicateLCD(void *pvParameters);

// Giao tiếp CAN
// void taskCommunicateCAN(void *pvParameters);

// Giao tiếp RS232/ RS422/ RS485
// void taskCommunicateProtocol(void *pvParameters);

// Điều khiển relay
// void taskControlRelay(void *pvParameters);

// In các dòng lệnh lên màn hình
// void taskPrintSerial(void *pvParameters);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, 26, 27);
  Wire.begin();
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    //while (1){delay(200);};
  } else {
    Serial.println("MPR121 found!");
    // Get the currently touched pads
  }
  Serial.println("next");
  // -------------------- Cấu hình task ----------------------
  // xTaskCreatePinnedToCore(
  //   taskCommunicateServer             // Task name
  //   ,
  //   "Communicatetoserver"             // Name for human
  //   ,
  //   2048                              // Stack size
  //   ,
  //   NULL                              // When no parameter is used, simply pass NULL
  //   ,
  //   1                                 // Priority
  //   ,
  //   NULL
  //   ,
  //   esp32_CORE0                       // Core on which the task will run
  // );

  xTaskCreatePinnedToCore(
    taskScanKeyboard  // Task name
    ,
    "Scan Keyboard and send command"  // Name for human
    ,
    20480  // Stack size
    ,
    NULL  // When no parameter is used, simply pass NULL
    ,
    1  // Priority
    ,
    NULL,
    esp32_CORE1  // Core on which the task will run
  );

  // xTaskCreatePinnedToCore(
  //   taskPrintSerial                   // Task name
  //   ,
  //   "Sendcommandtodisplay"         // Name for human
  //   ,
  //   2048                              // Stack size
  //   ,
  //   NULL                              // When no parameter is used, simply pass NULL
  //   ,
  //   1                                 // Priority
  //   ,
  //   NULL
  //   ,
  //   esp32_CORE1                       // Core on which the task will run
  // );
}

void loop() {
}
// void taskCommunicateServer(void *pvParameters)
// {
//   // Set up here
//   // pinMode(relayPin, OUTPUT);
//   setup_wifi();
//   setup_mqtt();
//   // delay(1000);

//   // Loop here
//   for (;;)
//   {
//     // Duy trì kết nối với server
//     if (!client.connected())  reconnect();
//     client.loop();

//     static unsigned long lastMsgTime = 0;
//     if (millis() - lastMsgTime > 60000)
//     {
//       lastMsgTime = millis();
//       String str = "Data";
//       client.publish("testtopic/sentData", str.c_str());
//     }
//     delay(1000);
//   }
// }

void taskScanKeyboard(void* pvParameters) {
  //delay(1500);

  for (;;) {

    while (1) {
      currtouched = cap.touched();
      Serial.println(currtouched);
      for (uint8_t i = 0; i < 12; i++) {

        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i))) {
          if (i == 0) {
            mySerial.print("screen0@");
            Serial.println("screen0@");
          }
          if (i == 1) {
            mySerial.print("screen1@");
            Serial.println("screen1@");
          }
          if (i == 2) {
            mySerial.print("screen2@");
            Serial.println("screen2@");
          }
          if (i == 3) {
            mySerial.print("screen3@");
            Serial.println("screen3@");
          }
          if (i > 3) {
            mySerial.print("-1@");
            Serial.println("-1@");
          }
        }
        if (!(currtouched & _BV(i)) && (lasttouched & _BV(i))) {
          // Serial.print(i); Serial.println(" released");
        }
      }

      // reset our state
      lasttouched = currtouched;
      delay(1000);
      break;
    }
  }
}

// void taskPrintSerial(void *pvParameters)
// {
//   // Set up here

//   // Loop here
//   for (;;)
//   {
//     delay(5000);
//     Serial.println("        core 1");
//   }
// }

/*

void setup_mqtt() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Serial.println("");
  Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received data [");
  Serial.print(topic);
  Serial.print("] ");
  String strPayload = "";
  for (int i = 0; i < length; i++) 
  {
    strPayload += (char)payload[i];
  }
  Serial.println(strPayload);

  if (strcmp(topic, "testtopic/publichStatus") == 0)
  {
    // if (strPayload == "relayOn") 
    // {
    //   digitalWrite(relayPin, HIGH);
    // } 
    // else if (strPayload == "relayOff") 
    // {
    //   digitalWrite(relayPin, LOW);
    // }
    // Serial.print("Relay state: ");
    // Serial.println(digitalRead(relayPin));
  } 
  else if (strcmp(topic, "testtopic/screen") == 0)
  {
    Serial.print("Data sent to display: ");
    if (strPayload == "screen1@") 
    {
      // digitalWrite(relayPin, LOW);
      // mySerial.print("screen1@");
      Serial.println("screen1@");
    } 
    else if (strPayload == "screen2@") 
    {
      // digitalWrite(relayPin, HIGH);
      // mySerial.print("screen2@");
      Serial.println("screen2@");
    }
    else if (strPayload == "screen3@") 
    {
      // digitalWrite(relayPin, LOW);
      // mySerial.print("screen3@");
      Serial.println("screen3@");
    }
  }
  else if (strcmp(topic, "testtopic/subOTP") == 0)
  {
    // Serial.print("Data sent to display: ");
    // mySerial.print(strPayload);
    Serial.print(strPayload);
  }

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientID = "ESP32Client-";
    clientID += String(random(0xffff), HEX);

    if (client.connect(clientID.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      // Đăng ký vào topic "emqx/esp32" khi kết nối thành công
      client.subscribe("testtopic/publichStatus");
      client.subscribe("testtopic/screen");
      client.subscribe("testtopic/subOTP");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

*/