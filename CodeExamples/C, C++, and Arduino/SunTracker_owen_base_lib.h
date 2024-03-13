#define BATTERY_VOL_PIN     9 // 
#define BUTTON_A            9 // same
#define BUTTON_B            6 
#define BUTTON_C            5
#define AZI_PIN             10
#define ALT_PIN             11
#define CARD_SELECT_PIN     12 
#define SERVO_POWER_PIN     13
#define DISPL_POWER_PIN     A4 
#define PAYLD_POWER_PIN     A5
// Pin definitions ^^^

#define MIN_ALT_ANGLE       0
#define MAX_ALT_ANGLE       90
#define MIN_AZI_ANGLE       10
#define MAX_AZI_ANGLE       170
// Servo limits ^^^ These are overkill to protect the servos and ensure safe behaviour
// also because my azimuth servo only does about 170 or fewer degrees and I do not want to
// ask for a third replacement. I swear it's not me :( 

#define DISPL_W             128 // Screen width...
#define DISPL_H             64  // ...and height

#define BAUD_RATE           115200 // Serial rate
#define LIGHT_WAIT_MAX      180 // Light sensor library maximum normal/healthy wait time
#define LIGHT_THRESHOLD     950 // Vector magnitude. I measured this on the 23rd of October at sunset, it is roughly correct 
                                // depending on what you define as "sunset"

#define ALT_IND             0 // altitude and azimuth indices
#define AZI_IND             1 // to ensure things don't get swapped

#define CPU_HZ              48000000 // 48 MHz
#define TMR_PRSCLR_DIV      1024 // second timer mode.
// These are no longer necessary as I binned complex interrupt code ^^

#define CALIBRATION_DELAY   3000 // 3 seconds as a very rough estimate when viewing calibration readout
#define SCREEN_ON_TIME      4 // 4 seconds
#define MAG_DECLINATION     8.57 // degrees; https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml?

#define LOOP_INTERVAL       300 // seconds as per client specifications
#define SLEEP_WAKEUP_TIME   4 // seconds, rough estimate
#define BUTTON_HANDLE_TIME  4 // seconds, rough estimate of function time

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

//  ______     _______    _______    _______        _         _________   ______    
// (  ___ \   (  ___  )  (  ____ \  (  ____ \      ( \        \__   __/  (  ___ \   
// | (   ) )  | (   ) |  | (    \/  | (    \/      | (           ) (     | (   ) )  
// | (__/ /   | (___) |  | (_____   | (__          | |           | |     | (__/ /   
// |  __ (    |  ___  |  (_____  )  |  __)         | |           | |     |  __ (    
// | (  \ \   | (   ) |        ) |  | (            | |           | |     | (  \ \   
// | )___) )  | )   ( |  /\____) |  | (____/\      | (____/\  ___) (___  | )___) )  
// |/ \___/   |/     \|  \_______)  (_______/      (_______/  \_______/  |/ \___/   
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

// Dear Paulo, please forgive me for what I did to your library
// I cannot say these changes made my life easier, and reading it is nightmarish at times
// nevertheless, it is quite robust and full of features

class Cubesat_Board {
    public:
        const int power_up_delay    = 100; //in milliseconds
        const byte light_addr[4]    = {BH1750_TO_VCC, BH1750_TO_VCC, BH1750_TO_GROUND, BH1750_TO_GROUND};
        const byte light_ch[4]      = {1, 0, 0, 1};
        const float voltageMin      = 3.3;
        const float voltageRange    = 0.9;
        const float volt_conv_fctr  = (3.3 / 1023.0) * 2;
        const float angle_mids[2]   = {(MAX_ALT_ANGLE + MIN_ALT_ANGLE)/2.0, (MAX_AZI_ANGLE + MIN_AZI_ANGLE)/2.0}; // middles of servo operating ranges

