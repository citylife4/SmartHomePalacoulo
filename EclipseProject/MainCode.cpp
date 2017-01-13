/*
 Email client sketch for IDE v1.0.5 and w5100/w5200
 Posted 7 May 2015 by SurferTim
 */

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <TimerOne.h>
#include <NTPClient.h>
// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   20


// this must be unique
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// change network settings to yours
IPAddress ip(192, 168, 1, 117);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

//Webserver
EthernetServer webServer(80);
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer



//SMTP server
char server[] = "smtpcorp.com";
int port = 2525;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset

//For echo sounder
int vcc = 2; //attach pin 2 to vcc
int trig = 3; // attach pin 3 to Trig
int echo = 4; //attach pin 4 to Echo
int gnd = 5; //attach pin 5 to GND

//For Time
EthernetUDP udp;
NTPClient timeClient(udp, "pt.pool.ntp.org");

EthernetClient client;

void checkDoor();
long returnDistance();
long microsecondsToCentimeters(long microseconds);
byte sendEmail(String tosend);
byte eRcv();
void efail();
void startServer(EthernetClient webClient);
void StrClear(char *str, char length);
char StrContains(char *str, char *sfind);



void setup() {
	//Pin Mode
	pinMode(vcc, OUTPUT);
	pinMode(gnd, OUTPUT);

	//Serial for debug
	Serial.begin(115200);
	Serial.println("Starting");

	//Another Debug
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	//Start Ethernet connection
	Ethernet.begin(mac, ip, gateway, gateway, subnet);
	delay(3000);

	//Start NTP
	timeClient.begin();
	delay(4000);

	Serial.print("Initialized at: ");
	Serial.println(timeClient.getFormattedTime());

//Timer 1 initialization
	//Timer 1 is very fast...

	//Timer1.initialize(60 * 1000000); //two minute timer
	//Timer1.attachInterrupt(checkDoor);
/*
	if (sendEmail("jdcitylife@gmail.com"))
			Serial.println(F("Email sent"));
		else
			Serial.println(F("Email failed"));
			*/
}

void loop() {

	timeClient.update();

	//Serial.println(timeClient.getSeconds());

	EthernetClient webClient = webServer.available();
	startServer(webClient);


}

int main(void) {
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

void startServer(EthernetClient webClient) {
	boolean currentLineIsBlank = true;
	if (webClient) {  // got client?
	        boolean currentLineIsBlank = true;
	        while (webClient.connected()) {
	            if (webClient.available()) {   // client data available to read
	                char c = webClient.read(); // read 1 byte (character) from client
	                // buffer first part of HTTP request in HTTP_req array (string)
	                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
	                if (req_index < (REQ_BUF_SZ - 1)) {
	                    HTTP_req[req_index] = c;          // save HTTP request character
	                    req_index++;
	                }
	                Serial.print(c);    // print HTTP request character to serial monitor
	                // last line of client request is blank and ends with \n
	                // respond to client only after last line received
	                if (c == '\n' && currentLineIsBlank) {
	                    // send a standard http response header
	                	webClient.println("HTTP/1.1 200 OK");
	                	webClient.println("Content-Type: text/html");
	                	webClient.println("Connnection: close");
	                	webClient.println("Refresh: 5");  // refresh the page automatically every 5 sec
	                	webClient.println();
	                    // open requested web page file
	                    if (StrContains(HTTP_req, "GET / ")
	                                 || StrContains(HTTP_req, "GET /index.htm")) {
	                        //webFile = SD.open("index.htm");        // open web page file
	                        Serial.print("hello");
	                    }
	                    else if (StrContains(HTTP_req, "GET /page2.htm")) {
	                      Serial.print("hello`1");

	                      webClient.println("<!DOCTYPE HTML>");
	                      webClient.println("<html>");
	                        // output the value of each analog input pin
	                        for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
	                          int sensorReading = analogRead(analogChannel);
	                          webClient.print("analog input ");
	                          webClient.print(analogChannel);
	                          webClient.print(" is ");
	                          webClient.print(sensorReading);
	                          webClient.println("<br />");
	                        }
	                        webClient.println("</html>");
	                        break;

	                    }
	                    // send web page to client

	                    // reset buffer index and all buffer elements to 0
	                    req_index = 0;
	                    StrClear(HTTP_req, REQ_BUF_SZ);
	                    break;
	                }
	                // every line of text received from the client ends with \r\n
	                if (c == '\n') {
	                    // last character on line of received text
	                    // starting new line with next character read
	                    currentLineIsBlank = true;
	                }
	                else if (c != '\r') {
	                    // a text character was received from client
	                    currentLineIsBlank = false;
	                }
	            } // end if (client.available())
	        } // end while (client.connected())
	        delay(1);      // give the web browser time to receive the data
	        webClient.stop(); // close the connection
	    } // end if (client)

}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
	for (int i = 0; i < length; i++) {
		str[i] = 0;
	}
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind) {
	char found = 0;
	char index = 0;
	char len;

	len = strlen(str);

	if (strlen(sfind) > len) {
	return 0;
	}
	while (index < len) {
	if (str[index] == sfind[found]) {
		found++;
		if (strlen(sfind) == found) {
			return 1;
		}
	} else {
		found = 0;
	}
	index++;
	}

	return 0;
}


