#include <U8g2lib.h>

const int vochtigheidSensorPin = A0;   // Pin waar de vochtigheidssensor is aangesloten
const int waterLevelSensorPin = A1;    // Pin waar de waterpeilsensor is aangesloten (analoog)
const int relaisPin = 2;               // Pin waar het relais is aangesloten
const int buzzerPin = 3;               // Pin waar de buzzer is aangesloten
const int knopPin = 4;                 // Pin waar de knop is aangesloten
const int WaterLevelThreshold = 400;   // Drempelwaarde voor het Waterniveau
const int VochtigheidThreshold = 500;  // Drempelwaarde de vochtigheid

bool knopIngedrukt = false;  // Een variabele om de knopstatus bij te houden
bool zoemerActief = false;   // Een variabele om de zoemerstatus bij te houden

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

void setup() {
  u8g2.begin();
  pinMode(vochtigheidSensorPin, INPUT);  // Stel de vochtigheidssensorpin in als invoer
  pinMode(waterLevelSensorPin, INPUT);   // Stel de waterpeilsensorpin in als invoer (analoog)
  pinMode(relaisPin, OUTPUT);            // Stel de relaispin in als uitvoer
  pinMode(buzzerPin, OUTPUT);            // Stel de buzzerpin in als uitvoer
  pinMode(knopPin, INPUT_PULLUP);        // Stel de knoppin in als invoer met pull-up weerstand

  Serial.begin(9600);  // Start de seriële communicatie met een baudrate van 9600
}

void loop() {
  int vochtigheid = analogRead(vochtigheidSensorPin);  // Lees de vochtigheidssensorwaarde
  int waterLevel = analogRead(waterLevelSensorPin);    // Lees de analoge waterpeilsensorwaarde

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_profont15_mr);  // Kies een lettertype

    u8g2.setCursor(0, 20);
    u8g2.print("WaterLevel: ");
    // Controleer of het waterpeil boven de drempelwaarde ligt en toon 1, anders toon 0
    if (waterLevel > WaterLevelThreshold) {
      u8g2.print("OK");
    } else {
      u8g2.print("Te Laag");
    }

    u8g2.setCursor(0, 40);  // Stel de positie in waar de tekst wordt weergegeven
    u8g2.print("Vochtigheid: ");
    u8g2.print(vochtigheid);

    // Controleer of de zoemer actief is en toon de zoemerstatus
    if (zoemerActief) {
      u8g2.setCursor(0, 60);
      u8g2.print("Alarm");
    }
  } while (u8g2.nextPage());

  Serial.print("Vochtigheid: ");
  Serial.println(vochtigheid);  // Stuur de vochtigheidswaarde naar de seriële monitor
  Serial.print("WaterLevel: ");
  Serial.println(waterLevel);  // Stuur de analoge waterpeilsensorwaarde naar de seriële monitor

  // Controleer of de knop is ingedrukt
  if (digitalRead(knopPin) == LOW) {
    knopIngedrukt = true;
  } else {
    knopIngedrukt = false;
  }

  Serial.print("Knop ingedrukt: ");
  Serial.println(knopIngedrukt);  // Toon de knopstatus in de seriële monitor

  // Controleer het waterpeil (pas de drempelwaarde aan)
  if ((waterLevel > WaterLevelThreshold && vochtigheid < VochtigheidThreshold) || (digitalRead(knopPin) == LOW && waterLevel > WaterLevelThreshold)) {
    // Schakel het relais in
    digitalWrite(relaisPin, HIGH);
    // Schakel de buzzer uit, aangezien het waterpeil boven de drempelwaarde ligt of de knop is ingedrukt
    digitalWrite(buzzerPin, LOW);
    zoemerActief = false;
  } else if (vochtigheid > VochtigheidThreshold) {
    digitalWrite(relaisPin, LOW);
    // Schakel de buzzer uit, aangezien het waterpeil boven de drempelwaarde ligt of de knop is ingedrukt
    digitalWrite(buzzerPin, LOW);
    zoemerActief = false;
    // Schakel de buzzer alleen in als het waterpeil onder de drempelwaarde ligt
    if (waterLevel <= WaterLevelThreshold) {
      digitalWrite(buzzerPin, HIGH);
      zoemerActief = true;
    } else {
      digitalWrite(buzzerPin, LOW);
      zoemerActief = false;
    }
  }

  delay(200);  // Wacht 200 milliseconde voordat je opnieuw controleert (pas de interval naar wens aan)
}
