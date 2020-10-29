#include "CTBot.h"
#include "env.h"
CTBot cubeBot;

String ssid = SSID;
String pass = SSID_PASSWORD;
String token = BOT_TOKEN;     				
int32_t users[] = USERS;

uint8_t relay = 14;      


void setup() {
	// initialize the Serial
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...");

	// connect the ESP8266 to the desired access point
	cubeBot.wifiConnect(ssid, pass);

	// set the telegram bot token
	cubeBot.setTelegramToken(token);

	// check if all things are ok
	if (cubeBot.testConnection())
		Serial.println("\ntestConnection OK");
	else
		Serial.println("\ntestConnection NOK");

	// set the pin connected to the LED to act as output pin
	pinMode(relay, OUTPUT);
	digitalWrite(relay, HIGH); // turn off the relay (inverted logic!)

}


void loop() {
	TBMessage msg;
	bool isUserFinded = false;

	// if there is an incoming message...
	if (cubeBot.getNewMessage(msg)) {
		for(int i = 0; i < sizeof(users); i++) {
			if (msg.sender.id == users[i]) {
				isUserFinded = true;
				if (msg.text.equalsIgnoreCase("+")) {              	// if the received message is "LIGHT ON"...
					digitalWrite(relay, LOW);                               	// turn on the LED (inverted logic!)
					cubeBot.sendMessage(msg.sender.id, "Light is now ON");  // notify the sender
				} else if (msg.text.equalsIgnoreCase("-")) {        // if the received message is "LIGHT OFF"...
					digitalWrite(relay, HIGH);                              	// turn off the relay (inverted logic!)
					cubeBot.sendMessage(msg.sender.id, "Light is now OFF"); // notify the sender
				} else {                                                    // otherwise...
					// generate the message for the sender
					String reply;
					reply = (String)"Welcome " + msg.sender.username + (String)". Try + or -.";
					cubeBot.sendMessage(msg.sender.id, reply);             // and send it
				}
				break;
			}
		}

		if(!isUserFinded) {
			String reply = (String)"Sorry, you are not in permission list. ChatID: " + msg.sender.id;
			cubeBot.sendMessage(msg.sender.id, reply);
		}
	}
	
	delay(500);
}
