#include <WiFiS3.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------------------- WIFI CONFIG ----------------------
const char* WIFI_SSID = "CorsoRobotica";
const char* WIFI_PASS = "passwordDifficilissima";
WiFiServer server(80);

// ---------------------- SENSORS & IO ----------------------
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int PIN_PIR   = 3;
const int PIN_LDR   = A0;

const int PIN_BTN   = 4;

const int PIN_BUZZ  = 6;

const int PIN_LED_R = 9;
const int PIN_LED_G = 10;
const int PIN_LED_B = 11;

// LCD I2C (prova 0x27, se non va prova 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- DASHBOARD HTML (PROGMEM) ----------
const char DASHBOARD_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>Lunar Habitat Module - Environmental Monitoring</title>
	<style>
		:root{
			--bg:#eef1f6;
			--card:#ffffff;
			--ink:#1f2a37;
			--muted:#6b7280;
			--top:#334b5d;
			--ok:#2e9b43;
			--warn:#f0b429;
			--alarm:#d93b3b;
			--shadow: 0 8px 18px rgba(0,0,0,.10);
			--radius:14px;
		}
		*{box-sizing:border-box}
		body{margin:0;font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial;background:var(--bg);color:var(--ink)}
		.wrap{max-width:1100px;margin:18px auto;padding:0 14px}
		.topbar{
			background:var(--top); color:#fff; padding:22px 18px;
			border-radius:var(--radius); box-shadow:var(--shadow);
			text-align:center; font-weight:700; font-size:26px;
		}
		.grid{display:grid; gap:14px; margin-top:14px}
		.grid.cards{grid-template-columns:repeat(4,1fr)}
		@media (max-width:1000px){ .grid.cards{grid-template-columns:repeat(2,1fr)} }
		@media (max-width:520px){ .grid.cards{grid-template-columns:1fr} }

		.card{
			background:var(--card); border-radius:var(--radius);
			box-shadow:var(--shadow); padding:14px 14px 12px;
			border:1px solid rgba(0,0,0,.06);
		}
		.card h3{margin:0 0 10px;font-size:22px;text-align:center}
		.hr{height:2px;background:#d7deea;border-radius:2px;margin:0 0 14px}
		.value{font-size:54px;font-weight:800;text-align:center;letter-spacing:-1px;margin:8px 0}
		.value small{font-size:22px;font-weight:700;color:var(--muted)}
		.pill{
			display:block;text-align:center;font-weight:800;
			padding:10px 10px;border-radius:10px;color:#fff;
			margin-top:10px;font-size:20px;
		}
		.pill.ok{background:var(--ok)}
		.pill.warn{background:var(--warn); color:#1b1b1b}
		.pill.alarm{background:var(--alarm)}

		.banner{
			background:var(--card); border-radius:var(--radius);
			box-shadow:var(--shadow); border:1px solid rgba(0,0,0,.06);
			overflow:hidden;
		}
		.banner .title{padding:14px 16px;font-size:22px;font-weight:700;color:var(--top)}
		.banner .body{
			background:var(--alarm); color:#fff;
			padding:22px 16px; display:flex; align-items:center; justify-content:space-between;
			gap:14px; font-weight:900; font-size:28px;
		}
		.badge{
			background:rgba(0,0,0,.15); padding:12px 18px;
			border-radius:12px; font-weight:900;
		}

		.grid.bottom{grid-template-columns:repeat(3,1fr)}
		@media (max-width:900px){ .grid.bottom{grid-template-columns:1fr} }

		.bigbox{
			display:flex; align-items:center; justify-content:center;
			border-radius:12px; padding:18px; font-weight:900; font-size:54px;
		}
		.bigbox.ok{background:var(--ok); color:#fff}
		.bigbox.warn{background:var(--warn); color:#1b1b1b}
		.bigbox.alarm{background:var(--alarm); color:#fff}

		.notice{font-size:18px;font-weight:800;text-align:center;padding:18px;color:var(--muted)}
		.kv{display:flex;justify-content:space-between;gap:10px;margin-top:10px;color:var(--muted);font-weight:700}
		.mono{font-family:ui-monospace,Menlo,Consolas,monospace}
	</style>
</head>
<body>
	<div class="wrap">
		<div class="topbar">Lunar Habitat Module - Environmental Monitoring</div>

		<div class="grid cards">
			<div class="card">
				<h3>Temperature</h3><div class="hr"></div>
				<div class="value"><span id="tVal">--</span><small>°C</small></div>
				<span class="pill ok" id="tPill">OK</span>
			</div>

			<div class="card">
				<h3>Humidity</h3><div class="hr"></div>
				<div class="value"><span id="hVal">--</span><small>%</small></div>
				<span class="pill ok" id="hPill">OK</span>
			</div>

			<div class="card">
				<h3>Light Level</h3><div class="hr"></div>
				<div class="value"><span id="lVal">--</span><small>raw</small></div>
				<span class="pill warn" id="lPill">WARNING</span>
			</div>

			<div class="card">
				<h3>Motion</h3><div class="hr"></div>
				<div class="value" style="font-size:44px"><span id="mVal">--</span></div>
				<span class="pill ok" id="mPill">OK</span>
			</div>
		</div>

		<div class="grid" style="margin-top:14px">
			<div class="banner" id="proxBox" style="display:none">
				<div class="title">Proximity Alert</div>
				<div class="body">
					<div id="proxText">TOO CLOSE TO CONTROL PANEL</div>
					<div class="badge" id="proxBadge">ALARM</div>
				</div>
			</div>
		</div>

		<div class="grid bottom" style="margin-top:14px">
			<div class="card">
				<h3>System Status:</h3><div class="hr"></div>
				<div class="bigbox ok" id="sysBox">OK</div>
				<div class="kv"><span>Mode</span><span class="mono" id="modeVal">--</span></div>
			</div>

			<div class="card">
				<h3>Warning:</h3><div class="hr"></div>
				<div class="notice" id="warnText">None</div>
			</div>

			<div class="card">
				<h3>Critical Alert:</h3><div class="hr"></div>
				<div class="notice" id="critText">None</div>
			</div>
		</div>
	</div>

<script>
function setPill(el, level){
	el.classList.remove("ok","warn","alarm");
	if(level==="OK"){ el.classList.add("ok"); el.textContent="OK"; }
	else if(level==="WARNING"){ el.classList.add("warn"); el.textContent="WARNING"; }
	else { el.classList.add("alarm"); el.textContent="ALARM"; }
}
function setSysBox(level){
	const box = document.getElementById("sysBox");
	box.classList.remove("ok","warn","alarm");
	box.textContent = level;
	if(level==="OK") box.classList.add("ok");
	else if(level==="WARNING") box.classList.add("warn");
	else box.classList.add("alarm");
}
async function tick(){
	try{
		const r = await fetch("/api", {cache:"no-store"});
		const j = await r.json();

		document.getElementById("tVal").textContent = j.temp_c ?? "--";
		document.getElementById("hVal").textContent = j.hum_pct ?? "--";
		document.getElementById("lVal").textContent = j.light_raw ?? "--";
		document.getElementById("mVal").textContent = (j.motion ? "Motion" : "No Motion");
		document.getElementById("modeVal").textContent = j.mode ?? "--";

		setPill(document.getElementById("tPill"), j.temp_status);
		setPill(document.getElementById("hPill"), j.hum_status);
		setPill(document.getElementById("lPill"), j.light_status);
		setPill(document.getElementById("mPill"), j.motion_status);

		setSysBox(j.system_status);

		document.getElementById("warnText").textContent = j.warning_text || "None";
		document.getElementById("critText").textContent = j.critical_text || "None";

		const prox = document.getElementById("proxBox");
		if(j.proximity_alarm){
			prox.style.display = "block";
			document.getElementById("proxText").textContent = j.proximity_text || "PROXIMITY ALERT";
			document.getElementById("proxBadge").textContent = "ALARM";
		}else{
			prox.style.display = "none";
		}
	}catch(e){
		// se fetch fallisce, non bloccare
	}
}
tick();
setInterval(tick, 1500);
</script>
</body>
</html>
)HTML";


// ---------------------- TIMING ----------------------
const unsigned long SENSOR_PERIOD_MS = 1000;     // campionamento sensori
const unsigned long LCD_IDLE_TIMEOUT_MS = 8000;  // dopo tot torna in "IDLE"
const unsigned long PIR_DEBOUNCE_MS = 200;       // filtro rimbalzi PIR
const unsigned long BUZZ_BEEP_MS = 250;          // beep intermittente

unsigned long t_lastSensors = 0;
unsigned long t_lastBtnEdge = 0;
unsigned long t_lastPirEdge = 0;
unsigned long t_lastLcdAction = 0;
unsigned long t_lastBuzzToggle = 0;

// ---------------------- STATE / THRESHOLDS ----------------------
// Soglie (modificabili)
float T_OK_MIN = 18.0;
float T_OK_MAX = 27.0;
float T_WARN_MARGIN = 1.5;     // zona "vicino soglia"

float H_OK_MIN = 30.0;
float H_OK_MAX = 70.0;
float H_WARN_MARGIN = 5.0;

// luce (0..1023). Con LDR a 5V e 10k a GND: più luce => più alto
int   L_OK_MIN = 200;          // troppo buio sotto (esempio)
int   L_WARN_MARGIN = 50;

// Se modalità "UNMANNED" attiva, movimento => ALARM
bool unmannedMode = false;

// ---------------------- MEASUREMENTS ----------------------
float tempC = NAN;
float humRH = NAN;
int lightRaw = 0;

bool pirNow = false;
bool pirLatched = false;     // "movimento recente" latched
unsigned long t_lastMotion = 0;

// ---------------------- MODULE STATUS ----------------------
enum ModuleStatus { OK_STATUS, WARN_STATUS, ALARM_STATUS };
ModuleStatus statusNow = OK_STATUS;

String anomalies = "";

// ---------------------- LCD MODE ----------------------
enum LcdMode { LCD_IDLE, LCD_SHOW_SENSORS, LCD_SHOW_ALARM };
LcdMode lcdMode = LCD_IDLE;

// ---------------------- HELPERS ----------------------
bool inRange(float x, float a, float b) { return (x >= a && x <= b); }
bool inRangeInt(int x, int a, int b) { return (x >= a && x <= b); }

void setRgb(bool r, bool g, bool b) {
  digitalWrite(PIN_LED_R, r ? HIGH : LOW);
  digitalWrite(PIN_LED_G, g ? HIGH : LOW);
  digitalWrite(PIN_LED_B, b ? HIGH : LOW);
}


// ---------------------- SETUP / LOOP ----------------------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);

  pinMode(PIN_BUZZ, OUTPUT);
  digitalWrite(PIN_BUZZ, LOW);

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);

Serial.println("set RGB...");
  setRgb(false, true, false); // start green

Serial.println("Setting DHT sensor...");
  dht.begin();

Serial.println("Wire...");

Serial.println("WiFi connecting...");
  // WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - t0 > 15000) break;
    delay(300);
    Serial.println("WiFi connecting...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
    IPAddress ip = WiFi.localIP();
    Serial.print("Dashboard: http://");
    Serial.println(ip);
  } else {
    delay(1500);
  }
}

void loop() {
  unsigned long now = millis();

  // Web
  if (WiFi.status() == WL_CONNECTED) {
    handleWeb();
  }
  delay(2000);
}


void applyStatusOutputs() {
  // LED RGB
  if (statusNow == OK_STATUS) {
    setRgb(false, true, false);       // Verde
  } else if (statusNow == WARN_STATUS) {
    setRgb(true, true, false);        // Giallo (R+G)
  } else {
    setRgb(true, false, false);       // Rosso
  }

  // Buzzer
  if (statusNow == ALARM_STATUS) {
    // beep intermittente non bloccante
    unsigned long now = millis();
    if (now - t_lastBuzzToggle >= BUZZ_BEEP_MS) {
      t_lastBuzzToggle = now;
      digitalWrite(PIN_BUZZ, !digitalRead(PIN_BUZZ));
    }
  } else {
    digitalWrite(PIN_BUZZ, LOW);
  }
}

ModuleStatus computeStatusAndAnomalies() {
  anomalies = "";

  bool alarm = false;
  bool warn  = false;

  // --- Temperature ---
  if (!isnan(tempC)) {
    if (!inRange(tempC, T_OK_MIN, T_OK_MAX)) {
      alarm = true;
      anomalies += "TEMP_OUT ";
    } else {
      // warning zone vicino alle soglie
      if (tempC <= (T_OK_MIN + T_WARN_MARGIN) || tempC >= (T_OK_MAX - T_WARN_MARGIN)) {
        warn = true;
        anomalies += "TEMP_NEAR ";
      }
    }
  } else {
    warn = true;
    anomalies += "TEMP_NA ";
  }

  // --- Humidity ---
  if (!isnan(humRH)) {
    if (!inRange(humRH, H_OK_MIN, H_OK_MAX)) {
      alarm = true;
      anomalies += "HUM_OUT ";
    } else {
      if (humRH <= (H_OK_MIN + H_WARN_MARGIN) || humRH >= (H_OK_MAX - H_WARN_MARGIN)) {
        warn = true;
        anomalies += "HUM_NEAR ";
      }
    }
  } else {
    warn = true;
    anomalies += "HUM_NA ";
  }

  // --- Light ---
  if (!inRangeInt(lightRaw, L_OK_MIN, 1023)) {
    // troppo buio
    warn = true;
    anomalies += "LIGHT_LOW ";
  } else {
    if (lightRaw <= (L_OK_MIN + L_WARN_MARGIN)) {
      warn = true;
      anomalies += "LIGHT_NEAR ";
    }
  }

  // --- Motion / occupancy ---
  // pirLatched indica "movimento recente"
  if (unmannedMode && pirLatched) {
    alarm = true;
    anomalies += "MOTION_UNMANNED ";
  }

  if (alarm) return ALARM_STATUS;
  if (warn)  return WARN_STATUS;
  return OK_STATUS;
}

void updateSensors() {
  // DHT
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t)) tempC = t;
  if (!isnan(h)) humRH = h;

  // LDR
  lightRaw = analogRead(PIN_LDR);

  // PIR con debounce minimo
  bool pirRead = (digitalRead(PIN_PIR) == HIGH);
  unsigned long now = millis();
  if (pirRead != pirNow && (now - t_lastPirEdge) > PIR_DEBOUNCE_MS) {
    pirNow = pirRead;
    t_lastPirEdge = now;
    if (pirNow) {
      pirLatched = true;
      t_lastMotion = now;
    }
  }

  // “latch” di movimento recente: si spegne dopo 10s senza motion
  if (pirLatched && (now - t_lastMotion) > 10000) {
    pirLatched = false;
  }

  // calcolo stato
  statusNow = computeStatusAndAnomalies();
  applyStatusOutputs();

  // LCD: se ALARM forza schermata allarme
  if (statusNow == ALARM_STATUS) {
    lcdMode = LCD_SHOW_ALARM;
    t_lastLcdAction = now;
  }
}

