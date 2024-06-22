/**@file */

/**
 * @brief Demonstration eines Stepper-Motors mit Encoder und OLED-Anzeige
 * 
 * Dieses Programm steuert einen Stepper-Motor über einen Encoder und zeigt Informationen auf einem OLED-Display an.
 * Es beinhaltet auch Taster- und Endschalter-Funktionalitäten.
 */


#include "AccelStepper.h" ///< Bibliothek für den Stepper-Motor
#include <SPI.h> ///< Bibliothek für die Kommunikation mit Displays
#include <Wire.h> ///< Bibliothek für die Kommunikation über das I2C-Protokoll
#include <Adafruit_GFX.h> ///< Bibliothek für viele Grafikfunktionen und Operationen für Displays
#include <Adafruit_SSD1306.h> ///< Bibliothek für die Ansteuerung von OLEDs mit SSD1306-Treiber
#include <Encoder.h> ///< Bibliothek für den Encoder

#define MOTOR_INTERFACE_TYPE 1 ///< Festlegung der Anbindung des Stepper-Motors, 1 bedeutet, dass der Motor über einen Treiber angeschlossen ist
#define SCREEN_WIDTH 128 ///< Festlegung der Breite des Displays
#define SCREEN_HEIGHT 64 ///< Festlegung der Höhe des Displays
#define OLED_RESET -1 ///< Festlegung des OLED-Resets
#define SCREEN_ADDRESS 0x3C ///< Festlegung der I2C-Adresse des Displays

#define PIN_STEP_DIR 4 ///< Verknüpfter digitaler Pin am Direction-Pin des Motortreibers 
#define PIN_STEP_STEP 5 ///< Verknüpfter digitaler Pin am Step-Pin des Motortreibers 
#define PIN_LED_RED 6 ///< Verknüpfter digitaler Pin am roten Kabel der LED
#define PIN_LED_GREEN 3 ///< Verknüpfter digitaler Pin am grünen Kabel der LED
#define PIN_LED_BLUE 9 ///< Verknüpfter digitaler Pin am blauen Kabel der LED
#define PIN_BUTTON 11 ///< Verknüpfter digitaler Pin am Taster
#define PIN_ENDSTOP 12 ///< Verknüpfter digitaler Pin am Endschalter
#define PIN_CLK 9 ///< Verknüpfter digitaler Pin am CLK-Pin des Encoders
#define PIN_DT 2 ///< Verknüpfter digitaler Pin am DT-Pin des Encoders
#define PIN_SW 3 ///< Verknüpfter digitaler Pin am SW-Pin des Encoders

AccelStepper stepper(MOTOR_INTERFACE_TYPE, PIN_STEP_STEP, PIN_STEP_DIR); ///< Definition des Stepper-Motors
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); ///< Definition des Displays
Encoder encoder(PIN_DT, PIN_CLK); ///< Definition des Encoders

int buttonStatus = HIGH; ///< Aktueller Status des Tasters (gedrückt oder nicht gedrückt)
int lastButtonStatus = HIGH; ///< Letzter Status des Tasters (gedrückt oder nicht gedrückt)
unsigned long lastDebounceTime = 0; ///< Die letzte Zeit, zu der der Taster gedrückt wurde
unsigned long debounceDelay = 50; ///< Eine Verzögerung für die Abfrage der Schalter, um die mechanischen Schwingungen auszugleichen
int endstopStatus = HIGH; ///< Aktueller Status des Endtasters (gedrückt oder nicht gedrückt)
int lastEndstopStatus = HIGH; ///< Letzter Status des Tasters (gedrückt oder nicht gedrückt)
unsigned long lastEndstopDebounceTime = 0; ///< Die letzte Zeit, zu der der Endschalter gedrückt wurde
int stage = 0; ///< Die Stufe, die für die Demonstration eingestellt ist
int speeds[] =        {1000, 1000,  500, 2000, 3500, 2000, 2000, 2000, 2000, 500, 3500}; ///< Geschwindigkeiten für jede Stufe
int accelerations[] = {1000, 2000, 2000, 2000, 2000,  300, 1000, 4000, 7000, 300, 7000}; ///< Beschleunigungen für jede Stufe
long oldPosition = -999; ///< Alte Position des Encoders für den Abgleich. Es ist nötig, dass ein Wert vorher eingestellt ist für die Funktion des Programms
const int safeSteps = 70; ///< Anzahl der Schritte, die als Sicherheitsabstand zum Endschalter gehalten werden
const int trackLength = 1750; ///< Anzahl der Schritte, die bis zum Ende der Bahn durchgeführt werden können

