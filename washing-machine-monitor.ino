#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <ESP8266WiFi.h>
#include "Configuration.h"


WiFiClient client;
Adafruit_SSD1306 display = Adafruit_SSD1306();



// Variables we need to define
char server[] = "api.thingspeak.com"; //thingspeak server
bool washing = false; // Is the washing machine on?
bool alert = false; // Is alert mode one

// The min / max times for checking for vibration events
unsigned long vibrateOffMinTime = 1000 * 3;
unsigned long vibrateOffMaxTime = 1000 * 50;
unsigned long vibrateOnMinTime = 1000 * 3;
unsigned long vibrateOnMaxTime = 1000 * 30;
unsigned long alertInterval = 1000 * 60 * 10; // How long to wait between alerts

// Hold whether or not we should start checking for vibrations on start / stop
bool checkingIfStart = false;
bool checkingIfStop = false;

// Our main time interval holder
unsigned long startMillis = 0; // Last time we got a "vibrating" reading
unsigned long lastAlertTime = 0; // Last time we send an alert

// Count on / off vibrations
int vibrationOne = 0;
int vibrationTwo = 0;


void setup() {
  // Init the WIFI connection
  initWifi();
  
  // Startup the OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  displayText("Ready...");
  delay(1000);

  // Init the vibration pin
  pinMode(VIBRATION_PIN, INPUT); 

  // Init the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
}

void loop() {
  // If we're not washing, check to see if we should be
  if (!washing) {
    checkStartingWashing();
  }
  else {
    // Otherwise, check to see if we're done washing
    checkDoneWashing();
  }

  // If we're not washing and it's in alert mode, run the alert process
  if (alert && !washing) {
    runAlert();
  }

  // Check the button press to clear the alert mode
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState) {
    alert = false;
    display.clearDisplay();
    display.display();
    displayText("Ready...");
  }

  // Debugging lines
  Serial.print("Washing: ");
  Serial.println(washing);

  Serial.print("Alert: ");
  Serial.println(alert);

  Serial.print("VibrationOne: ");
  Serial.println(vibrationOne);
  Serial.print("VibrationTwo: ");
  Serial.println(vibrationTwo);
  
  delay(50);

}




/**
 * Initialize the wifi connection
 */
void initWifi() {
  Serial.begin(115200);
  delay(100);
 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  displayText("Connecting to ");
  Serial.print("Connecting to ");
  Serial.println(WIFI_NETWORK);

  // The actual connection
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


/**
 * Check to see if we need to "start" washing
 */
void checkStartingWashing() {

  if (!checkingIfStart) {
    checkingIfStart = true;
    vibrationOne = 0;
    vibrationTwo = 0;
    startMillis = millis();
  }

  // Check the current state, increment the number of times it's hit it
  if (isVibrating()) {
    vibrationOne++;
  }
  else {
    vibrationTwo++;
  }

  if ( hasVibrationStarted() && millis() - startMillis > vibrateOnMinTime) {
    washing = true;
    checkingIfStart = false;
    display.clearDisplay();
    display.display();
    displayText("Washing...");
  }

  if (millis() - startMillis > vibrateOnMaxTime) {
    checkingIfStart = false;
  }
  
}


/**
 * Check to see if we're done washing
 */
void checkDoneWashing() {

  if (!checkingIfStop) {
    checkingIfStop = true;
    vibrationOne = 0;
    vibrationTwo = 0;
    startMillis = millis();
  }

  // Check the current state, increment the number of times it's hit it
  if (isVibrating()) {
    vibrationOne++;
  }
  else {
    vibrationTwo++;
  }

  if ( hasVibrationStopped() && millis() - startMillis > vibrateOffMinTime) {
    washing = false;
    alert = true;
    checkingIfStop = false;
    display.clearDisplay();
    display.display();
    displayText("Sending alert...");
  }

  if (millis() - startMillis > vibrateOffMaxTime) {
    checkingIfStop = false;
  }

}



/**
 * Check to see if vibrations have started
 */
bool hasVibrationStarted() {
  if (vibrationOne <= vibrationTwo * 1.5 && vibrationOne >= vibrationTwo * 0.7 && vibrationOne > 30 && vibrationTwo > 30) {
    return true;
  }

  return false;
}



/**
 * Check to see if vibrations have stopped
 */
bool hasVibrationStopped() {
  if (vibrationOne >= vibrationTwo * 20 || vibrationTwo >= vibrationOne * 20) {
    return true;
  }

  return false;
}


/**
 * Run the alert process
 */
void runAlert() {
  // Make sure we only send alerts periodically
  if (lastAlertTime == 0 || millis() - lastAlertTime > alertInterval) {
    
    // Connect to twilio and send the alert
    sendSMS(SEND_NUMBER, URLEncode("Washing mashing done!"));

    // Set the last time the alert was sent and take it out of alert mode
    lastAlertTime = millis();

    // Update the display
    display.clearDisplay();
    display.display();
    displayText("Press button to stop...");
  }
}


/**
 * Send an alert to twilio
 */
void sendSMS(String number,String message)
{  
  // Make a TCP connection to remote host
  if (client.connect(server, 80))
  {
    //api.thingspeak.com/apps/thinghttp/send_request?api_key={api key}&number={send to number}&message={text body}
    client.print("GET /apps/thinghttp/send_request?api_key=");
    client.print(API_KEY);
    client.print("&number=");
    client.print(number);
    client.print("&message=");
    client.print(message);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    Serial.println(F("Connection failed"));
  } 

  // Check for a response from the server, and route it
  // out the serial port.
  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      Serial.print(c);
    }      
  }
  Serial.println();
  client.stop();
}


/**
 * Read from the vibration sensor
 */
bool isVibrating() {
  return (digitalRead(VIBRATION_PIN)) ? true : false;
}


/**
 * Display text on the OLED
 */
void displayText(String text) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(text);
  display.setCursor(0,0);
  display.display();
}


/**
 * Encode any message to send via SMS
 */
String URLEncode(const char* msg)
{
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg!='\0'){
    if( ('a' <= *msg && *msg <= 'z')
      || ('A' <= *msg && *msg <= 'Z')
      || ('0' <= *msg && *msg <= '9') ) {
      encodedMsg += *msg;
    } 
    else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}

