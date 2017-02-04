/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Motion Sensor example using HC-SR501
 * http://www.mysensors.org/build/motion
 *
 */

// Enable debug prints
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_BAUD_RATE 57600
//#define MY_NODE_ID AUTO
#define MY_NODE_ID 3

#include <SPI.h>
#include <MySensors.h>

unsigned long SLEEP_TIME = 5*60*1000; // Sleep time between reports (in milliseconds)
// Digital input pin for door sensor. Will be high if door open. Used for interrupt.
#define DOOR_PIN 2
#define MOTION_PIN 3

// whether to send only changed values, or always send changes after sleep perioid
// #define SEND_ONLY_CHANGES

// Ids of the sensor children
#define DOOR_ID 2
#define MOTION_ID 3


// Initialize motion message
MyMessage msg_door(DOOR_ID, V_TRIPPED);
MyMessage msg_motion(MOTION_ID, V_TRIPPED);

void setup()
{
  Serial.begin(MY_BAUD_RATE);
  pinMode(DOOR_PIN, INPUT);
  digitalWrite(DOOR_PIN, HIGH);
  pinMode(MOTION_PIN, INPUT);
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Motion Sensor", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(DOOR_ID, S_DOOR);
  present(MOTION_ID, S_MOTION);
}

#ifdef SEND_ONLY_CHANGES
bool door_prev = false;
bool motion_prev = false;
#endif

void loop()
{
  // Read digital motion value
  boolean door = digitalRead(DOOR_PIN) == HIGH;
  boolean motion = digitalRead(MOTION_PIN) == HIGH;

  Serial.print(F("Door  :"));Serial.println(door);
  Serial.print(F("Motion:"));Serial.println(motion);

#ifdef SEND_ONLY_CHANGES
  if (door != door_prev) {
#endif
    send(msg_door.set(door?"1":"0"));
#ifdef SEND_ONLY_CHANGES
    door_prev = door;
  }
  if (motion != motion_prev) {
#endif
    send(msg_motion.set(motion?"1":"0"));
#ifdef SEND_ONLY_CHANGES
    motion_prev = motion;
  }
#endif
  // Sleep until interrupt comes in on motion sensor. Send update every two minute.
  sleep(digitalPinToInterrupt(DOOR_PIN), CHANGE,
        digitalPinToInterrupt(MOTION_PIN), CHANGE, SLEEP_TIME);
}
