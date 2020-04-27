/*
  DDS Waveform generator with Arduino Due
  Author: Christian Rausch (c) 2020 
  Using duetimer library:
  https://github.com/ivanseidel/DueTimer
 */
#include "DueTimer.h"

#define DDS_PHASERES pow(2,32)            // how many phase values can be stored in dds_phase                
#define DDS_WANTEDSAMPLEPERIOD_US 100     // desired time between samples at start of program
#define DDS_DACBITS 12                    // resolution of DAC-outputs in bits
#define DDS_AMPLITUDE_ADCSTEPS 2047       // waveform amplitude at start of program
#define DDS_OFFSET_ADCSTEPS 2047          // waveform offset at start of program
#define DDS_SINETABLELEN_BITS 14          // length of waveform table in bits

#define CMD_INPUT_DDSFREQ 'F'             // command for changing to output frequency input mode
#define CMD_INPUT_DDS_SAMPLEFREQ 'S'      // command for changing to sample frequency input mode
#define CMD_INPUT_DDS_SAMPLEPERIOD 'P'    // command for changing to sample period input mode
#define CMD_INPUT_DDS_AMPLITUDE 'A'       // command for changing to amplitude input mode 
#define CMD_INPUT_DDS_OFFSET 'O'          // command for changing to offset input mode

String inputString = "";                  // a String to hold incoming data
bool stringComplete = false;              // whether the string is complete
char command = ' ';
bool commandIsKnown = false;

volatile float     dds_sampleFreq_Hz;     // sample frequency in Hz
volatile float     dds_samplePeriod_us;   // sample period in us
volatile float     dds_freq = 0;          // frequency of output waveform
volatile uint32_t  dds_phase = 0;         // phase register has 32 bits
volatile uint32_t  dds_phaseinc =  0;     // phase increment register has 32 bits
volatile int16_t   dds_out;
volatile uint16_t  dds_amplitude_adcsteps = DDS_AMPLITUDE_ADCSTEPS;
volatile uint16_t  dds_offset_adcsteps = DDS_OFFSET_ADCSTEPS;
float              dds_sinetable[(int)pow(2, DDS_SINETABLELEN_BITS)];

 
void setup() {
  analogWriteResolution(12);  // set the analog output resolution to 12 bit (4096 levels)
  for (int i = 0; i < pow(2, DDS_SINETABLELEN_BITS); i++) {
    dds_sinetable[i] = sin(2.0 * M_PI * i/pow(2, DDS_SINETABLELEN_BITS));
  }
  // initialize serial:
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  Serial.print("Enter Freq: ");
  Timer3.attachInterrupt(timerHandler);
  Timer3.start(DDS_WANTEDSAMPLEPERIOD_US);   // desired: INT every DDS_WANTEDSAMPLEPERIOD_US us
  dds_samplePeriod_us = Timer3.getPeriod();  // actual time between INTs
  dds_sampleFreq_Hz = Timer3.getFrequency(); // actual INT frequency
}

