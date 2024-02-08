// Include Libraries
#include <Arduino.h> // Include the Arduino core library
#include <DHT.h> // Include the DHT library for the humidity and temperature sensor
#include <Stepper.h> // Include the Stepper library
#include <EEPROM.h>

#include <SoftwareSerial.h>
SoftwareSerial mySerial(0, 1);  // RX, TX

// Pin Definitions
#define DHT_PIN_DATA  7 // Define the pin for DHT data

#define inside_ledPin A2// Define the LED pin
#define auto_ledPin A3
#define outside_ledPin A4
#define sensorPin A0 // Define the pin for the rain sensor
#define ldr_pin A1

//Step Motor
int motorSpeed = 10; // Set the motor speed
Stepper myStepper(2048, 12,9,6,5); // Initialize the Stepper object with the specified parameters


bool isClothsOutside; // Flag to track if the motor has rotated
//bool isAuto;



//DHT Sensor
DHT dht(DHT_PIN_DATA); // Create a DHT object


//////////////Thingspeak Read and Write //////////////////////////////////

// Pin Definitions
#define WIFI_PIN_TX 8
#define WIFI_PIN_RX 13

#define LED_PIN_off 10
#define LED_PIN_on 11

SoftwareSerial espSerial(WIFI_PIN_RX, WIFI_PIN_TX);

const char* SSID = "admin";// Enter your Wi-Fi name
const char* PASSWORD = "admin"; // Enter your Wi-Fi password

String HOST = "api.thingspeak.com";
String PORT = "80";
String API = "ZQRO45V38G5PA209"; // Write API KEY

String motorStatus = "/channels/2225714/fields/1/last.txt";
//String isClothOutsideField = "/channels/2225714/fields/6/last.txt";
//String isAutoField = "/channels/2225714/fields/7/last.txt";


// Sample Values for Sensor Data
float Temperature;
float Humidity;
float rainValue;
float lightIntensity;

int countTrueCommand;
int countTimeCommand;
boolean found = false;

void setup() {
  Serial.begin(9600); // Initialize the serial communication
  mySerial.begin(9600); // SoftwareSerial communication

  myStepper.setSpeed(motorSpeed); // Set the motor speed

  while (!Serial); // Wait for the serial port to connect (needed for native USB)
  Serial.println("start"); // Print "start" message
  mySerial.println("Start");
  

  pinMode(inside_ledPin, OUTPUT); // Set the LED pin as an output
  pinMode(auto_ledPin, OUTPUT);
  pinMode(outside_ledPin, OUTPUT);

  pinMode(DHT_PIN_DATA, INPUT);
  

  digitalWrite(inside_ledPin, LOW); // Turn off the LED
  digitalWrite(auto_ledPin, LOW);
  digitalWrite(outside_ledPin, LOW);

  dht.begin(); // Initialize the DHT sensor


  ///////////////// Thingspeak //////////////////////////
  // Initialize ESP8266 serial communication
  espSerial.begin(9600);

  connectToWiFi();

  //Define testing LED pin output
  pinMode(LED_PIN_off, OUTPUT);
  pinMode(LED_PIN_on, OUTPUT);

  //isClothsOutside = readThingSpeak(isClothOutsideField);

  //delay(30);

  //isAuto = readThingSpeak(isAutoField);

  //delay(30);
  isClothsOutside = EEPROM.read(0);
  //isAuto = EEPROM.read(1);

}

void loop() {

  //Serial.print("isClothsOutside = ");
  //Serial.println(isClothsOutside);
  //mySerial.print("isClothsOutside = ");
  //mySerial.println(isClothsOutside);
  //Serial.print("isAuto = ");
  //Serial.println(isAuto);


  // LDR
  float lightIntensity = analogRead(ldr_pin); // Read the ldr_value intensity from analog pin A1

  delay(30);

  //DHT
  float Humidity = dht.readHumidity(); // Read the humidity value from the DHT sensor
  float Temperature = dht.readTempC(); // Read the temperature value from the DHT sensor in Celsius

  delay(30);

  //Rain Sensor
  float rainValue = readRainSensor(); // Read the rain sensor value

  delay(30);

  printToSerial(float(lightIntensity), float(Humidity), float(Temperature), float(rainValue));

  ////////////////////////////////////// From Here Onwards thingspeak ////////////

  float checkFieldStatus = readThingSpeak(motorStatus);
  Serial.println(checkFieldStatus);
  mySerial.println(checkFieldStatus);


  if (checkFieldStatus) {
      manualModeon();
      }
   
    
    else  {
      autoMode(lightIntensity, Humidity, rainValue);
    }
    EEPROM.write(0, isClothsOutside);
    //EEPROM.write(1, isAuto);

writeThingSpeak(Temperature, Humidity, rainValue, lightIntensity);
    
}

