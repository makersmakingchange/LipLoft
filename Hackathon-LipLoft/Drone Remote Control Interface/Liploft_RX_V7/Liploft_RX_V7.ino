//Developed by : MakersMakingChange + UBC hackathon team
//Firmware : Liploft_Firmware
//VERSION: 1.7 (11 Oct 2019) 
//HOW TO CONTROL THE DRONE: "arm" by puffing and siping once; sip/puff controls the throttle; LipSync joystick controls pitch and roll (working well); gryo calibration automated
//IMRPVOEMENTS: need to tune throttle sensitivity by manupulating the time-averaging array size as well as the step number increase/decrease

#include <MCP4261.h>
#include <SoftwareSerial.h>
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
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 500;           // interval at which to blink (milliseconds)

// #define DEBUG_BT
// #define DEBUG_XY_INPUT
// #define DEBUG_SP_INPUT
#define DEBUG_DP                  // DP = digital potentiometer
// #define DEBUG_STATES

#define PACKET_CONTENT_SIZE 9  //Content size for raw bluetooth packets
SoftwareSerial BTSerial(2, 3); // RX | TX

// Then choose any other free pin as the Slave Select (pin 10 if the default but doesnt have to be)
#define MCP4261_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select               -> CS  (Pin 01 on MCP4261 DIP)

// Its recommended to measure the rated end-end resistance (terminal A to terminal B)
// Because this can vary by a large margin, up to -+ 20%. And temperature variations.
float rAB_ohms = 5090.00; // 5k Ohm

// Instantiate Mcp4261 object, with default rW (=117.5 ohm, its typical resistance)
MCP4261 Mcp4261 = MCP4261( MCP4261_SLAVE_SELECT_PIN, rAB_ohms );

//float initThrottleOhms = 2340.0;
//float initSteeringOhms = 2200.0;

//float steeringOhms = initThrottleOhms;
//float throttleOhms = initSteeringOhms;

//===== Set up MCP4161 variables =======//
byte address = 0x00;
int CS1 = 10;
int CS2 = 9;
int CS3 = 8;
int CS4 = 7;
int LED = 5;
bool flag__first = 0;
int port;
float voltsin;
long stepnum;
int  timer_1s;
bool Flag_check;
bool Flag_Floating;


void setup()
{
  Serial.begin(9600);
  pinMode (CS1, OUTPUT);
  pinMode (CS2, OUTPUT);
  pinMode (CS3, OUTPUT);
  pinMode (CS4, OUTPUT); 
  pinMode (LED, OUTPUT);
  SPI.begin();

  // set the MCP to sned the desired voltage to each joystick potentiometer in the RC transmitter circuit
  digitalPotWrite(ch_throttle,volttostep(ch_throttle,minV_stick));
  digitalPotWrite(ch_yaw,volttostep(ch_yaw,midV_stick));
  digitalPotWrite(ch_pitch,volttostep(ch_pitch,midV_stick));
  digitalPotWrite(ch_roll,volttostep(ch_roll,midV_stick));
    
  //SETUP BT

      Serial.println("Setting up BT Connection");
      BTSerial.begin(115200);  // The Bluetooth Mate defaults to 115200bps
      
      BTSerial.print("$");  // Print three times individually
      BTSerial.print("$");
      BTSerial.print("$");  // Enter command mode
      delay(100);  // Short delay, wait for the Mate to send back CMD
      BTSerial.println("SU,9600");  // Temporarily Change the baudrate to 9600, no parity
      // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
      BTSerial.end();
      delay(100);
      BTSerial.begin(9600);  // Start bluetooth serial at 9600
      delay(15);
      Serial.println("Restarting BT");
      BTSerial.println("R,1");
//      SetupBTDevice();
      delay(15);

  // BT COMPLETE

//  // SETUP DP - BEGIN
//  throttleOhms = initThrottleOhms ;
//  steeringOhms = initSteeringOhms ;
//  Mcp4261.wiper0(throttleOhms);
//  Mcp4261.wiper1(steeringOhms);
//  // SETUP DP - END

//  setup_throttleOhms();
}

// steering resistance range
//float minSteeringOhms = 1310.0;  // 800
//float maxSteeringOhms = 3750.0; // 4450.0;
//float steeringOhmsRange = maxSteeringOhms - minSteeringOhms;

// Note: wiper = W_to_B = (rAB_ohms - W_to_A)

// dead-zone 2180-2560 ohms
//float minThrottleOhms = 2200; // 1250;
//float maxThrottleOhms = 2520; // 3420;

