//Developed by : MakersMakingChange + UBC hackathon team
//Firmware : Liploft_Interface_Firmware
//VERSION: 1.9 (23 Jan 2020) 

//HOW TO CONTROL THE DRONE: 

//Arm the drone by puffing and sipping once
//sip/puff controls the throttle
//LipSync joystick controls pitch and roll (working well)
//gryo calibration is automated

//IMRPVOEMENTS: need to tune throttle sensitivity by manupulating the time-averaging array size as well as the step number increase/decrease

#include <MCP4261.h>
#include <SPI.h>
#define array_size    3
#define ch_throttle   1
#define ch_yaw        2
#define ch_pitch      3
#define ch_roll       4 
#define arm_V         3
#define maxV_stick    2.6
#define minV_stick    0
#define midV_stick    1.6   // supplies 1.65 V to pitch and roll channels
#define maxvolts      5.06
#define FloatV        1.4
#define portnum_throttle  10
#define portnum_yaw       9
#define portnum_pitch     8
#define portnum_roll      7

//***PIN ASSIGNMENTS***//

#define SWITCH_CONFIG_PIN 11  //Config jumper
// Then choose any other free pin as the Slave Select (pin 10 if the default but doesnt have to be)
#define MCP4261_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select               -> CS  (Pin 01 on MCP4261 DIP)

#define CS1_PIN 10
#define CS2_PIN 9
#define CS3_PIN 8
#define CS4_PIN 7
#define LED_PIN 5

float   volt_throttle = minV_stick; //set up neutral volt for throttle (up or down)
float   volt_yaw      = midV_stick; //set up neutral volt for yaw (CW and CCW)
float   volt_pitch    = midV_stick; //forward and backward
float   volt_roll     = midV_stick; //left and right
int     Arm           = 3;          //flag for arming the drone, should be 3 for arming, swith to 1 to gyro calibration
int     array_val[array_size];
int     i;
int     j;
float   sp_avg;
float   sp_sum;
bool    Flag_calib = 0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED_PIN was updated

// constants won't change:
const long interval = 500;           // interval at which to blink (milliseconds)

#define DEBUG_BT true
// #define DEBUG_XY_INPUT
// #define DEBUG_SP_INPUT
#define DEBUG_DP                  // DP = digital potentiometer

#define PACKET_CONTENT_SIZE 9  //Content size for raw bluetooth packets

// Its recommended to measure the rated end-end resistance (terminal A to terminal B)
// Because this can vary by a large margin, up to -+ 20%. And temperature variations.
float rAB_ohms = 5090.00; // 5k Ohm

// Instantiate Mcp4261 object, with default rW (=117.5 ohm, its typical resistance)
MCP4261 Mcp4261 = MCP4261( MCP4261_SLAVE_SELECT_PIN, rAB_ohms );

//float initThrottleOhms = 2340.0;
//float initSteeringOhms = 2200.0;


// stepped throttle control
#define THROTTLE_STEPS 8

//===== Set up MCP4161 variables =======//
byte address = 0x00;

bool flag__first = 0;
int port;
float voltsin;
long stepnum;
int  timer_1s;
bool Flag_check;
bool Flag_Floating;

int puffHardMax = 70;
int puffMax = 410;
int sipMin = 650;

byte packet[PACKET_CONTENT_SIZE];
bool searchingHeader = true;
int receivedContentSize = 0;
int headerCount = 0;

// xMaxAdjust adjusts the steering input for reaching maximum steering angle
//int xMaxAdjust = -70;
//
int xLowMin = 700;
//int xLowMax = 800 + xMaxAdjust;
//float ooXLowRange = 1.0 / (xLowMax - xLowMin);
//
int xHighMin = 700;
//int xHighMax = 800 + xMaxAdjust;
//float ooXHighRange = 1.0 / (xHighMax - xHighMin);
//
int yLowMin = 700;
int yHighMin = 700;