const char* stageMessages[] = {"Keine Stufe eingestellt", "Stufe 1", "Stufe 2", "Stufe 3", "Stufe 4", "Stufe 5", "Stufe 6", "Stufe 7", "Stufe 8", "Stufe 9", "Stufe 10"}; ///< Anzeige der eingestellten Stufe auf dem Display
const char* messages[] = {"Start...", "Startbereit", "Programm wird ausgefuehrt...", "Vorgang abgebrochen, das Geraet muss neu gestartet werden", ""}; ///< Nachricht auf dem Display

/**
 * @brief Initialisierung des Systems
 * 
 * Diese Funktion wird einmal beim Start ausgeführt und initialisiert alle wichtigen Komponenten.
 */
void setup() {
    stepper.setMaxSpeed(1000); ///< Einstellung des Startwerts für die maximale Geschwindigkeit
    stepper.setAcceleration(1000); ///< Einstellung des Startwerts für die Beschleunigung
    pinMode(PIN_LED_RED, OUTPUT); ///< Initialisierung des Pins der roten LED
    pinMode(PIN_LED_GREEN, OUTPUT); ///< Initialisierung des Pins der grünen LED
    pinMode(PIN_LED_BLUE, OUTPUT); ///< Initialisierung des Pins der blauen LED
    pinMode(PIN_BUTTON, INPUT_PULLUP); ///< Initialisierung des Pins des Tasters
    pinMode(PIN_ENDSTOP, INPUT_PULLUP); ///< Initialisierung des Pins des Endschalters

    Serial.begin(9600); ///< Start der seriellen Kommunikation
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    attachInterrupt(digitalPinToInterrupt(PIN_SW), handleInterrupt, CHANGE); ///< Festlegen eines Interrupts, der das Programm stoppt, sobald er gedrückt wird

    display.display(); ///< Start des Displays, inklusive einer Startanimation von Adafruit
    delay(2000);
    display.setTextSize(2); ///< Festlegung der Textgröße
    display.setTextColor(SSD1306_WHITE); ///< Festlegung der Textfarbe
    updateDisplay(messages[0], messages[4]); ///< Anzeige der Startbenachrichtigung
    delay(1000);
    updateLEDs(false, true, false); ///< Start der LED
}

/**
 * @brief Hauptprogrammschleife
 * 
 * Diese Funktion wird kontinuierlich ausgeführt und steuert die Hauptlogik des Programms.
 */
void loop() {
    long newPosition = encoder.read(); ///< Auswertung der Encoder-Position
    if (newPosition != oldPosition) { ///< Falls die neue Position des Encoders nicht der alten Position des Encoders entspricht
        stage = constrain(newPosition / 4, 1, 10); ///< Übernahme der neuen Position für die Anzeige der Stufe auf dem Bildschirm
        updateDisplay(messages[1], stageMessages[stage]); ///< Anzeige der eingestellten Stufe auf dem Display
        oldPosition = newPosition; ///< Aktualisieren der alten Position
    }

    int buttonReading = digitalRead(PIN_BUTTON); ///< Auswertung des Tasters, ob er gedrückt wurde oder nicht
    if (buttonReading != lastButtonStatus) { ///< Wenn der Zustand des Tasters sich geändert hat
        lastDebounceTime = millis(); ///< Aktualisierung der Zeit, zu der der Taster gedrückt wurde
    }

    if ((millis() - lastDebounceTime) > debounceDelay) { ///< Falls die Zeit zwischen dem Drücken des Tasters größer ist, als die Verzögerung für die Auswertung des Schalters
        if (buttonReading != buttonStatus) { ///< Falls der gespeicherte Status nicht dem tatsächlichen Status des Tasters entspricht
            buttonStatus = buttonReading; ///< Speichern des aktuellen Status des Tasters
            if (buttonStatus == LOW) { ///< Falls der Taster gedrückt wurde
                stepper.setAcceleration(accelerations[0]); ///< Einstellung der Beschleunigung des Motors auf eine angemessene Beschleunigung
                // Schleife zum Finden der Referenzposition
                while (!findReference()) {
                    stepper.setSpeed(-200); ///< Geschwindigkeit des Steppers einstellen
                    stepper.runSpeed(); ///< Einen einzelnen Schritt ausführen
                    // delay(5); ///< Kurze Pause zwischen den Schritten
                }

                int currentStage = stage; ///< Speichern der eingestellten Stufe für den Demonstrationsprozess
                updateDisplay(messages[2], stageMessages[currentStage]); ///< Update des Displays
                updateLEDs(false, false, true); ///< Update der LEDs
                for (int i = 0; i < 2; i++) { ///< Anzahl der auszuführenden Wiederholungen der Demonstration
                    if (i == 0) {
                        stepper.setMaxSpeed(speeds[currentStage]); ///< Einstellung der Geschwindigkeit für die Ausführung der Schritte für den Sicherheitsabstand
                        stepper.setAcceleration(accelerations[currentStage]); ///< Ändern der Beschleunigung basierend auf der Stufe
                        stepper.move(trackLength + safeSteps); ///< Ausführung der Schritte für den Sicherheitsabstand
                        while (stepper.distanceToGo() != 0) {
                          stepper.run();
                        }
                    }
                    else {
                      stepper.setMaxSpeed(speeds[currentStage]); ///< Ändern der Geschwindigkeit basierend auf der Stufe
                      stepper.setAcceleration(accelerations[currentStage]); ///< Ändern der Beschleunigung basierend auf der Stufe
                      stepper.move(trackLength);
                      while (stepper.distanceToGo() != 0) { ///< Ausführung der Bewegung vom Motor weg
                        stepper.run();
                      }
                    }
                    stepper.move(-trackLength);
                    while (stepper.distanceToGo() != 0) { ///< Ausführung der Bewegung zum Motor hin
                        stepper.run();
                    }
                }
                updateDisplay(messages[1], stageMessages[currentStage]); ///< Update des Displays
            }
        }
    }
    lastButtonStatus = buttonReading; ///< Aktualisierung des letzten Status des Tasters
}