void lcdShowIdle() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Habitat Monitor");
  lcd.setCursor(0,1);
  if (unmannedMode) lcd.print("Mode: UNMANNED");
  else             lcd.print("Mode: MANNED  ");
}

void lcdShowSensors() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  if (isnan(tempC)) lcd.print("NA");
  else { lcd.print(tempC, 1); lcd.print("C"); }

  lcd.print(" H:");
  if (isnan(humRH)) lcd.print("NA");
  else { lcd.print(humRH, 0); lcd.print("%"); }

  lcd.setCursor(0,1);
  lcd.print("L:");
  lcd.print(lightRaw);
  lcd.print(" ");

  lcd.print("M:");
  lcd.print(pirLatched ? "YES" : "NO ");
}

void lcdShowAlarm() {
  lcd.clear();
  lcd.setCursor(0,0);
  if (statusNow == ALARM_STATUS) lcd.print("!!! ALARM !!!   ");
  else if (statusNow == WARN_STATUS) lcd.print("** WARNING **   ");
  else lcd.print("OK              ");

  lcd.setCursor(0,1);
  // mostra prime 16 char di anomalies
  String a = anomalies;
  if (a.length() == 0) a = "No anomalies";
  if (a.length() > 16) a = a.substring(0,16);
  lcd.print(a);
}