void setup()
{
  Serial.begin(9600);
  pinMode (CS1_PIN, OUTPUT);
  pinMode (CS2_PIN, OUTPUT);
  pinMode (CS3_PIN, OUTPUT);
  pinMode (CS4_PIN, OUTPUT); 
  pinMode (LED_PIN, OUTPUT);

  pinMode(SWITCH_CONFIG_PIN, INPUT_PULLUP);         // LOW: Config bluetooth (default with jumper in) HIGH: Don't Config bluetoot (jumper removed)
  SPI.begin();
  
  delay(1000);
  
  // set the MCP to sned the desired voltage to each joystick potentiometer in the RC transmitter circuit
  digitalPotWrite(ch_throttle,volttostep(ch_throttle,minV_stick));
  digitalPotWrite(ch_yaw,volttostep(ch_yaw,midV_stick));
  digitalPotWrite(ch_pitch,volttostep(ch_pitch,midV_stick));
  digitalPotWrite(ch_roll,volttostep(ch_roll,midV_stick));
    
  //SETUP BT

      Serial.println("Setting up BT Connection");
      Serial1.begin(115200);  // The Bluetooth Mate defaults to 115200bps
      
      Serial1.print("$");  // Print three times individually
      Serial1.print("$");
      Serial1.print("$");  // Enter command mode
      delay(100);  // Short delay, wait for the Mate to send back CMD
      Serial1.println("SU,9600");  // Temporarily Change the baudrate to 9600, no parity
      // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
      Serial1.end();
      delay(100);
      Serial1.begin(9600);  // Start bluetooth serial at 9600
      delay(15);
      Serial.println("Restarting BT");
      Serial1.println("R,1");
      delay(50);
      
      //configBluetoothModule();
      delay(15);

      // BT COMPLETE

}


//-----------------------------------------------------------

void loop()
{
  // compute delta time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) //every 0.5s program comes inside
  {
    // save the last time you blinked the LED_PIN
    previousMillis = currentMillis;
    if(Flag_Floating && !Flag_check)
    {
      timer_1s++;
      if(timer_1s>=1)
      {
        Serial.println("********Passed 1 sec");
        Flag_check=1;
        timer_1s=0;
      }
    }
     
  }
  
  // Keep reading from BT and send to Arduino Serial Monitor
 // while (Serial1.available())
 // {
    
    processIncomingByte(Serial1.read());
    //processButtonInput();
    //Serial.println(Serial1.read());
    Serial1.flush();   

}

//***BUTTON INPUT PROCESSING***//

void processButtonInput()
{
    if (digitalRead(SWITCH_CONFIG_PIN) == LOW) {
    delay(200);
    configBluetoothModule();
  }

}

//---------------------------------------------------------
// Packet Processing
//---------------------------------------------------------

void processIncomingByte(byte data)
{
  
  if (searchingHeader && data != 0xFF)
  {
#ifdef DEBUG_BT    
    Serial.println("XX");
#endif
    headerCount = 0;
    return;
  }
  
  if (searchingHeader && data == 0xFF)
  {
    if (headerCount == 0)
    {
#ifdef DEBUG_BT      
      Serial.print("H");
      delay(5000);
#endif      
    }
    headerCount++;
    if (headerCount == 2)
    {
#ifdef DEBUG_BT      
      Serial.print("h_");
#endif      
      searchingHeader = false;
      headerCount = 0;
      receivedContentSize = 0;
    }
  }
  else
  {
#ifdef DEBUG_BT   
    Serial.print("Bluetooth Data:"); 
    Serial.println(data);
#endif    
    
    packet[receivedContentSize++] = data;
    
    if (receivedContentSize == PACKET_CONTENT_SIZE)
    {
      int type = (int)packet[0];
      if (type == 0)
      {
        int xHigh = (packet[1] << 8) | packet[2];
        int xLow = (packet[3] << 8) | packet[4];
        int yHigh = (packet[5] << 8) | packet[6];
        int yLow = (packet[7] << 8) | packet[8];

//        processXYInput(xHigh, xLow, yHigh, yLow);
//        Serial.println((String)"xHigh = "+xHigh);   //Print the X and Y values received
//        Serial.println((String)"xLow = "+xLow);
//        Serial.println((String)"yHigh = "+yHigh);
//        Serial.println((String)"yLow = "+yLow);
//        Serial.println("------------------");
        processXYInput(xHigh, xLow, yHigh, yLow);
      }
      else
      {
        int value = (packet[1] << 8) | packet[2];

        if(!Arm)
        {
          
          array_val[i]=value;
          i++;
          if(i==array_size) i=0;
          for(j=0;j<array_size;j++)
          {
            sp_sum=sp_sum+array_val[j];
            //Serial.println((String)"-------------sp_Value = "+array_val[j] );    //Print the sip/puff value received
          }
          sp_avg=sp_sum/array_size;
          //Serial.println((String)"-------------sp_sum = "+sp_sum );    //Print the sip/puff value received
          sp_sum=0;
          value=(int)sp_avg;
          //Serial.println((String)"-------------Average int sip/puff = "+value );    //Print the sip/puff value received
        }
//        processSPInput(value);
//        Serial.println((String)"sip/puff = "+value);    //Print the sip/puff value received
//        Serial.println("******************"); 
          processPressureInput(value);
      }

      searchingHeader = true;
    }
  }
}