// resistance difference between minimum and maximum throttle
//float throttleRangeOhms = 200.0;

// forward throttle resistance range
//float fwdThrottleRangeOhms = -1 * throttleRangeOhms;
//float fwdThrottleMinOhms = minThrottleOhms - 10.0;
//float fwdThrottleMaxOhms = fwdThrottleMinOhms + fwdThrottleRangeOhms;

// reverse throttle resistance range
//float revThrottleRangeOhms = throttleRangeOhms;
//float revThrottleMinOhms = maxThrottleOhms;
//float revThrottleMaxOhms = revThrottleMinOhms + revThrottleRangeOhms;

// full throttle resistance
//float fullThrottleOhms = fwdThrottleMaxOhms - 1000;
//bool fullThrottleOn = false;
//float preFulLThrottleOhms = initThrottleOhms;

// stepped throttle control
#define THROTTLE_STEPS 8
//float fwdThrottleOhms[THROTTLE_STEPS];
//float revThrottleOhms[THROTTLE_STEPS];
//float throttleNonLinearPower = 2.2;
//int fwdThrottleStep = 0;
//int revThrottleStep = 0;

// states for debug logging
//int count = 0;
//int reportCount = 10;

// states for automatic testing
//int autoTest = 0;
//float dt = 0.0;
//int lastMillis = 0;
//float clipFactor = 2.0;
//float ooThrottlePeriod = 1.0 / 6000.0;
//float ooSteeringPeriod = 1.0 / 2000.0;

//-----------------------------------------------------------

//class DebouncedBool
//{
//private:
//  unsigned long m_debounceTime;
//  unsigned long m_lastStateUpdateTime;
//  bool m_state;
//  bool m_lastUpdateState;
//
//public:
//  DebouncedBool(bool state, unsigned long debounceTime)
//  {
//    m_state = state;
//    m_lastUpdateState = state;
//
//    m_debounceTime = debounceTime;
//    m_lastStateUpdateTime = 0;
//  }
//
//  void Update(bool state)
//  {
//    if (state != m_lastUpdateState)
//    {
//      m_lastUpdateState = state;
//      m_lastStateUpdateTime = millis();
//    }
//    else
//    {
//      if ((m_state != state) && ((millis() - m_lastStateUpdateTime) > m_debounceTime))
//      {
//        m_state = state;        
//      }
//    }
//  }
//
//  bool State()
//  {
//    return m_state;
//  }
//};

//-----------------------------------------------------------

//
//void setup_throttleOhms()
//{
//  for (int i=0; i<THROTTLE_STEPS; ++i)
//  {
//    fwdThrottleOhms[i] = fwdThrottleMinOhms + (pow((float)(i + 1.0)/THROTTLE_STEPS, throttleNonLinearPower) * fwdThrottleRangeOhms);
//  }
//
//  for (int i=0; i<THROTTLE_STEPS; ++i)
//  {
//    revThrottleOhms[i] = revThrottleMinOhms + (pow((float)(i + 1.0)/THROTTLE_STEPS, throttleNonLinearPower) * revThrottleRangeOhms);
//  }
//}

//void print_throttleOhms()
//{
//  for (int i=0; i<THROTTLE_STEPS; ++i)
//  {
//    Serial.print("fwdThrottleOhms[");
//    Serial.print(i);
//    Serial.print("] = ");
//    Serial.println(fwdThrottleOhms[i]);
//  }
//
//  for (int i=0; i<THROTTLE_STEPS; ++i)
//  {
//    Serial.print("revThrottleOhms[");
//    Serial.print(i);
//    Serial.print("] = ");
//    Serial.println(revThrottleOhms[i]);
//  }
//}