void handleButton() {
  // Pulsante con pullup: premuto = LOW
  static bool last = HIGH;
  bool now = digitalRead(PIN_BTN);

  unsigned long ms = millis();
  if (last == HIGH && now == LOW) {
    // press detected
    t_lastBtnEdge = ms;
  }
  if (last == LOW && now == HIGH) {
    // release detected: durata pressione
    unsigned long pressMs = ms - t_lastBtnEdge;

    if (pressMs > 800) {
      // long-press => toggle unmannedMode
      unmannedMode = !unmannedMode;
      lcdMode = LCD_SHOW_ALARM;  // mostra stato subito
      t_lastLcdAction = ms;
    } else {
      // short press => show sensors on-demand
      lcdMode = LCD_SHOW_SENSORS;
      t_lastLcdAction = ms;
    }
  }
  last = now;

  // timeout ritorno a idle se non allarme
  if (lcdMode != LCD_IDLE && statusNow != ALARM_STATUS) {
    if ((ms - t_lastLcdAction) > LCD_IDLE_TIMEOUT_MS) {
      lcdMode = LCD_IDLE;
    }
  }

  // refresh LCD “intelligente”
  static LcdMode lastMode = LCD_IDLE;
  static ModuleStatus lastStatus = OK_STATUS;
  static int lastLight = -1;
  static int lastMotion = -1;

  bool needUpdate = false;

  if (lcdMode != lastMode) needUpdate = true;
  if (statusNow != lastStatus) needUpdate = true;

  // se stai mostrando sensori, aggiorna solo se cambia “abbastanza”
  if (lcdMode == LCD_SHOW_SENSORS) {
    if (abs(lightRaw - lastLight) > 10) needUpdate = true;
    int mot = pirLatched ? 1 : 0;
    if (mot != lastMotion) needUpdate = true;
  }

  if (needUpdate) {
    if (lcdMode == LCD_IDLE) lcdShowIdle();
    else if (lcdMode == LCD_SHOW_SENSORS) lcdShowSensors();
    else lcdShowAlarm();

    lastMode = lcdMode;
    lastStatus = statusNow;
    lastLight = lightRaw;
    lastMotion = pirLatched ? 1 : 0;
  }
}