        int light_lux[4]            = {0, 0, 0, 0};
        int light_vec_mag           = 0;
        int angle_smr[2]            = {0, 0}; // alt, azi (servo angles)
        int angle_snsrbrd[2]        = {0, 0}; // alt, azi (azi respect to true north)
        int angle_sun[2]            = {0, 0}; // alt, azi (azi respect to true north)
        int angle_measure[2]        = {0, 0}; // alt, azi (azi respect to true north)

        double light_vec[3]         = {0.0, 0.0, 0.0}; // init at an impossible unit vector
        double angle_adj[2]         = {0.0, 0.0}; // alt, azi (servo adjustment angle)
        double sun_rel_angles[2]    = {0.0, 0.0}; // alt, azi (sun, relative to light sensor frame)

        float battery_voltage       = 3.3;
        float battery_percent       = 0.0;
        float step_size_store       = angle_mids[0]; // dummy variable store to reduce a tick of comparison

        bool ls_ready               = false; // ensure light sensors get initialised properly before any access is granted
        bool imu_ready              = false; // ensure IMU gets initialised properly before any access is granted
        bool moveToDo               = false; // flag; true: performing pointing operation
        bool sys_init               = false; // flag; true: all inits have been performed and board is ready.
        bool do_button_steps        = false; // whether or not to do button_c handling
        bool sleeping               = false; // whether or not the system is in the sleep control block
        bool in_interrupt           = false; // whether or not code is executing within an interrupt

        int start_time              = 0; // system start time, for loop timings
        int next_loop_time          = 0; // the next loop start time
        int button_press_time       = 0; // when button_c was last pressed
        int wake_periods            = 0; // first period will be 0th
        int cali_skips              = 0; // log how many are too many imu calibration skips

        DateTime now;                     // rather than having to keep creating this object throughout the code, let's just store one here we can update.
        DateTime this_loop;               // keep this only updating each loop
        Adafruit_SH1107 disp        = Adafruit_SH1107(DISPL_H, DISPL_W, &Wire);
        Adafruit_BNO055 imu         = Adafruit_BNO055(55); // IMU
        Servo aziServo;
        Servo altServo;
        RTC_PCF8523 rtc;
        hp_BH1750 light_sensor[4]; //for help look at: https://github.com/Starmbi/hp_BH1750/wiki

        void pld_power(bool is_on) {
          digitalWrite(PAYLD_POWER_PIN, !is_on);  //0: enable, 1:disable
          delay(power_up_delay);
        }

        void dpy_power(bool is_on) {
          digitalWrite(DISPL_POWER_PIN, !is_on);  //0: enable, 1:disable
          delay(power_up_delay * 2); // needs a little more time with deepsleep processes
        }

        void smr_power(bool is_on) {
          digitalWrite(SERVO_POWER_PIN, is_on);  //1: enable, 0:disable
          delay(power_up_delay);
        }

        Cubesat_Board(void) {
          //pinMode(BUTTON_A, INPUT_PULLUP); //  BUTTON_A =  BATTERY_VOL_PIN
          pinMode(BATTERY_VOL_PIN, INPUT); // Use pin BUTTON_A for battery reading
          pinMode(BUTTON_B, INPUT_PULLUP); // Debugging purposes; flow control
          pinMode(BUTTON_C, INPUT_PULLUP); // Screen on button (4ish second timeout depending on code execution)

          pinMode(CARD_SELECT_PIN, OUTPUT); // write to SD card
          
          pinMode(DISPL_POWER_PIN, OUTPUT); 
          pinMode(PAYLD_POWER_PIN, OUTPUT);
          pinMode(SERVO_POWER_PIN, OUTPUT);

          attachInterrupt(digitalPinToInterrupt(BUTTON_B), buttonBRelease, FALLING); // needed for debug
          attachInterrupt(digitalPinToInterrupt(BUTTON_C), buttonCRelease, FALLING); // needed for mission; trigger outside of sleep
          LowPower.attachInterruptWakeup(BUTTON_C, buttonCRelease, CHANGE); // needed for mission; trigger during sleep

          pld_power(0);
          smr_power(0);
          dpy_power(0);
        }