void loop() {
  // calculate a new phase increment value when a newline arrives:
  if (stringComplete) {
    switch (toupper(command)) {       // change to appropriate input mode if command is known 
      case CMD_INPUT_DDSFREQ:         commandIsKnown = true;
                                      if (isDigit(inputString[0])) {
                                        dds_freq = inputString.toFloat();
                                        dds_phaseinc = dds_freq * dds_samplePeriod_us * 1.0e-6 * DDS_PHASERES;
                                        Serial.println(dds_freq);
                                      }
                                      Serial.println("Enter dds_freq: ");
                                      break;
      case CMD_INPUT_DDS_SAMPLEFREQ:  commandIsKnown = true;
                                      if (isDigit(inputString[0])) {
                                        dds_sampleFreq_Hz = inputString.toFloat();
                                        Timer3.stop();
                                        Timer3.setFrequency(dds_sampleFreq_Hz);
                                        Timer3.start();
                                        dds_sampleFreq_Hz = Timer3.getFrequency();
                                        dds_samplePeriod_us = Timer3.getPeriod();
                                        dds_phaseinc = dds_freq * dds_samplePeriod_us * 1.0e-6 * DDS_PHASERES;
                                        Serial.println(dds_sampleFreq_Hz);
                                      }
                                      Serial.println("Enter dds_sampleFreq_Hz: ");
                                      break;
      case CMD_INPUT_DDS_SAMPLEPERIOD:commandIsKnown = true;
                                      if (isDigit(inputString[0])) {
                                        dds_samplePeriod_us = inputString.toFloat();
                                        Timer3.stop();
                                        Timer3.start(dds_samplePeriod_us);
                                        dds_sampleFreq_Hz = Timer3.getFrequency();
                                        dds_samplePeriod_us = Timer3.getPeriod();
                                        dds_phaseinc = dds_freq * dds_samplePeriod_us * 1.0e-6 * DDS_PHASERES;
                                        Serial.println(dds_samplePeriod_us);
                                      }
                                      Serial.println("Enter dds_samplePeriod_us: ");
                                      break;
      case CMD_INPUT_DDS_AMPLITUDE:   commandIsKnown = true;
                                      if (isDigit(inputString[0])) {
                                        dds_amplitude_adcsteps = inputString.toInt();
                                        Serial.println(dds_amplitude_adcsteps);
                                      }
                                      Serial.println("Enter dds_amplitude_adcsteps: ");
                                      break;
      case CMD_INPUT_DDS_OFFSET:      commandIsKnown = true;
                                      if (isDigit(inputString[0])) {
                                        dds_offset_adcsteps = inputString.toInt();
                                        Serial.println(dds_offset_adcsteps);
                                      }
                                      Serial.println("Enter dds_offset_adcsteps: ");
                                      break;
      default:                        commandIsKnown = false;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  if( commandIsKnown == false){  // change to "print interesting numbers"-mode  
    Serial.print(dds_freq);
    Serial.print(" Hz, dds_samplefreq_Hz =");
    Serial.print(dds_sampleFreq_Hz);
    Serial.print(" Hz, dds_phase = ");
    Serial.print(dds_phase);
    Serial.print(", dds_out = ");
    Serial.print(dds_out);
    Serial.print(", dds_phaseinc = ");
    Serial.print(dds_phaseinc);
    Serial.print(", dds_amplitude_adcsteps = ");
    Serial.print(dds_amplitude_adcsteps);
    Serial.print(", dds_offset_adcsteps = ");
    Serial.println(dds_offset_adcsteps);
  }
  
  // here you can insert other code...
  delay(500);
  
} // end of loop()

/******************************************************************** 
 *  The timerHandler is the place where all the DDS action happens!
 *  This timerHandler should allow a max. sample freq. > 100 ksps
 ********************************************************************
*/
void timerHandler(){
  dds_phase += dds_phaseinc;
  dds_out = dds_amplitude_adcsteps * dds_sinetable[dds_phase >> (32-DDS_SINETABLELEN_BITS)] + dds_offset_adcsteps;
  analogWrite(DAC1, dds_out); // write next sample to DAC
}

/******************************************************************** 
 *  This alternative_timerHandler uses direct calculation of the sine
 *  values instead of a table lookup for instructive purposes
 *  The Cortex M3 on the Arduino Due does not have an FPU so this 
 *  alternative is quite slow, limiting the sample frequency to
 *  about 10 ksps
 ********************************************************************
void alternative_timerHandler(){
  dds_phase += dds_phaseinc;
  dds_sinout = dds_amplitude_adcsteps * sin(2.0 * M_PI * dds_phase / pow(2,32)) + dds_offset_adcsteps;
  analogWrite(DACx, dds_sinout); // write next sample to DACx
}
*/

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (isAlpha(inChar)){
      // incoming character is a letter, so it is probably a command:
      command = inChar;
      inChar = '\n'; // return empty string if user has entered a letter
    }
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
