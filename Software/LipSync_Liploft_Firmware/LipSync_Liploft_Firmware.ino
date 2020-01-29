/*
//                                                                                                  
//  +++         .+++:    /++++++++/:.     .:/+++++/: .+++/`     .+++/  ++++.      ++++.     `-/++++++/:
//  oooo         .ooo:    +ooo:--:+ooo/   :ooo/:::/+/  -ooo+`   .ooo+`  ooooo:     .o-o`   `/ooo+//://+:
//  oooo         .ooo:    +ooo`    :ooo-  oooo`     `   .ooo+` .ooo+`   oooooo/`   .o-o`  .oooo-`       
//  oooo         .ooo:    +ooo`    -ooo-  -ooo+:.`       .ooo+.ooo/`    ooo:/oo+.  .o-o`  +ooo.         
//  oooo         .ooo:    +ooo.`..:ooo+`   `:+oooo+:`     `+ooooo/      ooo: :ooo- .o-o`  oooo          
//  oooo         .ooo:    +ooooooooo+:`       `-:oooo-     `+ooo/       ooo/  .+oo/.o-o`  +ooo.         
//  oooo         .ooo:    +ooo-...``             `oooo      /ooo.       ooo/   `/oo-o-o`  .oooo-        
//  oooo::::::.  .ooo:    +ooo`           :o//:::+ooo:      /ooo.       ooo/     .o-o-o`   ./oooo/:::/+/
//  +ooooooooo:  .ooo:    /ooo`           -/++ooo+/:.       :ooo.       ooo:      `.o.+      `-/+oooo+/-
//
//An assistive technology device which is developed to allow quadriplegics to operate drones by manipulation of a mouth-operated joystick with integrated sip and puff controls.
*/

//Developed by : MakersMakingChange + UBC hackathon team
//Firmware : LipSync_LipLoft_Firmware
//VERSION: 1.1 (23 Jan 2020) 


#include <EEPROM.h>
#include <math.h>


#define DEBUG_MODE true                           //Debug mode ( Enabled = true and Disabled = false )

//***PIN ASSIGNMENTS***//
#define MODE_SELECT_PIN 12                        // LipSync Mode Select
#define BUTTON_UP_PIN 8                           // Control Button 1: UP - digital input pin 8 (internally pulled-up)
#define BUTTON_DOWN_PIN 7                         // Control Button 2: DOWN - digital input pin 7 (internally pulled-up)
#define LED_1_PIN 4                               // LipSync LED Color1 : GREEN - digital output pin 5
#define LED_2_PIN 5                               // LipSync LED Color2 : RED - digital outputpin 4

#define TRANS_CONTROL_PIN A3                      // Bluetooth Transistor Control Pin - digital output pin A3
#define PIO4_PIN A4                               // Bluetooth PIO4_PIN Command Pin - digital output pin A4

#define PRESSURE_PIN A5                           // Sip & Puff Pressure Transducer Pin - analog input pin A5
#define X_DIR_HIGH_PIN A0                         // X Direction High (Cartesian positive x : right) - analog input pin A0
#define X_DIR_LOW_PIN A1                          // X Direction Low (Cartesian negative x : left) - digital output pin A1
#define Y_DIR_HIGH_PIN A2                         // Y Direction High (Cartesian positive y : up) - analog input pin A2
#define Y_DIR_LOW_PIN A10                         // Y Direction Low (Cartesian negative y : down) - analog input pin A10

//***CUSTOMIZE VALUES***//

#define RAW_PRESSURE_THRESHOLD 100                 //Sip/Puff threashold for raw bt mode

#define BT_CONFIG_FLAG false                      //Configure bluetooth ( Configure = true and Not Configure = false ). This is used to reset bluetooth module
#define JOYSTICK_DEFAULT_SPEED 30                 //Maximum default speed  (Not used)   
#define JOYSTICK_DELTA_SPEED 5                    //Delta value that is used to calculate speed levels (Not used)  
#define JOYSTICK_RADIUS 30.0                      //Constant joystick radius


