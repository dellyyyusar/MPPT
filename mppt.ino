#include <LCD.h>
#include <Wire.h>
#include <PWM.h>
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7);
float vout1 = 0.0;
float vout2 = 0.0;    
float vin1 = 0.0;
float vin2 = 0.0;
float R1 = 9870.0;  //10k 
float R2 = 987.0;  //1k
float R12 = 9990.0;  //10k 
float R22 = 999.0;  //1k
int value1 = 0;
int value2 = 0;

int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module
int RawValue1= 0;
int ACSoffset = 2500; 
double Voltage1 = 0;
double iin1 = 0;

int RawValue2= 0;
double Voltage2 = 0;
double iin2 = 0;

const int Vin_min = 5;    //vin minimal 6volt

const int Iin_max = 3.3;    // maksimal arus masukan 3.3A

const int Iout_max = 4.0;    //maksimal arus keluaran 4A

const int avgNum = 16;        // penjumlahan adc

int Vout_max = 13.8;    // vout maksimal

int Vout_trickle = 13.2; // var starting

int currentPrev = 0;    // Var arus terbaca sebelumnya
int currentDiff = 0;    // var perbedaan arus

int dutyCycle = 0;    // var start
int state = 0;        // variable to store state machine
            // State '0' is start-up

int led = 11;
int32_t frequency = 40000; // frekuensi switching

void setup() {
 
  Serial.begin(9600);
    lcd.begin (20,4); // LCD 16x2
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);

  InitTimersSafe();

  bool success = SetPinFrequencySafe(led, frequency);

  //jika suskses pin 13 hidup
  if (success) {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
  }
 
}

void loop() {
 
 // float average1 = 0;
  //float average2 = 0;
  //float iin1 = 0;
  //float iin2 = 0;
 // for(int i = 0; i < 1000; i++) {
  // average1  = average1 + (.044 * analogRead(A2) -3.78);
  //average2  = average2 + (.044 * analogRead(A4) -3.78);
  //5A mode, if 20A or 30A mode, need to modify this formula to 
    //(.19 * analogRead(A0) -25) for 20A mode and 
    //(.044 * analogRead(A0) -3.78) for 30A mode
  
    //delay(1);
 // }
  //for(int ii = 0; ii < 1000; ii++) {
   // average2  = average2 + (.044 * analogRead(A4) -3.78) / 1000;
  
  //5A mode, if 20A or 30A mode, need to modify this formula to 
    //(.19 * analogRead(A0) -25) for 20A mode and 
    //(.044 * analogRead(A0) -3.78) for 30A mode
  
    //delay(1);
  //}

 RawValue1 = analogRead(A2);
 Voltage1 = (RawValue1 / 1024.0) * 5000; // Gets you mV
 iin1 = ((Voltage1 - ACSoffset) / mVperAmp);

 
 RawValue2 = analogRead(A4);
 Voltage2 = (RawValue2 / 1024.0) * 5000; // Gets you mV
 iin2 = ((Voltage2 - ACSoffset) / mVperAmp);
  
  value1 = analogRead(A1);
  vout1 = (value1 * 5) / 1024.0;
  vin1 = vout1 / (R2 / (R1 + R2));

  value2 = analogRead(A3);
  vout2 = (value2 * 5) / 1024.0;
  vin2 = vout2 / (R22 / (R12 + R22));

  if ((vin1 >= Vin_min) && (state == 0))        
                
  {
    
    state = 1;           
  }

   currentDiff = iin2 - currentPrev;  


  if ((vin2 >= Vout_max) || (vin1 <= Vin_min))
  {
    dutyCycle = 0;        
               
  }
  else if (vin2 >= Vout_trickle)
  {
    dutyCycle = dutyCycle - 8;  
               
  }
  else if ((currentDiff < 0) || (iin2 > Iout_max))
  {
    dutyCycle = dutyCycle - 2;  
                
  }
  else
  {
    dutyCycle = dutyCycle;    
  }

  if (dutyCycle < 1)
  {
    dutyCycle = 0;     
  }
  else if (dutyCycle > 200)
  {
    dutyCycle = 200;   
  }

  analogWrite(led, dutyCycle);
  delay(100);
 RawValue1 = analogRead(A2);
 Voltage1 = (RawValue1 / 1024.0) * 5000; // Gets you mV
 iin1 = ((Voltage1 - ACSoffset) / mVperAmp);

 
 RawValue2 = analogRead(A4);
 Voltage2 = (RawValue2 / 1024.0) * 5000; // Gets you mV
 iin2 = ((Voltage2 - ACSoffset) / mVperAmp);
  
  value1 = analogRead(A1);
  vout1 = (value1 * 5) / 1024.0;
  vin1 = vout1 / (R2 / (R1 + R2));

  value2 = analogRead(A3);
  vout2 = (value2 * 5) / 1024.0;
  vin2 = vout2 / (R22 / (R12 + R22));
  
  currentPrev = iin2;
  dutyCycle = dutyCycle + 1;

  if ((vin2 >= Vout_max) || (vin1 <= Vin_min))
  {
    dutyCycle = 0;        
                
  }
  else
  {
    dutyCycle = dutyCycle;   
  }
  analogWrite(led, dutyCycle);
  delay(100);

  lcd.home();
  lcd.print("VIn=");
  lcd.setCursor(4,0);
  lcd.print(vin1);
  lcd.setCursor(10,0);
  lcd.print("VO=");
  lcd.setCursor(14,0);
  lcd.print(vin2);
  lcd.setCursor(0,1);
  lcd.print("Iin= ");
  lcd.setCursor(4,1);
  lcd.print(iin1);
  lcd.setCursor(10,1);
  lcd.print("IO= ");
  lcd.setCursor(14,1);
  lcd.print(iin2);
  lcd.setCursor(0,2);
  lcd.print("Duty Cycle=");
  lcd.setCursor(14,2);
  lcd.print(dutyCycle);
  lcd.setCursor(0,3);
  lcd.print("Freq=");
  lcd.setCursor(14,3);
  lcd.print(frequency);
  Serial.println(vin1);
  Serial.println(vin2);
  Serial.println(iin1);
  Serial.println(iin2);
  Serial.println(dutyCycle);
  //Serial.println(average1);
  //Serial.println(average2);





}
