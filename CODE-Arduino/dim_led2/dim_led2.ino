#define LDR_PIN A0
#define POT_PIN A1
#define LED_PIN_Y 9
#define LED_PIN_R 10
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN_Y, OUTPUT);
  pinMode(LED_PIN_R, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // qui misuro l'intensità della fotoresistenza
  float lum = misuraLuminosita();

  analogWrite(LED_PIN_Y, 255-lum);

  Serial.print("Fotoresistenza: ");
  Serial.print(lum);
  delay(50);

  // qui inserisco il potenziometro
  int sensorValue = analogRead(POT_PIN);
	// Converte la lettura (range da 0 a 1023) in voltaggi (0 - 5V):
	float voltage = sensorValue * (5.0 / 1023.0);
  Serial.print(" // Potenziometro: ");
	Serial.print(sensorValue);
	Serial.print(" - Tensione: ");
	Serial.println(voltage);
  analogWrite(LED_PIN_R, sensorValue * (5.0 / 255.0));
  delay(50);
}

float misuraLuminosita() {
  int valore_ldr = analogRead(LDR_PIN);
  float val_ldr_convertito = map(valore_ldr, 0, 1023, 0, 255);

  return val_ldr_convertito;
}

