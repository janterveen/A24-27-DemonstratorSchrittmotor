/**@file */

/**
 * @brief Programm für den Test der Funktionalität eines Tasters
 * 
 * Dieses Programm testet die Ansteuerung und Funktionalität eines Tasters
 */


int Taster = 11;   // Speichert den Pin, an dem der Taster angeschlossen ist

// speichert den Status des Tasters
int StatusTaster = 0;  

void setup()  
{
  // LED wird als OUTPUT (Ausgang) definiert
  pinMode(LED_BUILTIN, OUTPUT);   
  
  // der Taster wird als INPUT (Eingang) deklariert
  pinMode(Taster, INPUT);
  Serial.begin(9600);
}

void loop() 
{  
  // Taster abfragen
  StatusTaster = digitalRead(Taster);

  // wenn der Taster gedrückt wurde
  if (StatusTaster == HIGH) 
  {      
    // LED für 5 Sekunden (5000 ms) einschalten
    Serial.println(F("Knopf wurde gedrückt"));                         
    digitalWrite(LEDR,HIGH);   //Farbe der LED ändern
    digitalWrite(LEDG,LOW);
    digitalWrite(LEDB,LOW);
    delay(2000);  

    // LED ausschalten                
    digitalWrite(LEDR,LOW);   //Farbe der LED ändern
    digitalWrite(LEDG,LOW);
    digitalWrite(LEDB,HIGH);   
  }                               
}