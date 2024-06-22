/**@file */

/**
 * @brief Programm für den Test der Funktionalität einer LED
 * 
 * Dieses Programm testet die Ansteuerung und Funktionalität einer LED
 */

int Led_Rot = 3;
int Led_Gruen = 2;
int Led_Blau = 5;
  
void setup ()
{
  // Initialisierung Ausgangspins für die LEDs
  pinMode (Led_Rot, OUTPUT); 
  pinMode (Led_Gruen, OUTPUT);
  pinMode (Led_Blau, OUTPUT); 
}
  
void loop () //Hauptprogrammschleife
{
  digitalWrite (Led_Rot, HIGH); // LED wird eingeschaltet
  digitalWrite (Led_Gruen, LOW); // LED wird eingeschaltet
  digitalWrite (Led_Blau, LOW); // LED wird eingeschaltet
  delay (1000); // Wartemodus für 3 Sekunden
  
  digitalWrite (Led_Rot, LOW); // LED wird eingeschaltet
  digitalWrite (Led_Gruen, HIGH); // LED wird eingeschaltet
  digitalWrite (Led_Blau, LOW); // LED wird eingeschaltet
  delay (1000); // Wartemodus für weitere drei Sekunden in denen die LEDs dann umgeschaltet werden
   
  digitalWrite (Led_Rot, LOW); // LED wird eingeschaltet
  digitalWrite (Led_Gruen, LOW); // LED wird eingeschaltet
  digitalWrite (Led_Blau, HIGH); // LED wird eingeschaltet
  delay (1000); // Wartemodus für weitere drei Sekunden in denen die LEDs dann umgeschaltet werden
}