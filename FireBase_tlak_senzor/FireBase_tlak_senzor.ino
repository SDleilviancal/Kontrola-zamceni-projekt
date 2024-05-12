#include <WiFi.h>  // Importuje knihovnu pro WiFi připojení.
#include <Firebase_ESP_Client.h>  // Importuje knihovnu pro komunikaci s Firebase.
#include "addons/TokenHelper.h"  // Importuje doplněk pro správu tokenů.
#include "addons/RTDBHelper.h"  // Importuje doplněk pro práci s Realtime Database v Firebase.

#define WIFI_SSID "Tp-ling"  // Definuje název WiFi sítě, ke které se bude připojovat.
#define WIFI_PASSWORD "hromasova"  // Definuje heslo k WiFi síti.
#define API_KEY "AIzaSyDvkkze1-iYWDM3KZMWCIkQUFJ7qqFoXKo"  // Definuje API klíč pro přístup k Firebase.
#define DATABASE_URL "https://zameke-esp32-kontrola-default-rtdb.europe-west1.firebasedatabase.app/"  // Definuje URL adresu Realtime Database v Firebase.

const int led_zelena = 13;  // Definuje pin pro zelenou LED.
const int led_cervena = 12;  // Definuje pin pro červenou LED.
const int fsrPin = 32;  // Definuje pin pro připojení FSR senzoru.
int fsrValue = 0;  // Inicializuje proměnnou pro uložení hodnoty ze senzoru.
int fsrValue_posledni = 0;  // Inicializuje proměnnou pro uložení poslední hodnoty ze senzoru.
int kontrola = 0;  // Inicializuje proměnnou pro kontrolu stavu.

FirebaseData fbdo;  // Vytváří objekt pro komunikaci s Firebase.
FirebaseAuth auth;  // Vytváří objekt pro autentizaci.
FirebaseConfig config;  // Vytváří konfigurační objekt pro Firebase.

unsigned long sendDataPrevMillis = 0;  // Inicializuje proměnnou pro časování odesílání dat.
bool signupOk = false;  // Inicializuje proměnnou pro indikaci úspěšného přihlášení k Firebase.

void setup() {
  pinMode(led_zelena, OUTPUT);  // Nastavuje pin pro zelenou LED jako výstup.
  pinMode(led_cervena, OUTPUT);  // Nastavuje pin pro červenou LED jako výstup.
  Serial.begin(115200);  // Inicializuje sériovou komunikaci.

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Připojuje se k WiFi síti.

  // Čeká na úspěšné připojení k WiFi.
  while(WiFi.status() != WL_CONNECTED) {  
    Serial.print("."); 
    delay(300);
  }

  Serial.println();
  Serial.print("Připojeno s IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Nastavuje API klíč a URL adresu Realtime Database v Firebase.
  config.api_key = API_KEY;  
  config.database_url = DATABASE_URL;

  // Pokus o přihlášení k Firebase.
  if(Firebase.signUp(&config, &auth, "", "")) {  
    Serial.println("Přihlášení úspěšné");
    signupOk = true;
  } else {
    Serial.println("Při přihlašování došlo k chybě: ");
    Serial.println(config.signer.signupError.message.c_str());
  }

  // Nastavuje callback pro stav tokenu a inicializuje komunikaci s Firebase.
  config.token_status_callback = tokenStatusCallback;  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);  // Opětovně se připojuje k WiFi po ztrátě spojení.
}

void loop() {
  // Odesílá data do Realtime Database v Firebase.
  if(Firebase.ready() && signupOk && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0 )) {
    sendDataPrevMillis = millis();
    fsrValue = analogRead(fsrPin);
    if(Firebase.RTDB.setInt(&fbdo, "Sensor/fsrValue", fsrValue)) {  
      Serial.println(); Serial.print(fsrValue);
      Serial.print(" - úspěšně uloženo do: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    } else {
      Serial.println("Selhalo" + fbdo.errorReason());  
    }
  }

  // Přepíná stav zelené a červené LED podle hodnoty FSR senzoru.
  if(fsrValue > 1000) {  
    digitalWrite(led_cervena, LOW);
    digitalWrite(led_zelena, HIGH);
  } else {
    digitalWrite(led_zelena, LOW);
    digitalWrite(led_cervena, HIGH);
  }
}