/**
 * @brief Aktualisiert die Farbe der LEDs
 * 
 * @param red Gibt an, ob die rote LED an oder aus ist
 * @param green Gibt an, ob die grüne LED an oder aus ist
 * @param blue Gibt an, ob die blaue LED an oder aus ist
 */
void updateLEDs(bool red, bool green, bool blue) {
    digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN, green ? HIGH : LOW);
    digitalWrite(PIN_LED_BLUE, blue ? HIGH : LOW);
}

/**
 * @brief Aktualisiert das Display basierend auf der eingestellten Nachricht
 * 
 * @param line1 Erste Zeile der Nachricht
 * @param line2 Zweite Zeile der Nachricht (kann leer sein)
 */
void updateDisplay(const char* line1, const char* line2) {
    display.clearDisplay(); ///< Leeren der aktuellen Anzeige des Displays
    display.setCursor(0, 0);
    display.println(F(line1)); ///< Ausgabe der eingestellten Texte auf dem Display
    if (line2 != "") {
        display.println(F(line2));
    }
    display.display();
}

/**
 * @brief Führt die Referenzfahrt aus
 * 
 * @return true Wenn der Referenzpunkt gefunden wurde
 * @return false Wenn der Referenzpunkt noch nicht gefunden wurde
 */
bool findReference() {
    int endstopReading = digitalRead(PIN_ENDSTOP); ///< Auswertung des Endschalters
    if (endstopReading != lastEndstopStatus) { ///< Wenn der Zustand des Endschalters sich geändert hat
        lastEndstopDebounceTime = millis(); ///< Aktualisierung der Zeit, zu der der Endschalter gedrückt wurde
    }

    if ((millis() - lastEndstopDebounceTime) > debounceDelay) { ///< Falls die Zeit zwischen dem Drücken des Endschalters größer ist, als die Verzögerung für die Auswertung des Endschalters
        if (endstopReading != endstopStatus) { ///< Falls der gespeicherte Status nicht dem tatsächlichen Status des Endschalters entspricht
            endstopStatus = endstopReading; ///< Speichern des aktuellen Status des Endschalters
            if (endstopStatus == LOW) { ///< Falls der Endschalter gedrückt wurde
                stepper.stop(); ///< Motor stoppen
                return true; ///< Positive Rückmeldung der Funktion
            }
        }
    }
    lastEndstopStatus = endstopReading; ///< Aktualisierung des letzten Status des Endschalters
    return false; ///< Negative Rückmeldung der Funktion
}

/**
 * @brief Funktion für die Ausführung des Interrupts
 * 
 * Diese Funktion wird aufgerufen, wenn der Encoder-Taster gedrückt wird.
 */
void handleInterrupt() {
    stepper.stop(); ///< Stoppen des Motors
    updateDisplay(messages[3], messages[4]); ///< Aktualisierung der Anzeige auf dem Display
}
