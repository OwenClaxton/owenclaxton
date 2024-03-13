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

//  _______               _          _______        _         _________   ______    
// (  ____ \  |\     /|  ( (    /|  (  ____ \      ( \        \__   __/  (  ___ \   
// | (    \/  | )   ( |  |  \  ( |  | (    \/      | (           ) (     | (   ) )  
// | (__      | |   | |  |   \ | |  | |            | |           | |     | (__/ /   
// |  __)     | |   | |  | (\ \) |  | |            | |           | |     |  __ (    
// | (        | |   | |  | | \   |  | |            | |           | |     | (  \ \   
// | )        | (___) |  | )  \  |  | (____/\      | (____/\  ___) (___  | )___) )  
// |/         (_______)  |/    )_)  (_______/      (_______/  \_______/  |/ \___/   
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

// --> Helper functions and additional variables
// --> Must be imported after owen_base_lib

char dotws[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void serialStart(void) {
  // Very simple function breakout as we use this process in a few places
  Serial.begin(BAUD_RATE);
  int delay_period = 1000;
  while( !Serial && delay_period > 10) {
    // Ensure serial reconnects properly post-deepsleep operation
    delay(10);
    delay_period = delay_period - 10;
  }
  delay(delay_period);
}

void buttonBRelease(void) {
  brd.in_interrupt = true;

  if (!brd.moveToDo) {
    brd.moveToDo = true; // cleared in operate()
  }

  brd.in_interrupt = false;
}

void buttonCRelease(void) {
  brd.in_interrupt = true;

  if (!brd.do_button_steps) {
    brd.do_button_steps = true; // cleared in checkButtonPress_x()
  }

  brd.in_interrupt = false;
}

void checkButtonPress(bool blocking) {
  // very annoying function to do without a timer interrupt
  // must place calls to it throughout the code to constantly check
  if (brd.do_button_steps) { // press not logged
    brd.now = brd.rtc.now();
    if (brd.button_press_time == 0) {
      brd.button_press_time = brd.now.unixtime(); // update time
      brd.dpy_init(); // power on; 100ms delay embedded
    }

    while ( ( (int) brd.now.unixtime() - brd.button_press_time ) < SCREEN_ON_TIME ) {
      brd.disp.clearDisplay();
      brd.disp.setCursor(0, 0);
      
      brd.update_battery();

      char timeString[30] = "YYYY-MM-DD hh:mm:ss"; // define format for toString method
      brd.now.toString(timeString); // populate format

      brd.disp.printf("%.2fV, %02.0f%%\n", brd.battery_voltage, brd.battery_percent);
      brd.disp.printf("%s\n", timeString);
      brd.disp.display();

      if (!blocking) { // leave :)
        return;
      }

      delay(200);
      brd.now = brd.rtc.now();
    }

    brd.dpy_power(0); // power off
    brd.do_button_steps = false; // reset flag
    brd.button_press_time = 0;
  }
}

int clip(int number, int min, int max) {
  if (number < min) {
    return (int) min;
  } else if (number > max) {
    return (int) max;
  }
  return number;
}

int sum_array(int* array, int length) {
  int sum = array[0];
  for (int i = 0; i < length; i++) {
    sum = sum + array[i];
  }
  return sum;
}

int findMaxIndVectorInt(int* array, int length) {
  int biggest = 0;
  for (int i = 1; i < length; i++) {
    if (array[i] > array[biggest]) {
      biggest = i;
    }
  }
  return biggest;
}

void showCalibrationValues(void) {
  adafruit_bno055_offsets_t calibrationData;
  brd.imu.getSensorOffsets(calibrationData);
  Serial.println("//----Add this code to your setup function, after imu is initialised----");
  Serial.println("adafruit_bno055_offsets_t calibrationData;");
  Serial.print("calibrationData.accel_offset_x = "); Serial.print(calibrationData.accel_offset_x); Serial.println(";");
  Serial.print("calibrationData.accel_offset_y = "); Serial.print(calibrationData.accel_offset_y); Serial.println(";");
  Serial.print("calibrationData.accel_offset_z = "); Serial.print(calibrationData.accel_offset_z); Serial.println(";");
  Serial.print("calibrationData.gyro_offset_x = "); Serial.print(calibrationData.gyro_offset_x); Serial.println(";");
  Serial.print("calibrationData.gyro_offset_y = "); Serial.print(calibrationData.gyro_offset_y); Serial.println(";");
  Serial.print("calibrationData.gyro_offset_z = "); Serial.print(calibrationData.gyro_offset_z); Serial.println(";");
  Serial.print("calibrationData.mag_offset_z = "); Serial.print(calibrationData.accel_offset_z); Serial.println(";");
  Serial.print("calibrationData.mag_offset_x = "); Serial.print(calibrationData.gyro_offset_x); Serial.println(";");
  Serial.print("calibrationData.mag_offset_y = "); Serial.print(calibrationData.gyro_offset_y); Serial.println(";");
  Serial.print("calibrationData.accel_radius = "); Serial.print(calibrationData.accel_radius); Serial.println(";");
  Serial.print("calibrationData.mag_radius = "); Serial.print(calibrationData.mag_radius); Serial.println(";");
  Serial.println("imu.setSensorOffsets(calibrationData);\n\n"); 
}

void do_delay(int milliseconds, int step, bool doblocking) {
  int t = 0;
  while (t < milliseconds) {
    checkButtonPress(false);
    delay(step);
    t = t + step;
  }
  checkButtonPress(doblocking);
}