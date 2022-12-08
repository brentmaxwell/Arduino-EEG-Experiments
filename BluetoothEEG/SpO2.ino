// #include <Wire.h>
// #include <MAX30105.h>
// #include <spo2_algorithm.h>

// MAX30105 particleSensor;

// #define MAX_BRIGHTNESS 255
// #define BUFFER_LENGTH 100


// int32_t spo2; //SPO2 value
// int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
// int32_t heartRate; //heart rate value
// int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

// uint32_t irBuffer[BUFFER_LENGTH]; //infrared LED sensor data
// uint32_t redBuffer[BUFFER_LENGTH];  //red LED sensor data
// void setupSpO2()
// {
//   // Initialize sensor
//   particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
//   byte ledBrightness = 60; //Options: 0=Off to 255=50mA
//   byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
//   byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
//   byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
//   int pulseWidth = 411; //Options: 69, 118, 215, 411
//   int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

//   particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
// }

// void loopSpO2()
// {
//   static int t = 0; //buffer length of 100 stores 4 seconds of samples running at 25sps

//   //read the first 100 samples, and determine the signal range
//   if(t < BUFFER_LENGTH){

//     while (particleSensor.available() == false) //do we have new data?
//       particleSensor.check(); //Check the sensor for new data
//     uint32_t red = particleSensor.getRed();
//     uint32_t ir = particleSensor.getIR();
//     redBuffer[t] = red;
//     irBuffer[t] = ir;
//     particleSensor.nextSample(); //We're finished with this sample so move to next sample
//     double output[2] = { red, ir };
//     //writeData("CR", micros(), output, 2 );
//     t++;
//   }
//   else {
//     maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_LENGTH, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
//     for (byte i = 25; i < BUFFER_LENGTH; i++) {
//       redBuffer[i - 25] = redBuffer[i];
//       irBuffer[i - 25] = irBuffer[i];
//     }
//     t = 75;
//     double output[4] = { validHeartRate, heartRate, validSPO2, spo2 };
//     writeData("CD", micros(), output, 4);
//   }
// }
