/**@file */

/**
 * @brief Programm für den Test der Funktionalität eines Tasters
 * 
 * Dieses Programm testet die Ansteuerung und Funktionalität eines Tasters mit zusätzlichem Debounce-Delay
 */


int Taster = 3;   // Speichert den Pin, an dem der Taster angeschlossen ist

// speichert den Status des Tasters
int StatusTaster = HIGH;  
int letzterStatusTaster = HIGH; // speichert den letzten Status des Tasters
unsigned long letzteZeit = 0; // speichert die Zeit des letzten Zustandswechsels
unsigned long debounceDelay = 50; // Entprellzeit in Millisekunden

void setup()  
{
  // LED wird als OUTPUT (Ausgang) definiert
  pinMode(LED_BUILTIN, OUTPUT);   

  // der Taster wird als INPUT_PULLUP (Eingang mit internem Pull-up-Widerstand) deklariert
  pinMode(Taster, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() 
{  
  // aktuellen Tasterstatus abfragen
  int reading = digitalRead(Taster);

  // prüfen, ob der Tasterzustand sich geändert hat
  if (reading != letzterStatusTaster) {
    // Zeit des letzten Zustandswechsels speichern
    letzteZeit = millis();
  }

  // prüfen, ob die Entprellzeit überschritten wurde
  if ((millis() - letzteZeit) > debounceDelay) {
    // wenn sich der aktuelle Tasterzustand vom gespeicherten unterscheidet
    if (reading != StatusTaster) {
      StatusTaster = reading;

      // wenn der Taster gedrückt wurde (LOW bedeutet gedrückt, da wir INPUT_PULLUP verwenden)
      if (StatusTaster == LOW) {
        Serial.println(F("Knopf wurde gedrückt"));                         
        digitalWrite(LEDR, HIGH);   // Farbe der LED ändern
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, LOW);
        delay(2000);  

        // LED ausschalten                
        digitalWrite(LEDR, LOW);   // Farbe der LED ändern
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, HIGH);   
      }
    }
  }

  // den letzten Tasterstatus für den nächsten Durchlauf speichern
  letzterStatusTaster = reading;
}
