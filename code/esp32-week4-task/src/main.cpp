#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#define BH1750_ADDRESS 0x23
#define BH1750_DATALEN 2

#define LED1 5
#define LED2 4
#define LED3 16
#define LED4 17


#define BUTTON_PIN 15
#define EEPROMsize 32

#define BOOL_STR(ot) (ot?"1":"0")
#define STR_BOOL(dt) (dt=="1"? 1 : 0)

unsigned char writePage[256] = "";
unsigned char readPage[256] = "";

void bh1750Request(int address);
int bh1750GetData(int address);

void bacaEEPROM(int alamat, char * Data);
void tulisEEPROM(int alamat, char * Data, String label);

//bool otomatis;
String data_mem;

byte buff[2];
unsigned short lux = 0;

volatile bool interruptState = false;
int totalInterruptCounter = 0;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux =  portMUX_INITIALIZER_UNLOCKED;
//portMUX_TYPE gpioIntMux =portMUX_INITIALIZER_UNLOCKED;

int indekData,salah,lokasiIndek[4];
char bacaData[EEPROMsize], ssid[EEPROMsize],password[EEPROMsize],otomatis[EEPROMsize],receivedData[EEPROMsize];

int dataIndex = 0;

unsigned char statusLED = LOW, statusTombol;
int led_status = LOW;
char LED[4]={LED1,LED2,LED3,LED4};
int j,indexstrok;
char *strings[128];
char *ptr = NULL;
 
char *pertama;
char *kedua;

void IRAM_ATTR gpioISR();
void sensorNyala();
void readEEPROM(int alamat, char * Data);
void writeEEPROM(int alamat, char * Data, String text);


//fungsi untuk menyalakan LED 1-4 
void led(bool led1, bool led2, bool led3,bool led4){
  if(led1 == 1) {
      digitalWrite(LED1, HIGH);
  }else{
      digitalWrite(LED1, LOW);
  }
    if(led2 == 1) {
      digitalWrite(LED2, HIGH);
  }else{
      digitalWrite(LED2, LOW);
  }
    if(led3 == 1) {
      digitalWrite(LED3, HIGH);
  }else{
      digitalWrite(LED3, LOW);
  }
    if(led4 == 1) {
      digitalWrite(LED4, HIGH);
  }else{
      digitalWrite(LED4, LOW);
  }
}

void setup() {
    // put your setup code here, to run once:       
    
    Serial.begin(9600);
    Serial.println("Tugas Week IV");
    
    Wire.begin();
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUILTIN_LED, OUTPUT);

    attachInterrupt(BUTTON_PIN,&gpioISR,FALLING);

    EEPROM.begin(128);
    delay(100);
    readEEPROM(0,otomatis);
    readEEPROM(32,ssid);
    readEEPROM(64,password);

    Serial.print("SSID :");
    Serial.println(ssid);
    Serial.print("password :");
    Serial.println(password);


    Serial.print("Otomatis :");

    if(otomatis[0]=='1') {
        Serial.println(" ON");
        led_status = 1;
        digitalWrite(BUILTIN_LED, HIGH);
    }else{
        Serial.println(" OFF");
        led_status = 0;
        digitalWrite(BUILTIN_LED, LOW);
    for(int i=1; i<5; i++){
        Serial.print("LED");  Serial.print(i-1);
        if(otomatis[i]=='1'){
          Serial.println(" ON");
          digitalWrite(LED[i-1], HIGH);
        }
        else{
          Serial.println(" OFF");
          digitalWrite(LED[i-1], LOW);
        }
          
      }
    }


}

void loop() {

if (statusTombol == LOW && digitalRead(BUTTON_PIN) == LOW){
    portENTER_CRITICAL(&timerMux);    
    statusTombol = HIGH;    
    portEXIT_CRITICAL(&timerMux);
    delay(1000);
    attachInterrupt(BUTTON_PIN,&gpioISR,RISING);
    led_status =! led_status;

    if(led_status) { 
      otomatis[0] = '1';
      digitalWrite(BUILTIN_LED, HIGH);
      writeEEPROM(0,otomatis," Otomatis: 1 ");
    }else{
      otomatis[0] = '0';
      digitalWrite(BUILTIN_LED, LOW);
      for ( int i = 1; i < 5; i++ ){
        if ( digitalRead(LED[i-1]) )
          otomatis[i] = '1';
        else
          otomatis[i] = '0';
      }
      writeEEPROM(0,otomatis," Otomatis: 0 ");
    }   
    memset(otomatis,0,EEPROMsize); 
  }
  if(led_status)
    sensorNyala();

  //Write EEPROM
  if(Serial.available()){
    receivedData[dataIndex] = Serial.read();
    dataIndex++;

    if(receivedData[dataIndex - 1] == '\n'){
      dataIndex = 0;
      
      //parse
      byte indexstrok = 0;
      ptr = strtok(receivedData, ";");
      while (ptr != NULL){
        strings[indexstrok] = ptr;
        indexstrok++;
        ptr = strtok(NULL, ";");
      }
      for (int i=0; i<indexstrok; i++){
        pertama  = strings[0];
        kedua = strings[1];
      }
    

      Serial.print("SSID     :");   Serial.println(pertama);
      Serial.print("Password :");   Serial.println(kedua);
      writeEEPROM(32,pertama,"SSID");
      memset(pertama,0,32);  

      writeEEPROM(64,kedua,"Password");
      memset(kedua,0,32);

    }
  }
 
}

void bh1750Request(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x10);
  Wire.endTransmission();
}


int bh1750GetData(int address) {
  int i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  return i;
}

void IRAM_ATTR gpioISR(){
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
  portENTER_CRITICAL(&timerMux);
  statusTombol=LOW;
  portEXIT_CRITICAL(&timerMux);
}

void sensorNyala(){
        bh1750Request(BH1750_ADDRESS);
        delay(1000);

        if (bh1750GetData(BH1750_ADDRESS) == BH1750_DATALEN) {
            lux = (((unsigned short)buff[0] << 8) | (unsigned short)buff[1]) / 1.2;
            String sentence = "Nilai intensitas cahaya: " + String(lux) + " lux";
            Serial.println(sentence);
            if (lux >= 0 && lux <= 250) {
                led(1,1,1,1);
            } else if (lux > 250 && lux <= 500){
                led(1,1,1,0);
            } else if (lux > 500 && lux <= 750){
                led(1,1,0,0);
            } else if (lux > 750 && lux <= 1000){
                led(1,0,0,0);
            } else {
                led(0,0,0,0);
            }
        }
}

void readEEPROM(int alamat, char * Data){
  for(int i = 0; i < 32; i++)
    Data[i]=EEPROM.read(alamat+i);
}

void writeEEPROM(int alamat, char * Data, String text){
  Serial.println("Start writing to EEPROM : " + text);
  for(int i = 0; i < 32; i++)
    EEPROM.write(alamat + i,Data[i]);
  EEPROM.commit();
}