//***VARIABLE DECLARATION***//

int joystickSpeedCounter = 5;                       //Cursor speed counter which is set to 4 by default (Not used)  


int xHigh, yHigh, xLow, yLow;                       
int xRight, xLeft, yUp, yDown;

int spNeutral;                                    //Individual neutral starting positions for each FSR

int xHighMax, xLowMax, yHighMax, yLowMax;         //Max FSR values which are set to the values from EEPROM


float xHighYHighRadius, xHighYLowRadius, xLowYLowRadius, xLowYHighRadius;
float xHighYHigh, xHighYLow, xLowYLow, xLowYHigh;


int commMode = 0;                                 
int bluetoothConfigDone;                          // Binary check of completed Bluetooth configuration (Not Used)
unsigned int puffCount, sipCount;                 //The puff and long sip incremental counter variables
int pollCounter = 0;                              //poll counter


float yHighDebug, yLowDebug, xHighDebug, xLowDebug;

//Pressure sensor variables
float sipThreshold, puffThreshold;

//Drone Speed Level structure 
typedef struct {
  int _delay;
  float _factor;
  int _maxSpeed;
} _joystick;

//Define characteristics of each speed level ( Example: 5,-1.0,10,2 ) (Not used)
_joystick setting1 = {5, -1.1, JOYSTICK_DEFAULT_SPEED - (5 * JOYSTICK_DELTA_SPEED)};
_joystick setting2 = {5, -1.1, JOYSTICK_DEFAULT_SPEED - (4 * JOYSTICK_DELTA_SPEED)};
_joystick setting3 = {5, -1.1, JOYSTICK_DEFAULT_SPEED - (3 * JOYSTICK_DELTA_SPEED)};
_joystick setting4 = {5, -1.1, JOYSTICK_DEFAULT_SPEED - (2 * JOYSTICK_DELTA_SPEED)};
_joystick setting5 = {5, -1.1, JOYSTICK_DEFAULT_SPEED - (JOYSTICK_DELTA_SPEED)};
_joystick setting6 = {5, -1.1, JOYSTICK_DEFAULT_SPEED};
_joystick setting7 = {5, -1.1, JOYSTICK_DEFAULT_SPEED + (JOYSTICK_DELTA_SPEED)};
_joystick setting8 = {5, -1.1, JOYSTICK_DEFAULT_SPEED + (2 * JOYSTICK_DELTA_SPEED)};
_joystick setting9 = {5, -1.1, JOYSTICK_DEFAULT_SPEED + (3 * JOYSTICK_DELTA_SPEED)};
_joystick setting10 = {5, -1.1, JOYSTICK_DEFAULT_SPEED + (4 * JOYSTICK_DELTA_SPEED)};
_joystick setting11 = {5, -1.1, JOYSTICK_DEFAULT_SPEED + (5 * JOYSTICK_DELTA_SPEED)};

_joystick joystickParams[11] = {setting1, setting2, setting3, setting4, setting5, setting6, setting7, setting8, setting9, setting10, setting11};

int single = 0;
int puff1, puff2;

bool settingsEnabled = false; 

//-----------------------------------------------------------------------------------//

//***MICROCONTROLLER AND PERIPHERAL MODULES CONFIGURATION***//

