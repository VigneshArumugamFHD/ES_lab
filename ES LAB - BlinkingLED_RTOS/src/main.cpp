#include <Arduino.h>
#include <cmath>
#include "motorDriver.h"
#include "sensorDriver.h"
#include "AWS.h"
#include "parsedData.h"


void taskOne( void * parameter);
void taskTwo( void * parameter);
void taskThree( void * parameter);
void taskFour( void * parameter);
void taskFive( void * parameter);


static int16_t angle_upper;
static int16_t angle_lower;
static double degree;

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
                    2,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    taskFour,          /* Task function. */
                    "TaskFour",        /* String with name of task. */
                    2048,              /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

//  xTaskCreate(
//                     taskFive,          /* Task function. */
//                     "TaskFive",        /* String with name of task. */
//                     2048,              /* Stack size in bytes. */
//                     NULL,             /* Parameter passed as input of the task */
//                     2,                /* Priority of the task. */
//                     NULL);            /* Task handle. */

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

    for(;;)
    {
      Serial.print("\n");

      awsobj.stayConnected();
      
      vTaskDelay(30 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 4");
    vTaskDelete( NULL );

   
}


void taskThree( void * parameter )
{
     //example of a task that executes for some time and then is deleted
    for(;;)
    {
      // Serial.print("\nHello from task 2"); 
        short num = target_y - rover_y;
    short den = target_x - rover_x;

    double slope = (double)num/(double)den;
    double radian = atan(slope);
    degree = (radian*180)/3.1415;

    if(target_x > rover_x)
    {
      degree = (double)360 - degree;
    }
    else
    {
      
    }

    angle_upper = (int16_t)degree + 30;
    angle_lower = (int16_t)degree - 30;


     if(rover_angle!=0)
     {
       if((rover_angle<=angle_upper)&&(rover_angle>=angle_lower))
        {
          motorobject_motor.set_speed(MotorA, Backward, 200);
          motorobject_motor.set_speed(MotorB, Forward, 200);
         }
        else
        {
          motorobject_motor.set_speed(MotorA, Backward, 150);
          motorobject_motor.set_speed(MotorB, Backward, 150);
        }
     }
     else
     {
        motorobject_motor.set_speed(MotorA, Backward, 0);
        motorobject_motor.set_speed(MotorB, Forward, 0);
     }
     

    // delay(1000);

     vTaskDelay(30 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 2");
    vTaskDelete( NULL );
}


void taskFour( void * parameter )
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

    vTaskDelay(913 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 3");
    vTaskDelete( NULL );
}


void taskFive(void * parameter)
{
  for(;;)
  {    
    short num = target_y - rover_y;
    short den = target_x - rover_x;

    double slope = (double)num/(double)den;
    double radian = atan(slope);
    degree = (radian*180)/3.1415;

    if(target_x > rover_x)
    {
      degree = (double)360 - degree;
    }
    else
    {
      
    }

    angle_upper = (int16_t)degree + 30;
    angle_lower = (int16_t)degree - 30;

    Serial.println(degree);
    Serial.println(angle_upper);
    Serial.println(angle_lower);
    Serial.println(rover_angle);


    vTaskDelay(30 / portTICK_PERIOD_MS);
  }

   Serial.println("Ending task: 5");
    vTaskDelete( NULL );
}