// ---------------------- WEB DASHBOARD ----------------------
String statusToString(ModuleStatus s) {
  if (s == OK_STATUS) return "OK";
  if (s == WARN_STATUS) return "WARNING";
  return "ALARM";
}

void sendHttp(WiFiClient &client) {
  String html =
    "<!doctype html><html><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<meta http-equiv='refresh' content='2'>"
    "<title>Lunar Habitat Dashboard</title>"
    "<style>"
    "body{font-family:system-ui,Arial;margin:16px;}"
    ".card{padding:12px;border:1px solid #ccc;border-radius:12px;margin-bottom:12px;}"
    ".big{font-size:22px;font-weight:700;}"
    ".mono{font-family:ui-monospace,Menlo,monospace;}"
    "</style></head><body>"
    "<h2>Lunar Habitat Module</h2>"
    "<div class='card'><div>Global status</div><div class='big'>" + statusToString(statusNow) + "</div>"
    "<div>Mode: <span class='mono'>" + String(unmannedMode ? "UNMANNED" : "MANNED") + "</span></div>"
    "</div>"
    "<div class='card'>"
    "<div class='big'>Sensors</div>"
    "<div>Temperature: <span class='mono'>" + (isnan(tempC) ? String("NA") : String(tempC,1)) + " C</span></div>"
    "<div>Humidity: <span class='mono'>" + (isnan(humRH) ? String("NA") : String(humRH,0)) + " %</span></div>"
    "<div>Light (raw): <span class='mono'>" + String(lightRaw) + "</span></div>"
    "<div>Motion: <span class='mono'>" + String(pirLatched ? "YES" : "NO") + "</span></div>"
    "</div>"
    "<div class='card'>"
    "<div class='big'>Active anomalies</div>"
    "<div class='mono'>" + (anomalies.length() ? anomalies : String("None")) + "</div>"
    "<p>Tip: long-press button to toggle UNMANNED mode.</p>"
    "</div>"
    "</body></html>";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.print(html);
}