void setup() {
  
  Serial.begin(115200);                           //Setting baud rate for serial communication which is used for diagnostic data returned from Bluetooth and microcontroller
  Serial1.begin(115200);                          //Setting baud rate for Bluetooth AT command 

  pinMode(LED_1_PIN, OUTPUT);                     //Set the LED pin 1 as output(GREEN LED)
  pinMode(LED_2_PIN, OUTPUT);                     //Set the LED pin 2 as output(RED LED)
  pinMode(TRANS_CONTROL_PIN, OUTPUT);             //Set the transistor pin as output
  pinMode(PIO4_PIN, OUTPUT);                      //Set the bluetooth command mode pin as output

  pinMode(PRESSURE_PIN, INPUT);                   //Set the pressure sensor pin input
  pinMode(X_DIR_HIGH_PIN, INPUT);                 //Define Force sensor pinsas input ( Right FSR )
  pinMode(X_DIR_LOW_PIN, INPUT);                  //Define Force sensor pinsas input ( Left FSR )
  pinMode(Y_DIR_HIGH_PIN, INPUT);                 //Define Force sensor pinsas input ( Up FSR )
  pinMode(Y_DIR_LOW_PIN, INPUT);                  //Define Force sensor pinsas input ( Down FSR )

  pinMode(MODE_SELECT_PIN, INPUT_PULLUP);     
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); 
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  pinMode(2, INPUT_PULLUP);                       //Set unused pins as inputs with pullups
  pinMode(3, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);

  if(DEBUG_MODE==true) {
    Serial.println("Beginning initialization...");
  }


  joystickInitialization();                       //Set the Home joystick and generate movement threshold boundaries
  delay(10);
  pressureSensorInitialization();                 //Set the pressure sensor threshold boundaries
  delay(10);
  setDefault();                                   //Set the default values that are stored in EEPROM
  delay(10);
  bluetoothConfigure();                           //Conditionally configure the Bluetooth module
  delay(10);
  if(DEBUG_MODE==true) {Serial.println("Initialization complete.");}
  
  ledBlink(4, 250, 3);                            //End initialization visual feedback

  displayVersion();                               //Display firmware version number

  //Functions below are for diagnostic feedback only
  if(DEBUG_MODE==true) {
      Serial.println((String)"bluetoothConfigDone: "+EEPROM.get(0, puff1));
      delay(5);
      Serial.println((String)"yHighComp: "+EEPROM.get(6, yHighDebug));
      delay(5);
      Serial.println((String)"yLowComp: "+EEPROM.get(10, yLowDebug));
      delay(5);
      Serial.println((String)"xHighComp: "+EEPROM.get(14, xHighDebug));
      delay(5);
      Serial.println((String)"xLowComp: "+EEPROM.get(18, xLowDebug));
      delay(5);
  }

}

//-----------------------------------------------------------------------------------//

//***START OF MAIN LOOP***//

void loop() {

  settingsEnabled=serialSettings(settingsEnabled);       //Check to see if setting option is enabled in Lipsync

  xHigh = (float)analogRead(X_DIR_HIGH_PIN);             //Read analog values of FSR's : A0
  xLow = (float)analogRead(X_DIR_LOW_PIN);               //Read analog values of FSR's : A1
  yHigh = (float)analogRead(Y_DIR_HIGH_PIN);             //Read analog values of FSR's : A0
  yLow = (float)analogRead(Y_DIR_LOW_PIN);               //Read analog values of FSR's : A10
  
  xHighYHigh = sqrt(sq(((xHigh - xRight) > 0) ? (float)(xHigh - xRight) : 0.0) + sq(((yHigh - yUp) > 0) ? (float)(yHigh - yUp) : 0.0));     //The sq() function raises thr input to power of 2 and is returning the same data type int->int
  xHighYLow = sqrt(sq(((xHigh - xRight) > 0) ? (float)(xHigh - xRight) : 0.0) + sq(((yLow - yDown) > 0) ? (float)(yLow - yDown) : 0.0));    //The sqrt() function raises input to power 1/2, returning a float type
  xLowYHigh = sqrt(sq(((xLow - xLeft) > 0) ? (float)(xLow - xLeft) : 0.0) + sq(((yHigh - yUp) > 0) ? (float)(yHigh - yUp) : 0.0));          //These are the vector magnitudes of each quadrant 1-4. Since the FSRs all register
  xLowYLow = sqrt(sq(((xLow - xLeft) > 0) ? (float)(xLow - xLeft) : 0.0) + sq(((yLow - yDown) > 0) ? (float)(yLow - yDown) : 0.0));         //a larger digital value with a positive application force, a large negative difference

  //Check to see if the joystick has moved
  if ((xHighYHigh > xHighYHighRadius) || (xHighYLow > xHighYLowRadius) || (xLowYLow > xLowYLowRadius) || (xLowYHigh > xLowYHighRadius)) {
    //Add to the poll counter
    pollCounter++;
    delay(20); 
    if (pollCounter >= 3) {
       sendRawBTPacket(0, (int)xHigh, (int)xLow, (int)yHigh, (int)yLow);
    }
  }
  //Perform buttons functions
  processButtonInput();

  //Perform pressure sensor sip and puff functions
  processRawPressureInput();
}

