
//Lipsync_Bluetooth_Config.ino
//This firmware is used to configure bluetooth module in Lipsync Liploft unit if LipSync_Liploft_Firmware.ino can't be used.

#define TRANS_CONTROL_PIN A3                      // Bluetooth Transistor Control Pin - digital output pin A3
#define PIO4_PIN A4                               // Bluetooth PIO4_PIN Command Pin - digital output pin A4


//-----------------------------------------------------------------------------------//

//***MICROCONTROLLER AND PERIPHERAL MODULES CONFIGURATION***//

void setup() {
  
  Serial.begin(115200);                           //Setting baud rate for serial communication which is used for diagnostic data returned from Bluetooth and microcontroller
  Serial1.begin(115200);                          //Setting baud rate for Bluetooth AT command 

  pinMode(TRANS_CONTROL_PIN, OUTPUT);             //Set the transistor pin as output

  
  pinMode(PIO4_PIN, OUTPUT);                      //Set the bluetooth command mode pin as output

  Serial.println("Start the Lipsync bluetooth config process."); 
  bluetoothCommandMode();                                //Call Bluetooth command mode function to enter command mode
  bluetoothConfigSequence();
  Serial.println("End the Lipsync bluetooth config process."); 

}

//-----------------------------------------------------------------------------------//

//***START OF MAIN LOOP***//

void loop() {
}

//***END OF Main LOOP***//

//-----------------------------------------------------------------------------------//




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

void bluetoothConfigSequence(void) {
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
}
