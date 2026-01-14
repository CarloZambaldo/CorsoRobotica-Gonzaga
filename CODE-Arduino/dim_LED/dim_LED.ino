const int sensorPin = A0;	// pin analogico collegato al partitore con fotoresistenza
const int ledPin    = 9;	// LED su pin PWM

int sensorValue = 0;		// valore letto dalla fotoresistenza (0-1023)
int ledValue    = 0;		// valore PWM per il LED (0-255)

void setup() {
	Serial.begin(9600);		// avvia la comunicazione seriale
	pinMode(ledPin, OUTPUT);
  // NOTA: i pin ANALOGICI vengono automaticamente configurati come ingressi analogici quando chiami analogRead():
  // non serve: pinMode(sensorPin, INPUT)
}

void loop() {
	// Legge il valore analogico dalla fotoresistenza
	sensorValue = analogRead(sensorPin);

	// Mappa il valore: più buio -> LED più acceso
	// Se al buio il valore è alto, invertiamo la mappa
	ledValue = map(sensorValue, 0, 1023, 255, 0);

	// Applica il PWM al LED
	analogWrite(ledPin, ledValue);

	// Stampa i valori sulla seriale
	Serial.print("Fotoresistenza: ");
	Serial.print(sensorValue);
	Serial.print("   LED PWM: ");
	Serial.println(ledValue);

	delay(100);	// piccolo delay per non inondare la seriale
}