        bool write_to_sd(char* buffer) {

          now = rtc.now();
          char filename[20] = "YYYYMMDD.csv";
          now.toString(filename); // specifications want a new file for each day
          
          int timeout = 4000; // set maximum 4 seconds of attempting to write
          int new_timeout = sdr_init(true);
          timeout = timeout - new_timeout;
          checkButtonPress(true);

          if ( SD.exists(filename) ) {
            Serial.printf("[UPDATE] File exists. New entries will be appended.\n");
          }

          File sd_card_file = SD.open(filename, FILE_WRITE);
          bool flagmessage = true;
          while ( !sd_card_file ) { // SD card removed D:
            if (flagmessage) {
              Serial.printf("[ERROR] Couldnt open '%s'. Retrying...\n", filename); 
              flagmessage = false;
            }

            new_timeout = sdr_init(true);
            timeout = timeout - new_timeout;
            checkButtonPress(false);
            delay(50);
            timeout = timeout - 50;

            if ( timeout < 0) {
              Serial.printf("[ERROR] Failed to open %s. Cannot write.\n", filename);
              return false; // failed
            }

            sd_card_file = SD.open(filename, FILE_WRITE);
          }
          Serial.printf("[UPDATE] SD Card initialisation Success.\n");
          checkButtonPress(true);
      
          sd_card_file.println(buffer);
          sd_card_file.close();
          delay(100);
          return true; // success
        }

        int get_lux_from_sensor_id(uint8_t ls) {
          // we actually never use this function
          if(ls < 4){
              light_sensor[ls].start();
              while(!light_sensor[ls].hasValue(true)){
                  //kill time until it reads
              }
              return light_sensor[ls].getLux();
          }
          else{
              return -1;
          }
        }

        bool update_light_vector(void) {
          // must be performed after update_light_luxes() to crunch vector math
          int lux_reordered[4] = {light_lux[0], light_lux[2], light_lux[1], light_lux[3]};
          // If everything is at 62 degrees:
          // --> Math from Professor's lecture slides, crunched off-script to reduce intensity
          double N1 = 1.065027; //1.065027234094756;
          double N2 = 0.283143; //0.283142512672260;
          light_vec[0] = (double) N1 * (lux_reordered[0] - lux_reordered[1]);
          light_vec[1] = (double) N1 * (lux_reordered[2] - lux_reordered[3]);
          light_vec[2] = (double) N2 * (lux_reordered[0] + lux_reordered[1] + lux_reordered[2] + lux_reordered[3]);

          // normalise
          double vecMag = sqrt(sq(light_vec[0]) + sq(light_vec[1]) + sq(light_vec[2]));
          light_vec[0] = light_vec[0] / vecMag;
          light_vec[1] = light_vec[1] / vecMag;
          light_vec[2] = light_vec[2] / vecMag;
          light_vec_mag = (int) round(vecMag);
          
          // angle in deg
          double f_azi_rad = atan2( light_vec[0], light_vec[2]);
          double f_alt_rad = atan2( light_vec[1], light_vec[0]*sin(f_azi_rad) + light_vec[2]*cos(f_azi_rad) );
          sun_rel_angles[AZI_IND] = (double) f_azi_rad * 57296.0 / 1000.0;
          sun_rel_angles[ALT_IND] = (double) f_alt_rad * 57296.0 / 1000.0;

          if (light_vec_mag > LIGHT_THRESHOLD) return true; // enough light to be day
          return false; // nighttime :(
        }

        void update_battery(void) {
          // calc voltage and % levels
          battery_voltage = analogRead(BATTERY_VOL_PIN) * volt_conv_fctr;
          battery_percent = ((battery_voltage - voltageMin)/voltageRange) * 100.0;
        }

