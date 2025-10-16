#include "mbed.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>


AnalogIn my_analog_in{PA_3};
AnalogIn my_analog_in2{PA_4};

enum TorqueProfile {
   LINEAR,
   SMOOTH,
   AGRESSIVE
};

TorqueProfile currentProfile = SMOOTH;

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

   while(true){
      double Pedal1 =my_analog_in.read() * 3.3f;
      double Pedal2 =my_analog_in2.read() * 3.3f;
      printf("Analog in 1: %f\n", Pedal1);
      printf("Analog in 2: %f\n", Pedal2);
      
      float finalPercentage = getPedalPercentage(my_analog_in, my_analog_in2);
      printf("Final Percentage: %f\n", finalPercentage);

      
      float torque = applyTorque(finalPercentage);
      printf("Torque: %f\n", torque);
      
      ThisThread::sleep_for(500ms);
   }
   return 0;
}
