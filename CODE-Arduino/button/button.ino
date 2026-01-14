int ledPin = 13;
int buttonPin = 2;

void setup() {
	pinMode(ledPin, OUTPUT);
	pinMode(buttonPin, INPUT);   // resistenza esterna (no INPUT_PULLUP)

	Serial.begin(9600);
}

void loop() {
	int buttonState = digitalRead(buttonPin);

	// LED segue lo stato del pulsante
	if (buttonState == LOW) {
		digitalWrite(ledPin, HIGH);
	} else {
		digitalWrite(ledPin, LOW);
	}

	// Stampa lo stato del bottone
	Serial.println(buttonState);

	delay(100);
}
