#include "mbed.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Accelerator Pedal Position Sensors (APPS)
AnalogIn my_analog_in{PA_3};
AnalogIn my_analog_in2{PA_4};

// Brake pressure (used for RTD enable)
AnalogIn brakeSensor{PA_5};

// Ready-to-Drive (RTD) enable button (digital input)
DigitalIn rtdButton{PB_0};

// Buzzer output (used for RTD sound)
DigitalOut buzzer{PA_6};

// Torque Profile Selection
enum TorqueProfile { LINEAR, SMOOTH, AGRESSIVE };
TorqueProfile currentProfile = SMOOTH;

// Function to get pedal percentage
float getPedalPercentage(AnalogIn &one, AnalogIn &two){
   double Pedal1 =one.read()*3.3f;
   double Pedal2 =two.read()*3.3f;

   double percentage1 = ((Pedal1 - 0.25f)/(2.0f));
   double percentage2 = ((Pedal2 - 0.3f)/(2.4f));
   
   double finalPercentage = ((percentage1 + percentage2)/2.0f) * 100.0f;
   double diffPercentage = fabs(percentage1 - percentage2);

   if(finalPercentage > 10){
      if(diffPercentage < .10){
         finalPercentage = 0.0f;
      }
   }

   if(Pedal1 < 0.25||Pedal2 < 0.3||Pedal1 > 2.25||Pedal2 > 2.7){
      finalPercentage = 0.0f;
   }
   
   if(finalPercentage < 0){
      finalPercentage = 0.0f;
   }

   return finalPercentage;
}

// function that applies torque given pedal percentage and torque profile
float applyTorque(float pedalPercent){
   switch(currentProfile){
      case LINEAR:
         return pedalPercent;
      case SMOOTH:
         return (pedalPercent * pedalPercent)/100.0f;
      case AGRESSIVE:
         return pow(pedalPercent/100.0f, 0.5f) * 100.0f;
      default:
         return pedalPercent;
   }
}

int main()
{
   bool tractiveSystemActive = false;
   bool readyToDrive = false;

   while(true){
      tractiveSystemActive = true;

      // read break sensor and check if pressed
      float brakeVoltage = brakeSensor.read() * 3.3f;
      float brakePct = (brakeVoltage / 3.3f) * 100.0f;   
      bool brakePressed = (brakePct > 80.0f);            

      // read RTD button
      bool buttonPressed = (rtdButton.read() == 1); 

      //read raw analog data from pedal
      double Pedal1 =my_analog_in.read() * 3.3f;
      double Pedal2 =my_analog_in2.read() * 3.3f;

      printf("Analog in 1: %f\n", Pedal1);
      printf("Analog in 2: %f\n", Pedal2);

      float finalPct = getPedalPercentage(my_analog_in, my_analog_in2);
      float torque = applyTorque(finalPct);
      printf("Torque: %f\n", torque);

      // untested code others are tested
      if (!readyToDrive) {

         //not ready print 0
         printf("0\n");  

         //if all conditions are met sound buzzer and mark as ready to drive
         if (tractiveSystemActive && brakePressed && buttonPressed) {
            buzzer = 1;
            ThisThread::sleep_for(1s);  
            buzzer = 0;
            readyToDrive = true;
            printf(">>> READY TO DRIVE <<<\n");
         }
        } 
      
      ThisThread::sleep_for(500ms);
   }
   return 0;
}
