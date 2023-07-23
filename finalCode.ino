// Include Libraries
#include <Arduino.h> // Include the Arduino core library
#include <DHT.h> // Include the DHT library for the humidity and temperature sensor
#include <Stepper.h> // Include the Stepper library

#include <SoftwareSerial.h>

// Pin Definitions
#define DHT_PIN_DATA  2 // Define the pin for DHT data
#define ledPin 6 // Define the LED pin
#define sensorPin A0 // Define the pin for the rain sensor

//Step Motor
int motorSpeed = 10; // Set the motor speed
Stepper myStepper(2048, 8, 9, 10, 11); // Initialize the Stepper object with the specified parameters
bool motorRotated = false; // Flag to track if the motor has rotated

//DHT Sensor
DHT dht(DHT_PIN_DATA); // Create a DHT object


//////////////Thingspeak Read and Write //////////////////////////////////

// Pin Definitions
#define WIFI_PIN_TX 2
#define WIFI_PIN_RX 3

#define LED_PIN_off 4
#define LED_PIN_on 7

SoftwareSerial espSerial(WIFI_PIN_RX, WIFI_PIN_TX);

const char* SSID = "abc"; // Enter your Wi-Fi name
const char* PASSWORD = "1234567890"; // Enter your Wi-Fi password

String HOST = "api.thingspeak.com";
String PORT = "80";
String API = "ZQRO45V38G5PA209"; // Write API KEY

String motorStatus = "/channels/2225714/fields/1/last.txt";

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
  
  myStepper.setSpeed(motorSpeed); // Set the motor speed
  
  while (!Serial); // Wait for the serial port to connect (needed for native USB)
  Serial.println("start"); // Print "start" message
  
  pinMode(ledPin, OUTPUT); // Set the LED pin as an output
  digitalWrite(ledPin, LOW); // Turn off the LED
  
  dht.begin(); // Initialize the DHT sensor

  
  ///////////////// Thingspeak //////////////////////////
  // Initialize ESP8266 serial communication
  espSerial.begin(9600);

  connectToWiFi();

  //Define testing LED pin output
  pinMode(LED_PIN_off, OUTPUT);
  pinMode(LED_PIN_on, OUTPUT);
  
}

void loop(){ 
  // LDR
  float lightIntensity = analogRead(A1); // Read the ldr_value intensity from analog pin A1
  
  delay(300);
  
  //DHT 
  float Humidity = dht.readHumidity(); // Read the humidity value from the DHT sensor
  float Temperature = dht.readTempC(); // Read the temperature value from the DHT sensor in Celsius

  delay(300);

  //Rain Sensor
  float rainValue = readRainSensor(); // Read the rain sensor value

  delay(300);

  printToSerial(float(lightIntensity), float(Humidity), float(Temperature), float(rainValue));

  ////////////////////////////////////// From Here Onwards thingspeak ////////////

  float checkFieldStatus = readThingSpeak(motorStatus);
  Serial.println(checkFieldStatus);

  if (checkFieldStatus == true) {
    digitalWrite(LED_PIN_on, HIGH);
    digitalWrite(LED_PIN_off, LOW);
  } else {
    digitalWrite(LED_PIN_off, HIGH);
    digitalWrite(LED_PIN_on, LOW);
  }
 
  delay(5000);

  writeThingSpeak(Temperature, Humidity, rainValue, lightIntensity);

  delay(5000);

  //// end thingspeak/////
  
  
  Serial.println(motorRotated); // Print the motor rotation status

  if ((lightIntensity < 15 || Humidity > 100 || rainValue > 100 || checkFieldStatus == true) && !motorRotated)
  {
    Serial.println(F("raining")); // Print "raining" message
    digitalWrite(ledPin, HIGH); // Turn on the LED
    myStepper.step(1024); // Rotate the stepper motor 1024 steps
    motorRotated = true; // Set the flag to indicate that the motor has rotated 
  }
  
  else if (!(lightIntensity < 15 || Humidity > 100 || rainValue > 100) && motorRotated)
  {
    Serial.println(F("not raining")); // Print "not raining" message
    digitalWrite(ledPin, LOW); // Turn off the LED
    myStepper.step(-1024); // Rotate the stepper motor 1024 steps in the opposite direction
    motorRotated = false; // Reset the flag to indicate that the motor has not rotated  
  }

  if (motorRotated){delay(120000);}
  else{delay(30000);}  
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

  Serial.print(F("Humidity: ")); // Print the label
  Serial.print(Humidity_); // Print the humidity value
  Serial.print(F(" [%]\t")); // Print the unit for humidity
  
  Serial.print(F("Temperature: ")); // Print the label
  Serial.print(Temperature_); // Print the temperature value
  Serial.println(F(" [C]")); // Print the unit for temperature

  
  Serial.print(F("RainValue ")); // Print the label
  Serial.println(rainValue_); // Print the rain sensor value
}



//////////////////////////// codes for read and write //////


void connectToWiFi(){
  // Reset ESP8266 module
  sendCommand("AT+RST", 5000, "ready");

  // Set ESP8266 to client mode
  sendCommand("AT+CWMODE=1", 2000, "OK");

  // Connect to Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
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


float readThingSpeak(String FIELD_PATH_) {
    // Make an HTTP GET request to Thingspeak for "FIELD_PATH_"
  String getRequest = "GET " + FIELD_PATH_;
  sendCommand("AT+CIPMUX=1", 5000, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 10000, "OK");
  sendCommand("AT+CIPSEND=0," + String(getRequest.length() + 2), 5000, ">");
  espSerial.println(getRequest);
  delay(1000); // Give some time for the response

  // Read and parse the value from the response
  String response;
  while (espSerial.available())
  {
    char c = espSerial.read();
    response += c;
  }
  Serial.println(response);

  // Find the start and end indices of the numeric value
  int valueStartIndex = response.indexOf(":") + 1;
  int valueEndIndex = response.indexOf("0,CLOSED");
  String value_ = response.substring(valueStartIndex, valueEndIndex);

  // Remove any additional characters (e.g., newlines) from the value
  value_.trim();
  //String value;
  //if (isNumber(value_)) {
    //  float value = value_.toFloat(); // Convert the string to a float
    //} else {
     // String value = value_;
      //Serial.println("Error: Invalid input. The string is not a valid number.");
      
    //}
  // Serial.print("Field 2 Value: ");
  // Serial.println(value);
  float value = value_.toFloat();
  sendCommand("AT+CIPCLOSE=0", 5000, "OK");
  return value;
}


float writeThingSpeak(float Temperature_,float Humidity_,float rainValue_,float lightIntensity_) {

  String getData = "GET /update?api_key=" + API + "&field2=" + Temperature_ +
                   "&field3=" + Humidity_ + "&field4=" + rainValue_ + "&field5=" + lightIntensity_;
  
  sendCommand("AT+CIPMUX=1", 5000, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 10000, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 5000, ">");
  
  espSerial.println(getData);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0", 5000, "OK");
}


void sendCommand(String command, unsigned int timeout, const char* expected)
{
  espSerial.println(command);
  unsigned long startTime = millis();
  String response;
  
  while (millis() - startTime < timeout)
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    } 
    if (response.indexOf(expected) != -1)
    {
      found = true;
      break;
    }
  }
  if (found)
  {
    Serial.println("OK");
    countTrueCommand++;
  }
  else
  {
    Serial.println("Fail");
    countTrueCommand = 0;
  }
  found = false;
}
