/*
   Email client sketch for IDE v1.0.5 and w5100/w5200
   Posted 7 May 2015 by SurferTim
*/
 
#include <SPI.h>
#include <Ethernet.h>
#include <TimerOne.h>  
#include <NTPClient.h>

 
// this must be unique
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
// change network settings to yours
IPAddress ip(   192,168,1,117 );    
IPAddress gateway( 192, 168, 1,254 );
IPAddress subnet( 255, 255, 255, 0 );
 
char server[] = "smtpcorp.com";
int port = 2525;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset

int vcc = 2; //attach pin 2 to vcc
int trig = 3; // attach pin 3 to Trig
int echo = 4; //attach pin 4 to Echo
int gnd = 5; //attach pin 5 to GND

EthernetUDP udp;
NTPClient timeClient(udp , "pt.pool.ntp.org");
 
EthernetClient client;
 
void setup()
{
  pinMode (vcc,OUTPUT);
  pinMode (gnd,OUTPUT);

  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  delay(3000);
  
  
  timeClient.begin();
  delay(3000);
  Serial.println(F("Ready. Press 'e' to send."));
  Serial.println(timeClient.getFormattedTime());
}
 
void loop()
{
  byte inChar;
 
 // inChar = Serial.read();


  if(timeClient.getSeconds() == 20)
  {
    if(returnDistance() > 400)
    {
      if(sendEmail()) Serial.println(F("Email sent"));
      else Serial.println(F("Email failed"));
    }
  }

  timeClient.update();


  Serial.println(timeClient.getSeconds());

  delay(1000);
}

long returnDistance()
{
  digitalWrite(vcc, HIGH);
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;
  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echo,INPUT);
  duration = pulseIn(echo, HIGH);
  
  // convert the time into a distance
  
  cm = microsecondsToCentimeters(duration);
  
  return cm;


}

long microsecondsToCentimeters(long microseconds)
{
// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}

 
byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;
 
  if(client.connect(server,port) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending hello"));
// replace 1.2.3.4 with your Arduino's ip
  client.println("EHLO  192.168.1.117");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending User"));
// Change to your base64 encoded user
  client.println("Y2FzYXBhbGFjb3Vsbw==");
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending Password"));
// change to your base64 encoded password
  client.println("NlphdUJVZ0ZZMFQ0");
 
  if(!eRcv()) return 0;
 
// change to your email address (sender)
  Serial.println(F("Sending From"));
  client.println("MAIL From: casapalacoulo@fe.up.pt");
  if(!eRcv()) return 0;
 
// change to recipient address
  Serial.println(F("Sending To"));
  client.println("RCPT To: jdcitylife@gmail.com");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending DATA"));
  client.println("DATA");
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending email"));
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
 
// change to recipient address
  client.println("To: You jdcitylife@gmail.com");
 
// change to your address
  client.println("From: Me casapalacoulo@fe.up.pt");
 
  client.println("Subject: Arduino email test\r\n");
 
  client.println("This is from my Arduino!");
  

  client.println(timeClient.getFormattedTime());
  
 
  client.println(".");
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending QUIT"));
  client.println("QUIT");
  if(!eRcv()) return 0;
 
  client.stop();
 
  Serial.println(F("disconnected"));
 
  return 1;
}
 
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
 
  respCode = client.peek();
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
 
  return 1;
}
 
 
void efail()
{
  byte thisByte = 0;
  int loopCount = 0;
 
  client.println(F("QUIT"));
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println(F("disconnected"));
}