//***END OF Main LOOP***//

//-----------------------------------------------------------------------------------//

//***BUTTON INPUT PROCESSING***//

void processButtonInput()
{
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
    delay(200);
    ledClear();
    delay(50);
    if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      joystickManualCalibration();                      //Call joystick calibration if both push button up and down are pressed 
    } 
  }

  if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
    delay(200);
    ledClear();
    delay(50);
    if (digitalRead(BUTTON_UP_PIN) == LOW) {
      joystickManualCalibration();                      //Call joystick calibration if both push button up and down are pressed 
    } 
  }
}

//***PRESSURE INPUT PROCESSING***//

void processRawPressureInput()
{
  int value = analogRead(PRESSURE_PIN);
  if (value > spNeutral - RAW_PRESSURE_THRESHOLD && value < spNeutral + RAW_PRESSURE_THRESHOLD)
  {
    return;
  }
  sendRawBTPacket(2, value);
}
//***END OF INPUT PROCESSING***//

//-----------------------------------------------------------------------------------//
//***SERIAL SETTINGS FUNCTION TO CHANGE SPEED AND COMMUNICATION MODE USING SOFTWARE***//

bool serialSettings(bool enabled) {

    String inString = "";  
    bool settingsFlag = enabled;                   //Set the input parameter to the flag returned. This will help to detect that the settings actions should be performed.
    
     if (Serial.available()>0)  
     {  
       inString = Serial.readString();            //Check if serial has received or read input string and word "settings" is in input string.
       if (settingsFlag==false && inString=="settings") {
       Serial.println("Actions:");                //Display list of possible actions 
       Serial.println("C,(0, or 1)");
       Serial.println("B,(0, or 1)");
       settingsFlag=true;                         //Set the return flag to true so settings actions can be performed in the next call to the function
       }
       else if (settingsFlag==true && inString.length()==((2*2)-1)){ //Check if the input parameter is true and the received string is 3 characters only
        inString.replace(",","");                 //Remove commas 
        if(inString.length()==2) {                //Perform settings actions if there are only two characters in the string.
          writeSettings(inString);
          Serial.println("Successfully changed.");
        }   
        Serial.println("Exiting the settings.");
        settingsFlag=false;   
       }
       else if (settingsFlag==true){
        Serial.println("Exiting the settings.");
        settingsFlag=false;         
       }
       Serial.flush();  
     }  
    return settingsFlag;
}

//***PERFORM SETTINGS FUNCTION TO CHANGE SPEED AND COMMUNICATION MODE USING SOFTWARE***//

void writeSettings(String changeString) {
    char changeChar[changeString.length()+1];
    changeString.toCharArray(changeChar, changeString.length()+1);
    //Configure Bluetooth module if received "B,1", Don't configure Bluetooth module if received "B,1"
    if(changeChar[0]=='B' && changeChar[1]=='0') {
      Serial.println("Don't Configure Bluetooth module");
      delay(10);
    } else if (changeChar[0]=='B' && changeChar[1]=='1') {
      bluetoothCommandMode();                                //Call Bluetooth command mode function to enter command mode
      Serial.println("Configure Bluetooth module");
      bluetoothConfigSequenceForRawTx();
      delay(10);
    } 

    if (changeChar[0]=='C' && changeChar[1]=='1') {
      bluetoothCommandMode();                                //Joystick Manual Calibration
      Serial.println("Joystick Manual Calibration");
      joystickManualCalibration();
      delay(10);
    } 
}

