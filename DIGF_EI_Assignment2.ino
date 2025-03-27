#include <Servo.h>  // Include the Servo library
#include <Keyboard.h> // Include the Keyboard HID library

// Define pin assignments
#define SENSOR_PIN A3    // Analog pin for the ultrasonic sensor
#define SERVO_PIN 9      // Digital pin for the servo motor

// Create a Servo object
Servo myServo;

// Global variable to store the measured distance in centimeters
float distanceCM = 0.0;

// Define distance breakpoints for the servo movement
const int stepDistances[3] = {150, 100, 50};  // Distance thresholds (cm)
const int servoPositions[3] = {0, 90, 180};  // Corresponding servo angles

// Variable to track the current servo angle
int currentServoAngle = 0;

// Define tolerance to prevent frequent fluctuations
const float tolerance = 5.0;  

void setup() {
  Serial.begin(9600);  // Start serial communication for debugging
  myServo.attach(SERVO_PIN);  // Attach the servo motor
  myServo.write(0);  // Set the servo to the initial position (0°)
  Serial.println("System Initialized. Servo at 0°.");

  Keyboard.begin(); // Inititalize Keyboard HID
}

void loop() {
  // Read the sensor value and convert it to distance
  readSensor();
  
  // Update the servo position based on the detected distance
  updateServo();
}

// Function to read and convert the sensor's analog value to distance in cm
void readSensor() {
  int sensorValue = analogRead(SENSOR_PIN);  // Read the analog value
  float voltage = sensorValue * (3.3 / 1023.0);  // Convert to voltage (assuming 3.3V reference)
  distanceCM = voltage * 120;  // Convert voltage to distance in cm
  
  Serial.print("Distance: ");
  Serial.print(distanceCM);
  Serial.println(" cm");
}

// Function to update the servo based on the detected distance
void updateServo() {
  int newAngle = currentServoAngle;  // Store the current angle
  
  // Check which step the distance falls into and send the corresponding keyboard key press to play soundscape for that life stage
  for (int i = 0; i < 3; i++) {
    if (abs(distanceCM - stepDistances[i]) <= tolerance) {
      if ((stepDistances[i] <=150) && (stepDistances[i] > 100)){
        Keyboard.releaseAll();
        Keyboard.press('d');
      }
      else if ((stepDistances[i] <=100) && (stepDistances[i] > 50)){
        Keyboard.releaseAll();
        Keyboard.press('f');
      }
      else if (stepDistances[i] <=50){
        Keyboard.releaseAll();
        Keyboard.press('g');
      }
      
      newAngle = servoPositions[i];  // Set the new angle based on the detected step
      break;  // Exit loop once a match is found
    }
  }

  // If no person is detected (distance > 110 cm), return to 0° and release all keyboard keys
  if (distanceCM > 110 || distanceCM < 10) {
    newAngle = 0;
    Keyboard.releaseAll();
  }

  // Update the servo only if the new position is different
  if (newAngle != currentServoAngle) {
    currentServoAngle = newAngle;  
    myServo.write(currentServoAngle);  
    Serial.print("Moving Servo to: ");
    Serial.print(currentServoAngle);
    Serial.println("°");
  }
}
