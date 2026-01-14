#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

String nomi[] = {"Davide", "Donato", "Nicolas", "Sveva", "Lidia", "Claudia", "Federico", "Lorenzo", "Francesco"};
String complimenti[] = {"sei forte!", "grande!", "ottimo lavoro!", "ben fatto!", "continua cosi!", "wow!", "super!", "sei un mito!", "top! :)"};

int num_nomi = sizeof(nomi) / sizeof(nomi[0]);
int num_compl = sizeof(complimenti) / sizeof(complimenti[0]);

int i = 0;
String old_name = "";
String old_compl = "";

void setup() {
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0,0);
    lcd.print("Hello world!");
    delay(700);

    lcd.setCursor(1,1);
    lcd.print("wow! Funziona!!");
    delay(1000);

    lcd.clear();

    // Inizializza il generatore di numeri casuali
    randomSeed(analogRead(0));
}

void loop() {

  if (i >= num_nomi) {
    i = 0;
  }

  // ========== Riga 0: saluto ==========
  lcd.setCursor(0,0);
  lcd.print("Ciao ");

  // pulizia mirata del nome precedente
  lcd.setCursor(5,0);
  for (int k = 0; k < old_name.length(); k++) {
      lcd.print(" ");
  }

  // Scrivi il nome nuovo
  lcd.setCursor(5,0);
  lcd.print(nomi[i]);
  old_name = nomi[i];

  // ========== Riga 1: complimento casuale ==========
  int idx = random(num_compl);

  lcd.setCursor(0,1);
  for (int k = 0; k < old_compl.length(); k++) {
      lcd.print(" ");
  }

  lcd.setCursor(0,1);
  lcd.print(complimenti[idx]);
  old_compl = complimenti[idx];

  i++;
  delay(1200);
}