        int servoMove(int alt_pos, int azi_pos, int wait, bool relative, bool power, bool stepping) {
          // total servo movement control
          int new_alt_pos = 0;
          int new_azi_pos = 0;
          if (relative == true) { // move relative to current pos
            new_alt_pos = clip(alt_pos + angle_smr[ALT_IND], MIN_ALT_ANGLE, MAX_ALT_ANGLE);
            new_azi_pos = clip(azi_pos + angle_smr[AZI_IND], MIN_AZI_ANGLE, MAX_AZI_ANGLE); // generate new pos then clip
          } else { // absolute movement
            new_alt_pos = clip(alt_pos, MIN_ALT_ANGLE, MAX_ALT_ANGLE);
            new_azi_pos = clip(azi_pos, MIN_AZI_ANGLE, MAX_AZI_ANGLE); // generate new pos then clip
          }

          if (new_alt_pos == angle_smr[ALT_IND] && new_azi_pos == angle_smr[AZI_IND]) {
            return 0; // does not result in change in orientation, therefore skip
          }

          if (power) {
            smr_power(1);
          }

          if (!stepping) { // no stepping, move direct to destination and miss out on my cool feature implementation
            altServo.write(new_alt_pos);
            aziServo.write(new_azi_pos);

            delay(wait + 1);

          } else { //move smoothly by stepping :)
            float num_steps = abs(new_alt_pos - angle_smr[ALT_IND]) > abs(new_azi_pos - angle_smr[AZI_IND]) ? abs(new_alt_pos - angle_smr[ALT_IND]) : abs(new_azi_pos - angle_smr[AZI_IND]);
            for (int i = 1; i < num_steps; i++) {
              altServo.write( angle_smr[ALT_IND] + round(i * ((new_alt_pos - angle_smr[ALT_IND])/num_steps)) );
              aziServo.write( angle_smr[AZI_IND] + round(i * ((new_azi_pos - angle_smr[AZI_IND])/num_steps)) );
              delay(wait + 1);
            }
            altServo.write(new_alt_pos); // ensure snap to correct final position
            aziServo.write(new_azi_pos);
            delay(wait + 1);
          }

          if (power) {
            smr_power(0);
          }

          angle_smr[ALT_IND] = new_alt_pos;
          angle_smr[AZI_IND] = new_azi_pos;

          return 1;
        }

        void update_light_luxes(void) { // no point having four loops with this many items, unnecessary variable allocation
          if (ls_ready) {
            light_sensor[0].start(); // queue all up to minimise time in this function
            light_sensor[1].start();
            light_sensor[2].start();
            light_sensor[3].start();
            int wait_period = 0;
            while( !light_sensor[0].hasValue(true) || !light_sensor[1].hasValue(true) || !light_sensor[2].hasValue(true) || !light_sensor[3].hasValue(true)) {
              // wait for all sensors to be ready
              // if one sensor has an issue, this may hang forever...
              wait_period = wait_period + 10;
              delay(10);
              if (wait_period > LIGHT_WAIT_MAX) {
                Serial.printf("[ERROR] update_light_luxes(): Exceeded maximum wait time.\n");
                break;
              }
            }
            light_lux[0] = light_sensor[0].getLux();
            light_lux[1] = light_sensor[1].getLux();
            light_lux[2] = light_sensor[2].getLux();
            light_lux[3] = light_sensor[3].getLux();
          } else {
            Serial.printf("[ERROR] update_light_luxes(): ls_ready FALSE\n");
          }
          //Serial.printf("Luxes: %d | %d | %d | %d\n", light_lux[0], light_lux[1], light_lux[2], light_lux[3]);
        }

