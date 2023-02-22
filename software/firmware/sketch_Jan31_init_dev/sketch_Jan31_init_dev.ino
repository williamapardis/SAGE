#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_MPRLS.h"
#include "RTClib.h"

//TwoWire I2Czero = TwoWire(0);
TwoWire I2Cone = TwoWire(1);

//Real time clock DS3231 0x68
RTC_DS3231 rtc;

//Pressure Temperature and Relative Humidity Sensor BME280
Adafruit_BME280 bme; // I2C

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect 0x18
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr1 = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect 0x18
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr2 = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

const int SAMPLE_PERIOD = 1000;
const int BUFFER_SIZE = 50;
char buf[BUFFER_SIZE];
float timer,stop;

void setup() {

  //ADC bit resolution setting
  analogReadResolution(12);

  //I2Czero.begin(SDA,SCL);
  Wire.begin();
  I2Cone.begin(SDA1,SCL1);

  Serial.begin(115200);
  //while (!Serial) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //}
  Serial1.begin(9600);
  Serial.println("Begin");

  //start clock
  if (! rtc.begin(&I2Cone)) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  //set time
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // default setting
  if (!bme.begin(&I2Cone)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
  }  

  if (! mpr1.begin(0x18,&I2Cone)) {
    Serial.println("Failed to communicate with MPRLS1 sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }

  if (! mpr2.begin()) {
    Serial.println("Failed to communicate with MPRLS2 sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }

  timer = millis();
  
}



void loop() {


    //timing
    if(timer > millis()){timer = millis();}

    
    
    //Serial.println(millis()-timer);
    delay(250);
    
    if(millis() - timer > SAMPLE_PERIOD){
      timer = millis();

      //Convience header
      Serial.println("Time[ISO],BME_T[C],BME_P[hPa],BME_RH[%],MPR1_P[hPa],MPR2_P[hPa],A0[12BIT],A3[12BIT]");

      //Time Aquire
      DateTime now = rtc.now();
      Serial.print(now.year(), DEC);
      Serial.print(now.month(), DEC);
      Serial.print(now.day(), DEC);
      Serial.print("T");
      Serial.print(now.hour(), DEC);
      Serial.print(now.minute(), DEC);
      Serial.print(now.second(), DEC);
      Serial.print(',');

      //BME280 Aquire
      Serial.print(bme.readTemperature());Serial.print(',');
      Serial.print(bme.readPressure() / 100.0F);Serial.print(',');
      Serial.print(bme.readHumidity());Serial.print(',');

      //MPRLS Aquire
      Serial.print(mpr1.readPressure());Serial.print(',');
      Serial.print(mpr2.readPressure());Serial.print(',');

      Serial.print(analogRead(A0));Serial.print(',');
      Serial.print(analogRead(A3));Serial.print(',');

      Serial.println();

      stop = millis();
      Serial.println(timer-stop);
      Serial.println();
      Serial.println();

      
    }
  
}

