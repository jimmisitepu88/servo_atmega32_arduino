#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {1, 2, 3},
  {4, 5, 6},
  {7, 8, 9},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {24, 25, 26, 27};
byte colPins[COLS] = {28, 29, 30};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
#define BINTANG 42 // nilai BINTANG
#define PAGAR 35 // nilai pagar
long dtKey = 0;
int key;
String line1 = "";
String line2 = "";


#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define rel_pompa 2
#define rel_peltier 4
#define rel_heater 5

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp_air;
float temp_tubuh;
int state = 0;

#define STOP 0
#define RUN 1

float set_sudut = 0;
float ref_sudut = 0;

void setup() {

  delay(100);
  pinMode(rel_pompa, OUTPUT);
  pinMode(rel_peltier, OUTPUT);
  pinMode(rel_heater, OUTPUT);

  Serial.begin(115200);
  sensors.begin();
  lcd.init();
  lcd.backlight();
  init_servo();
  OCR1A = 65 + (0/4.35); // sudut 0 derajat
  Serial.println("system mulai");
}

void loop() {
  key = keypad.getKey();
  if (key) {
    baca_key(); lcd.clear(); delay(20);
  }
  update_lcd();
  control();
}

void baca_key() {
  switch (state) {
    case STOP:
      if ( key == 48 )key = 0;
      if ( key < 10) {
        dtKey = dtKey * 10 + key;
        if(dtKey >= 180)dtKey = 180;
        set_sudut = dtKey;
      }
      if (key == BINTANG) {
        dtKey = 0;
        set_sudut = 0;
      }
      if (key == PAGAR) {
        state = RUN;
      }
      break;
    case RUN:
      if (key == BINTANG) {
        state = STOP;
      }
      break;
  }
}

void get_text() {
  switch (state) {
    case STOP:
      line1 = "Set Sudut";
      line2 = "Sudut: " + String(set_sudut,0);
      break;
    case RUN:
      line1 = "Running...";
      line2 = "Sudut: " + String(set_sudut,0);
      break;
  }
}

void update_lcd() {
  get_text();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void control(){
  switch(state){
    case STOP:
      OCR1A = 65 + (0/4.35); // sudut 0 derajat
      //Serial.println("coba stop");
    break;

    case RUN:
      ref_sudut = set_sudut/180*2100;
      OCR1A = 65 + (ref_sudut/4.35);
      Serial.print("coba run: ");
      Serial.println(ref_sudut);
    break;
  }
}

void init_servo(){
  DDRD |= (1 << PD5); /* Make OC1A pin as output */
  TCNT1 = 0;    /* Set timer1 count zero */
  ICR1 = 2499;    /* Set TOP count for timer1 in ICR1 register */

  /* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
  TCCR1A = (1 << WGM11) | (1 << COM1A1);
  TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10) | (1 << CS11);
}