        bool correctDirection(void) {
          // orient sensorboard to the sun vector
          int step_count = 20; // absolute max; dirty way to prevent looped behaviour
          int old_alt_adj = 0;
          int old_azi_adj = 0;
          int new_alt_adj = 0;
          int new_azi_adj = 0;
          moveToDo = true;
          bool daytime = false;
          bool reachednightmode = false;

          smr_power(1);

          while (moveToDo) {
            checkButtonPress(false);

            update_light_luxes();
            daytime = update_light_vector();

            if (!daytime && reachednightmode == false) { // only attempt once
              Serial.printf("\tPotential night mode...\n");
              reachednightmode = true;
              servoMove(MIN_ALT_ANGLE, angle_mids[AZI_IND], 10, false, false, true);
              update_light_luxes();
              daytime = update_light_vector(); // try again
              if (!daytime) {
                Serial.printf("\tNight mode.\n");
                break; // escape, no work to do here.
              }
            }

            new_alt_adj = (int) round( -0.15 * sun_rel_angles[ALT_IND] );
            new_azi_adj = (int) round(  0.30 * sun_rel_angles[AZI_IND] );

            if ( step_count == 0 || (new_alt_adj == 0 && new_azi_adj == 0) || (new_alt_adj == -old_alt_adj && new_azi_adj == -old_azi_adj) ) { 
              // Catch too many steps, no step at all, or oscillations.
              moveToDo = false; // exit condition
            } else {
              int changedAngle = servoMove(new_alt_adj, new_azi_adj, 5, true, false, true);
              if (changedAngle == 0) {
                moveToDo = false; // servos have either zeroed in on target, or are hitting their limits
              }
              step_count = step_count - 1;
              old_alt_adj = new_alt_adj;
              old_azi_adj = new_azi_adj;
            }
            daytime = true;

          }
          smr_power(0);
          checkButtonPress(true);
          return daytime;
        }

        void displayCalStatus(void) { // Display sensor calibration status
            uint8_t system = 0, gyro = 0, accel = 0, mag = 0; // init at zero (worst case); 3 is fully calibrated
            imu.getCalibration(&system, &gyro, &accel, &mag);
            Serial.printf("[IMU CAL] SYS: %d {G: %d A: %d M: %d}\n", system, gyro, accel, mag);
        }

        void measure_absolute(void) {
          // Measure the absolute altitude and azimuth of the current position relative to world frame

          // Grab and normalise vectors from accelerometer and magnetometer:
          imu::Vector<3> accel_vec = imu.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
          double accel_vec_mag = sqrt(sq(accel_vec.x()) + sq(accel_vec.y()) + sq(accel_vec.z()));
          double accel_norm[3] = {accel_vec.x()/accel_vec_mag, accel_vec.y()/accel_vec_mag, accel_vec.z()/accel_vec_mag};

          imu::Vector<3> magne_vec = imu.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
          double magne_vec_mag = sqrt( sq(magne_vec.x()) + sq(magne_vec.y()) + sq(magne_vec.z()) );
          double magne_norm[3] = {magne_vec.x()/magne_vec_mag, magne_vec.y()/magne_vec_mag, magne_vec.z()/magne_vec_mag};
        
          // Use reduced mathematics for frame rotation to compensate the magnetometer for tilt
          // --> We get pretty funky here, cobbling a few things together.
          // --> To make this level of transformation insanity work, we need some pretty specific angles in particular frames

          double ang_the = atan2( accel_norm[1], accel_norm[2] ); // theta
          double ang_phi = atan2(-accel_norm[0], sqrt( sq(accel_norm[1]) + sq(accel_norm[2]) ) ); // phi

          // Rotate the magnetic field vector x and y components to compensate for tilt
          double x_compensated =  magne_norm[0] * cos(ang_phi) + \
                                  magne_norm[1] * sin(ang_the) * sin(ang_phi) + \
                                  magne_norm[2] * cos(ang_the) * sin(ang_phi);

          double y_compensated =  magne_norm[1] * cos(ang_the) + \
                                 -magne_norm[2] * sin(ang_the);

          double azi_rad = atan2(accel_norm[1], accel_norm[0]);
          double alt_rad = atan2(accel_norm[2], sqrt( sq(accel_norm[0]) + sq(accel_norm[1]) ) );
          double mag_rad = atan2( y_compensated, x_compensated);

          int azi_deg = (int) ( ( azi_rad * (57296.0 / 1000.0) ) - 90); // -270 -> 89; -90 to rotate axes to align with magnetic north direction
          int alt_deg = 90 - (int) ( alt_rad * (57296.0 / 1000.0) );
          int mag_deg = (int) ( (mag_rad * 57296.0 / 1000.0) - MAG_DECLINATION);
          mag_deg = (mag_deg + 90 - azi_deg + 360) % 360; // 0 -> 359; +90 to rotate axes; -azimuth_deg to account for twisted reference

          // Save our true azimuth (magnetic heading) and altitude for external access
          angle_measure[ALT_IND] = alt_deg;
          angle_measure[AZI_IND] = mag_deg;

          // It is a bit terrifying how this function works, but it does :)

          //Serial.printf("Za: %4d \tZm: %4d \tTa: %4d\n", azi_deg, mag_deg, alt_deg);
          //Serial.printf("X: % 0.2f   Y: % 0.2f   Z: % 0.2f\n", accel_norm[0], accel_norm[1], accel_norm[2]);
          //Serial.printf("Theta: %d Phi: %d Heading: %d\n", (int) (ang_the * 57296.0 / 1000.0), (int) (ang_phi * 57296.0 / 1000.0), mag_deg);
        }

