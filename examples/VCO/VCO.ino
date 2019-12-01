
#include "Arduino.h"
#include "Demiurge.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

CvInPort cv1(1);
CvInPort cv2(2);
CvInPort cv3(3);
CvInPort cv4(4);

GatePort gate(1);

AudioOutPort *out1;
AudioOutPort *out2;

VCO vco1(DEMIURGE_SINE);

Mixer mixer(2);
FixedSignal fixed(1.0);
Pan pan;

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");
  Demiurge::runtime().begin();
  Serial.println("runtime() called");



  Serial.println("setting up mixer");
   mixer.configure(2, &cv1, &fixed);
   mixer.configure(1, &cv2, &fixed);

  Serial.print("Mixer:");
  Serial.println((__uint32_t ) &mixer);
  Serial.print("CV1:");
  Serial.println((__uint32_t ) &cv1);
  Serial.print("CV2:");
  Serial.println((__uint32_t ) &cv2);
  Serial.print("CV3:");
  Serial.println((__uint32_t ) &cv3);
  Serial.print("CV4:");
  Serial.println((__uint32_t ) &cv4);

  Serial.print("Fixed:");
  Serial.println((__uint32_t ) &fixed);
  
  Serial.println("setting up vco");
   vco1.configureFrequency(&mixer);
   vco1.configureAmplitude(&cv3);

  Serial.println("setting up pan");
   pan.configure(&vco1,&cv4);

  Serial.println("setting up output ports");
   out1 = new AudioOutPort(1);   
   out2 = new AudioOutPort(2);   
   out1->configure(pan.outputLeft());
   out2->configure(pan.outputRight());
}

void loop() {
  double value1 = vco1.currentValue();
  uint32_t t = (uint32_t) Demiurge::runtime().lastMeasure();
  Serial.print( t );
  Serial.print( "   " );
  uint32_t over = Demiurge::runtime().overruns();
  Serial.print( over );
  Serial.print( "   " );
  Serial.println( value1 );
  delay(872);
}