void handleWeb() {
	WiFiClient client = server.available();
	if (!client) return;

	// attesa breve dati
	unsigned long start = millis();
	while (client.connected() && !client.available()) {
		if (millis() - start > 200) break;
	}

	String reqLine = "";
	if (client.available()) reqLine = client.readStringUntil('\r');

	// Consuma il resto header
	while (client.available()) client.read();

	// Parse minimale: "GET /path HTTP/1.1"
	String path = "/";
	int s1 = reqLine.indexOf(' ');
	int s2 = reqLine.indexOf(' ', s1 + 1);
	if (s1 >= 0 && s2 > s1) path = reqLine.substring(s1 + 1, s2);

	if (path == "/api") {
		sendApi(client);
	} else {
		sendDashboard(client); // "/" e tutto il resto
	}

	delay(5);
	client.stop();
}

String tempStatus() {
	if (isnan(tempC)) return "WARNING";
	if (tempC < T_OK_MIN || tempC > T_OK_MAX) return "ALARM";
	if (tempC <= (T_OK_MIN + T_WARN_MARGIN) || tempC >= (T_OK_MAX - T_WARN_MARGIN)) return "WARNING";
	return "OK";
}

String humStatus() {
	if (isnan(humRH)) return "WARNING";
	if (humRH < H_OK_MIN || humRH > H_OK_MAX) return "ALARM";
	if (humRH <= (H_OK_MIN + H_WARN_MARGIN) || humRH >= (H_OK_MAX - H_WARN_MARGIN)) return "WARNING";
	return "OK";
}

