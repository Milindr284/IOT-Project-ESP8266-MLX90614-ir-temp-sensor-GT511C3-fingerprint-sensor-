#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#include <Wire.h> 
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
FPS_GT511C3 fps(D5, D6);


const char* ssid     = "POCO PHONE";   //replace with your own SSID
const char* password = "milindr284";    //replace with your own password
const char* host = "api.pushingbox.com";

String member = "";
String t = "";
int flag = 0;
void setup()
{
  mlx.begin();
  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  lcd.print("WELCOME");
  delay(2000);

  lcd.clear();

  
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  lcd.setCursor(0, 0);
  Serial.print("Connecting to WiFi...       ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected          ");
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.setCursor(4, 1);
  lcd.print(WiFi.localIP());
  delay(1500);
  mlx.begin();
}
void loop()
{
  fps.Open();
  fps.SetLED(true);
  lcd.clear();
  lcd.print("Put your finger"); //the command to print to the LCD
  lcd.setCursor(0, 1); //sets the cursor to the 0th column in the 1st row
  lcd.print("  on the scanner");
  delay(1000);
   //scan and identify the finger when one is put on it
  while(fps.IsPressFinger() == false) delay(100);
  
  lcd.clear(); //clears the screen and sets the cursor to 0,0
  fps.CaptureFinger(false); //captures the finger for identification
  int id = fps.Identify1_N(); //identifies print and stores the id
  if (id <200) //<- change id value depending model you are using
    {
      //if the fingerprint matches, provide the matching template ID
      lcd.clear();
      lcd.print("Access granted");
      lcd.print(               id);
      lcd.setCursor(0,1);
      lcd.print("U are in         ");
      float Temp = mlx.readObjectTempC();
    Serial.print("connecting to ");
    Serial.println(host);
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) 
    {
      Serial.println("connection failed");
      return;
    }

    // We now create a URL for the request
    String url = "/pushingbox?";
    url += "devid=";
    url += "v90B63DB78XXXXXX";
    url += "&Name=" + String(id);
    url += "&Temp=" + String(Temp);

    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      Serial.print("Data Sent!");
    }

    Serial.println();
    Serial.println("closing connection");
    }
    else{
    //if unable to recognize
      lcd.clear();
      lcd.print("Finger not found");
      lcd.setCursor(0,1);
      lcd.print("U are Out         ");
      delay(1000);
    }
}
