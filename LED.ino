#include <ArduinoBLE.h>
#include <Servo.h>
#include "Ultrasonic.h"

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 

#define MOTORA_1 2
#define MOTORA_2 3
#define MOTORB_1 4
#define MOTORB_2 5
#define LEDBACKR 6
#define LEDBACKL 7
#define LEDFRONTR 1
#define LEDFRONTL 10
#define SOUNDCAR 8
#define SERVOMOTOR 9
#define SENSORSOUND A0


char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char thisChar = 0;
int keyIndex = 0;

int status = WL_IDLE_STATUS;

WiFiServer server(23);

boolean alreadyConnected = false;

Ultrasonic ultrasonic(20, 19);
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

char cmd = 0;
Servo obstacle;
boolean goesFwd = false;
long distance;
long checkRight;
long checkLeft;
int pos;

void setup()
{
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(MOTORA_1, OUTPUT);
  pinMode(MOTORA_2, OUTPUT);
  pinMode(MOTORB_1, OUTPUT);
  pinMode(MOTORB_2, OUTPUT);
  pinMode(LEDBACKR, OUTPUT);
  pinMode(LEDBACKL, OUTPUT);
  pinMode(LEDFRONTR, OUTPUT);
  pinMode(LEDFRONTL, OUTPUT);
  pinMode(SOUNDCAR, OUTPUT);
  obstacle.attach(9);
  obstacle.write(90);

  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");

    while (1);
  }

  BLE.setLocalName("L");
  BLE.setAdvertisedService(ledService);

  ledService.addCharacteristic(switchCharacteristic);

  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  Serial.println("Start serviciu!");
  BLE.advertise();

    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // start the server:
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
  
}

void goForward()
{
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);
  digitalWrite(MOTORB_1, HIGH);
  digitalWrite(MOTORB_2, LOW);
}

void stopNow()
{

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void goBackWard()
{
  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, HIGH);
}

void goLeft()
{

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);
}

void goRight()
{

  digitalWrite(MOTORB_1, HIGH);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_2, HIGH);
  digitalWrite(MOTORA_1, LOW);
}

void frontLightsOn()
{

  digitalWrite(LEDFRONTR, HIGH);
  digitalWrite(LEDFRONTL, HIGH);
}

void frontLightsOff()
{

  digitalWrite(LEDFRONTR, LOW);
  digitalWrite(LEDFRONTL, LOW);
}

void backLightsOn()
{

  digitalWrite(LEDBACKR, HIGH);
  digitalWrite(LEDBACKL, HIGH);
}

void backLightsOff()
{

  digitalWrite(LEDBACKR, LOW);
  digitalWrite(LEDBACKL, LOW);
}

void hornOn()
{
  tone(SOUNDCAR, 340);
}

void hornOff()
{
  noTone(SOUNDCAR);
}

void moveStop()
{

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void moveForward()
{

  if (!goesFwd)
  {

    goesFwd = true;

    digitalWrite(MOTORA_1, HIGH);
    digitalWrite(MOTORA_2, LOW);
    digitalWrite(MOTORB_1, HIGH);
    digitalWrite(MOTORB_2, LOW);
  }
}

void moveBackward()
{

  goesFwd = false;

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, HIGH);
}

void turnRight()
{

  digitalWrite(MOTORB_1, HIGH);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_2, HIGH);
  digitalWrite(MOTORA_1, LOW);

  delay(250);

  digitalWrite(MOTORB_1, HIGH);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_2, HIGH);
  digitalWrite(MOTORA_1, LOW);
}

void turnLeft()
{

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);

  delay(250);

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);
}

