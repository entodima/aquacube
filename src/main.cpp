#include "CTBot.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include ".env.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
CTBot cubeBot;

CTBotReplyKeyboard cubeOnReplyKeyboard;
CTBotReplyKeyboard cubeOffReplyKeyboard;
CTBotReplyKeyboard cubeUpdateReplyKeyboard;

bool lampIsEnabled;

#define LIGHT_ON_CALLBACK  "lightON" 
#define LIGHT_OFF_CALLBACK  "lightOFF" 

void setup() {
	Serial.begin(115200);

	cubeBot.wifiConnect(ssid, pass);
	cubeBot.setTelegramToken(token);

	cubeOnReplyKeyboard.addButton("💡 Включить свет");
	cubeOnReplyKeyboard.addRow();
	cubeOnReplyKeyboard.addButton("🔄 Обновить статус");

	cubeOffReplyKeyboard.addButton("🌃 Выключить свет");
	cubeOffReplyKeyboard.addRow();
	cubeOffReplyKeyboard.addButton("🔄 Обновить статус");

	cubeUpdateReplyKeyboard.addButton("🔄 Обновить статус");

	pinMode(relay, OUTPUT);
	digitalWrite(relay, LOW);
	lampIsEnabled = false;

	timeClient.setTimeOffset(10800);
	timeClient.update();
 	//Serial.println(timeClient.getFormattedTime());


	for (size_t i = 0; i < adminsCount; i++) {
		cubeBot.sendMessage(users[i], "👋 Aqua Cube запущен!\n🌃 Свет выключен", cubeOnReplyKeyboard);
	}
}


void loop() {
	TBMessage msg;
	bool isUserFinded = false;

	if (cubeBot.getNewMessage(msg)) { // получение сообщения
		for(size_t i = 0; i < sizeof(users)/sizeof(int); i++) {	// проверка, есть ли пользоватлель в белом списке
			if (msg.sender.id == users[i]) {					
				if (msg.messageType == CTBotMessageText) {		// это текст?
					if (msg.text == "💡 Включить свет") {		// команда включения света
						digitalWrite(relay, HIGH);        		
						lampIsEnabled = true;
						cubeBot.sendMessage(msg.sender.id, "💡 Свет включен", cubeOffReplyKeyboard);  	
					} else if (msg.text == "🌃 Выключить свет") {	// команда выключения света
						digitalWrite(relay, LOW);        
						lampIsEnabled = false;
						cubeBot.sendMessage(msg.sender.id, "🌃 Свет выключен", cubeOnReplyKeyboard);  	
					} else if (msg.text = "🔄 Обновить статус") {	// команда получения статуса лампы
						if (lampIsEnabled) {				
							cubeBot.sendMessage(msg.sender.id, "💡 Cейчас cвет включен", cubeOffReplyKeyboard);
						} else {
							cubeBot.sendMessage(msg.sender.id, "🌃 Cейчас cвет выключен", cubeOnReplyKeyboard);
						}
					} else {										// если пользователь отправил не команду, а произвольный текст
						cubeBot.sendMessage(msg.sender.id, "Воспользуйся клавиатурой ниже");	
						if (lampIsEnabled) {
							cubeBot.sendMessage(msg.sender.id, "💡 Cейчас cвет включен", cubeOffReplyKeyboard);
						} else	{
							cubeBot.sendMessage(msg.sender.id, "🌃 Cейчас cвет выключен", cubeOnReplyKeyboard);
						}
					}
				}

				isUserFinded = true;
				break;
			}
		}

		if(!isUserFinded) {
			String reply = (String)"Тебя нет в списке разрешенных контактов. ChatID: " + msg.sender.id;
			cubeBot.sendMessage(msg.sender.id, reply);
		}
		
	}
	
	delay(50);
}
