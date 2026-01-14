#include <WiFiS3.h>
#include "Arduino_LED_Matrix.h"

char ssid[] = "CorsoRobotica";      // <--- metti il nome della tua rete
char pass[] = "passwordDifficilissima";  // <--- metti la password

WiFiServer server(80);
ArduinoLEDMatrix matrix;

// Cuore 8x12
uint8_t heart[8][12] = {
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,0,0,0,0,0,0,1,1,0},
	{0,1,1,0,0,0,0,0,0,1,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,1,0,0},
	{0,0,0,1,1,1,1,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0}
};

// Immagine "spenta" (tutti 0)
uint8_t emptyImg[8][12] = {0};

bool matrixON = false;

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		; // aspetta la seriale (se necessario)
	}

	matrix.begin();
	matrix.renderBitmap(emptyImg, 8, 12);  // all off

	// Controlla che il modulo WiFi esista
	if (WiFi.status() == WL_NO_MODULE) {
		Serial.println("Modulo WiFi non trovato!");
		while (true) {
			delay(1000);
		}
	}

	Serial.print("Connessione a '");
	Serial.print(ssid);
	Serial.println("' in corso...");

	WiFi.begin(ssid, pass);

	// Aspetta la connessione
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println();
	Serial.println("Connesso!");
	Serial.print("IP dell'Arduino sulla rete locale: ");
	delay(3000);
	Serial.println(WiFi.localIP());

	// Avvia il server HTTP
	server.begin();
	Serial.println("Server HTTP avviato sulla porta 80");
}

void loop() {
	WiFiClient client = server.available();
	if (!client) {
		return;
	}

	Serial.print("Nuova connessione client (");
	Serial.print(client.remoteIP());
	Serial.println(")");

	// Leggi la prima riga della richiesta HTTP (es. "GET /on HTTP/1.1")
	String requestLine = client.readStringUntil('\r');
	Serial.print("Richiesta: ");
	Serial.println(requestLine);
	client.readStringUntil('\n'); // consuma il '\n'

	// Controlla se la richiesta contiene /on o /off
	if (requestLine.indexOf("GET /on ") >= 0) {
		matrixON = true;
		matrix.renderBitmap(heart, 8, 12);
	} else if (requestLine.indexOf("GET /off ") >= 0) {
		matrixON = false;
		matrix.renderBitmap(emptyImg, 8, 12);
	}

	// Svuota il resto dell'header HTTP
	while (client.available()) {
		char c = client.read();
		if (c == '\n') {
			break; // fine header
		}
	}

	// Risposta HTTP
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html; charset=utf-8");
	client.println("Connection: close");
	client.println();
	client.println("<!DOCTYPE html>");
	client.println("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>");
	client.println("<title>Matrice LED integrata</title></head>");
	client.println("<body style=\"font-family:sans-serif; text-align:center;\">");
	client.println("<h1>Cuore sulla matrice LED</h1>");

	client.print("<p>Stato attuale: <b>");
	client.print(matrixON ? "ON" : "OFF");
	client.println("</b></p>");

	client.println("<p>");
	client.println("<a href=\"/on\"><button style=\"font-size:2em; padding:10px 20px; margin:5px;\">ON</button></a>");
	client.println("<a href=\"/off\"><button style=\"font-size:2em; padding:10px 20px; margin:5px;\">OFF</button></a>");
	client.println("</p>");

	client.println("<p>Da smartphone/PC nella stessa rete, apri questo indirizzo:</p>");
	client.print("<p><code>http://");
	client.print(WiFi.localIP());
	client.println("</code></p>");

	client.println("</body></html>");

	client.stop();
	Serial.println("Client disconnesso");
}
