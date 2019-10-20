#include <Arduino.h>

#define DEBOUNCE 10  // how many ms to debounce, 5+ ms is usually plenty
#define DOUBLECLICK 300  // how many ms for double-press detect

//define the buttons and LEDs
byte buttons[] = {A1, A2, A3, A4};
 
//determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)

const byte cmdCC01 = 0xB0;  // Control change, channel 1
const byte cmdCC02 = 0xB1;  //                 channel 2
const byte cmdPC01 = 0xC0;  // Program change, channel 1
const byte cmdPC02 = 0xC1;  //                 channel 2

//track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];
int stomp[NUMBUTTONS];
long lastpress[NUMBUTTONS];

byte data;

void setup() {
  // Set up serial ports
  Serial.begin(9600);
  Serial.println("Init...");

  Serial1.begin(31250);

  // Initialize to switch one
  stomp[0] = 0;
  stomp[1] = 127;
  stomp[2] = 127;
  stomp[3] = 127;
  sendCommand(cmdPC01, 5, 0xFF);
  sendCommand(cmdPC02, 1, 0xFF);
  sendCommand(cmdCC01, 102, 127);
}

// 0: Dotted long - Dotted short
// 1: Swell - long / short
// 2: Off long - Off short
// 3: Quarter short - Quarter Long

void loop() {
  // Check for midi in and send it as midi thru
  if (Serial1.available()) {
    data = Serial1.read();
    Serial.println(data);
    Serial1.write(data);
  }
  
 byte thisSwitch=thisSwitch_justPressed();

  switch(thisSwitch)
  {  
  case 0: 
    Serial.println("switch 1 just pressed");
    if (stomp[thisSwitch]==0) {
      stomp[thisSwitch] = 127;
      sendCommand(cmdPC01, 5, 0xFF);  // patch change the Timeline
      sendCommand(cmdPC02, 0, 0xFF);  // patch change the BigSky
      sendCommand(cmdCC01, 102, 127); // engage the Timeline
    } else {
      stomp[thisSwitch] = 0;
      sendCommand(cmdPC01, 5, 0xFF);
      sendCommand(cmdPC02, 1, 0xFF);
      sendCommand(cmdCC01, 102, 127);
    }
    break;
  case 1: 
    Serial.println("switch 2 just pressed");
    if (stomp[thisSwitch]==0) {
      stomp[thisSwitch] = 127;
      sendCommand(cmdPC01, 3, 0xFF);
      sendCommand(cmdPC02, 1, 0xFF);
      sendCommand(cmdCC01, 102, 127);
    } else {
      stomp[thisSwitch] = 0;
      sendCommand(cmdPC01, 3, 0xFF);
      sendCommand(cmdPC02, 2, 0xFF);
      sendCommand(cmdCC01, 102, 127);
    }
    break;
  case 2: 
    Serial.println("switch 3 just pressed");
    if (stomp[thisSwitch]==0) {
      stomp[thisSwitch] = 127;
      sendCommand(cmdPC01, 4, 0xFF);
      sendCommand(cmdPC02, 0, 0xFF);
      sendCommand(cmdCC01, 102, 127);
    } else {
      stomp[thisSwitch] = 0;
      sendCommand(cmdPC01, 4, 0xFF);
      sendCommand(cmdPC02, 1, 0xFF);
      sendCommand(cmdCC01, 102, 127);
    }
    break;
  case 3: 
    Serial.println("switch 4 just pressed");
    if (stomp[thisSwitch]==0) {
      stomp[thisSwitch] = 127;
      sendCommand(cmdCC01, 102, 0);  // bypass the Timeline
      sendCommand(cmdPC02, 1, 0xFF);
    } else {
      stomp[thisSwitch] = 0;
      sendCommand(cmdCC01, 102, 0);  // bypass the Timeline
      sendCommand(cmdPC02, 2, 0xFF);
    }
    break; 
  }

}

void sendCommand(byte cmd, byte data1, byte data2) {
    Serial1.write(cmd);
    Serial1.write(data1);
    if (data2 != 0xFF) {
      Serial1.write(data2);
    }
 
     //prints the values in the serial monitor so we can see what note we're playing
    Serial.print("cmd: ");
    Serial.print(cmd);
    Serial.print(", data1: ");
    Serial.print(data1);
    Serial.print(", data2: ");
    Serial.println(data2);
}

void check_switches() {
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       //when we start, we clear out the "just" indicators
    justreleased[index] = 0;
    currentstate[index] = digitalRead(buttons[index]);   //read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        justreleased[index] = 1; // just released
      }
      pressed[index] = !currentstate[index];  //remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index]; //keep a running tally of the buttons
  }
}

byte thisSwitch_justPressed() {
  byte thisSwitch = 255;
  check_switches();  //check the switches &amp; get the current state
  for (byte i = 0; i < NUMBUTTONS; i++) {
    current_keystate[i]=justpressed[i];
    if (current_keystate[i] != previous_keystate[i]) {
      if (current_keystate[i]) thisSwitch=i;
    }
    previous_keystate[i]=current_keystate[i];
  }  
  return thisSwitch;
}
