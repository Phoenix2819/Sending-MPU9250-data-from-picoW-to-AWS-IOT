#include <MPU9250_asukiaaa.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include "secrets.h"

MPU9250_asukiaaa mySensor;
WiFiClientSecure net;
PubSubClient client(net);

char payloadBuffer[65];

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  while (!client.connect(THINGNAME)) {
    delay(100);
  }
}

void setup() {
  digitalWrite(LED_BUILTIN,HIGH);
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  
  connectAWS();
}

void loop() {
  mySensor.accelUpdate();
  StaticJsonDocument<65> doc;
  doc.add(String(mySensor.accelX()));
  doc.add(String(mySensor.accelY()));
  doc.add(String(mySensor.accelZ()));

  serializeJson(doc, payloadBuffer, sizeof(payloadBuffer));
  client.publish("ur topic", payloadBuffer);

  delay(1); // Shorter delay to publish data more frequently
}