        void measure_base_angle(void) {
          // measure angle of sensorboard when "parallel"
          int old_alt = angle_smr[ALT_IND]; // save current pos
          int old_azi = angle_smr[AZI_IND]; 
          calibrateImu(false, true); // wait on the delay because we will ...
          servoMove(MIN_ALT_ANGLE, angle_mids[AZI_IND], 3, false, true, true);
          do_delay(CALIBRATION_DELAY, 100, true); // ... do it here instead

          measure_absolute();
          angle_snsrbrd[ALT_IND] = angle_measure[ALT_IND];
          angle_snsrbrd[AZI_IND] = angle_measure[AZI_IND];
          
          servoMove(old_alt, old_azi, 3, false, true, true); // restore original position
        }

        void measure_sun_angle(void) {
          // must be done after sun_rel_angles are ready and the servos are pointed correctly.
          measure_absolute();
          angle_sun[ALT_IND] = angle_measure[ALT_IND] + ( (int) sun_rel_angles[ALT_IND] );
          angle_sun[AZI_IND] = angle_measure[AZI_IND] + ( (int) sun_rel_angles[AZI_IND] );
        }

        void calibrateImu(bool endhold, bool move) {
          // imu calibration time
          if (!imu_ready) {
            Serial.printf("[ERROR] calibrateImu(): IMU not ready!\n");
          }
          uint8_t system = 0, gyro = 0, accel = 0, mag = 0; // init at zero (worst case); 3 is fully calibrated
          imu.getCalibration(&system, &gyro, &accel, &mag);
          if (accel == 3 && mag == 3) {
            if (cali_skips < 6) { // 30 minutes since last calibration
              Serial.printf("\tSkipping IMU calibration: A, M Healthy.\n");
              cali_skips++;
              return; // avoid calibrating as it requires servo movement and power expenditure
            } else {
              Serial.printf("\tForcing IMU calibration.\n");
            }
          }
          cali_skips = 0;
          // Used this script to generate: (reduction of an example .ino, removing EEPROM requirements)
          // https://gist.github.com/Happsson/bffb41f083b3bb2d88af2b2e27cb3372
          adafruit_bno055_offsets_t calibrationData;
          calibrationData.accel_offset_x  = -36;
          calibrationData.accel_offset_y  = 5;
          calibrationData.accel_offset_z  = -47;

          calibrationData.gyro_offset_x   = -2;
          calibrationData.gyro_offset_y   = 2;
          calibrationData.gyro_offset_z   = 1;

          calibrationData.mag_offset_z    = -47;
          calibrationData.mag_offset_x    = -2;
          calibrationData.mag_offset_y    = 2;

          calibrationData.accel_radius    = 1000;
          calibrationData.mag_radius      = 792;
          imu.setSensorOffsets(calibrationData);

          imu.setExtCrystalUse(true);
          delay(200);
          if (move) {
            int save_alt = angle_smr[ALT_IND];
            int save_azi = angle_smr[AZI_IND];
            servoMove(MAX_ALT_ANGLE, MIN_AZI_ANGLE, 5, false, true, true);
            servoMove(MAX_ALT_ANGLE, MAX_AZI_ANGLE, 5, false, true, true);
            servoMove(save_alt, save_azi, 5, false, true, true);
          }
          if (endhold) {
            do_delay(CALIBRATION_DELAY, 100, true);
            displayCalStatus();
          }
        }

