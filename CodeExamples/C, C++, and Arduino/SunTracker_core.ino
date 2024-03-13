#include <Arduino.h>          //  core functions
#include <ArduinoLowPower.h>  //  sleep behaviours
#include <SPI.h>              //  serial fun
#include <SD.h>               //  SD read & write
#include <Wire.h>             //  i2c
#include <Adafruit_Sensor.h>  //  Adafruit sensing
#include <Adafruit_GFX.h>     //  Screen 1
#include <Adafruit_SH110X.h>  //  Screen 2
#include <Servo.h>            //  Servomotors
#include <Adafruit_BNO055.h>  //  IMU
#include <utility/imumaths.h> //  IMU math :)
#include <hp_BH1750_dual.h>   //  Light Sensors
#include <RTClib.h>           //  RTC
#include <time.h>             //  Standard C timing functions

// ________                         _________ .__                  __             /\                            
// \_____  \__  _  __ ____   ____   \_   ___ \|  | _____  ___  ___/  |_ ____   ___)/  ______                    
//  /   |   \ \/ \/ _/ __ \ /    \  /    \  \/|  | \__  \ \  \/  \   __/  _ \ /    \ /  ___/                    
// /    |    \     /\  ___/|   |  \ \     \___|  |__/ __ \_>    < |  |(  <_> |   |  \\___ \                     
// \_______  /\/\_/  \___  |___|  /  \______  |____(____  /__/\_ \|__| \____/|___|  /____  >                    
//   ______\/____________\/__ __\/______  __\/__________\/____  \/____  _________ \/___  \/____________________ 
//  /   _____\__    ___/  _  \\______   \ \__    ___\______   \ /  _  \ \_   ___ \|    |/ _\_   _____\______   \
//  \_____  \  |    | /  /_\  \|       _/   |    |   |       _//  /_\  \/    \  \/|      <  |    __)_ |       _/
//  /        \ |    |/    |    |    |   \   |    |   |    |   /    |    \     \___|    |  \ |        \|    |   \
// /_______  / |____|\____|__  |____|_  /   |____|   |____|_  \____|__  /\______  |____|__ /_______  /|____|_  /
//         \/                \/       \/                    \/        \/        \/        \/       \/        \/ 

//  _______    _______    _______    _______ 
// (  ____ \  (  ___  )  (  ____ )  (  ____ \
// | (    \/  | (   ) |  | (    )|  | (    \/
// | |        | |   | |  | (____)|  | (__    
// | |        | |   | |  |     __)  |  __)   
// | |        | |   | |  | (\ (     | (      
// | (____/\  | (___) |  | ) \ \__  | (____/\
// (_______/  (_______)  |/   \__/  (_______/
//                                          

/**
  * Owen Thomas Claxton
  * owenclaxton@kaist.ac.kr
  * 20226466
  * October, 2022
  *
  * Core Module
  *
  * Midterm Project for AE200: Introductory Space Project
  * Korea Advanced Institute of Science and Technology
  * 
  * This work is solely my own. I have referenced websites where appropriate
  * if I used templates or examples.
  *
  **/

// Constructors
void serialStart(void);
void buttonBRelease(void);
void buttonCRelease(void);
void checkButtonPress(bool blocking);
int clip(int number, int min, int max);
int findMaxIndVectorInt(int* array, int length);
void do_delay(int milliseconds, int step, bool doblocking);
void setup(void);
void loop(void);
void operate(void);

#include "owen_base_lib.h"          //  Customised <main_board_v0x5.h>
#include "owen_func_lib.h"          //  Customised <main_board_v0x5.h>

void setup(void) {
  serialStart();
  Serial.printf("\n[UPDATE] Initial boot sequence commencing.\n");
  // Boot sequence
  brd.rtc_init(); // realtime clock
  // brd.dpy_init(); // skip display init as we will toggle power to it
  brd.lss_init(); // light sensors
  brd.smr_init(); // servomotors
  brd.imu_init(); // inertial measurement unit
  brd.sdr_init(false); // SD card initialisation

  delay(1000);
  Serial.printf("[UPDATE] Board booted. Ready.\n");
  brd.sys_init = true;
}

