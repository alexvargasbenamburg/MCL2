#ifndef DEFINES_H
#define DEFINED_H

#define DEBUG                       false
#define FACTORYRESET_ENABLE         1         // reset to know good state
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"   // minimun version of firmware
#define MODE_LED_BEHAVIOUR          "DISABLE" // Blue led mode
#define PIN_PIXELES                 A1        // Neopixel pin      
#define NUM_PIXELES                 9        // number of neopixels
#define VERCION                     1 
#define VERBOSE_MODE                false

enum MODO{
  MODO_ESTATICO,
  MODO_OLA,
  MODO_ARCOIRIS,
  MODO_DESTELLOS,
  MODO_RESPIRAR,
  
};


//calculo proporciones para evitar calculos innesesarios
const float propPixel = (2.*PI)/(float)NUM_PIXELES; // posicion de cada pixel
const float propSin = 360./2; // mapes de 2(resultado de seno + 1) a 360(tono en hsv)

#endif