void manualModeon() {
  Serial.println(F("Manual mode on"));
  mySerial.println("Manual mode on");
  digitalWrite(LED_PIN_on, HIGH);
  digitalWrite(LED_PIN_off, LOW);
  digitalWrite(auto_ledPin, LOW);
  turnOnInsideClothes();
  
}


void autoMode(int lightIntensity, int Humidity, int rainValue) {
  digitalWrite(LED_PIN_on, LOW);
  digitalWrite(LED_PIN_off, HIGH);
  Serial.println(F("Auto mode on"));
  mySerial.println("Auto mode on");
  digitalWrite(auto_ledPin, HIGH);

  if (lightIntensity < 20|| Humidity > 100 || rainValue > 150)  {
    turnOnInsideClothes();
  } else  {
    turnOnOutsideClothes();
  }
}

void turnOnInsideClothes() {
  
  if (isClothsOutside){
    myStepper.step(1024);
    }
  digitalWrite(inside_ledPin, HIGH);
  Serial.println(F("Now inside clothes ")); 
  mySerial.println("Now inside clothes ");
  digitalWrite(outside_ledPin, LOW);
  isClothsOutside = false;
  delay(5000);
}

void turnOnOutsideClothes() {
  
  if (!isClothsOutside){
    myStepper.step(-1024);}
  digitalWrite(inside_ledPin, LOW);
  Serial.println(F("Now outside clothes "));
  mySerial.println("Now outside clothes ");
  isClothsOutside = true;
  digitalWrite(outside_ledPin, HIGH);
  delay(1000);
  
}

// This function returns the analog data of Rain Sensor
int readRainSensor() {
  int sensorValue = analogRead(sensorPin);  // Read the analog value from the rain sensor
  int outputValue = map(sensorValue, 0, 1023, 255, 0); // Map the 10-bit data to 8-bit data
  //analogWrite(ledPin, outputValue); // Generate PWM signal
  return outputValue; // Return the analog rain value
}


void printToSerial(float(lightIntensity_), float(Humidity_), float(Temperature_), float(rainValue_)) {

  Serial.print(F("LDR Value ")); // Print the label
  Serial.println(lightIntensity_); // Print the ldr_value intensity value

  mySerial.print("LDR Value ");
  mySerial.println(lightIntensity_);

  Serial.print(F("Humidity: ")); // Print the label
  Serial.print(Humidity_); // Print the humidity value
  Serial.println(F(" [%]\t")); // Print the unit for humidity

  Serial.print("Temperature: "); // Print the label
  Serial.print(Temperature_); // Print the temperature value
  Serial.println(" C"); // Print the unit for temperature


  Serial.print(F("RainValue ")); // Print the label
  Serial.println(rainValue_); // Print the rain sensor value

  mySerial.print("RainValue ");
  mySerial.println(rainValue_);

  
}



//////////////////////////// codes for read and write //////


void connectToWiFi() {
  // Reset ESP8266 module
  sendCommand("AT+RST", 5000, "ready");

  // Set ESP8266 to client mode
  sendCommand("AT+CWMODE=1", 2000, "OK");

  // Connect to Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
  mySerial.println("Connecting to Wi-Fi...");
  sendCommand("AT+CWJAP=\"" + String(SSID) + "\",\"" + String(PASSWORD) + "\"", 10000, "OK");
}


bool isNumber(const String &str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i)) && str.charAt(i) != '.' && str.charAt(i) != '-') {
      return false;
    }
  }
  return true;
}


