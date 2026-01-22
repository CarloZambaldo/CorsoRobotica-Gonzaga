# CorsoRobotica-Gonzaga

Repository ufficiale del **Corso di Robotica e Informatica applicata**  
Istituto Gonzaga – Laboratori Arduino, Raspberry Pi e introduzione al Machine Learning.

Questo repository raccoglie **materiale didattico, esempi di codice e progetti pratici** utilizzati durante il corso, con l’obiettivo di fornire agli studenti una base solida su:
- programmazione embedded (Arduino),
- scripting e automazione (Python su Raspberry Pi),
- concetti fondamentali di Machine Learning,
- integrazione hardware–software.

---

## 📁 Struttura della repository

```

CorsoRobotica-Gonzaga
│
├── @slides/                 # Slide del corso (lezioni teoriche)
├── @appunti/                # Appunti e materiale di supporto
│
├── CODE-Arduino/            # Sketch Arduino
│   ├── DHT/                 # Sensore temperatura/umidità
│   ├── HC-SR04/              # Sensore ultrasuoni
│   ├── LCD_display/          # Display LCD
│   ├── button/               # Gestione pulsanti
│   ├── dim_LED/              # PWM e controllo luminosità LED
│   ├── servo/                # Controllo servo motore
│   ├── matrix/               # LED matrix
│   ├── wifi/                 # Connessione WiFi
│   └── on-off-serverwifi/    # Controllo via server WiFi
│
├── CODE-Raspberry/
│   ├── primi/                # Primi script Python
│   ├── MachineLearning/      # Esempi di ML (supervised, unsupervised, RL)
│   └── server-llm/           # Server Python (Flask)
│
├── Progetti/
│   └── ARDUINO/
│       └── progettone/       # Progetto finale Arduino
│
└── README.md

````

---

## 🧠 Obiettivi didattici

- Comprendere la **differenza tra hardware e software**
- Imparare a leggere e modificare codice esistente
- Interfacciare **sensori e attuatori**
- Introdurre concetti base di:
  - Machine Learning supervisionato
  - Machine Learning non supervisionato
  - Reinforcement Learning
- Sviluppare piccoli **progetti completi e funzionanti**

---

## 🔧 Requisiti

### Hardware
- Arduino UNO / UNO R4 WiFi
- Raspberry Pi (3 / 4 / 5)
- Breadboard
- Sensori:
  - DHT11 / DHT22
  - HC-SR04
  - pulsanti, LED, servo motori
- Cavi jumper

### Software
- Arduino IDE
- Python ≥ 3.8
- Librerie Python

---

## 🚀 Utilizzo del codice Arduino

1. Aprire **Arduino IDE**
2. Selezionare la board corretta
3. Aprire uno sketch:

   ```
   CODE-Arduino/<nome_progetto>/<nome_progetto>.ino
   ```
4. Collegare l’hardware come spiegato a lezione
5. Caricare lo sketch sulla board

Ogni cartella contiene **un esempio indipendente**, progettato per una singola lezione o argomento.

---

## 🐍 Utilizzo del codice Raspberry Pi

### Script Python base

Percorso:

```
CODE-Raspberry/primi/
```

Esecuzione:

```bash
python3 nome_script.py
```

---

### Machine Learning

Percorso:

```
CODE-Raspberry/MachineLearning/
```

Include esempi di:

* apprendimento supervisionato
* apprendimento non supervisionato
* reinforcement learning

Gli script sono **didattici** e pensati per visualizzare il comportamento degli algoritmi.

---

## 🌐 Server Python / LLM

Percorso:

```
CODE-Raspberry/server-llm/app.py
```

Avvio:

```bash
streamlit app.py
```

Utilizzato per introdurre:

* concetto di server
* backend
* interazione tramite browser

---

## 🧩 Progettone finale ARDUINO

Percorso:

```
Progetti/ARDUINO/progettone/
```

Il progetto finale integra:

* più sensori
* logica di controllo
* struttura modulare del codice

Rappresenta una **sintesi pratica delle competenze acquisite** durante il corso.

---

## 🎓 Destinatari

* Studenti delle scuole superiori
* Principianti in programmazione
* Corsi introduttivi di robotica e informatica

---

## 📜 Licenza

Materiale didattico destinato all’uso educativo.
L’uso commerciale non è consentito senza autorizzazione.

---

## ✍️ Autore

**Carlo Zambaldo**
Corso di Robotica – Istituto Gonzaga
