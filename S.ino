#include "HX711.h"
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Ai";
const char* password = "12345678";
int controlValue;
float w;
#define IR1_PIN 
#define IR2_PIN 
#define IR3_PIN 
#define Motor 
#define servoPin1 
#define servoPin2 
#define proximityPin 
#define conveyorMotorPin 
#define DT 
#define SCK 

WebServer server(80);

Servo servo1;
HX711 scale;
const float calibration_factor = 117073.17;

int current_pos = 0;
int target_pos = 0;

void handleControl() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.print("Raw body: ");
    Serial.println(body);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    Serial.print("Received Control Value: ");
    Serial.println(controlValue);

    // You can add logic to use controlValue here if needed

    server.send(200, "application/json", "{\"status\":\"OK\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No Data\"}");
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(IR3_PIN, INPUT);
  pinMode(Motor, OUTPUT);
  digitalWrite(Motor, LOW);
  pinMode(proximityPin, INPUT);
  pinMode(conveyorMotorPin, OUTPUT);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo1.write(90);
  servo2.write(90);
  digitalWrite(conveyorMotorPin, LOW);

  scale.begin(DT, SCK);
  scale.set_scale(calibration_factor);
  Serial.println("Remove all weight from the scale...");
  delay(3000);
  scale.tare();
  Serial.println("Scale tared. Ready to weigh.");

  // Connect to existing WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/control", HTTP_POST, handleControl);
  server.begin();
  Serial.println("Web server started.");
}


void loop() {
  server.handleClient();
  if (digitalRead(proximityPin) == 1)
  {
    controlValue = 6;
    servo1.write(30);
  }
  switch (controlValue)
  {
    case 0 :
      servo1.write(90);
      servo2.write(90);
      digitalWrite(conveyorMotorPin, LOW);
      controlValue=0;
    break;
    case 1:
      moveToPosition(1);
      digitalWrite(conveyorMotorPin, HIGH);
      delay(12000);
      digitalWrite(conveyorMotorPin, LOW);
      servo2.write(180);
      delay(1000);
      servo2.write(90);
      controlValue=0;
    break;
    case 2:
    digitalWrite(conveyorMotorPin, HIGH);
      delay(12000);
      digitalWrite(conveyorMotorPin, LOW);
      w = getStableWeight();
      if (w>1)
      {
        moveToPosition(2);
      }
      else 
      {
        moveToPosition(3);
      }
      servo2.write(180);
      delay(1000);
      servo2.write(90);
      controlValue=0;
    break;
    case 3:
      digitalWrite(conveyorMotorPin, HIGH);
      delay(12000);
      digitalWrite(conveyorMotorPin, LOW);
      w = getStableWeight();
      if (w>1)
      {
        moveToPosition(2);
      }
      else 
      {
        moveToPosition(3);
      }
      servo2.write(180);
      delay(1000);
      servo2.write(90);
      controlValue=0;
    break;
    case 4:
      moveToPosition(4);
      digitalWrite(conveyorMotorPin, HIGH);
      delay(12000);
      digitalWrite(conveyorMotorPin, LOW);
      servo2.write(180);
      delay(1000);
      servo2.write(90);
      controlValue=0;
    break;
    case 5:
      moveToPosition(5);  
      digitalWrite(conveyorMotorPin, HIGH);
      delay(12000);
      digitalWrite(conveyorMotorPin, LOW);
      servo2.write(180);
      delay(1000);
      servo2.write(90);
      controlValue=0;
    break;
    case 6: 
      servo1.write(30);
      digitalWrite(conveyorMotorPin, HIGH);
      delay(7000);
      digitalWrite(conveyorMotorPin, LOW);
      servo1.write(90);
      controlValue=0;
    break;
    default:
      servo2.write(90);
      digitalWrite(conveyorMotorPin, LOW);
      controlValue=0;
    break;
  }
}

void moveToPosition(int pos) {
  if (pos == current_pos) {
    Serial.println("Already at the desired position.");
    return;
  }

  digitalWrite(Motor, HIGH);
  Serial.println("Moving...");

  while (true) {
    if (isAtPosition(pos)) {
      digitalWrite(Motor, LOW);
      Serial.print("Position ");
      Serial.print(pos);
      Serial.println(" Reached.");
      current_pos = pos;
      break;
    }
  }
}

bool isAtPosition(int pos) {
  const int confirmTime = 200;
  unsigned long startTime = millis();

  while (millis() - startTime < confirmTime) {
    int ir1 = digitalRead(IR1_PIN);
    int ir2 = digitalRead(IR2_PIN);
    int ir3 = digitalRead(IR3_PIN);

    bool match = false;
    switch (pos) {
      case 1: match = (ir1 == 0 && ir2 == 1 && ir3 == 1); break;
      case 2: match = (ir1 == 0 && ir2 == 0 && ir3 == 1); break;
      case 3: match = (ir1 == 0 && ir2 == 0 && ir3 == 0); break;
      case 4: match = (ir1 == 1 && ir2 == 0 && ir3 == 0); break;
    }

    if (!match) return false;
    delay(10);
  }

  return true;
}

float getStableWeight() {
  const int samples = 15;
  float readings[samples];

  for (int i = 0; i < samples; i++) {
    while (!scale.is_ready());
    readings[i] = scale.get_units(1);
    delay(50);
  }

  for (int i = 0; i < samples - 1; i++) {
    for (int j = i + 1; j < samples; j++) {
      if (readings[j] < readings[i]) {
        float temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }
    }
  }

  return readings[samples / 2];
}