void loop()
{
  // compute delta time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) //every 0.5s program comes inside
  {
    // save the last time you blinked the LED
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
  while (BTSerial.available())
  {
    processIncomingByte(BTSerial.read());
    BTSerial.flush();   
  }

 /*
  // Keep reading from BT and send to Arduino Serial Monitor
  if (BTSerial.available())
  {
    //Serial.println("--Received--");
    Serial.print((char)BTSerial.read());
  }
  // Keep reading from Arduino Serial Monitor and send to BT
  if (Serial.available())
  {
    BTSerial.write((char)Serial.read());
    Serial.println("--sent--");
  }   
*/

//  processCommands();
//  mockProcessSP();
//  
//  if (autoTest)
//  {
//    updateAutoTest();
//  }
//    
//  Mcp4261.wiper0(throttleOhms);
//  Mcp4261.wiper1(steeringOhms);

//#ifdef DEBUG_DP
//  if (count > reportCount)
//  {
//    count = 0;
//    Serial.print(steeringOhms);
//    Serial.print(" ");
//    Serial.print(throttleOhms);
//    Serial.print(" ");
//    Serial.print( inThrottleDeadZone() ? "STP" : ((throttleOhms < initThrottleOhms) ? "FWD" : "REV"));
//    Serial.print(" ");
//    Serial.print(fwdThrottleStep);
//    Serial.print(", ");
//    Serial.print(revThrottleStep);
//    Serial.print(", ");
//    Serial.print(normAbsThrottle());
//    Serial.print(", ");
//    Serial.print(throttleOhms);
//    Serial.print(" ");
//    Serial.println( inSteeringDeadZone() ? "." : ((steeringOhms < initSteeringOhms) ? ">" : "<"));
//  }
//  count = count + 1;
//#endif
}

//---------------------------------------------------------
// Testing
//---------------------------------------------------------

//void toggleAutoTest()
//{
//  autoTest = !autoTest;
//  if (autoTest)
//  {
//    Serial.println("Enable autotest");
//  }
//  else
//  {
//    Serial.println("Disble autotest");
//    throttleOhms = initThrottleOhms ;
//    steeringOhms = initSteeringOhms ;
//  }
//}
//
//void updateAutoTest()
//{
//  steeringOhms = minSteeringOhms + 
//    (maxSteeringOhms - minSteeringOhms) * 0.5 * (sin(2 * PI * millis() * ooSteeringPeriod) + 1); 
//
//  float clippedSin = min(1.0, max(-1.0, sin(2 * PI * millis() * ooThrottlePeriod) * clipFactor));
//
//  throttleOhms = minThrottleOhms + 
//    (maxThrottleOhms - minThrottleOhms) * 0.5 * (clippedSin + 1); 
//}
//
//void processCommands()
//{
//  if (Serial.available())
//  {
//    int cmd = Serial.read();
//    if (cmd == 'a')
//    {
//       toggleAutoTest();     
//    }
//    else if (cmd == 'g')
//    {
//       Serial.println("throttle -1");
//       throttleOhms -= 1;  
//    }
//    else if (cmd == 'f')
//    {
//       Serial.println("throttle -10");       
//       throttleOhms -= 10;
//    }
//    else if (cmd == 'd')
//    {
//       Serial.println("throttle -100");       
//       throttleOhms -= 100;
//    }
//    else if (cmd == 'h')
//    {
//       Serial.println("throttle +1");       
//       throttleOhms += 1;
//    }    
//    else if (cmd == 'j')
//    {
//       Serial.println("throttle +10");       
//       throttleOhms += 10;
//    }
//    else if (cmd == 'k')
//    {
//       Serial.println("throttle +100");       
//       throttleOhms += 100;
//    }
//    else if (cmd == 'r')
//    {
//       Serial.println("reset");       
//       throttleOhms = initThrottleOhms;
//       steeringOhms = initSteeringOhms;
//    }
//    else if (cmd == 'q')
//    {
//       Serial.println("steering -= 10");       
//       steeringOhms -= 10;
//    }
//    else if (cmd == 'w')
//    {
//       Serial.println("steering += 10");       
//       steeringOhms += 10;
//    }
//    else if (cmd == '[')
//    {
//      processMockSip();
//    }
//    else if (cmd == ']')
//    {
//      processMockPuff();
//    }    
//    else if (cmd == '\\')
//    {
//      processMockPuffHard();
//    }    
//    else if (cmd == 'p')
//    {
//      processMockSPNeutral();
//    }
//    else if (cmd == '=')
//    {
//      print_throttleOhms();
//    }
//  }
//}

//---------------------------------------------------------
// Packet Processing
//---------------------------------------------------------

byte packet[PACKET_CONTENT_SIZE];
bool searchingHeader = true;
int receivedContentSize = 0;
int headerCount = 0;
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
        my_processXYInput(xHigh, xLow, yHigh, yLow);
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
          my_processSPInput(value);
      }

      searchingHeader = true;
    }
  }
}

//---------------------------------------------------------
// Joystick XY Processing
//---------------------------------------------------------

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
//
//int yHighStopMin = 800;
//int yLowFullThrottleMin = 775;
//int yLowFullThrottleMax = 650;

