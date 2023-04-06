void setup() {
  while(!Serial){delay(10000);break;}
  Serial.begin(115200);
  Serial1.begin(115200);

}

void loop() {
  checkIncoming();
}

void checkIncoming(){
  if(Serial.available()){
    if(Serial.read()=='S'){
      Serial.println("setting");
      parseCMD(Serial);
    }
  }
  if(Serial1.available()){

  }
}

void parseCMD(Stream &port) {
  cmd = port.read();
  switch (cmd){
    case 'L':
      //read in load number and feed to duty cycle function
      setDuty(Serial.read());        
      break;
  }
}

void setDuty(char load) {
  if(Serial.read()==':'){
    char Ibuf[6];
    Serial.readBytesUntil('\n',Ibuf,5);
    double duty = atof(Ibuf);
    Serial.print("duty clcyle: ");Serial.println(duty);
    if(duty > 1){
      Serial.println("invalid duty cycle, select 0-1");
    }
    else{
      Serial.print("load ");Serial.print(load);Serial.print(" set to: ");Serial.print(duty*100,0);Serial.println('%');
      Serial.println((int)4095*duty);
    }
  }
}