#include "CTBot.h"
#include ".env.h"
CTBot cubeBot;

uint relay = 14;      


void setup() {
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...\n");

	cubeBot.wifiConnect(ssid, pass);
	cubeBot.setTelegramToken(token);

	bool isSuccessConnection = false;

	if (cubeBot.testConnection()) {
		Serial.println("testConnection OK");
		isSuccessConnection = true;
	} else {
		Serial.println("testConnection NOK");
	}

	pinMode(relay, OUTPUT);
	digitalWrite(relay, HIGH); 			// turn off the relay (inverted logic!)

	String successMessage = "👋 Aqua Cube запущен!";
	if (isSuccessConnection) {
		successMessage = successMessage + "\n✅ Успешно подключено к серверу";
	} else {
		successMessage = successMessage + "\n❌ Ошибка подключения к серверу";
	}
	
	for (size_t i = 0; i < adminsCount; i++) {
		cubeBot.sendMessage(users[i], successMessage);
	}
}


void loop() {
	TBMessage msg;
	bool isUserFinded = false;

	// if there is an incoming message...
	if (cubeBot.getNewMessage(msg)) {
		for(size_t i = 0; i < sizeof(users)/sizeof(int); i++) {
			if (msg.sender.id == users[i]) {
				isUserFinded = true;
				if (msg.text.equalsIgnoreCase("+")) {              				// if the received message is "LIGHT ON"...
					digitalWrite(relay, LOW);                               	// turn on the LED (inverted logic!)
					cubeBot.sendMessage(msg.sender.id, "💡 Свет включен");  	// notify the sender
				} else if (msg.text.equalsIgnoreCase("-")) {        			// if the received message is "LIGHT OFF"...
					digitalWrite(relay, HIGH);                              	// turn off the relay (inverted logic!)
					cubeBot.sendMessage(msg.sender.id, "🌃 Свет выключен"); 	// notify the sender
				} else {                                                    	// otherwise...
					String reply;
					reply = (String)"Привет, " + msg.sender.username + (String)". Попробуй отправить + или -.";
					cubeBot.sendMessage(msg.sender.id, reply);
				}
				break;
			}
		}

		if(!isUserFinded) {
			String reply = (String)"Тебя нет в списке разрешенных контактов. ChatID: " + msg.sender.id;
			cubeBot.sendMessage(msg.sender.id, reply);
		}
	}
	
	delay(500);
}
