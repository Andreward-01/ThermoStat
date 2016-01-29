//Include neccessary libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(7,8,9,10,11,12);      // initialize the lcd panels with the digital pins used



#define ONE_WIRE_BUS 6        
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);      //instantiate the objects needed to run the thermometer

/*define the buttons used for input
button 1 = lower desired temperature
button 2 = increase desired temperature
button 3 = switch between cool mode and heat mode
also, define the outputs for Relay 1 and 2
which control the air conditonier and heater respectively*/

#define button_1 2
#define button_2 3
#define button_3 4
#define Relay_1 A4
#define Relay_2 A5
#define cool 0
#define heat 1


int desired_Temp = 75;               //Default set temp is 75
unsigned long time_stamp = 0;        //The time_stamp is for the debouncing implementation seen in the interrupt code
int state = 0;                       // tells whether the thermostat is in heat mode or cool mode. Cool mode is 0, heat mode is 1

//Setup--------------------------------------------------
void setup()
{

  // set up the LCD's number of columns and rows:
  //as well as the column titles
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.setCursor(8,0);
  lcd.print("Goal:");
  
  
  Serial.begin(9600);
  sensors.begin();
 
  pinMode(button_1, INPUT_PULLUP);
  pinMode(button_2, INPUT_PULLUP);
  pinMode(button_3, INPUT_PULLUP);
  pinMode(Relay_1,OUTPUT);
  pinMode(Relay_2,OUTPUT);
  
  digitalWrite(Relay_1, HIGH);
  digitalWrite(Relay_2, HIGH);

  InitializeInterrupt();
} 
//Loop---------------------------------------------------
void loop()
{
  sensors.requestTemperatures(); 
  lcd.setCursor(0, 1);
  int temp = sensors.getTempFByIndex(0);
  lcd.print(temp);
  lcd.setCursor(8,1);
  lcd.print(desired_Temp);
  lcd.setCursor(15,1);
  lcd.print(state);
  
  
if (state == cool){
  digitalWrite(Relay_2,HIGH);
  if ((temp-1) > desired_Temp){
  //activate air conditioner, probably 
  digitalWrite(Relay_1, LOW);
  }
}

if (state == heat){
  digitalWrite(Relay_1,HIGH);
  if ((temp+1) < desired_Temp){
    //activate heater, probably 
    digitalWrite(Relay_2, LOW);
  }
}

if (temp == desired_Temp){
  // turn off heater or conditioner, probably 
  digitalWrite(Relay_1, HIGH);
  digitalWrite(Relay_2, HIGH);
}



} 
//Interrupt Setup------------------------------------


void InitializeInterrupt(){
  cli();                 // switch interrupts off while messing with their settings  
  PCICR = 0x04;          // Enable PCINT2 interrupt (digital 2-6 and 11-13)
  PCMSK2 = 0x1C;         // set the pins that trip the interrupt (00111000) in my case
  sei();                 // turn interrupts back on
}


//When buttons are pressed, pin values go to 0
ISR(PCINT2_vect) {
  uint8_t pins = ((PIND & 0x1C) >> 2);    //(Read all pins and apply a bit mask, and bit shift to the right two so we're left with just the bits we're interested in)
  if (millis() - time_stamp > 50){        //ensure that button bouncing doesn't cause accidental input (debouncing)
     if(pins == 0b110){                   //If button 1 is pressed decrease desired temp
      --desired_Temp;
      Serial.println(desired_Temp);
    }
    if (pins == 0b101){                  //If buton 2 is pressed, increase desired temp
      ++desired_Temp;
      Serial.println(desired_Temp);
    }
    if (pins == 0b011){                  //If button 3 is pressed, change the thermostat mode
        if (state == cool) state = heat;
        else state = cool; 
        Serial.println(state);
    }
  }
  time_stamp = millis();
}