        void imu_init(void) {
          while (!imu.begin()) {
            Serial.printf("[ERROR] IMU not detected. Waiting...\n");
            delay(200);
          }
          imu_ready = true;
          calibrateImu(true, true);
          Serial.printf("[UPDATE] IMU initialised.\n");
        }

        int sdr_init(bool docheck) {
          bool flagmessage = true;
          int timeout = 3000;
          while ( !SD.begin(CARD_SELECT_PIN) ) { // see if the card is present and can be initialized:
            if (flagmessage) { // only do once per while(){}
              Serial.printf("[ERROR] SD Card initialisation failed. Retrying...\n");
              flagmessage = false;
            }
            if (timeout < 0) {
              Serial.printf("[ERROR] SD Card initialisation failed. Cannot access.\n");
              return 3000 - timeout; // failed
            }
            if (docheck) {
              checkButtonPress(false);
            }
            delay(300); // chill out for a bit, reduce excessive crunching. idle() would be better
            timeout = timeout - 300;
          }
          return 3000 - timeout; // success
        }

        void smr_init(void) {
          if (angle_mids[1] > step_size_store) {
            step_size_store = angle_mids[1] / 4.0;
          }

          altServo.attach(ALT_PIN); //  attaching first seems to help relax
          aziServo.attach(AZI_PIN); //            initialisation 'jerkiness'
          servoMove(angle_mids[ALT_IND], angle_mids[AZI_IND], 500, false, true, false);  // Initialise servos at mid

          Serial.printf("[UPDATE] Servomotors initialised.\n");
        }

        void lss_init(void){
          byte t_addr, t_ch;
          for (byte ls = 0; ls < 4; ls++){
              t_addr = light_addr[ls];
              t_ch   = light_ch[ls];

              if(!light_sensor[ls].begin(t_addr, t_ch)) {
                  Serial.println("[ERROR] Light sensor error. Please check.\n");
              }
          }
          ls_ready = true;
          Serial.printf("[UPDATE] Light sensors ready.\n");
        }

        void dpy_init(void ) {
          dpy_power(1);
          disp.begin(0x3C, true); // Address 0x3C default
          disp.clearDisplay();
          disp.setTextSize(1);
          disp.setTextColor(SH110X_WHITE);
          disp.setRotation(1);
          disp.setCursor(0,0);
        }

        void rtc_init(void) {
          pld_power(1); //ensure clock is running at all times

          while (!rtc.begin()) { // Initialize the RTC.
            Serial.print("[ERROR] RTC not detected. Waiting...\n");
            Serial.flush();
            delay(1000);
          }

          if (!rtc.initialized() || rtc.lostPower()) {
            // I do believe I haven't calibrated this since the submission of the assignment, unless
            // a power issue triggered the event. In either case, I am yet to see a single second of drift.
            Serial.printf("[UPDATE] RTC either uninitialised or lost power. Setting time...\n");
            delay(2000); // ALLOW RTC CRYSTAL TO STABILISE
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            int offset = round( ( 72.96747 / 1000000 * 1000000) / 4.34); // Drift, 1Ms, deviation in ppm, offset mode for TwoHours
            rtc.calibrate(PCF8523_TwoHours, offset); // Perform calibration once drift (seconds) and observation period (seconds)
          }
          rtc.start(); // CLEAR 'STOP' BIT
          Serial.printf("[UPDATE] RTC initialised.\n");
      }
};

Cubesat_Board brd = Cubesat_Board(); // The interface for our main board