//***DISPLAY VERSION FUNCTION***//

void displayVersion(void) {

  Serial.println(" --- ");
  Serial.println("LipSync LipLoft Firmware Ver1.1 (23 January 2020)");
  Serial.println(" --- ");

}

//***SIP SECONDARY FUNCTION SELECTION***//



//***LED ON FUNCTION***//

void ledOn(int ledNumber) {
  switch (ledNumber) {
    case 1: {
        digitalWrite(LED_1_PIN, HIGH);
        delay(5);
        digitalWrite(LED_2_PIN, LOW);
        break;
      }
    case 2: {
        digitalWrite(LED_2_PIN, HIGH);
        delay(5);
        digitalWrite(LED_1_PIN, LOW);
        break;
      }
  }
}

//***LED CLEAR FUNCTION***//

void ledClear(void) {
  digitalWrite(LED_1_PIN, LOW);
  digitalWrite(LED_2_PIN, LOW);
}

//***LED BLINK FUNCTION***//

void ledBlink(int numBlinks, int delayBlinks, int ledNumber) {
  if (numBlinks < 0) numBlinks *= -1;

  switch (ledNumber) {
    case 1: {
        for (int i = 0; i < numBlinks; i++) {
          digitalWrite(LED_1_PIN, HIGH);
          delay(delayBlinks);
          digitalWrite(LED_1_PIN, LOW);
          delay(delayBlinks);
        }
        break;
      }
    case 2: {
        for (int i = 0; i < numBlinks; i++) {
          digitalWrite(LED_2_PIN, HIGH);
          delay(delayBlinks);
          digitalWrite(LED_2_PIN, LOW);
          delay(delayBlinks);
        }
        break;
      }
    case 3: {
        for (int i = 0; i < numBlinks; i++) {
          digitalWrite(LED_1_PIN, HIGH);
          delay(delayBlinks);
          digitalWrite(LED_1_PIN, LOW);
          delay(delayBlinks);
          digitalWrite(LED_2_PIN, HIGH);
          delay(delayBlinks);
          digitalWrite(LED_2_PIN, LOW);
          delay(delayBlinks);
        }
        break;
      }
    case 6: {
        digitalWrite(LED_1_PIN, LOW);
        digitalWrite(LED_2_PIN, LOW);
        break;
      }
  }
}


//***JOYSTICK INITIALIZATION FUNCTION***//

void joystickInitialization(void) {
  xHigh = analogRead(X_DIR_HIGH_PIN);               //Set the initial neutral x-high value of joystick
  delay(10);

  xLow = analogRead(X_DIR_LOW_PIN);                 //Set the initial neutral x-low value of joystick
  delay(10);

  yHigh = analogRead(Y_DIR_HIGH_PIN);               //Set the initial neutral y-high value of joystick
  delay(10);

  yLow = analogRead(Y_DIR_LOW_PIN);                 //Set the initial Initial neutral y-low value of joystick
  delay(10);

  xRight = xHigh;
  xLeft = xLow;
  yUp = yHigh;
  yDown = yLow;

  xHighYHighRadius = JOYSTICK_RADIUS;
  xHighYLowRadius = JOYSTICK_RADIUS;
  xLowYLowRadius = JOYSTICK_RADIUS;
  xLowYHighRadius = JOYSTICK_RADIUS;

  EEPROM.put(6, yUp);
  delay(5);
  EEPROM.put(10, yDown);
  delay(5);
  EEPROM.put(14, xRight);
  delay(5);
  EEPROM.put(18, xLeft);
  delay(5);

  Serial.println("Joystick Initialization completed.");

}

