#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
	Serial.begin(115200);
	dht.begin();
}

void loop() {
	delay(2000);  // minimo richiesto dal DHT11

	float h = dht.readHumidity();
	float t = dht.readTemperature(); // °C

	stampaRisultato(h,t);
}


void stampaRisultato(float h, float t){
  if (isnan(h) || isnan(t)) {
		Serial.println("DHT11 read error");
		return;
	}

	Serial.print("T = ");
	Serial.print(t);
	Serial.print(" C   H = ");
	Serial.print(h);
	Serial.println(" %");
}