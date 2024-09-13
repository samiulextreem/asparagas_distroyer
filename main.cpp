#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ESP32Servo.h"
#include "BluetoothSerial.h"





BluetoothSerial SerialBT;

int SERVO_FINGER_PIN = 12;     //servo motor to control fin 1
int BLADE_FINGER_PIN = 13;     //servo motor to control fin 2
int PROP_Pin =  14;     // PWM control of PROPELLER


Servo servo_finger;         // servo PWM initialization for servo motor 1
Servo servo_blade;        // servo PWM initialization for servo motor 1
ESP32PWM motor_prop_F;   // PWM initialization for power control of propeller motor 



//Declaring several function to control each part of the submarine independently from each other with their each loop


void SERVO_FINGER( void *pvParameters );   // Function to control servo fin
void SERVO_BLADE( void *pvParameters );   // Function to control servo fin
void USER_INPUT( void *pvParameters );   // Function to control user input


void BT_HANDLER( void *pvParameters );    //bluetooth function initiation






// declaring message passing queue to pass information from one function to another
static QueueHandle_t finger_Q = NULL;
static QueueHandle_t blade_Q = NULL;



void setup() {
  // put your setup code here, to run once:
 
	Serial.begin(115200);
	analogReadResolution(12);                     //// Set ADC resolution to 12 bits (0-4095)
	SerialBT.begin("ESP32");




	servo_finger.attach(SERVO_FINGER_PIN,500,2400);
	servo_blade.attach(BLADE_FINGER_PIN,500,2400);

	finger_Q = xQueueCreate(1, sizeof(int));
	blade_Q = xQueueCreate(1, sizeof(int));


	
	//initializing all the functions to run independently from each other using real time operating system API
	xTaskCreate(SERVO_FINGER, "SERVO_FINGER", 4096, NULL, 5, NULL);
  	xTaskCreate(SERVO_BLADE, "SERVO_BLADE", 4096, NULL, 5, NULL);
	xTaskCreate(USER_INPUT, "USER_INPUT", 4096, NULL, 5, NULL);

	Serial.println("setup done");
}



void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  //Serial.println("looping");
	


}

void SERVO_FINGER(void *pvParameters) {

	Serial.print("starting ... task 1 loop .\n");
	delay(200);
	int finger_pos = 0;
	while(1) {
		delay(500);
		if (xQueueReceive(finger_Q, &finger_pos, 10) == pdTRUE)
		{
			Serial.printf("task 1 reporting ::  setting finger servo position to %d\n",finger_pos);
			SerialBT.printf("task 1 reporting :: setting finger servo position to %d\n",finger_pos);
			servo_finger.write(finger_pos);
		}
		


	}
}

void SERVO_BLADE(void *pvParameters) {

	Serial.print("starting ... task 2 loop .\n");
	delay(200);
	int blade_pos = 0;
	while (1) {
		delay(500);
		if (xQueueReceive(blade_Q, &blade_pos, 10) == pdTRUE)
		{
			Serial.printf("task 2 reporting :: setting blade servo position to %d\n",blade_pos);
			SerialBT.printf("task 2 reporting :: setting blade servo position to %d\n",blade_pos);
			servo_blade.write(blade_pos);
		}
	}
}


void USER_INPUT(void *pvParameters) {

	Serial.print("starting ... task 3 loop .\n");
	delay(200);
	int finger_pos = 0;
	int blade_pos = 0;
	while (1) {
		delay(500);
		if (Serial.available() > 0){
			String input = Serial.readString();

			if (input.length() > 0){
				if(input[input.length()-1] == '\n'){
					Serial.printf("task 3 reporting :: user input %s\n",input.c_str());
					SerialBT.printf("task 3 reporting :: user input %s\n",input.c_str());
					if(input.startsWith("F") || input.startsWith("f")){
						finger_pos = input.substring(1).toInt();
						Serial.printf("task 3 reporting :: finger servo position %d\n",finger_pos);
						SerialBT.printf("task 3 reporting :: finger servo position %d\n",finger_pos);
						xQueueSend(finger_Q, &finger_pos, 10);

					}
					else if(input.startsWith("B") || input.startsWith("b")){
						blade_pos = input.substring(1).toInt();
						Serial.printf("task 3 reporting :: blade servo position %d\n",blade_pos);
						SerialBT.printf("task 3 reporting :: blade servo position %d\n",blade_pos);
						xQueueSend(blade_Q, &blade_pos, 10);
					}

				}
			}
			
		}
		if (SerialBT.available() > 0){
			String input = SerialBT.readString();

			if (input.length() > 0){
				if(input[input.length()-1] == '\n'){
					Serial.printf("task 3 reporting :: user input %s\n",input.c_str());
					SerialBT.printf("task 3 reporting :: user input %s\n",input.c_str());
					if(input.startsWith("F") || input.startsWith("f")){
						finger_pos = input.substring(1).toInt();
						Serial.printf("task 3 reporting :: finger servo position %d\n",finger_pos);
						SerialBT.printf("task 3 reporting :: finger servo position %d\n",finger_pos);
						xQueueSend(finger_Q, &finger_pos, 10);

					}
					else if(input.startsWith("B") || input.startsWith("b")){
						blade_pos = input.substring(1).toInt();
						Serial.printf("task 3 reporting :: blade servo position %d\n",blade_pos);
						SerialBT.printf("task 3 reporting :: blade servo position %d\n",blade_pos);
						xQueueSend(blade_Q, &blade_pos, 10);
					}

				}
			}
			
		}

		
	}
}