//***PRESSURE SENSOR INITIALIZATION FUNCTION***//

void pressureSensorInitialization(void) {
  
  spNeutral = analogRead(PRESSURE_PIN);

}

//***RAW BLUETOOTH PACKET FUNCTION***//
void sendRawBTPacket(int inputSource, int xLow, int xHigh, int yLow, int yHigh)
{ 
  byte packet[11];

  packet[0] = 0xFF;
  packet[1] = 0xFF;
  packet[2] = lowByte(inputSource);
  packet[3] = highByte(xHigh);
  packet[4] = lowByte(xHigh);
  packet[5] = highByte(xLow);
  packet[6] = lowByte(xLow);
  packet[7] = highByte(yHigh);
  packet[8] = lowByte(yHigh);
  packet[9] = highByte(yLow);
  packet[10] = lowByte(yLow);
  
  sendBtPacket(packet, 11);
}

void sendRawBTPacket(int inputSource, int value)
{
  byte packet[11];

  packet[0] = 0xFF;
  packet[1] = 0xFF;
  packet[2] = lowByte(inputSource);
  packet[3] = highByte(value);
  packet[4] = lowByte(value);
  packet[5] = 0;
  packet[6] = 0;
  packet[7] = 0;
  packet[8] = 0;
  packet[9] = 0;
  packet[10] = 0;

  Serial.println(value);
  sendBtPacket(packet, 11);
}

void sendBtPacket(byte packet[], int bufferSize)
{
  Serial.println("Sending raw packet ...");
  Serial1.write(packet, bufferSize);
  Serial1.flush();
  
  delay(10);
}

//----------------------RN-42 BLUETOOTH MODULE INITIALIZATION SECTION----------------------//

//***BLUETOOTH CONFIGURATION STATUS FUNCTION***//

void bluetoothConfigStatus(void) {
  int eepromVal = 3;                                //Local integer variable initialized and defined for use with EEPROM GET function
  EEPROM.get(0, eepromVal);                         //Assign value of EEPROM memory at index zero (0) to int variable BT_EEPROM
  delay(10);
  bluetoothConfigDone = (eepromVal == 1) ? eepromVal : 0;  //Define the bluetoothConfigDone to 0 if the device is set for the first time
  delay(10);
}

//***BLUETOOTH CONFIGURATION FUNCTION***//

void bluetoothConfigure(void) {
  
  bluetoothConfigStatus();                                 //Check if Bluetooth has previously been configured
  delay(10);
  if (bluetoothConfigDone == 0 || BT_CONFIG_FLAG) {        //If Bluetooth has not been configured or Bluetooth config flag is true then execute configuration sequence
    bluetoothCommandMode();                                //Call Bluetooth command mode function to enter command mode
    Serial.println("Configure Bluetooth module");
    bluetoothConfigSequenceForRawTx();
    delay(10);
  } else {
    Serial.println("Bluetooth has previously been configured.");
    delay(10);
  }
}

//***BLUETOOTH CMD MODE FUNCTION***//

void bluetoothCommandMode(void) {
  digitalWrite(TRANS_CONTROL_PIN, HIGH);            //Set the transistor base pin to HIGH to ensure Bluetooth module is off
  digitalWrite(PIO4_PIN, HIGH);                     //Set the command pin to high
  delay(10);

  digitalWrite(TRANS_CONTROL_PIN, LOW);             //Set the transistor base pin to LOW to power on Bluetooth module
  delay(10);

  for (int i = 0; i < 3; i++) {                     //Cycle HIGH and LOW the PIO4_PIN pin 3 times with 1 sec delay between each level transition
    digitalWrite(PIO4_PIN, HIGH);
    delay(150);
    digitalWrite(PIO4_PIN, LOW);
    delay(150);
  }

  digitalWrite(PIO4_PIN, LOW);                      //Set the PIO4_PIN pin low as per command mode instructions
  delay(10);
  Serial1.print("$$$");                             //Enter Bluetooth command mode
  delay(50);                                        //Add time delay to visual inspect the red LED is flashing at 10Hz which indicates the Bluetooth module is in Command Mode
  Serial.println("Bluetooth CMD Mode Activated");
}

