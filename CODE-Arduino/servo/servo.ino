#include <Servo.h> //Inserire la libreria Servo

Servo Servo1; //Il nome del servo è Servo1

void setup() {
  Servo1.attach (6); //Il Servo1 è collegato al pin digitale 9
}

void loop() {
//Imposto la posizione a cui si deve spostare
  Servo1.write(0);  
  delay (2000);
  Servo1.write(90);
  delay (2000);
  Servo1.write(180);
  delay (2000);
}
