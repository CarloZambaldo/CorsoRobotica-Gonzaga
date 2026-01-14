#include <WiFiS3.h>

// ---------- WIFI ----------
const char WIFI_SSID[] = "CorsoRobotica";
const char WIFI_PASS[] = "passwordDifficilissima";

WiFiServer server(80);

void setup() {
	Serial.begin(115200);
	delay(500);

	Serial.println("Booting web interface...");

  Serial.println("Connecting to WiFi...");
	WiFi.begin(WIFI_SSID, WIFI_PASS);

	unsigned long t0 = millis();
	while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
		Serial.println("Retrying: connecting to WiFi...");
		delay(1000);
	}

	if (WiFi.status() == WL_CONNECTED) {
		server.begin();
		IPAddress ip = WiFi.localIP();
		Serial.print("WiFi connected. IP: ");
		Serial.println(ip);
		Serial.println("Open browser at: http://<IP>/");
	} else {
		Serial.println("WiFi NOT connected");
	}
}

void loop() {
	if (WiFi.status() != WL_CONNECTED) return;

	WiFiClient client = server.available();
	if (!client) return;

	// Attendi dati (timeout corto)
	unsigned long start = millis();
	while (client.connected() && !client.available()) {
		if (millis() - start > 200) break;
	}

	// Legge la prima riga della richiesta (GET / HTTP/1.1)
	String req = "";
	if (client.available()) {
		req = client.readStringUntil('\r');
	}

	// Pulisce il resto dell'header
	while (client.available()) client.read();

	// ---------- RISPOSTA TESTUALE ----------
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/plain; charset=utf-8");
	client.println("Connection: close");
	client.println();

	client.println("Web interface online");
	client.print("Uptime (ms): ");
	client.println(millis());

	delay(5);
	client.stop();
}
