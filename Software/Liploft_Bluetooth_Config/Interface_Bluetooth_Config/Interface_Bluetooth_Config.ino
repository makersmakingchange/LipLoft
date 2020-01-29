
//Interface_Bluetooth_Config.ino
//This firmware is used to configure bluetooth module in Liploft Interface unit if LipSync_Liploft_Firmware.ino can't be used.


//-----------------------------------------------------------------------------------//

//***MICROCONTROLLER AND PERIPHERAL MODULES CONFIGURATION***//

void setup() {
  
  Serial.begin(115200);                           //Setting baud rate for serial communication which is used for diagnostic data returned from Bluetooth and microcontroller
  Serial1.begin(9600);                          //Setting baud rate for Bluetooth AT command 

  Serial.println("Start the interface bluetooth config process."); 
  bluetoothCommandMode();                                //Call Bluetooth command mode function to enter command mode
  bluetoothConfigSequence();
  Serial.println("End the interface bluetooth config process."); 

}

//-----------------------------------------------------------------------------------//

//***START OF MAIN LOOP***//

void loop() {
}

//***END OF Main LOOP***//

//-----------------------------------------------------------------------------------//




//***BLUETOOTH CMD MODE FUNCTION***//

void bluetoothCommandMode(void) {
    
      Serial1.print("$");  // Print three times individually
      Serial1.print("$");
      Serial1.print("$");  // Enter command mode
      delay(100);  // Short delay, wait for the Mate to send back CMD   
}

//***BLUETOOTH CONFIG FUNCTION***//

void bluetoothConfigSequence(void) {
  Serial1.println("ST,255");                        //Turn off the 60 sec timer for command mode
  delay(15);
  Serial1.println("SA,2");                          //Set Authentication Value to 2
  delay(15);
  Serial1.println("SX,0");                          //Set Bonding to 0 or disabLED_PIN
  delay(15);
  Serial1.println("SN,Liploft");                  //Set the name of BT module (MOTORMOUTH)
  delay(15);
  Serial1.println("SM,3");                          //Set the Pairing mode to Master - Auto
  delay(15);
  Serial1.println("R,1");                           //Reboot BT module
  delay(15);
}