//void processXYInput(int xHigh, int xLow, int yHigh, int yLow)
//{
//  // convert XLow and xHigh inputs to steering
//  float steering = 0.0;
//  if (xLow > xLowMin)
//  {
//     // right
//     steering = -1.0 * min(1.0, (xLow - xLowMin) * ooXLowRange);
//  }
//  else if (xHigh > xHighMin)
//  {
//    // left
//    steering = min(1.0, (xHigh - xHighMin) * ooXHighRange);
//  }
//
//  float normSteering = 0.5 * (steering + 1.0);
//  steeringOhms = NormSteeringToOhms(normSteering);
//
//  // 
//  if (yHigh > yHighStopMin)
//  {
//    processStop();
//  }
//
//#if 0
//  if (yLow > yLowFullThrottleMin)
//  {
//    processFullThrottle();
//  }
//  else if (yLow < yLowFullThrottleMax)
//  {
//    resetFullThrottle();
//  }
//#endif
// 
//#ifdef DEBUG_XY_INPUT
//  Serial.print(xHigh);
//  Serial.print(" ");
//  Serial.print(xLow);
//  Serial.print(", ");
//  Serial.print(steering);
//  Serial.print(" ");
//  Serial.println(steeringOhms);
//
//  Serial.print(yHigh);
//  Serial.print(" ");
//  Serial.println(yLow);
//#endif
//}

void my_processXYInput(int xHigh, int xLow, int yHigh, int yLow)
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

int puffHardMax = 70;
int puffMax = 410;
int sipMin = 650;

//bool mockPuffHard = false;
//bool mockPuff = false;
//bool mockSip = false;
//bool mockSPNeutral = false;
//
//void processMockPuffHard()
//{
//  Serial.println("processMockPuffHard");
//  resetMockStates();
//  mockPuffHard = true;  
//}
//
//void processMockPuff()
//{
//  Serial.println("processMockPuff");
//  resetMockStates();
//  mockPuff = true;
//}

//void processMockSip()
//{
//  Serial.println("processMockSip");
//  resetMockStates();
//  mockSip = true;
//}
//
//void processMockSPNeutral()
//{
//  Serial.println("processMockSPNeutral");
//  resetMockStates();
//  mockSPNeutral = true;
//}

//void resetMockStates()
//{
//  mockPuffHard = false;
//  mockPuff = false;
//  mockSip = false;
//  mockSPNeutral = false;
//}
//
//void mockProcessSP()
//{
//  if (mockPuffHard)
//  {
//    processPuffHard();
//  }
//  else if (mockPuff)
//  {
//    processPuff();
//  }
//  else if (mockSip)
//  {
//    processSip();
//  }
//  else if (mockSPNeutral)
//  {
//    processSPNeutral();
//  }
//}

//void processSPInput(int value)
//{
//  // puff
//  if (value < puffHardMax)
//  {
//    processPuffHard();
//  }
//  else if (value < puffMax)
//  {
//    processPuff();
//  }
//  else if (value > sipMin)
//  {
//    processSip();
//  }
//  else
//  {
//    processSPNeutral();
//  }
//
//#ifdef DEBUG_SP_INPUT
//  Serial.print("sp ");
//  Serial.println(value);
//#endif
//}

