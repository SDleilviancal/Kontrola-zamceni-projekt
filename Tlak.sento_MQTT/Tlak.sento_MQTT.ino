#include <WiFi.h>  // Importuje knihovnu pro Wi-Fi připojení.
#include <PubSubClient.h>  // Importuje knihovnu pro MQTT komunikaci.

// Nastavení Wi-Fi
const char* ssid = "Tp-ling";      // Definuje název Wi-Fi sítě.
const char* password = "hromasova"; // Definuje heslo k Wi-Fi síti.

// Nastavení MQTT
const char* mqtt_server = "test.mosquitto.org"; // Definuje adresu MQTT brokera.
WiFiClient espClient;                           // Vytváří instanci klienta pro Wi-Fi.
PubSubClient client(espClient);                 // Vytváří instanci klienta pro MQTT.

const int fsrPin = 32;         // Definuje pin pro FSR senzor.
int fsrValue = 0;              // Inicializuje proměnnou pro uchování aktuální hodnoty FSR senzoru.
int fsrValue_posledni = 0;     // Inicializuje proměnnou pro uchování poslední hodnoty FSR senzoru.
int kontrola = 0;              // Inicializuje proměnnou pro kontrolu stavu.

void setup() {
  Serial.begin(115200);      // Spouští sériovou komunikaci s baudovou rychlostí 115200.
  pinMode(fsrPin, INPUT);    // Nastavuje pin pro FSR senzor jako vstupní.
}

void setup_wifi() {
  delay(10);  // Zpoždění pro stabilizaci.
  Serial.println();  // Vytiskne prázdný řádek na sériový port.
  Serial.print("Připojování k ");  // Vytiskne text "Připojování k ".
  Serial.println(ssid);  // Vytiskne název Wi-Fi sítě.
  
  WiFi.begin(ssid, password); // Připojuje se k Wi-Fi síti s definovaným názvem a heslem.

  while (WiFi.status() != WL_CONNECTED) {  // Čeká, dokud není zařízení úspěšně připojeno k Wi-Fi síti.
    delay(500);  // Zpoždění 500 ms.
    Serial.print(".");  // Vytiskne tečku na sériový port.
  }

  Serial.println("");  // Vytiskne prázdný řádek na sériový port.
  Serial.println("Wi-Fi připojeno");  // Vytiskne text "Wi-Fi připojeno" na sériový port.
  Serial.println("IP adresa: ");  // Vytiskne text "IP adresa: " na sériový port.
  Serial.println(WiFi.localIP());  // Vytiskne lokální IP adresu na sériový port.
}

void loop() {
  fsrValue = analogRead(fsrPin); // Čte hodnotu ze senzoru a ukládá ji do proměnné fsrValue.
  Serial.println(fsrValue);       // Vytiskne aktuální hodnotu FSR senzoru na sériový port.

  if (isnan(fsrValue)) {  // Kontroluje, zda je načtená hodnota FSR senzoru platná.
    Serial.println("Selhalo čtení z FSR senzoru!");  // Vytiskne chybové hlášení na sériový port.
    return;  // Okamžitě ukončí funkci loop().
  }

  if (abs(fsrValue - fsrValue_posledni) >= 100) { // Kontroluje, zda došlo ke změně hodnoty FSR senzoru o více než 100.
    WiFi.mode(WIFI_STA); // Nastavuje režim Wi-Fi na stanici.
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi(); // Pokud není zařízení připojeno k Wi-Fi, provádí se připojení.
      client.setServer(mqtt_server, 1883); // Nastavuje adresu a port MQTT brokera.
    }
    if (!client.connected()) {
      reconnect(); // Pokud není zařízení připojeno k MQTT, provádí se opětovné připojení.
    }
    client.loop(); // Udržuje spojení s MQTT serverem.

    if (fsrValue >= 2000) {
      kontrola = 1; // Pokud je hodnota FSR senzoru větší nebo rovna 2000, proměnná kontrola je nastavena na 1.
    } else if (fsrValue >= 0) {
      kontrola = 0; // Pokud je hodnota FSR senzoru menší než 2000, proměnná kontrola je nastavena na 0.
    }
    
    // Vytváří JSON objekt pro poslání na MQTT.
    String jsonData = "{\"kontrola\": " + String(kontrola) + "}";

    // Publikuje JSON objekt na MQTT s tématem "FG/FSR".
    client.publish("FG/FSR", jsonData.c_str());

    delay(5000); // Čeká 5 sekund.
  }

  fsrValue_posledni = fsrValue; // Ukládá aktuální hodnotu FSR senzoru do proměnné pro příští iteraci.
  WiFi.mode(WIFI_OFF);           // Vypíná Wi-Fi režim pro úsporu energie.
}

void reconnect() {
  // Opětovné připojení k MQTT.
  while (!client.connected()) {
    Serial.print("Připojování k MQTT...");  // Vytiskne text "Připojování k MQTT..." na sériový port.
    if (client.connect("ESP8266Client")) {  // Pokusí se připojit k MQTT brokeru s klientem "ESP8266Client".
      Serial.println("připojeno");  // Vytiskne text "připojeno" na sériový port, pokud je připojení úspěšné.
    } else {
      Serial.print("selhalo, rc=");  // Vytiskne text "selhalo, rc=" na sériový port.
      Serial.print(client.state());  // Vytiskne stav klienta MQTT na sériový port.
      Serial.println(" zkouším za 5 sekund znovu");  // Vytiskne text "zkouším za 5 sekund znovu" na sériový port.
      delay(5000);  // Čeká 5 sekund.
    }
  }
}