void checkDoor() {

	//if Debug


	//Else

	Serial.print("Checking @ ");
	Serial.println(timeClient.getSeconds());

	if (timeClient.getSeconds() == 20) {
		long distance = returnDistance();

		Serial.print("Distance is: ");
		Serial.println(distance);

		if (distance > 400) {
			Serial.println("SENDING EMAIL");
			if (sendEmail("jdcitylife@gmail.com"))
				Serial.println(F("Email sent"));
			else
				Serial.println(F("Email failed"));
		}
	} else {
		Serial.println("Not correct Time");
	}

}

long returnDistance() {
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
	pinMode(echo, INPUT);
	duration = pulseIn(echo, HIGH);

	// convert the time into a distance

	cm = microsecondsToCentimeters(duration);

	return cm;

}

long microsecondsToCentimeters(long microseconds) {
// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
	return microseconds / 29 / 2;
}

byte sendEmail(String tosend) {
	byte thisByte = 0;
	byte respCode;

	if (client.connect(server, port) == 1) {
		Serial.println(F("Connected"));
	} else {
		Serial.println(F("Connection failed"));
		return 0;
	}

	if (!eRcv())
		return 0;

	Serial.println(F("Sending hello"));
// replace 1.2.3.4 with your Arduino's ip
	client.println("EHLO  192.168.1.117");
	if (!eRcv())
		return 0;

	Serial.println(F("Sending auth login"));
	client.println("auth login");
	if (!eRcv())
		return 0;

	Serial.println(F("Sending User"));
// Change to your base64 encoded user
	client.println("Y2FzYXBhbGFjb3Vsbw==");

	if (!eRcv())
		return 0;

	Serial.println(F("Sending Password"));
// change to your base64 encoded password
	client.println("NlphdUJVZ0ZZMFQ0");

	if (!eRcv())
		return 0;

// change to your email address (sender)
	Serial.println(F("Sending From"));
	client.println("MAIL From: portao@casapalacoulo.us.to");
	if (!eRcv())
		return 0;

// change to recipient address
	Serial.println(F("Sending To"));
	String Rcp = "RCPT To: " + tosend;
	client.println(Rcp);
	if (!eRcv())
		return 0;

	Serial.println(F("Sending DATA"));
	client.println("DATA");
	if (!eRcv())
		return 0;

	Serial.println(F("Sending email"));
	timeClient.update();
	Serial.println(timeClient.getFormattedTime());

// change to recipient address
	String toYou = "To: You " + tosend;
	client.println(toYou);

// change to your address

	client.println("From: Me portao@casapalacoulo.us.to");

	client.println("Subject: Arduino email test\r\n");

	client.println("This is from my Arduino!");

	client.println(timeClient.getFormattedTime());

	client.println(".");

	if (!eRcv())
		return 0;

	Serial.println(F("Sending QUIT"));
	client.println("QUIT");
	if (!eRcv())
		return 0;

	client.stop();

	Serial.println(F("disconnected"));

	return 1;
}

byte eRcv() {
	byte respCode;
	byte thisByte;
	int loopCount = 0;

	while (!client.available()) {
		delay(1);
		loopCount++;

		// if nothing received for 10 seconds, timeout
		if (loopCount > 10000) {
			client.stop();
			Serial.println(F("\r\nTimeout"));
			return 0;
		}
	}

	respCode = client.peek();

	while (client.available()) {
		thisByte = client.read();
		Serial.write(thisByte);
	}

	if (respCode >= '4') {
		efail();
		return 0;
	}

	return 1;
}

void efail() {
	byte thisByte = 0;
	int loopCount = 0;

	client.println(F("QUIT"));

	while (!client.available()) {
		delay(1);
		loopCount++;

		// if nothing received for 10 seconds, timeout
		if (loopCount > 10000) {
			client.stop();
			Serial.println(F("\r\nTimeout"));
			return;
		}
	}

	while (client.available()) {
		thisByte = client.read();
		Serial.write(thisByte);
	}

	client.stop();

	Serial.println(F("disconnected"));
}
