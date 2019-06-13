#include "Wire.h"
#include "Arduino.h"

#define SEESAW_STATUS_BASE          0x00
#define SEESAW_TOUCH_BASE           0x0F
#define SEESAW_STATUS_TEMP          0x04
#define SEESAW_TOUCH_CHANNEL_OFFSET 0x10

#define WAKEUP_BEAMS_TIME           60 // every 2 minutes measure and send reports 
#define DEBUG                       0


float tempC;
int capMoist;
int capMoist2;
int minVal = 300;
int maxVal = 1300;
float getTemp();
int touchRead(uint8_t);
int touchRead2(uint8_t);
void blink(uint8_t, uint16_t);

void setup() {
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    Wire.begin();
    tempC = getTemp();
    capMoist = touchRead2(0);
    capMoist2 = map(capMoist, minVal, maxVal, 0, 100);

}

void loop() {
    tempC = getTemp();
    capMoist = touchRead2(0);
    capMoist2 = map(capMoist, minVal, maxVal, 0, 100);

    Serial.print("Temperature: "); Serial.print(tempC); Serial.println("*C");
    Serial.print("Capacitive: "); Serial.print(capMoist2); Serial.println("%");
    Serial.print("Capacitive: "); Serial.println(capMoist);
    delay(2000);
}



void blink(uint8_t nBlinks, uint16_t delayTime){
  for(uint8_t i = 0; i < nBlinks; i++){
    delay(delayTime);
    digitalWrite(13, HIGH);
    delay(delayTime);
    digitalWrite(13, LOW);
  }
}

float getTemp(){
	uint8_t buf[4];
	read(SEESAW_STATUS_BASE, SEESAW_STATUS_TEMP, buf, 4, 1000);
	int32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return (1.0/(1UL << 16)) * ret;
}

int touchRead(uint8_t pin){
	uint8_t buf[2];
    int readings = 0;
	uint8_t p = pin;

    for(int i = 0; i < 20; i++){
        read(SEESAW_TOUCH_BASE, SEESAW_TOUCH_CHANNEL_OFFSET + p, buf, 2, 1000);
	    int ret = (int) (((uint16_t)buf[0] << 8) | buf[1]);
        readings = readings + ret;
        delay(10);
    }
	return (int) readings/20;
}

int touchRead2(uint8_t pin){
	  uint8_t buf[2];
	  uint8_t p = pin;
    read(SEESAW_TOUCH_BASE, SEESAW_TOUCH_CHANNEL_OFFSET + p, buf, 2, 1000);
    int ret = (int) (((uint16_t)buf[0] << 8) | buf[1]);
	  return ret;
}

void read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t delay){
  uint8_t pos = 0;
  
  while(pos < num){
    uint8_t read_now = min(32, num - pos);
    Wire.beginTransmission((uint8_t)0x36);
    Wire.write((uint8_t)regHigh);
    Wire.write((uint8_t)regLow);
    Wire.endTransmission();
    
    delayMicroseconds(delay);
    
    Wire.requestFrom((uint8_t)0x36, read_now);
    
    for(int i=0; i<read_now; i++){
      buf[pos] = Wire.read();
      pos++;
    }
  }
}