//***BLUETOOTH CONFIG FUNCTION***//

void bluetoothConfigSequenceForRawTx(void) {

  ledBlink(2,100,1);
  delay(5);
  ledOn(1);
  delay(5);
  
  Serial1.println("ST,255");                        //Turn off the 60 sec timer for command mode
  delay(15);
  Serial1.println("SA,0");                          //Set Authentication Value to 2
  delay(15);
  Serial1.println("SX,0");                          //Set Bonding to 0 or disabled
  delay(15);
  Serial1.println("SN,LipSyncLipLoft");             //Set the name of BT module (Lipsync_Shawn)
  delay(15);
  Serial1.println("SM,0");                          //Set the Pairing mode to Slave
  delay(15);
  Serial1.println("SH,0200");                       //Configure device as HID mouse
  delay(15);
  Serial1.println("S~,0");                          //SPP Mode
  delay(15);
  Serial1.println("SQ,0");                          //Configure for latency NOT throughput : "SQ,0"
  delay(15);
  Serial1.println("S?,0");                          //Enable the role switch for better performance of high speed data
  delay(15);
  Serial1.println("R,1");                           //Reboot BT module
  delay(15);
  
  int val0 = 1;
  int val1 = joystickSpeedCounter;

  //Save the default joystick counter value and configuration completed value at EEPROM. This action will happen once when a new device is configured 
  
  EEPROM.put(0, val0);                              //Save the configuration completed value at EEPROM address location 0
  delay(15);
  EEPROM.put(2, val1);                              //Save the default joystick speed counter value at EEPROM address location 2
  delay(15);

  ledBlink(2,100,1);
  delay(5);
}


//***MANUAL JOYSTICK POSITION CALIBRATION FUNCTION***///

void joystickManualCalibration(void) {

  ledBlink(2,100,2);
  delay(5);
  ledOn(2);
  delay(5);

  xHigh = analogRead(X_DIR_HIGH_PIN);               //Set the initial neutral x-high value of joystick
  delay(10);
  Serial.println(xHigh);

  xLow = analogRead(X_DIR_LOW_PIN);                 //Set the initial neutral x-low value of joystick
  delay(10);
  Serial.println(xLow);

  yHigh = analogRead(Y_DIR_HIGH_PIN);               //Set the Initial neutral y-high value of joystick
  delay(10);
  Serial.println(yHigh);

  yLow = analogRead(Y_DIR_LOW_PIN);                 //Set the initial neutral y-low value of joystick
  delay(10);
  Serial.println(yLow);

  xRight = xHigh;
  xLeft = xLow;
  yUp = yHigh;
  yDown = yLow;

  delay(10);

  EEPROM.put(6, yUp);
  delay(5);
  EEPROM.put(10, yDown);
  delay(5);
  EEPROM.put(14, xRight);
  delay(5);
  EEPROM.put(18, xLeft);
  delay(5);


  Serial.println("Home calibration completed.");

  ledBlink(2,100,2);
  delay(5);

}

//***SET DEFAULT VALUES FUNCTION***///

void setDefault(void) {

  int defaultConfigSetup;
  int defaultJoystickSpeed;
  int defaultIsSet;

  EEPROM.get(4, defaultIsSet);
  delay(10);

  if (defaultIsSet != 1) {

    defaultConfigSetup = 0;
    EEPROM.put(0, defaultConfigSetup);
    delay(10);

    defaultJoystickSpeed = 4;
    EEPROM.put(2, defaultJoystickSpeed);
    delay(10);


    defaultIsSet = 1;
    EEPROM.put(4, defaultIsSet);
    delay(10);

  }
}