void commandForward()
{
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);
  digitalWrite(MOTORB_1, HIGH);
  digitalWrite(MOTORB_2, LOW);

  delay(1500);

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void commandBackward()
{
  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, HIGH);

  delay(1500);

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void commandLeft()
{
  obstacle.write(180);
  delay(500);
  obstacle.write(90);
  delay(500);
  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, HIGH);
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);

  delay(1500);

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void commandRight()
{
  obstacle.write(0);
  delay(500);
  obstacle.write(90);
  delay(500);
  digitalWrite(MOTORB_1, HIGH);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_2, HIGH);
  digitalWrite(MOTORA_1, LOW);

  delay(1500);

  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void commandStop()
{
  digitalWrite(MOTORB_1, LOW);
  digitalWrite(MOTORB_2, LOW);
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void loop()
{

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  
  // if a central is connected to peripheral:
  if (central)
  {
    Serial.print("Connected to central: ");

    Serial.println(central.address());

    while (central.connected())
    {
      char a = switchCharacteristic.value();
      distance = ultrasonic.Ranging(CM);
      Serial.print("Signal:");
      Serial.print(a);
      Serial.println();
        if (a == 'O')
        {
          if (distance > 20)
          {
            goForward();
          }
          else if (distance <= 20)
          {
            stopNow();
            delay(500);
            goBackWard();
            delay(300);
            stopNow();
            checkRight = ultrasonic.Ranging(CM);
            
            obstacle.write(90); 
            delay(550);

            if (checkLeft < checkRight)
            {
              obstacle.write(160);
              delay(550);
              obstacle.write(90);
              goLeft();
              delay(400);
            }
            else if (checkLeft > checkRight)
            {
              obstacle.write(30);
              delay(550);
              obstacle.write(90);
              goRight();
              delay(400);
            }
            else if (checkLeft <= 20 && checkRight <= 20)
            {
              goBackWard();
              delay(200);
              goLeft();
              delay(400);
            }
          }
        }

      if (a == 'E') {
          digitalWrite(LEDFRONTR, HIGH);
          digitalWrite(LEDFRONTL, HIGH);
          delay(1000);
          digitalWrite(LEDFRONTR, LOW);
          digitalWrite(LEDFRONTL, LOW);
          delay(1000);
          digitalWrite(LEDBACKR, HIGH);
          digitalWrite(LEDBACKL, HIGH);
          delay(1000);
          digitalWrite(LEDBACKR, LOW);
          digitalWrite(LEDBACKL, LOW);
          delay(1000);
      }
      
      if (switchCharacteristic.written())
      {
        if (a == 'F')
        {
          digitalWrite(LEDFRONTR, HIGH);
          digitalWrite(LEDFRONTL, HIGH);
        }
        else if (a == 'B')
        {
          digitalWrite(LEDBACKR, HIGH);
          digitalWrite(LEDBACKL, HIGH);
        }
        else if (a == 'H')
        {
          hornOn();
        }
        else if (a == 'X')
        {
          digitalWrite(LEDBACKR, LOW);
          digitalWrite(LEDFRONTR, LOW);
          digitalWrite(LEDBACKL, LOW);
          digitalWrite(LEDFRONTL, LOW);
          hornOff();
          stopNow();
          obstacle.write(90);
        }
        else if(a == 'h')
        {
          hornOff();  
        }
        else if(a == 'f')
        {
          digitalWrite(LEDFRONTR, LOW);
          digitalWrite(LEDFRONTL, LOW);
        }
        else if(a == 'b') {
          digitalWrite(LEDBACKR, LOW);
          digitalWrite(LEDBACKL, LOW);
        }
        else if (a == 'W')
        {
          goForward();
        }
        else if (a == 'S')
        {
          goBackWard();
        }
        else if (a == 'A')
        {
          goLeft();
        }
        else if (a == 'D')
        {
          goRight();
        }
        else if (a == 'w')
        {
          stopNow();
        }
        else if (a == 's')
        {
          stopNow();
        }
        else if(a == 'l')
        {
          stopNow();
        }
        else if(a == 'r')
        {
          stopNow();
        }
        else if(a == 'e') {
          digitalWrite(LEDBACKR, LOW);
          digitalWrite(LEDFRONTR, LOW);
          digitalWrite(LEDBACKL, LOW);
          digitalWrite(LEDFRONTL, LOW);
        }
        else if(a == 'L') {
          commandForward();
        }
        else if(a == 'M') {
          commandBackward();
        }
        else if(a == 'N') {
          commandLeft();
        }
        else if(a == 'K') {
          commandRight();
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LEDBACKR, LOW);
    digitalWrite(LEDFRONTR, LOW);
    digitalWrite(LEDBACKL, LOW);
    digitalWrite(LEDFRONTL, LOW);
    hornOff();
    stopNow();
    obstacle.write(90);
  }

    // wait for a new client:
  WiFiClient client = server.available();

//  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clear out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.print("Hello, client! We received your request: ");
      char c = client.read();
      client.print(c);
      alreadyConnected = true;
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      thisChar = client.read();
    
      
      // echo the bytes back to the client:
      server.write(thisChar);
      // echo the bytes to the server as well:
      Serial.write(thisChar);   
    }

     if(thisChar == 'w') {
         digitalWrite(LEDFRONTR, HIGH);
         digitalWrite(LEDFRONTL, HIGH);
         goForward();
      } else if(thisChar == 's') {
         digitalWrite(LEDBACKR, HIGH);
         digitalWrite(LEDBACKL, HIGH);
         goBackWard();
      } else if(thisChar == 'a') {
         digitalWrite(LEDFRONTR, HIGH);
         digitalWrite(LEDBACKL, HIGH);
         goLeft();
      } else if(thisChar == 'd') {
         digitalWrite(LEDFRONTL, HIGH);
         digitalWrite(LEDBACKR, HIGH);
         goRight();
      } else if(thisChar == 'x') {
         digitalWrite(LEDBACKR, HIGH);
         digitalWrite(LEDBACKL, HIGH);
         stopNow();
      } else if(thisChar == ';') {
         digitalWrite(LEDFRONTR, LOW);
         digitalWrite(LEDFRONTL, LOW);
         digitalWrite(LEDBACKR, LOW);
         digitalWrite(LEDBACKL, LOW);
         stopNow();
      }
  }
}
