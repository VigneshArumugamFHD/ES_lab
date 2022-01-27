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

enum Side
{
  Right = 0,  /* Rover need to trun LEFT, because obstacle is at Left side */
  Left = 1  /* Rover need to trun LEFT, because obstacle is at Right side */
};

/*Angle of rover pointing towards Target with tolerance upper value*/
static int16_t angle_upper;
/*Angle of rover pointing towards Target with tolerance lower value*/
static int16_t angle_lower;
/*Degree between the current rover angle to the target point*/
static double degree;
/*Obstacle detected status variable*/
static boolean obstacle_detected =  false;
/*Variable having the current side where rover need to turn to avoid obstacle*/
static Side rover_side;


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

         

      if((arr[0]<150) && (arr[0]>0))
      {
        /*Turn right*/
        
        rover_side = Right;
        obstacle_detected = true;
        
      }
      else if((arr[2]<150) && (arr[2]>0))
      {
        /*Turn left*/
        rover_side = Left;
        obstacle_detected = true;
      }
      else
      {
        obstacle_detected = false;
      }
  

    vTaskDelay(30 / portTICK_PERIOD_MS);

    }
    Serial.println("Ending task: 3");
    vTaskDelete( NULL );
}

void taskFour( void * parameter )
{
     
    for(;;)
    {
      /*Implementing slope formula of two points[(y2-y1)/(x2-x1)][target point & rover point]*/      
      short num = target_y - rover_y;
      short den = target_x - rover_x;
      double slope = (double)num/(double)den;

      /*Applying tan inverse formula to find the angle between target point and rover direction*/
      double radian = atan(slope);
      /*Converting radian to degree*/
      degree = (radian*180)/3.1415;
 
    /*If degree is in negative changing that into positive value*/
    if(degree < 0)
    {
      degree = ((double)-1)* degree;
    }
    else
    {
      /*DO nothing*/
    }


    /*Divided the whole area into four coordinates w.r.t rover and target position*/
    /*This will make the rover to change its direction towards target within minimum rotation*/
    if((target_x > rover_x)&&(target_y>rover_y))
    {
      /*4th coordinate*/
      degree = (double)360 - degree;
    }
    else if((target_x>rover_x)&&(rover_y>target_y))
    {
      /*Do nothing since target is in 1st coordinate*/
    }
    else if((rover_x>target_x)&&(rover_y>target_y))
    {
      /*2nd coordinate*/
      degree = (double)180 - degree;
    }
    else if((rover_x>target_x)&&(target_y>rover_y))
    {
        /*3rd coordinate*/
        degree = (double)180 + degree;
    }
    else
    {

    }

    /*Applying 30 degree tolarence to the angle*/
    angle_upper = (int16_t)degree + 30;
    angle_lower = (int16_t)degree - 30;

      /*Ensuring rover coordinate is a valid value*/
     if(rover_angle!=0)
     {
       if(obstacle_detected != true)
       {
          if((rover_angle<=angle_upper)&&(rover_angle>=angle_lower))
        {
          /*Move forward*/
          motorobject_motor.set_speed(MotorA, Backward, 250);
          motorobject_motor.set_speed(MotorB, Forward, 250);
         }
        else
        {
          if(degree<(rover_angle+180))
          { /*Spinning left*/
            motorobject_motor.set_speed(MotorA, Backward, 150);
            motorobject_motor.set_speed(MotorB, Backward, 150);
          }
          else
          { /*Spinning Right*/
            motorobject_motor.set_speed(MotorA, Forward, 150);
            motorobject_motor.set_speed(MotorB, Forward, 150);
          }
          
        }
       }
       else
       {
         if(rover_side == Right)
         {
           /*Spinning Right*/
           obstacle_detected = false;
            motorobject_motor.set_speed(MotorA, Forward, 150);
            motorobject_motor.set_speed(MotorB, Forward, 150);
            delay(300);
            /*Move forward*/
            motorobject_motor.set_speed(MotorA, Backward, 250);
            motorobject_motor.set_speed(MotorB, Forward, 250);
            delay(200);

         }
         if(rover_side == Left)
         {
            /*Spinning Left*/
            obstacle_detected = false;
            motorobject_motor.set_speed(MotorA, Backward, 150);
            motorobject_motor.set_speed(MotorB, Backward, 150);
            delay(300);
            /*Move forward*/
            motorobject_motor.set_speed(MotorA, Backward, 250);
            motorobject_motor.set_speed(MotorB, Forward, 250);
            delay(200);
         }
         
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


