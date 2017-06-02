/*
 * Phult@gmail.com
 * https://github.com/phult
 * ------------------------------------------------------------
 * Pin layout should be as follows:
 * Signal         Pin
 *                Arduino Uno      
 * ------------------------------------------------------------
 * SPI Reset      9
 * SPI SS         10
 * SPI MOSI       11
 * SPI MISO       12
 * SPI SCK        13
 * SERVO          2
 * OPEN BTN       3
 * ACCEPT LED     4
 * DENIED LED     5
 * BUZZER SPK     6   
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 2
#define OPEN_BUTTON_PIN 3
#define ACCEPT_LED_PIN 4
#define DENIED_LED_PIN 5
#define SPEAKER_PIN 6

Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);       
int isOpened = 0;

void setup() {
  pinMode(ACCEPT_LED_PIN, OUTPUT);
  pinMode(DENIED_LED_PIN, OUTPUT);
  pinMode(OPEN_BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);     
  SPI.begin();            
  mfrc522.PCD_Init();    
  myservo.attach(SERVO_PIN);
  close();
  //open(15000);
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    unsigned long uidDec = 0;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      unsigned long uidDecTemp = mfrc522.uid.uidByte[i];
      uidDec = uidDec * 256 + uidDecTemp;
    } 
    /*Serial.print("Card UID: ");
    Serial.println(uidDec);*/
    if (uidDec == 3228631156) {
      open(20000);
    } else {      
      blinkLight(DENIED_LED_PIN, 200);
      checkToClose(false);      
    }
    // Halt PICC
    //mfrc522.PICC_HaltA();
  } else {
    if (digitalRead(OPEN_BUTTON_PIN) == LOW) {
      open(20000);
    } else {
      checkToClose(true);
    }
  }
}

void open(int delaytime) {
  if (isOpened == 0) {
   onLight(ACCEPT_LED_PIN);
   spinServo(90);  
   beep();
   isOpened = 1;
   delay(delaytime);
  }
}

int isIgnoreFirstTimeClosed = 0;
void checkToClose(bool ignoreFirstTime) {
  if (isOpened == 1) {
    if (ignoreFirstTime == true) {
      if (isIgnoreFirstTimeClosed == 0) {
        isIgnoreFirstTimeClosed = 1;
      } else if (isIgnoreFirstTimeClosed == 1) {
        isIgnoreFirstTimeClosed == 0;
        close();
      }
    } else {    
      close();
    }
  }  
}

void close() {
  offLight(ACCEPT_LED_PIN);
  spinServo(0);
  isOpened = 0;
}

void onLight(int pin){
  digitalWrite(pin, 1);
}

void offLight(int pin){        
  digitalWrite(pin, 0);
}

void blinkLight(int pin, int interval){
  onLight(pin);
  delay(interval);           
  offLight(pin);
  delay(interval); 
}

void beep(){
  digitalWrite(SPEAKER_PIN, 1);
  delay(80);
  digitalWrite(SPEAKER_PIN, 0);
  delay(150);
  digitalWrite(SPEAKER_PIN, 1);
  delay(80);
  digitalWrite(SPEAKER_PIN, 0);
}

void spinServo(int angle){
  myservo.write(angle);
  delay(200);
}
