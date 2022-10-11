/*
 * This program is a property of SME Dehradun. 
 * Without permission, Use of this program for commercial purpose is strictly prohibited.
 * Contact us at schematicslab@gmail.com for any query. 
 * 
 * **********************
 * PH Sensor- 13
 * TDS Sensor- 34
 * DS18B20 Temperature Sensor- 14
 * Float Sensor- 12
 * Led - 5 // Red Led
 */


// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME           "Device"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#include "BlynkEdgent.h"

#define USE_WROVER_BOARD

#define PH_Pin 13          //pH meter Analog output to Arduino Analog Input 
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

int fl_Sensor=12;
int fval=0;
int Redled=5;
//BlynkTimer timer;
int f;  // for float value to string converstion
float val; // also works with double. 
char buff2[10];
String valueString = "";
String PH_Val = ""; 

int DSPIN = 14; // Dallas Temperature Sensor
int TDS_Sensor = 34;
float Aref = 3.3;

float ec_Val = 0;
unsigned int tds_value = 0;
float ecCal = 1;

WidgetLED ledglow(V5); // add virtual LED to V5

void float_Data()
{
 fval = digitalRead(fl_Sensor);  
 if (fval == LOW) 
  { 
    digitalWrite(Redled, HIGH);
    ledglow.on();
    Serial.println( "WATER LEVEL - HIGH"); 
  } 
  else 
  { 
    digitalWrite(Redled, LOW);
    ledglow.off();
    Serial.println( "WATER LEVEL - LOW" ); 
  } 
}
void TDS_Data()
{
//  double wTemp = TempRead()* 0.0625;  // conversion accuracy is 0.0625 / LSB
  double wTemp= 27;
  float V_level= Aref / 1024.0;
  float rawEc = analogRead(TDS_Sensor) * V_level;  // Raw  data of EC
  float T_Cof = 1.0 + 0.02 * (wTemp - 25.0);  // Temperature Coefficient
  
  ec_Val = (rawEc / T_Cof) * ecCal;// temperature and calibration compensation
  tds_value = (133.42 * pow(ec_Val, 3) - 255.86 * ec_Val * ec_Val + 857.39 * ec_Val) * 0.5; 
  double Far= (((wTemp *9)/5) + 32); // Temp in Far*
  
  Serial.print("TDS:"); Serial.println(tds_value);
  Serial.print("EC:"); Serial.println(ec_Val, 2);
  Serial.print("Temperature (oC):"); Serial.println(wTemp,2);   
  Serial.print("Temperature (oF):"); Serial.println(Far,2);
  Serial.print("");
 
  Blynk.virtualWrite(V1, wTemp);
  Blynk.virtualWrite(V2, Far);
  Blynk.virtualWrite(V3, tds_value);
  Blynk.virtualWrite(V4, ec_Val);
}
void PH_Data()
{
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(PH_Pin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
  avgValue+=buf[i];
  float phValue=(float)avgValue*3.3/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  
  PH_Val =  dtostrf(phValue, 4, 2, buff2);  //4 is mininum width, 6 is precision
  Serial.print(PH_Val);
  valueString = "";
  delay(1000);
  Blynk.virtualWrite(V0, PH_Val);
}
void setup()
{
  Serial.begin(9600);
  delay(100);
  pinMode(fl_Sensor, INPUT_PULLUP); // Float Sensor
  pinMode(Redled, OUTPUT); // Red LED

  BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();
  PH_Data();
  TDS_Data();
  float_Data();
  delay(500);
}
