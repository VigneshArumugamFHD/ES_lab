/**
 * ESP32 AWS Library
 * 
 * Functions to get the crawler coordinates from the Camera over AWS IoT
 * 
 * Authors: Vipul Deshpande, Jaime Burbano
 */


/*
  Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "AWS.h"
#include "parsedData.h"

/* The MQTT topics that this device should publish/subscribe to */
#define AWS_IOT_PUBLISH_TOPIC   "esp32/rover" 
#define AWS_IOT_SUBSCRIBE_TARGET_TOPIC "esp32/target"
#define AWS_IOT_SUBSCRIBE_ROVER_TOPIC "esp32/rover"
#define ROVER_VALUES_NUM 4
#define TARGET_VALUES_NUM 2

#define TARGET_X_COOR 0
#define TARGET_Y_COOR 1

#define ROVER_X_COOR 1
#define ROVER_Y_COOR 2
#define ROVER_ANGLE  3


WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

int16_t target_x = 0;
int16_t target_y = 0;

int16_t rover_x = 0;
int16_t rover_y= 0;
int16_t rover_angle = 0;

myawsclass::myawsclass() {

}


void messageHandler(String &topic, String &payload) 
{
  // Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  boolean number_detected = false;
  int16_t rover[ROVER_VALUES_NUM];
  int16_t target[TARGET_VALUES_NUM];
  String temp;
  int16_t value = 0;
  char store[3];
  int16_t digit_num = 0;
  deserializeJson(doc, payload);

  

  if(topic == "esp32/target")
  {
    temp = "target";
    int num = 0;

    const char* message = doc[temp];

    Serial.println(message);
    for(;*message != '\0';*++message)
    {
      if((*message>=48)&&(*message<=57))
      {
        number_detected = true;
        store[digit_num] = *message;
        if(digit_num == 2)
        {

        }
        else
        {
          store[digit_num + 1] = '\0';
        }
        
        digit_num += 1;
      }
      else
      {
        if(number_detected == true)
        {
          value = atoi(store);
          target[num] = value;
          num++;
          digit_num = 0;
          number_detected = false;
        }
        else
        {
          /*Do Nothing*/
        }
      }
    }

   target_x = target[TARGET_X_COOR];
   target_y = target[TARGET_Y_COOR];

  }
  else if(topic == "esp32/rover")
  {
    temp = "rover";
    int num = 0;
    const char* message = doc[temp];
    Serial.println(message);
    
    for(;*message != '\0';*++message)
    {
      if((*message>=48)&&(*message<=57))
      {
        number_detected = true;
        store[digit_num] = *message;
        
        if(digit_num == 2)
        {

        }
        else
        {
          store[digit_num + 1] = '\0';
        }

        digit_num += 1;
      }
      else
      {
        if(number_detected == true)
        {
          value = atoi(store);
          rover[num] = value;
          num++;
          digit_num = 0;
          number_detected = false;
        }
        else
        {
          /*Do Nothing*/
        }
      }
    }

    rover_x = rover[ROVER_X_COOR];
    rover_y = rover[ROVER_Y_COOR];
    rover_angle = rover[ROVER_ANGLE];

  }
  else
  {

  }
     
}

void myawsclass::stayConnected() {
  client.loop();
}

void myawsclass::connectAWS() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("Connecting...!");
  }

  Serial.print("CONNECTED...!\n");

  /* Configure WiFiClientSecure to use the AWS IoT device credentials */
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  /* Connect to the MQTT broker on the AWS endpoint we defined earlier */
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  /* Create a message handler */
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  /* Subscribe to a topic */
  client.subscribe(AWS_IOT_SUBSCRIBE_TARGET_TOPIC);
  client.subscribe(AWS_IOT_SUBSCRIBE_ROVER_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void myawsclass::publishMessage(int16_t sensorValue) {

  StaticJsonDocument<200> doc;
  //doc["time"] = millis();
  doc["sensor"] = sensorValue;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); /* print to client */

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


myawsclass awsobject = myawsclass();  /* creating an object of class aws */