void my_processSPInput(int value)
{
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
      digitalWrite(LED, HIGH);
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


//bool lastPuffState = false;
//bool lastSipState = false;
//
//DebouncedBool debouncedPuff = DebouncedBool(lastPuffState, 1);
//DebouncedBool debouncedSip = DebouncedBool(lastSipState, 1);
//
//void processSipPuffSteps()
//{
//  // off->on edge trigger
//
//  bool puffState = debouncedPuff.State();
//  bool sipState = debouncedSip.State();
//
//#if 0 // def DEBUG_STATES    
//  Serial.print("puff: ");
//  Serial.print(lastPuffState);
//  Serial.print(" -> ");
//  Serial.print(puffState);  
//
//  Serial.print(", ");
//  
//  Serial.print("sip: ");
//  Serial.print(lastSipState);
//  Serial.print(" -> ");
//  Serial.println(sipState);  
//#endif
//  
//  if (puffState && !lastPuffState)
//  {
//    processThrottleStepUp();
//  }
//  lastPuffState = puffState;
//
//  if (sipState && !lastSipState)
//  {
//    processThrottleStepDown();
//  }
//  lastSipState = sipState;
//}
//
//void processThrottleStepUp()
//{
//  if (fwdThrottleStep > revThrottleStep)
//  {
//    fwdThrottleStep = min(THROTTLE_STEPS, fwdThrottleStep + 1);    
//  }
//  else if (revThrottleStep > fwdThrottleStep)
//  {
//    revThrottleStep = max(0, revThrottleStep - 1);
//  }
//  else
//  {
//    fwdThrottleStep = 1;
//    revThrottleStep = 0;
//  }
//
//  updateThrottleOhms();
//}
//
//void processThrottleStepDown()
//{
//  if (revThrottleStep > fwdThrottleStep)
//  {
//    revThrottleStep = min(THROTTLE_STEPS, revThrottleStep + 1);    
//  }
//  else if (fwdThrottleStep > revThrottleStep)
//  {
//    fwdThrottleStep = max(0, fwdThrottleStep - 1);
//  }
//  else
//  {
//    revThrottleStep = 1;
//    fwdThrottleStep = 0;
//  }
//
//  updateThrottleOhms();
//}
//
//void updateThrottleOhms()
//{
//  if (fwdThrottleStep > 0)
//  {
//    throttleOhms = fwdThrottleOhms[fwdThrottleStep-1];
//  }
//  else if (revThrottleStep > 0)
//  {
//    throttleOhms = revThrottleOhms[revThrottleStep-1];
//  }
//  else
//  {
//    throttleOhms = initThrottleOhms;
//  }
//}
//
//void processPuff()
//{
//#ifdef DEBUG_STATES    
//  Serial.println("puff");
//#endif
//
//  resetFullThrottle();
//
//  debouncedPuff.Update(true);
//  debouncedSip.Update(false);  
//  processSipPuffSteps();
//}
//
//void processPuffHard()
//{
//#ifdef DEBUG_STATES    
//  Serial.println("puff hard");
//#endif 
//
//  processFullThrottle();
//  debouncedPuff.Update(true);
//  debouncedSip.Update(false);
//}
//
//void processSip()
//{
//#ifdef DEBUG_STATES    
//  Serial.println("sip");
//#endif  
//
//  resetFullThrottle();
//
//  debouncedSip.Update(true);
//  debouncedPuff.Update(false);
//  processSipPuffSteps();
//}
//
//void processSPNeutral()
//{
//  resetFullThrottle();
//    
//  debouncedPuff.Update(false);
//  debouncedSip.Update(false);
//  processSipPuffSteps();  
//}
//
//void processStop()
//{
//#ifdef DEBUG_STATES    
//  Serial.println("--stop--");
//#endif  
//
//  throttleOhms = initThrottleOhms;
//  fwdThrottleStep = 0;
//  revThrottleStep = 0;
//}
//
//void processFullThrottle()
//{
//#ifdef DEBUG_STATES    
//  Serial.println("!!full throttle!!");
//#endif 
//
//  if (!fullThrottleOn)
//  {
//    preFulLThrottleOhms = throttleOhms;
//    throttleOhms = fullThrottleOhms;
//
//    fullThrottleOn = true;
//  }
//}
//
//void resetFullThrottle()
//{
//  if (fullThrottleOn)
//  {    
//    throttleOhms = preFulLThrottleOhms;
//    fullThrottleOn = false; 
//  }
//}
//
//bool inThrottleDeadZone()
//{
//  return ((throttleOhms > minThrottleOhms) && (throttleOhms < maxThrottleOhms));
//}
//
//bool isThrottleRev()
//{
//  return (throttleOhms >= revThrottleMinOhms);
//}
//
//bool isThrottleFwd()
//{
//  return (throttleOhms <= fwdThrottleMinOhms);
//}
//
//float normAbsThrottle()
//{
//  if (isThrottleFwd())
//  {
//    return (throttleOhms - fwdThrottleMinOhms) / fwdThrottleRangeOhms;
//  }
//  else if (isThrottleRev())
//  {
//    return (throttleOhms - revThrottleMinOhms) / revThrottleRangeOhms;
//  }
//  return 0.0;
//}

//int steeringDeadZoneOhms = 500;
//
//bool inSteeringDeadZone()
//{
//  return (abs(steeringOhms - initSteeringOhms) < steeringDeadZoneOhms);
//}
//
//float NormSteeringToOhms(float normSteering)
//{
//  return minSteeringOhms  + (normSteering * steeringOhmsRange);
//}

void SetupBTDevice()
{
  BTSerial.println("ST,255");                        //Turn off the 60 sec timer for command mode
  delay(15);
  BTSerial.println("SA,2");                          //Set Authentication Value to 2
  delay(15);
  BTSerial.println("SX,0");                          //Set Bonding to 0 or disabled
  delay(15);
  BTSerial.println("SN,MOTORMOUTH");                  //Set the name of BT module
  delay(15);
  BTSerial.println("SM,3");                          //Set the Pairing mode to Master - Auto
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
