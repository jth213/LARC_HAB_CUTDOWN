/* Description : Livonia Amateur Radio Club High Altitude Baloon Project - Cutdown Module               
   Hardware    : ET-GSM SIM900B + ET-MINI MT8870 + Arduino */

#define RELAY_PIN 7 // Connect relay 1 pin to digital pin 7
#define Q1_PIN   8  // Connect Data Q1 to digital pin 3
#define Q2_PIN   9  // Connect Data Q2 to digital pin 4
#define Q3_PIN   10 // Connect Data Q3 to digital pin 5
#define Q4_PIN   11 // Connect Data Q4 to digital pin 6
#define StD_PIN  12 // Connect StD to digital pin 2
#define LED_PIN  13 // Connect LED to digital pin 13

enum state { disarmed, armed, fire };
byte DTMF_value;
String seq="";
unsigned long timer;
int timerelapsed;
state currentstate;
String dtmfchar;

void setup()
{
 // Configures Serial Port @ 9600,8,N,1
 Serial.begin(9600);

 // Configure PIN mode
 pinMode(StD_PIN, INPUT);
 pinMode(Q1_PIN, INPUT);
 pinMode(Q2_PIN, INPUT);
 pinMode(Q3_PIN, INPUT);
 pinMode(Q4_PIN, INPUT);
 pinMode(RELAY_PIN, OUTPUT);
 pinMode(LED_PIN, OUTPUT);
 timer = 0;
 timerelapsed = 0;
 currentstate = disarmed;
 BlinkLED(500);
 digitalWrite(RELAY_PIN, HIGH);
 Serial.println("Listening for commands");
}

void loop()
{
  DTMF_value = GetDTMF(); // Reads DTMF code
  //if(DTMF_value < 255)
  //{
  //  Serial.println(DTMF_value);
  //}
      
  if (DTMF_value >= 1 && DTMF_value <= 12) 
  {
    
    switch (DTMF_value)
    {
     case 11:
      dtmfchar = "*";
     break;
    case 12:
       dtmfchar = "#";
      break;
     default:
      dtmfchar = (String)DTMF_value;
     break; 
    }
    //Serial.print(dtmfchar);
  seq += (String)dtmfchar;
    timerelapsed = 0;
    timer =  millis();
    //evaluate seq
    //Arm sequence    
    if (seq == "") // put the arm sequence between the ""
    {
      currentstate = armed;
      timerelapsed = 0;
      timer = 0;
      seq="";
      Serial.println();
      Serial.println("ARMED");
      digitalWrite(LED_PIN, HIGH);
    }
    //Disarm sequence
    if (seq == "") // put the disarm sequence between the ""
    {
      currentstate = disarmed;
      timerelapsed = 0;
      timer = 0;
      seq = "";
      Serial.println();
      Serial.println("DISARMED");
      digitalWrite(LED_PIN, LOW);
    }
    //Fire sequence
    if (seq == "") // put the fire sequence between the ""
    {
      if (currentstate == armed)
      {
        timerelapsed = 0;
        timer = 0;
        seq = "";
        Serial.println();
        Serial.println("FIRE");
        digitalWrite(RELAY_PIN, LOW);
        BlinkLED(100);
        delay(20000);
        digitalWrite(RELAY_PIN, HIGH);
        currentstate=disarmed;
        timerelapsed = 0;
        timer = 0;
        seq = "";
        Serial.println();
        Serial.println("DISARMED");
      }
    }
  } 
  else
  {    
    if (timer > 0)
    {
      timerelapsed = (millis() - timer)/1000;
      if (timerelapsed == 5)
      {
        seq = "";
        timer = 0;
        Serial.println();
        Serial.println("Time out");
      }
   } 
 }
}


void BlinkLED(int speed)
{
  for (int i = 0; i < 5; i++)
  {
     digitalWrite(LED_PIN, HIGH);
      delay(speed);
     digitalWrite(LED_PIN, LOW);
     delay(speed);
  } 
}

byte GetDTMF()
{
  static byte last_state = LOW; // initialization
  byte ret_val = 0xff;          // default - not valid

  if (digitalRead(StD_PIN)==HIGH
      && last_state == LOW) {
    // valid DTMF signal => decode it
    last_state = HIGH; // remember last state

    ret_val = 0;
    if (digitalRead(Q1_PIN)) ret_val |= 0x01;
    if (digitalRead(Q2_PIN)) ret_val |= 0x02;
    if (digitalRead(Q3_PIN)) ret_val |= 0x04;
    if (digitalRead(Q4_PIN)) ret_val |= 0x08;
  }
  else if (digitalRead(StD_PIN) == LOW) {
    // pulse is not valid => enable to check DTMF signal again
    last_state = LOW;
  }
  return (ret_val);
}

