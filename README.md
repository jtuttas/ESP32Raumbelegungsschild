#  Raumbelegung
## Installation des Powershell Scripts
Das Powershell Skript verbindet sich einmal am Tag mit einer Stundenplansoftware und läd die Raumbelegung in ein JSON *untisData*. Die Räume werden in einem JSON *untisRooms* abgelegt. Ferner legt es in $PSScriptRoot ein JSON mit dem Namen *untis.json* an, dieses enthält den aktuellen Tag (DayOfYear). 

Damit sich das Powershell Script mit Webuntis verbinden kann müssen den Credentials einmal richtig gesetzt werden und in der Datei *$Home/keys.json* gespeichert werden.

Fener nutzt das Powershell Script die Library *M2Mqtt*, die DLL dieser Library muss im Skript richtig gesetzt werden!

## Installation des (Node.js) Webservers
Der Webserver ist in TypeScript geschrieben und nutzt Node.js.  Um die notwendigen Abhängigkeiten zu installieren muss zunächst.
```
npm install
```
aufgerufen werden. Anschließend kann der Server über
```
node index.js
```
gestartet werden. Der Server läuft default auf dem Port 3000 und kann über http://127.0.0.1:3000 aufgerufen werden. Die konfiguration des Servers geschient über eine Datei *config.json* mit folgendem Aussehen:
```json
{
    "version": "1.0",
    "mqttServer": "service.joerg-tuttas.de",
    "server": "ws://127.0.0.1",
    "port": "3000"
}
```
Dabei haben die Items folgende Bedeutung:
- mqttServer: Adresse des MQTT Servers mit dem sich verbunden wird
- server: Adresse des Websocket
- port: Port des Websockets

## ESP32 Raumbelegungsanzeige
Ist kein Topic hinterlegt oder wird beim Starten des ESP32 der interne GPIO Button gedrückt, so startet der ESP32 im Access Point Modus. Es wird ein offener WLAN angezeigt. Wird sich mit diesem WLAN verbunden kann die Kanfigurationsseite durch Eingabe von *http://192.168.4.1* aufgerufen werden. Es erscheint folgende Webseite:

![config1](ESP32Raumschild/screen1.PNG)

Hier kann der MQTT Topic eingegeben werden. Ein Klick auf "Absenden" startet den ESP32 dann neu.

![config2](ESP32Raumschild/screen2.PNG)


### Verkabelung

Display | ESP32 
--- | --- 
BUSY | D4 
RST | D21 
DC | D22 
CS | D5 
CLK | D18 
DIN | D23 
GND | GND 
3.3V | 3.3V 

## Video
[![ESP32 E-Ink Namensschild](http://img.youtube.com/vi/hAL28c2Mg04/0.jpg)](http://www.youtube.com/watch?v=hAL28c2Mg04)