void loop(void) { // timing control loop

  // Hold Block //////////////////////////////////////////////////////////////////////////////////////////////////////////
  brd.now = brd.rtc.now();
  bool flagmessage = true;
  while (brd.sys_init == false // provide fast c++ statement bypass
          && brd.now.unixtime() < brd.next_loop_time) { // while the current unixtime is earlier than the loop start time
    if (flagmessage) { // only do once per while(){}
      Serial.printf("[UPDATE] Waiting for threshold...\n");
      flagmessage = false;
    }
    checkButtonPress(false);
    delay(300); // chill out for a bit, reduce excessive crunching. idle() would be better
    brd.now = brd.rtc.now(); // update reference time for while(){}
  }
  checkButtonPress(true);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Update Block ////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (brd.sys_init) { // if first loop (only do this once!) ...
    flagmessage = true;
    while (brd.now.unixtime() % LOOP_INTERVAL > 0) { // ...force initialisation on every 0th second of every LOOP_INTERVAL...
      if (flagmessage) { // only do once per while(){}
        Serial.printf("[UPDATE] Waiting until 0th second...\n");
        flagmessage = false;
      }
      checkButtonPress(false);
      delay(300);
      brd.now = brd.rtc.now(); //...and therefore every 0th second of every day if a day is divisible by LOOP_INTERVAL...
    }
    brd.start_time = brd.now.unixtime(); // ...initialise the startTime...
    brd.next_loop_time = brd.start_time; // ...and feed it into the next_loop_time to be used...
    brd.sys_init = false;
  }
  brd.this_loop = brd.rtc.now();
  brd.next_loop_time = brd.next_loop_time + LOOP_INTERVAL; // ... here (addition is then every loop).
  brd.wake_periods++;
  checkButtonPress(true);
  //Serial.printf("[%05d] Loop commenced.\n", brd.wake_periods);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  operate(); // do operation

  // Sleep Control Block /////////////////////////////////////////////////////////////////////////////////////////////////
  brd.sleeping = true; // toggle flag, disable Button C callback function extension
  brd.now = brd.rtc.now();
  
  // initialise while loop variables
  int time_to_next_loop = brd.next_loop_time - ( (int) brd.now.unixtime() + 1 ); // second of extra leeway
  int sleep_time = 0;
  int time_before = 0;
  int time_after = 0;

  while(time_to_next_loop > (BUTTON_HANDLE_TIME + SLEEP_WAKEUP_TIME) ) { 
    
    checkButtonPress(true); // BUTTON_HANDLE_TIME spent here
    brd.now = brd.rtc.now();
    time_before = (int) brd.now.unixtime();

    time_to_next_loop = brd.next_loop_time - ( time_before + 1 );
    sleep_time = time_to_next_loop - SLEEP_WAKEUP_TIME;

    if (sleep_time > 0) { // if enough time, perform sleep operation
      //Serial.printf("\tEntering deepsleep for %d of %d seconds.\n", sleep_time, time_to_next_loop); // #!#

      delay(500);       // encapsulated in SLEEP_WAKEUP_TIME. Ensure flush doesn't prematurely lose bytes
      Serial.flush(); 
      Serial.end();

      LowPower.deepSleep( sleep_time * 1000 ); // convert to milliseconds. wakeup in the region of 2-4 seconds
      // escapes here are either timeout, or the LowPower interrupt on Button C
      delay(100); // encapsulated in SLEEP_WAKEUP_TIME. Ensure LowPower interrupt executes and flag is set properly
      
      serialStart();

      brd.now = brd.rtc.now();
      time_after = (int) brd.now.unixtime();
      time_to_next_loop = brd.next_loop_time - ( time_after + 1);
      //Serial.printf("\tWoke. Slept for: %d. Time left: %d\n", time_after - time_before, time_to_next_loop);
    } else {
      //Serial.printf("\tInsufficient time for sleep operation. Proceed to Hold Block.\n");
      break;
    }
    
  }
  brd.sleeping = false;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void operate(void) { // operations to be performed once every LOOP_SECONDS (300)

  brd.measure_base_angle(); // time consuming; contains checkButtonPress()

  brd.angle_sun[ALT_IND] = 0;
  brd.angle_sun[AZI_IND] = 0;
  brd.sun_rel_angles[ALT_IND] = 0;
  brd.sun_rel_angles[AZI_IND] = 0;
  brd.light_vec_mag = 0;

  // sunset 5:44, assume dark at 6:30
  // sunrise 6:45, assume light at 6:00
  brd.now = brd.rtc.now();
  double time_now = (double) brd.now.hour() + ( (double) brd.now.minute() ) / 60.0;
  if ( (time_now < 18.5 && time_now >  6) ) {
    bool daytime = brd.correctDirection(); // time consuming; contains checkButtonPress()
    if (daytime) {
      brd.update_light_luxes();
      brd.update_light_vector();
      brd.measure_sun_angle();
    }
  } else {
    Serial.printf("\t[%0.2f] Night time. Skipping sun tracking and calculations.\n", time_now);
  }
  brd.update_battery();

  char timeString[30] = "YYYY-MM-DD hh:mm:ss"; // define format for toString method
  brd.this_loop.toString(timeString); // populate format

  char buffer[120]; // extra big just in case of bad numbers :)
  //sprintf(buffer, "[%05d][%s] Battery: %02.2f%%%/%02.2fV\0", \
    brd.wake_periods, timeString, brd.battery_percent, brd.battery_voltage); // log time and number of seconds passed between execution
  
  sprintf(buffer, "%10d, %02.2f, %10d, %4d, %4d, %4d, %4d, %4d, %4d", \
    brd.this_loop.unixtime(), brd.battery_percent, brd.light_vec_mag, brd.angle_sun[AZI_IND], brd.angle_sun[ALT_IND], \
    brd.angle_snsrbrd[AZI_IND], brd.angle_snsrbrd[ALT_IND], brd.angle_smr[AZI_IND], brd.angle_smr[ALT_IND]);

  checkButtonPress(true);
  brd.write_to_sd(buffer);  //  write without new line
  Serial.println(buffer);   //  serial write but add new line :)
}