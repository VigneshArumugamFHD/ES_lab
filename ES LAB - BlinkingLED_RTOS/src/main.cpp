#include <Arduino.h>
#include "motorDriver.h"
#include "sensorDriver.h"
#include <OPT3101.h>
#include <Wire.h>

void taskOne( void * parameter);
// void taskTwo( void * parameter);
void sensorLoop(void);
void sensorsetup();

mclass motorobject_motorA =  mclass();
mclass motorobject_motorB =  mclass();

// sclass sensorobj = sclass();

OPT3101 sensor1;

#define LED_BOARD 2 //change here the pin of the board to V2

void setup(){
  pinMode(LED_BOARD, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  xTaskCreate(
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    2048,              /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  // xTaskCreate(
  //                   taskTwo,          /* Task function. */
  //                   "TaskTwo",        /* String with name of task. */
  //                   2048,              /* Stack size in bytes. */
  //                   NULL,             /* Parameter passed as input of the task */
  //                   1,                /* Priority of the task. */
  //                   NULL);            /* Task handle. */
}

void loop(){
delay(1000);
}

void taskOne( void * parameter )
{

    
  
    //example of a task that executes for some time and then is deleted
    for(;;)
    {
      Serial.print("\nHello from task 1");
      
      //Switch on the LED
      digitalWrite(LED_BOARD, HIGH);
      // Pause the task for 1000ms
      delay(100); //This delay doesn't give a chance to the other tasks to execute
      //vTaskDelay(100 / portTICK_PERIOD_MS); //this pauses the task, so others can execute
      // Switch off the LED
      digitalWrite(LED_BOARD, LOW);
      // Pause the task again for 500ms
      vTaskDelay(500 / portTICK_PERIOD_MS);

     motorobject_motorA.SETUP();
     motorobject_motorB.SETUP();

       motorobject_motorA.set_speed(MotorA, Forward, 50);
       motorobject_motorB.set_speed(MotorB, Backward, 50);
     
     

     sensorsetup();

        sensorLoop();

   

    }
    Serial.println("Ending task: 1");
    vTaskDelete( NULL );
}
 
// void taskTwo( void * parameter)
// {
  
//     //create an endless loop so the task executes forever
//     for(;;)
//     {
//         Serial.print("\nHello from task: 2");
//         vTaskDelay(100 / portTICK_PERIOD_MS);
//         sensorsetup();

//         sensorLoop();

//     }
//     Serial.print("\nEnding task 2"); //should not reach this point but in case...
//     vTaskDelete( NULL );
// }


void sensorsetup()
{
  Wire.begin(27,26);
 
  // Wait for the serial port to be opened before printing
  // messages (only applies to boards with native USB).
  while (!Serial) {}
 
  sensor1.init();
  if (sensor1.getLastError())
  {
    Serial.print(F("Failed to initialize OPT3101: error "));
    Serial.println(sensor1.getLastError());
    while (1) {}
  }
 
  // This tells the OPT3101 how many readings to average
  // together when it takes a sample.  Each reading takes
  // 0.25 ms, so getting 256 takes about 64 ms.
  // The library adds an extra 6% margin of error, making
  // it 68 ms.  You can specify any power of 2 between
  // 1 and 4096.
  sensor1.setFrameTiming(32);
 
  // 1 means to use TX1, the middle channel.
  sensor1.setChannel(0);
 
  // Adaptive means to automatically choose high or low brightness.
  // Other options you can use here are High and Low.
  sensor1.setBrightness(OPT3101Brightness::Adaptive);
}


void sensorLoop()
{  
    sensor1.sample();
    Serial.print("\n");
    Serial.print(sensor1.distanceMillimeters);
    Serial.print(";");
    sensor1.setChannel(1);  
    sensor1.sample();
    Serial.print(sensor1.distanceMillimeters);
    Serial.print(";");
    sensor1.setChannel(2);      
    sensor1.sample();
    Serial.print(sensor1.distanceMillimeters);

     
  Serial.println();
  // delay(125);

}
