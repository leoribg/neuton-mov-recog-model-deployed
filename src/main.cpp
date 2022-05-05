#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include "neuton.h"

enum Target {
  TARGET_IDLE,
  TARGET_UP_AND_DOWN,
  TARGET_LEFT_AND_RIGHT,
  TARGET_ROTATE
};

#define NUM_SAMPLES       600
#define GESTURE_TARGET    TARGET_ROTATE

typedef struct {
  float acc_x;
  float acc_y;
  float acc_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
} vector_t;

vector_t data[NUM_SAMPLES];

int i = 0;
int gesture_count = 0;

// Function prototypes
void save_acc_data(float x, float y, float z);
void save_gyro_data(float x, float y, float z);

void setup() {
  // put your setup code here, to run once:

  // Initialize Serial
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Started");

  // Initialize IMU
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in G's");
  Serial.println("X\tY\tZ");

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
  Serial.println("X\tY\tZ");
}

void loop() {
  // put your main code here, to run repeatedly:

  // IMU variables
  float x, y, z;

  // Read IMU
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    // Read accelerometer
    IMU.readAcceleration(x, y, z);

    // Save accelerometer data
    save_acc_data(x, y, z);

    // Read gyroscope
    IMU.readGyroscope(x, y, z);

    // Save gyroscope data
    save_gyro_data(x, y, z);

    i++;
    if(i == NUM_SAMPLES) {
      /* Set model input */
      if (neuton_model_set_inputs((input_t*)data) == 0)
      {
        uint16_t predictedClass;
        float *probabilities;
        /* Run inference */
        if (neuton_model_run_inference(&predictedClass, &probabilities) == 0)
        {
          // code for handling prediction result
          Serial.print("Detected gesture: ");
          if(predictedClass == 0) {
            Serial.println("Idle");
          } else if(predictedClass == 1) {
            Serial.println("Up and down");
          } else if(predictedClass == 2) {
            Serial.println("Left and right");
          } else {
            Serial.println("Rotate");
          }
          Serial.print("Probabilities: ");
          for (int j = 0; j < neuton_model_outputs_count(); j++)
          {
            Serial.print(probabilities[j]);
            Serial.print(" ");
          }
          Serial.println();
        }
      }
      i = 0;
    }
  }
}

void save_acc_data(float x, float y, float z) {
  data[i].acc_x = x;
  data[i].acc_y = y;
  data[i].acc_z = z;
}

void save_gyro_data(float x, float y, float z) {
  data[i].gyro_x = x;
  data[i].gyro_y = y;
  data[i].gyro_z = z;
}