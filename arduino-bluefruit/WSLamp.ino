#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <ColorTools.h>

#include "Defines.h"

// Create the bluefruit object
Adafruit_BluefruitLE_SPI ble(8, 7, 4);
// Create the neopixel object
NeoPixel pixeles = NeoPixel(NUM_PIXELES, PIN_PIXELES);

namespace {
  //Modo actual
  MODO modo = MODO_OLA;
  
  //colores estaticos
  AVB::HSV hsv(0, 1.0, 1.0);
  AVB::RGB rgb(32,0,0);

  float maxB=1.;
  float valor=0;
  double vel = 0.001;
  uint32_t t;
  
  
};

void setColor(AVB::RGB &color){
  for(uint8_t i=0;i<NUM_PIXELES;++i){
    pixeles.setPixelColor(i,color);
  }
  pixeles.update();
}

void setup(void){
  #if DEBUG
    while(!Serial);
    delay(500);
    Serial.begin(19200);
  #endif
  /* Initialise the pixels */
  pixeles.begin();
  setColor(rgb);
  
  /* Initialise the module */
  while ( !ble.begin(VERBOSE_MODE) ){
    #if DEBUG
      Serial.println("CANT INITIALISE THE MODULE");
    #endif
    delay(100);
  }
  
  if ( FACTORYRESET_ENABLE ){
    while ( ! ble.factoryReset() ){
      #if DEBUG
        Serial.println("CANT INITIALISE THE MODULE");
      #endif
      delay(100);
    }
  }
  ble.echo(false);

  ble.verbose(false);  // debug info is a little annoying after this point!

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) ){
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }
  // Set Bluefruit to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);
  Serial.println("OKOK");
}

void loop(void){
  while(ble.available()){
    int r,g,b;
    if(ble.read()=='!'){
      switch(ble.read()){
        case 'H':
          hsv.h = ble.read()<<8 | ble.read();
          hsv.s = (ble.read())/100.;
          hsv.v = (ble.read())/100.;
          toRGB(hsv,rgb);
          setColor(rgb);
          #if DEBUG
            Serial.print("HSV:");
            Serial.print(hsv.h,0);
            Serial.print(", ");
            Serial.print(hsv.s,2);
            Serial.print(", ");
            Serial.println(hsv.v,2);
          #endif
          modo = MODO_ESTATICO;
          break;
        case 'C':
          rgb.r = ble.read();
          rgb.g = ble.read();
          rgb.b = ble.read();
          setColor(rgb);
          #if DEBUG
            Serial.print("RGB:");
            Serial.print(rgb.r);
            Serial.print(", ");
            Serial.print(rgb.g);
            Serial.print(", ");
            Serial.println(rgb.b);
           #endif
           modo = MODO_ESTATICO;
           break;
         case 'O':
          vel = ble.read()/1000.;
          hsv.v = ble.read()/100.;
          hsv.s = 1.;
          #if DEBUG
            Serial.print("OLAS: (V Vel)");
            Serial.print(hsv.v,2);
            Serial.print(", ");
            Serial.println(vel,3);
           #endif
           modo = MODO_OLA;
           break;
         case 'A':
          vel = ble.read()/500.;
          hsv.v = ble.read()/100.;
          hsv.s = 1.;
          #if DEBUG
            Serial.print("Arcoiris: (V Vel)");
            Serial.print(hsv.v,2);
            Serial.print(", ");
            Serial.println(vel,3);
           #endif
           modo = MODO_ARCOIRIS;
           break;
         case 'D':
           hsv.h = ble.read()<<8 | ble.read();
           hsv.s = (ble.read())/100.;
           hsv.v = 0;
           vel = ble.read()/100.;
           maxB = ble.read()/100.;
           #if DEBUG
            Serial.print("Destellos: (H S maxB Vel)");
            Serial.print(hsv.h,0);
            Serial.print(", ");
            Serial.print(hsv.s,2);
            Serial.print(", ");
            Serial.print(maxB,2);
            Serial.print(", ");
            Serial.println(vel,3);
           #endif
           modo = MODO_DESTELLOS;
           break;
         case 'R':
           hsv.h = ble.read()<<8 | ble.read();
           hsv.s = ble.read()/100.;
           vel = ble.read()/30000.;
           maxB = ble.read()/100.;
           #if DEBUG
            Serial.print("Respirar(H S maxB Vel): ");
            Serial.print(hsv.h,0);
            Serial.print(", ");
            Serial.print(hsv.s,2);
            Serial.print(", ");
            Serial.print(maxB,2);
            Serial.print(", ");
            Serial.println(vel,5);
           #endif
           modo = MODO_RESPIRAR;
         default:
          #if DEBUG
            Serial.write(ble.read());
          #else
            ble.read();
          #endif
      }
    }
  }

  switch(modo){
    case MODO_OLA:
      hsv.h = valor;
      toRGB(hsv,rgb);
      setColor(rgb);
      valor += vel;
      if(valor >360) valor -= 360;
      break;
    case MODO_ARCOIRIS:   
      for(uint8_t pixel=0; pixel<NUM_PIXELES;++pixel){
        hsv.h = (sin(pixel*propPixel + (valor*2*PI/360.)) + 1)*propSin;
        pixeles.setPixelColor(pixel ,hsv);
      }
      pixeles.update();
      valor += vel;
      if(valor >360) valor -= 360;
      break;
    case MODO_DESTELLOS:
      toRGB(hsv,rgb);
      setColor(rgb);
      hsv.v *= 0.99;//cae de en 1%
      if(millis()-t > 250){
        if((random(1000)/1000.)<vel){
          hsv.v = maxB;
        }
        t = millis();
      }
      break;
    case MODO_RESPIRAR:
      hsv.v = ((exp(sin(valor)) -0.36787944117)/2.35040238729)*maxB; //funcion de respirar recomendada por Lady Ada, e^sin(x), constantes calculadas a para 1/e y e-(1/e)
      toRGB(hsv,rgb);
      setColor(rgb);
      valor += vel;
      if(valor>6) valor -= 6.25;
      break;
  }
}
