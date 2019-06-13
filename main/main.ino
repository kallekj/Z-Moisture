#include "Wire.h"
#include <ZUNO_DHT.h>

#define SEESAW_STATUS_BASE          0x00
#define SEESAW_TOUCH_BASE           0x0F
#define SEESAW_STATUS_TEMP          0x04
#define SEESAW_TOUCH_CHANNEL_OFFSET 0x10

#define ZUNO_CHANNEL_NUMBER_ONE     1

#define WAKEUP_BEAMS_TIME           255 // every 2 minutes measure and send reports 
#define DEBUG                       0

ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_MOISTURE,
                                          SENSOR_MULTILEVEL_SCALE_PERCENTAGE_VALUE,
                                          SENSOR_MULTILEVEL_SIZE_TWO_BYTES,
                                          SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS,
                                          humidity_getter));

ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_FREQUENTLY_AWAKE);

float tempC;
int capMoist;
//float getTemp();
int touchRead(uint8_t);
void blink(uint8_t, uint16_t);

void setup() {
  pinMode(13, OUTPUT);
  Wire.begin();
  //tempC = getTemp();
  capMoist = touchRead(0);
  blink(3, 50);

  if(zunoGetWakeReason() == ZUNO_WAKEUP_REASON_WUT)
  {
    // Send reports for both channels in case 
    // device was woke up by timer 
    // You can add here your logic to check histeresis of these values...
    blink(7, 50);
    zunoSendReport(1);

  }
  // Set a timer to wakeup the device 
  zunoSetBeamCountWU(WAKEUP_BEAMS_TIME);

  blink(1, 2000);
}

void loop() {
  zunoSendDeviceToSleep(); 
}

/*
float temperature_getter(){
  return tempC;
}
*/

int humidity_getter(){
  return capMoist;
}

void blink(uint8_t nBlinks, uint16_t delayTime){
  for(uint8_t i = 0; i < nBlinks; i++){
    delay(delayTime);
    digitalWrite(13, HIGH);
    delay(delayTime);
    digitalWrite(13, LOW);
  }
}

/*
float getTemp(){
	uint8_t buf[4];
	read(SEESAW_STATUS_BASE, SEESAW_STATUS_TEMP, buf, 4, 1000);
	int32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return (1.0/(1UL << 16)) * ret;
}
*/

int touchRead(uint8_t pin){
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