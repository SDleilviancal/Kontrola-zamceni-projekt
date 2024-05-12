TaskHandle_t Task1;  // Handle pro úlohu Task1
TaskHandle_t Task2;  // Handle pro úlohu Task2
SemaphoreHandle_t sem;  // Handle pro semafor

#include <WiFi.h>
#include <PubSubClient.h>

// Nastavení WiFi
const char* ssid = "Tp-ling";  // Název vaší WiFi sítě
const char* password = "hromasova";  // Heslo k WiFi síti

// Nastavení MQTT
const char* mqtt_server = "test.mosquitto.org"; // Adresa MQTT brokera
WiFiClient espClient;  // WiFi klient
PubSubClient client(espClient);  // MQTT klient

const int fsrPin = 32;  // Pin pro připojení FSR senzoru
int fsrValue = 0;  // Proměnná pro hodnotu ze senzoru
int fsrValue_posledni = 0;  // Proměnná pro poslední hodnotu ze senzoru
int kontrola = 0;  // Proměnná pro kontrolu stavu

void Task1code(void * pvParameters) {
  Serial.print("Task1 běží na jádře ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    // Přečtení hodnoty z tlakového senzoru
    fsrValue = analogRead(fsrPin);
    Serial.println(fsrValue);

    if (isnan(fsrValue)) {
      Serial.println("Chyba při čtení ze senzoru FSR!");
      return;
    }

    // Zasílání signálu pro probuzení Task2code
    if (abs(fsrValue - fsrValue_posledni) >= 100) {
      if(fsrValue >= 2000){
        kontrola = 1;
      } else if(fsrValue >= 0){
        kontrola = 0;
      }
    }

    fsrValue_posledni = fsrValue;
    delay(5000);
  }
}

void Task2code(void *pvParameters) {
  
  Serial.print("Task2 běží na jádře ");
  Serial.println(xPortGetCoreID());
 

  for (;;) {
    // Čekání na signál od Task1code
    xSemaphoreTake(sem, portMAX_DELAY);

    // Pokud není zařízení připojeno k WiFi, připojí se
    if(WiFi.status() != WL_CONNECTED){
      setup_wifi();
      client.setServer(mqtt_server, 1883);
    }

    // Pokud není klient připojen k MQTT, pokusí se znovu připojit
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    // Vytvoření JSON objektu pro odeslání
    String jsonData = "{\"kontrola\": " + String(kontrola) + "}";

    // Odeslání zprávy na MQTT
    client.publish("FG/FSR", jsonData.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(fsrPin, INPUT);

  // Vytvoření semaforu
  sem = xSemaphoreCreateBinary();

  // Vytvoření úlohy pro Task1code
  xTaskCreatePinnedToCore(
      Task1code,   /* Funkce úlohy */
      "Task1",     /* Název úlohy */
      10000,       /* Velikost zásobníku úlohy */
      NULL,        /* Parametr úlohy */
      1,           /* Priorita úlohy */
      &Task1,      /* Handle pro úlohu */
      0);          /* Přiřazení úlohy k jádru 0 */
  delay(500);

  // Vytvoření úlohy pro Task2code
  xTaskCreatePinnedToCore(
      Task2code,   /* Funkce úlohy */
      "Task2",     /* Název úlohy */
      10000,       /* Velikost zásobníku úlohy */
      NULL,        /* Parametr úlohy */
      1,           /* Priorita úlohy */
      &Task2,      /* Handle pro úlohu */
      1);          /* Přiřazení úlohy k jádru 1 */
  delay(500);
}

void setup_wifi() {
  delay(10);
  // Připojení k WiFi síti
  Serial.println();
  Serial.print("Připojování k ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi připojeno");
  Serial.println("IP adresa: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Opětovné připojení k MQTT
  while (!client.connected()) {
    Serial.print("Připojování k MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("připojeno");
    } else {
      Serial.print("selhalo, rc=");
      Serial.print(client.state());
      Serial.println(" zkouším znovu za 5 sekund");
      delay(5000);
    }
  }
}

void loop() {
}
