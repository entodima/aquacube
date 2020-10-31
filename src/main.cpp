#include "CTBot.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
//#include <EEPROM.h>
#include ".env.h"

CTBot cubeBot;
CTBotInlineKeyboard cubeKbdOn;
CTBotInlineKeyboard cubeKbdOff;
CTBotInlineKeyboard cubeKbdOnOff;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#define LIGHT_ON_CALLBACK  "lightON" 
#define LIGHT_OFF_CALLBACK  "lightOFF" 

void setup() {
	//Serial.begin(115200);

	cubeBot.wifiConnect(ssid, pass);
	cubeBot.setTelegramToken(token);
	
	cubeKbdOn.addButton("💡 Включить", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
	cubeKbdOnOff.addButton("💡 Включить", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
	cubeKbdOff.addButton("🌃 Выключить", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);
	cubeKbdOnOff.addButton("🌃 Выключить", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);

	pinMode(relay, OUTPUT);
	digitalWrite(relay, LOW);

	timeClient.setTimeOffset(10800);
	timeClient.update();
 	//Serial.println(timeClient.getFormattedTime());


	for (size_t i = 0; i < adminsCount; i++) {
		cubeBot.sendMessage(users[i], "👋 Aqua Cube запущен!\n🌃 Свет выключен", cubeKbdOn);
	}
}


void loop() {
	TBMessage msg;
	bool isUserFinded = false;

	// if there is an incoming message...
	if (cubeBot.getNewMessage(msg)) {
		for(size_t i = 0; i < sizeof(users)/sizeof(int); i++) {
			if (msg.sender.id == users[i]) {
				if (msg.messageType == CTBotMessageText) {
					isUserFinded = true;
					if (msg.text.equalsIgnoreCase("+")) {              				// if the received message is "LIGHT ON"...
						digitalWrite(relay, HIGH);                               	// turn on the LED 
						cubeBot.sendMessage(msg.sender.id, "💡 Свет включен", cubeKbdOff);  	// notify the sender
					} else if (msg.text.equalsIgnoreCase("-")) {        			// if the received message is "LIGHT OFF"...
						digitalWrite(relay, LOW);                              		// turn off the relay
						cubeBot.sendMessage(msg.sender.id, "🌃 Свет выключен", cubeKbdOn); 	// notify the sender
					} else {
						String reply;
						reply = (String)"Привет, " + msg.sender.username + (String)". Попробуй отправить + или -, или воспользуйся кнопками ниже";
						cubeBot.sendMessage(msg.sender.id, reply, cubeKbdOnOff);
					}
					break;
				} else if (msg.messageType == CTBotMessageQuery) {
					isUserFinded = true;
					if (msg.callbackQueryData.equals(LIGHT_ON_CALLBACK)) {
						digitalWrite(relay, HIGH);  
						cubeBot.endQuery(msg.callbackQueryID);
						cubeBot.sendMessage(msg.sender.id, "💡 Свет включен", cubeKbdOff);
					} else if (msg.callbackQueryData.equals(LIGHT_OFF_CALLBACK)) {
						digitalWrite(relay, LOW);  
						cubeBot.endQuery(msg.callbackQueryID);
						cubeBot.sendMessage(msg.sender.id, "🌃 Свет выключен", cubeKbdOn);
					}

					break;
				}
				
			}
		}

		if(!isUserFinded) {
			String reply = (String)"Тебя нет в списке разрешенных контактов. ChatID: " + msg.sender.id;
			cubeBot.sendMessage(msg.sender.id, reply);
		}
		
	}
	
	delay(50);
}
