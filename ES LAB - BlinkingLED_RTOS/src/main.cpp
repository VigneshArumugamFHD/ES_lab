#include <Arduino.h>
#include "motorDriver.h"
#include "sensorDriver.h"
#include "AWS.h"


void taskOne( void * parameter);
void taskTwo( void * parameter);
void taskThree( void * parameter);
void taskFour( void * parameter);

mclass motorobject_motor =  mclass();

sclass sensorobj = sclass();

myawsclass awsobj = myawsclass();

#define LED_BOARD 2 //change here the pin of the board to V2

void setup(){
  pinMode(LED_BOARD, OUTPUT);
  Serial.begin(9600);

  motorobject_motor.SETUP();

  sensorobj.SETUP();

  awsobj.connectAWS();
  
  delay(1000);
  xTaskCreate(
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    2048,              /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    2048,              /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    2,                /* Priority of the task. */
                    NULL);            /* Task handle. */

   xTaskCreate(
                    taskThree,          /* Task function. */
                    "TaskThree",        /* String with name of task. */
                    2048,              /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    taskFour,          /* Task function. */
                    "TaskFour",        /* String with name of task. */
                    2048,              /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    2,                /* Priority of the task. */
                    NULL);            /* Task handle. */

}

void loop(){
delay(1000);
}

void taskOne( void * parameter )
{
    //example of a task that executes for some time and then is deleted
    for(;;)
    {
      // Serial.print("\nHello from task 1");
      
      //Switch on the LED
      digitalWrite(LED_BOARD, HIGH);
      // Pause the task for 1000ms
      delay(100); //This delay doesn't give a chance to the other tasks to execute
      //vTaskDelay(100 / portTICK_PERIOD_MS); //this pauses the task, so others can execute
      // Switch off the LED
      digitalWrite(LED_BOARD, LOW);
      // Pause the task again for 500ms
      vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 1");
    vTaskDelete( NULL );
}


void taskTwo( void * parameter )
{

     

    //example of a task that executes for some time and then is deleted
    for(;;)
    {
      // Serial.print("\nHello from task 2"); 

     motorobject_motor.set_speed(MotorA, Forward, 100);
     motorobject_motor.set_speed(MotorB, Backward, 100);

    // delay(1000);

     vTaskDelay(100 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 2");
    vTaskDelete( NULL );
}

void taskThree( void * parameter )
{
  
  int16_t* arr;

    for(;;)
    {
      Serial.print("\n");

      arr = sensorobj.reading();

    Serial.print("X = ");
    Serial.print(arr[0]);
    Serial.print(" Y = ");
    Serial.print(arr[1]);
    Serial.print(" Z = ");
    Serial.print(arr[2]);

    vTaskDelay(900 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 3");
    vTaskDelete( NULL );
}

void taskFour( void * parameter )
{
      for(;;)
    {
      Serial.print("\n");

      awsobj.stayConnected();
      
      vTaskDelay(100 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 4");
    vTaskDelete( NULL );
}