//---------------------------------------------------------
// Joystick XY Processing
//---------------------------------------------------------

void processXYInput(int xHigh, int xLow, int yHigh, int yLow)
{

// Process Roll
  if (xLow > xLowMin) //xLowMin=700
  {
//    Serial.println("Right");
    volt_roll = volt_roll + 0.03;
    if(volt_roll > maxV_stick)
    {
      volt_roll = maxV_stick;
    }
    Serial.println((String)"Roll Right = "+volt_roll);
  }
  else if (xHigh > xHighMin) //xLowMin=700
  {
//    Serial.println("Left");
    volt_roll = volt_roll - 0.03;
    if(volt_roll < minV_stick)
    {
      volt_roll = minV_stick;
    }
    Serial.println((String)"Roll Left = "+volt_roll);
  }
  else 
  {
    volt_roll = midV_stick;
//    Serial.println((String)"Roll neutral = "+volt_roll);
  }
  digitalPotWrite(ch_roll,volttostep(ch_roll,volt_roll));

// Process Pitch
  if (yHigh > yHighMin) //xLowMin=700
  {
//    Serial.println("Back");
    volt_pitch = volt_pitch - 0.04;
    if(volt_pitch < minV_stick)
    {
      volt_pitch = minV_stick;
    }
    Serial.println((String)"Pitch backward = "+volt_pitch);
  }
  else if (yLow > yLowMin) //xLowMin=700
  {
//    Serial.println("Forward");
    volt_pitch = volt_pitch + 0.04;
    if(volt_pitch > maxV_stick)
    {
      volt_pitch = maxV_stick;
    }
    Serial.println((String)"Pitch forward = "+volt_pitch);
  }
  else 
  {
    volt_pitch = midV_stick;
//    Serial.println((String)"Pitch neutral = "+volt_pitch);
  }
   digitalPotWrite(ch_pitch,volttostep(ch_pitch,volt_pitch));
 
#ifdef DEBUG_XY_INPUT
  Serial.print(xHigh);
  Serial.print(" ");
  Serial.print(xLow);
  Serial.print(", ");
  Serial.print(steering);
  Serial.print(" ");
  Serial.println(steeringOhms);

  Serial.print(yHigh);
  Serial.print(" ");
  Serial.println(yLow);
#endif
}

//---------------------------------------------------------
// Sip Puff Processing
//---------------------------------------------------------

