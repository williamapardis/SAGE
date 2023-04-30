//libraries
#include <Wire.h>
#include <time.h> 
#include "WiFi.h"
#include <Adafruit_TestBed.h>
extern Adafruit_TestBed TB;
//I2C device libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_PWMServoDriver.h>
#include "Adafruit_MPRLS.h"
//SD libraries
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define DEFAULT_I2C_PORT &Wire
#define SECONDARY_I2C_PORT &Wire1

//time offsets
long timezone = 1; 
byte daysavetime = 1;

const char* ssid = "Scientists2.4";
const char* password = "FalkorTooSC1";

IPAddress IP;

const char* html="<html>"
      "<head>"
      "<meta charset=\"utf-8\">"
      "<title>ESP32-S2 Web control NeoPixel</title>"
      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\">"
      "</head>"
      "<body>"
      "Click <a href=\"/H\">here</a> to turn the RGB LED (NeoPixel) ON.<br>"
      "Click <a href=\"/L\">here</a> to turn the RGB LED (NeoPixel) OFF.<br>"
      "</body>"
      "</html>";

// Create AsyncWebServer object on port 80
//AsyncWebServer server(80);
// Set web server port number to 80
WiFiServer server(80);

//BME280 Pressure Temerpature and Relative Humidity
Adafruit_BME280 bme; // I2C

//PWM driver 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr0 = Adafruit_MPRLS(RESET_PIN, EOC_PIN);
// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr1 = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

const int BUFFER_SIZE = 100;
char buf[BUFFER_SIZE];

void setup() {

  analogReadResolution(12);


  Wire1.setPins(SDA1, SCL1);

  //serial ports init
  Serial.begin(115200);Serial.flush();
  Serial1.begin(115200);Serial1.flush();
  while(!Serial){delay(10000);break;}


  //First try to connect to WiFi to sync time
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //collect time from web
  Serial.println("Contacting Time Server");
  configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
	struct tm tmstruct ;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
	Serial.printf("\nNow is... %d-%02d-%02d %02d:%02d:%02d\n\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  WiFi.disconnect();
  
  //initilize SD card
  Serial.print("initilizing SD card... ");
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
  }
  Serial.println("SD initilized");  
  //print storage
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  //check or creat data diretory 
  char dataDir[30];
  sprintf(dataDir,"/data/%d%02d%02dT%02d%02d%02d",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  createDir(SD,dataDir);
  //list root directory
  listDir(SD, "/data", 0);


  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  ssid = "SAGE-Access-Point";
  password = "WHOI1930";
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();


  //Scan I2C bus and print the available addresses
  Serial.println("scanning I2C busses");
  Serial.print("Default port (Wire0) ");
  TB.theWire = DEFAULT_I2C_PORT;
  TB.printI2CBusScan();  
  Serial.print("Secondary port (Wire1) ");
  TB.theWire = SECONDARY_I2C_PORT;
  TB.printI2CBusScan();

  //initilize BME280
  Serial.print("initilizing BME280, bus 1, addr 0x76... "); 
    if (!bme.begin(&Wire1)) {
        Serial.println("failed");
    }
  Serial.println("found BME280 sensor");
  
  //initilize PWM driver
  Serial.print("initilizing PWM driver, bus 0, addr 0x40... ");
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  Serial.println("PWM driver initilized");

  //initilizing MPRLS pressure sensors
  Serial.print("initilizing MPRLS pressure sensor, bus 0, addr 0x18... ");
  if (! mpr0.begin(0x18,&Wire)) {
    Serial.println("failed");
  }
  Serial.println("found MPRLS sensor");
  Serial.print("initilizing MPRLS pressure sensor, bus 1, addr 0x18... ");
  if (! mpr1.begin(0x18,&Wire1)) {
    Serial.println("failed");
  }
  Serial.println("found MPRLS sensor");

}

//for(int i=0;i<256;i++){
//    dacWrite(17,i);delay(100);Serial.println(i);
//}

void loop() {
  incoming();
  //wifiIncoming();
}


void wifiIncoming(){
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.print(html);
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        Serial.println(currentLine);
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          Serial.println("\n--- ON ---");
        }
        if (currentLine.endsWith("GET /L")) {
          Serial.println("\n--- OFF ---");
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }  
}

void incoming(){
  while(Serial.available()>0){
    Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
    Serial.print("command recieved: ");Serial.println(buf);
    parseCMD(buf);
    memset(buf, 0, BUFFER_SIZE);;
  }
}

void parseCMD(char cmd[]) {
  if(cmd[0]=='S'){
    //read in load number and feed to duty cycle function
    set(cmd);        
  }
  else if(cmd[0]=='G'){
    Serial.println(get(cmd));
  }
}

void set(char cmd[]) {
  switch (cmd[1]){

    case 'L':   //setting PWM loads
      if(cmd[3]==':'){
        double duty = atof(&cmd[4]);
        int load = cmd[2]- '0';
        Serial.print("duty clcyle: ");Serial.println(duty);
        if(duty > 1){
          Serial.println("invalid duty cycle, select 0-1");
        }
        else{
          Serial.print("load ");Serial.print(load);Serial.print(" set to: ");Serial.print(duty*100,0);Serial.println('%');
          Serial.println((int)4095*duty);
          pwm.setPWM(load, 0, (int)4095*duty);
        }
      }
    break;


    case 'A':   //setting analog outputs
      if(cmd[3]==':'){
        double volts = atof(&cmd[4]);
        int out = 4095*(volts/3.3);
        Serial.print("setting A");Serial.print(cmd[2]);Serial.print(":");Serial.println(out);      
        switch(cmd[2]){
          case '0': analogWrite(A0,out); break;
          case '1': analogWrite(A1,out); break;        
        }
      }
    break;        
  }
}

double get(char cmd[]){
  switch (cmd[1]){
    
    case 'A':   //get analog input
      switch(cmd[2]){
        case '0': return analogRead(A0);
        case '1': return analogRead(A1);
        case '2': return analogRead(A2);
        case '3': return analogRead(A3);
      }
    break;


    case 'P':   //get pressure hPa      
      switch(cmd[2]){
      case '0':
        return mpr0.readPressure();
        break;
      case '1':
        return mpr1.readPressure();
        break;
      case '2':
        return bme.readPressure() / 100.0F;
        break;
      default:
        invalid(cmd);
        break;
    }
    break;

    case 'I':
      Serial.print("AP IP address: ");
      Serial.println(IP);
    break;
    

  }
}

void invalid(char cmd[]){
  Serial.print(cmd);Serial.println(" is a invalid command");
}