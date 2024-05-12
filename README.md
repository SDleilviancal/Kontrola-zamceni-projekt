# Kontrola-zamceni-projekt - Delincak, Sliva

ESP32 zjišťuje změny stavu na talkovém senzoru a poté posílá data na Firebase. Z Firebase si tyto data bere Andriod Studio a podle této hodnoty vypisuje do mobilní aplikace, zdali je zamčeno nebo odemčeno.

Naším hlavním cílem v projektu je poskytnout lidem pohodlnou možnost kontroly stavu dveří jejich domu prostřednictvím jednoduché aplikace, kterou mohou používat odkudkoliv a kdykoliv. Instalace našeho zařízení je flexibilní a lze ji provést téměř kdekoli bez potřeby výměny zámku u dveří nebo dokonce celého rámu. To umožňuje univerzální využití nejen u dveří domu, ale také například u oken nebo dokonce dveří auta. Kromě toho jsme integrovali LED diody pro vizuální signalizaci stavu dveří, což je užitečné například v situaci, kdy člověk prochází kolem dveří a nemá u sebe mobilní telefon.

Postup:
Téměř celý den jsme se snažili přijít na originální nápad pro náš projekt. Jakmile jsme měli jasnou představu o projektu, sepsali jsme seznam potřebných součástek a programů. Následně jsme vše potřebné nakoupili a začali jsme testovat funkčnost.
Jako první možnost odesílání dat na cloud bylo pouhé zobrazení na HTML stránce pomocí IP adresy. Po diskuzi jsme se rozhodli, že toto nemá cenu ani programovat, protože by to fungovalo pouze v lokální síti.
Další možností bylo posílání dat ze senzoru na MQTT Mosquitto a pomocí aplikace tyto data odebírat. Mosquitto bylo až příliš složité na propojení s aplikací bez placených částí. Toto jsme zjistili až po dokončení kódu.
Dále jsme použili Arduino Cloud, to nám poskytlo jednoduché prostředí pro psaní kódu, který se automatický propojil s aplikací, kterou Arduino Cloud poskytuje. Z toho jsme sešli kvůli budoucím vylepšením a úpravám úspory energie.
Nakonec jsem se rozhodli použít Firebase a Android Studio. Firebase se snadno propojilo jak s Arduino IDE, tak s Andriod Studio. Díky tomuto výběru nás nic neomezuje jak v kódu, tak v aplikace.

Také jsme zkoušeli použít obě dvě jádra ESP32 pro úsporu enregie. Jedno jádro sloužilo ke čtení dat ze senzoru a druhé k posílení dat na cloud. Zjistili jsme, že místo uspory energie se začalo využívat ještě více energie, čili jsme od toho upustili.

Momentální spotřeba je 110mA - 150mA.

Použité součástky: - ESP32
                   - Tlakový senzor FSR402
                   - Červená a zelená LED dioda
                   - Odpor - 10kΩ
                   - V-A Mestr a měřič kapacity DC KWS-V20-USB tester (použito na zjištění spotřeby proudu a také mAh za určitý čas)
Použité programy: - Arduino IDE
                  - Android Studio
                  - Firebase

Zdroje, které byly použity při tvorbě peojektu:
https://developer.android.com/
https://developer.android.com/
https://chatgpt.com/
https://stackoverflow.com/
https://cedalo.com/blog/mqtt-on-android-guide-using-mosquitto/
https://firebase.google.com/docs
https://www.w3schools.com/
https://cdn-learn.adafruit.com/downloads/pdf/force-sensitive-resistor-fsr.pdf
https://deepbluembedded.com/esp32-sleep-modes-power-consumption/
https://lms.teleinformatika.eu/course/view.php?id=44#mapy
https://drive.google.com/file/d/1RAib5vfwIk0eB0e6A1n9jL11hRgoGNa-/view
Dalibor Slíva (bratr Martina Slívy)