void processPressureInput(int value)
{
  
  if(DEBUG_BT) {
    Serial.print("Pressure value :");
    Serial.println(value);    
  }

  if(Arm)
  {
    Serial.println((String)"SP Value = "+ value);
    if (Arm == 3 && value < puffHardMax)//70
    {
      volt_throttle = arm_V; 
      Serial.println((String)"puff Volt = "+ volt_throttle);
      digitalPotWrite(ch_throttle, volttostep(ch_throttle,volt_throttle));
      Arm = 2;
      Serial.println((String)"Arm value = "+ Arm);
      delay(250);
      
      
    } 
    else if (Arm == 2 && value > 850 && value < 3000)
    {
      
      volt_throttle = minV_stick;
      Serial.println((String)"sip Volt = "+ volt_throttle);
      Serial.println((String)"==================");
      Serial.println((String)"|   Arm complete   |");
      Serial.println((String)"==================");
      digitalPotWrite(ch_throttle, volttostep(ch_throttle,volt_throttle));
      digitalWrite(LED_PIN, HIGH);
      Arm = 1;
    }
    else if (Arm == 1 && value > 850 && value < 3000)
    {
      delay(500);
      digitalPotWrite(ch_throttle,volttostep(ch_throttle,minV_stick));
      digitalPotWrite(ch_yaw,volttostep(ch_yaw,minV_stick));
      digitalPotWrite(ch_pitch,volttostep(ch_pitch,minV_stick));
      digitalPotWrite(ch_roll,volttostep(ch_roll,minV_stick));
      delay(1000);
      digitalPotWrite(ch_throttle,volttostep(ch_throttle,minV_stick));
      digitalPotWrite(ch_yaw,volttostep(ch_yaw,midV_stick));
      digitalPotWrite(ch_pitch,volttostep(ch_pitch,midV_stick));
      digitalPotWrite(ch_roll,volttostep(ch_roll,midV_stick));
      Arm = 0;
      Serial.println((String)"=======================");
      Serial.println((String)"| Gyro Cali complete  |");
      Serial.println((String)"======================="); 
    }
  }
  else
  {

    if (value < puffHardMax) //puffHardMax = 70
    {
      if(volt_throttle<1)
      {
        volt_throttle = volt_throttle + 0.1;
      }
      else
      {
        volt_throttle = volt_throttle + 0.0175;
      }
      
      if (volt_throttle > maxV_stick)
      {
        volt_throttle = maxV_stick;
      } 
      Serial.println((String)"puffHard Volt = "+ volt_throttle);
      Flag_Floating=0;
      Flag_check=0;
    }
    else if (value <= puffMax) //puffMax = 410
    {
      if(volt_throttle<1)
      {
        volt_throttle = volt_throttle + 0.1;
      }
      else
      {
        volt_throttle = volt_throttle + 0.0175;
      }
      
      if (volt_throttle > maxV_stick)
      {
        volt_throttle = maxV_stick;
      }    
      Serial.println((String)"puff Volt = "+ volt_throttle);
      Flag_Floating=0;
      Flag_check=0;
    }
    else if (value > 410 && value <= 650) // puffMin = 650
    {
      if(volt_throttle<1)
      {
        volt_throttle = volt_throttle + 0.1;
      }
      else
      {
        volt_throttle = volt_throttle + 0.0125;
      }

      if (volt_throttle < minV_stick)
      {
        volt_throttle = minV_stick;
      }
      Serial.println((String)"puff Volt = "+ volt_throttle);
      Flag_Floating=0;
      Flag_check=0;
    }
    else if (value > 650 && value <= 2000) //sipMin
    {
      if(volt_throttle>FloatV)
      {
        volt_throttle = volt_throttle - 0.0175;
        if (!Flag_Floating && volt_throttle < FloatV)
        {
          Serial.println("_________________________________Floating Drone for 1sec");
          Flag_Floating=1;
          volt_throttle = FloatV;
        }
        
      }
      else
      {
        if(!Flag_Floating && !Flag_check)//Landing the drone which doesnt go above hovering/floating voltage before
        {
          volt_throttle = volt_throttle -0.02;
          if (volt_throttle < minV_stick)
          {
            volt_throttle = minV_stick;
            Flag_check=0;
            Flag_Floating=0;
          }
        }
        else if (Flag_Floating && !Flag_check)  //We want to wait 0.5 sec for hovering drone and keep the trottle voltage to FloatV
        {
          volt_throttle = volt_throttle -0.01;
          if(volt_throttle < FloatV)
          {
            Serial.println("__________________Waiting for 1sec");
            Flag_Floating=1;
            volt_throttle = FloatV;//keep the volt_throttle in the level of floating for 3 sec
          }
        }
        else if(Flag_Floating && Flag_check)//After 1sec the Flag_check will become 1 => drop the voltage of throttle to land the drone
        {
          volt_throttle = volt_throttle -0.02;
          if (volt_throttle < minV_stick)
          {
            volt_throttle = minV_stick;
            Flag_check=0;
            Flag_Floating=0;
          }
        }       
//        Serial.println((String)"sipAv1 Volt = "+ volt_throttle);
      }
    }
  digitalPotWrite(ch_throttle, volttostep(ch_throttle,volt_throttle));

#ifdef DEBUG_SP_INPUT
  Serial.print("sp ");
  Serial.println(value);
#endif

  }
}


void configBluetoothModule()
{
  Serial1.print("$$$");                        //Turn off the 60 sec timer for command mode
  delay(50);
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

// ============Matthew============ //
int volttostep(int ps, float voltage){
  float totalres;
  float r0;
  if (ps==ch_throttle){
    totalres = 9800;
    r0 = 98.5;
  }
  else if (ps==ch_yaw){
    totalres = 9700;
    r0 = 94;
  }
  else if (ps==ch_pitch){
    totalres = 10000; 
    r0 = 94;
  }
  else if (ps==ch_roll){
    totalres = 10000; 
    r0 = 94;     
  }

  return (totalres - 98.5 - (voltage/maxvolts)*(totalres + 5600 - r0))/(1-(voltage/maxvolts))*(256.0/totalres); 

}

int digitalPotWrite(int ps, int value)
{
  int portnum;

  if(ps==ch_throttle){
    portnum = portnum_throttle;
  }
  else if(ps==ch_yaw){
    portnum = portnum_yaw;
  }
  else if(ps==ch_pitch){
    portnum = portnum_pitch;
  }
  else if(ps==ch_roll){
    portnum = portnum_roll;
  }
digitalWrite(portnum, LOW);
SPI.transfer(address);
SPI.transfer(value);
digitalWrite(portnum, HIGH);
}