String lightStatus() {
	// lightRaw: 0..1023 (più alto = più luce, nel wiring che ti avevo dato)
	if (lightRaw < L_OK_MIN) return "WARNING";
	if (lightRaw <= (L_OK_MIN + L_WARN_MARGIN)) return "WARNING";
	return "OK";
}

String motionStatus() {
	// In UNMANNED, movimento è critico => ALARM
	if (unmannedMode && pirLatched) return "ALARM";
	return "OK";
}

String buildApiJson() {
	String sys = statusToString(statusNow);
	String mode = unmannedMode ? "UNMANNED" : "MANNED";

	// warning_text e critical_text: deriviamo da "anomalies"
	// Qui: se ALARM => critical, se WARN => warning
	String warningText = "";
	String criticalText = "";

	if (statusNow == WARN_STATUS) warningText = anomalies;
	if (statusNow == ALARM_STATUS) criticalText = anomalies;

	// "proximity_alarm": nello screenshot c'è un pannello dedicato
	// Se vuoi davvero la prossimità serve un sensore distanza (es. VL53L0X/ultrasuoni).
	// Per ora lo leghiamo a un caso: movimento in UNMANNED => proximity_alarm true
	bool proximityAlarm = (unmannedMode && pirLatched);
	String proximityText = "TOO CLOSE TO CONTROL PANEL";

	String json = "{";
	json += "\"temp_c\":" + (isnan(tempC) ? String("null") : String(tempC,1)) + ",";
	json += "\"hum_pct\":" + (isnan(humRH) ? String("null") : String(humRH,0)) + ",";
	json += "\"light_raw\":" + String(lightRaw) + ",";
	json += "\"motion\":" + String(pirLatched ? "true" : "false") + ",";

	json += "\"temp_status\":\"" + tempStatus() + "\",";
	json += "\"hum_status\":\"" + humStatus() + "\",";
	json += "\"light_status\":\"" + lightStatus() + "\",";
	json += "\"motion_status\":\"" + motionStatus() + "\",";

	json += "\"system_status\":\"" + sys + "\",";
	json += "\"mode\":\"" + mode + "\",";

	json += "\"warning_text\":\"" + warningText + "\",";
	json += "\"critical_text\":\"" + criticalText + "\",";

	json += "\"proximity_alarm\":" + String(proximityAlarm ? "true" : "false") + ",";
	json += "\"proximity_text\":\"" + proximityText + "\"";
	json += "}";
	return json;
}

void sendApi(WiFiClient &client) {
	String json = buildApiJson();
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: application/json; charset=utf-8");
	client.println("Connection: close");
	client.println();
	client.print(json);
}

void sendDashboard(WiFiClient &client) {
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html; charset=utf-8");
	client.println("Connection: close");
	client.println();
	client.print((__FlashStringHelper*)DASHBOARD_HTML);
}
