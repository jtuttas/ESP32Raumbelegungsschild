
// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Dispay ePaper for Arduino : https://forum.arduino.cc/index.php?topic=436411.0

// mapping suggestion from Waveshare SPI e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

// mapping suggestion for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11
#include <Arduino.h>
#include <PubSubClient.h>
#include "esp_wpa2.h"
#include <WiFi.h>
#include <GxEPD.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// GDEH029A1
#include <GxGDEH029A1/GxGDEH029A1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

#include <BitmapGraphics.h>
#include <ArduinoJson.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define uS_TO_S_FACTOR 1000000

GxIO_Class io(SPI, SS, 22, 21);
GxEPD_Class display(io, 16, 4);

// TODO: Hier WLAN Zugangsdaten anpassen
#define EAP_ID "tuttas"
#define EAP_USERNAME "tuttas"
#define EAP_PASSWORD "geheim"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);
//static const char* ssid = "MMBBS-Intern";
const char *ssid = "FRITZ!Box Fon WLAN 7390";
const char *password = "geheim";
const char *mqtt_server = "service.joerg-tuttas.de";

// Definition der Schulstunden (plus 1 Minute)
// 8:01,8:46,9:31,9:51,10:36,11:21,11:41,12:26,13:11,13:31,14:16,15:01,15:21,16:06,16:51
const int hUpdate[] = {8, 8, 9, 9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 16};
const int mUpdate[] = {1, 46, 31, 51, 36, 21, 41, 26, 11, 31, 16, 1, 21, 6, 51};
WiFiClient espClient;
PubSubClient client(espClient);
// TODO: topic entfernen
RTC_RODATA_ATTR char topic[1023] = "mmbbs/2/06";
const int BUTTON_PIN = 0;
RTC_DATA_ATTR int bootCount = 0;
static const uint8_t LED_BUILTIN = 16;
WiFiServer server(80);
const String contentTypeText = "Content-Type: text/html\r\n";
const String header_ok = "HTTP/1.1 200 OK\r\n";
const String htmlHead = "<!DOCTYPE html><html lang=de><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\"><title>Raumschild Config</title></head><body><h1>Raumschild Konfiguration</h1>";
const String htmlForm = "<form action=\"/\" method=\"GET\"><input id=\"topic\" type=\"text\" name=\"topic\" value=\"#topic#\" placeholder=\"topic\"><button type=\"submit\">Absenden</button></form>";

// Anzahl von Sekunden f. den Deep Sleep
unsigned long deepSpeeptime = 0;
String urlDecode(String urlChars)
{

    urlChars.replace("%0D%0A", String('\n'));
    urlChars.replace("+", " ");
    urlChars.replace("%20", " ");
    urlChars.replace("%21", "!");
    urlChars.replace("%22", String(char('\"')));
    urlChars.replace("%23", "#");
    urlChars.replace("%24", "$");
    urlChars.replace("%25", "%");
    urlChars.replace("%26", "&");
    urlChars.replace("%27", String(char(39)));
    urlChars.replace("%28", "(");
    urlChars.replace("%29", ")");
    urlChars.replace("%2A", "*");
    urlChars.replace("%2B", "+");
    urlChars.replace("%2C", ",");
    urlChars.replace("%2D", "-");
    urlChars.replace("%2E", ".");
    urlChars.replace("%2F", "/");
    urlChars.replace("%30", "0");
    urlChars.replace("%31", "1");
    urlChars.replace("%32", "2");
    urlChars.replace("%33", "3");
    urlChars.replace("%34", "4");
    urlChars.replace("%35", "5");
    urlChars.replace("%36", "6");
    urlChars.replace("%37", "7");
    urlChars.replace("%38", "8");
    urlChars.replace("%39", "9");
    urlChars.replace("%3A", ":");
    urlChars.replace("%3B", ";");
    urlChars.replace("%3C", "<");
    urlChars.replace("%3D", "=");
    urlChars.replace("%3E", ">");
    urlChars.replace("%3F", "?");
    urlChars.replace("%40", "@");
    urlChars.replace("%41", "A");
    urlChars.replace("%42", "B");
    urlChars.replace("%43", "C");
    urlChars.replace("%44", "D");
    urlChars.replace("%45", "E");
    urlChars.replace("%46", "F");
    urlChars.replace("%47", "G");
    urlChars.replace("%48", "H");
    urlChars.replace("%49", "I");
    urlChars.replace("%4A", "J");
    urlChars.replace("%4B", "K");
    urlChars.replace("%4C", "L");
    urlChars.replace("%4D", "M");
    urlChars.replace("%4E", "N");
    urlChars.replace("%4F", "O");
    urlChars.replace("%50", "P");
    urlChars.replace("%51", "Q");
    urlChars.replace("%52", "R");
    urlChars.replace("%53", "S");
    urlChars.replace("%54", "T");
    urlChars.replace("%55", "U");
    urlChars.replace("%56", "V");
    urlChars.replace("%57", "W");
    urlChars.replace("%58", "X");
    urlChars.replace("%59", "Y");
    urlChars.replace("%5A", "Z");
    urlChars.replace("%5B", "[");
    urlChars.replace("%5C", String(char(65)));
    urlChars.replace("%5D", "]");
    urlChars.replace("%5E", "^");
    urlChars.replace("%5F", "_");
    urlChars.replace("%60", "`");
    urlChars.replace("%61", "a");
    urlChars.replace("%62", "b");
    urlChars.replace("%63", "c");
    urlChars.replace("%64", "d");
    urlChars.replace("%65", "e");
    urlChars.replace("%66", "f");
    urlChars.replace("%67", "g");
    urlChars.replace("%68", "h");
    urlChars.replace("%69", "i");
    urlChars.replace("%6A", "j");
    urlChars.replace("%6B", "k");
    urlChars.replace("%6C", "l");
    urlChars.replace("%6D", "m");
    urlChars.replace("%6E", "n");
    urlChars.replace("%6F", "o");
    urlChars.replace("%70", "p");
    urlChars.replace("%71", "q");
    urlChars.replace("%72", "r");
    urlChars.replace("%73", "s");
    urlChars.replace("%74", "t");
    urlChars.replace("%75", "u");
    urlChars.replace("%76", "v");
    urlChars.replace("%77", "w");
    urlChars.replace("%78", "x");
    urlChars.replace("%79", "y");
    urlChars.replace("%7A", "z");
    urlChars.replace("%7B", String(char(123)));
    urlChars.replace("%7C", "|");
    urlChars.replace("%7D", String(char(125)));
    urlChars.replace("%7E", "~");
    urlChars.replace("%7F", "Â");
    urlChars.replace("%80", "`");
    urlChars.replace("%81", "Â");
    urlChars.replace("%82", "â");
    urlChars.replace("%83", "Æ");
    urlChars.replace("%84", "â");
    urlChars.replace("%85", "â¦");
    urlChars.replace("%86", "â");
    urlChars.replace("%87", "â¡");
    urlChars.replace("%88", "Ë");
    urlChars.replace("%89", "â°");
    urlChars.replace("%8A", "Å");
    urlChars.replace("%8B", "â¹");
    urlChars.replace("%8C", "Å");
    urlChars.replace("%8D", "Â");
    urlChars.replace("%8E", "Å½");
    urlChars.replace("%8F", "Â");
    urlChars.replace("%90", "Â");
    urlChars.replace("%91", "â");
    urlChars.replace("%92", "â");
    urlChars.replace("%93", "â");
    urlChars.replace("%94", "â");
    urlChars.replace("%95", "â¢");
    urlChars.replace("%96", "â");
    urlChars.replace("%97", "â");
    urlChars.replace("%98", "Ë");
    urlChars.replace("%99", "â¢");
    urlChars.replace("%9A", "Å¡");
    urlChars.replace("%9B", "âº");
    urlChars.replace("%9C", "Å");
    urlChars.replace("%9D", "Â");
    urlChars.replace("%9E", "Å¾");
    urlChars.replace("%9F", "Å¸");
    urlChars.replace("%A0", "Â");
    urlChars.replace("%A1", "Â¡");
    urlChars.replace("%A2", "Â¢");
    urlChars.replace("%A3", "Â£");
    urlChars.replace("%A4", "Â¤");
    urlChars.replace("%A5", "Â¥");
    urlChars.replace("%A6", "Â¦");
    urlChars.replace("%A7", "Â§");
    urlChars.replace("%A8", "Â¨");
    urlChars.replace("%A9", "Â©");
    urlChars.replace("%AA", "Âª");
    urlChars.replace("%AB", "Â«");
    urlChars.replace("%AC", "Â¬");
    urlChars.replace("%AE", "Â®");
    urlChars.replace("%AF", "Â¯");
    urlChars.replace("%B0", "Â°");
    urlChars.replace("%B1", "Â±");
    urlChars.replace("%B2", "Â²");
    urlChars.replace("%B3", "Â³");
    urlChars.replace("%B4", "Â´");
    urlChars.replace("%B5", "Âµ");
    urlChars.replace("%B6", "Â¶");
    urlChars.replace("%B7", "Â·");
    urlChars.replace("%B8", "Â¸");
    urlChars.replace("%B9", "Â¹");
    urlChars.replace("%BA", "Âº");
    urlChars.replace("%BB", "Â»");
    urlChars.replace("%BC", "Â¼");
    urlChars.replace("%BD", "Â½");
    urlChars.replace("%BE", "Â¾");
    urlChars.replace("%BF", "Â¿");
    urlChars.replace("%C0", "Ã");
    urlChars.replace("%C1", "Ã");
    urlChars.replace("%C2", "Ã");
    urlChars.replace("%C3", "Ã");
    urlChars.replace("%C4", "Ã");
    urlChars.replace("%C5", "Ã");
    urlChars.replace("%C6", "Ã");
    urlChars.replace("%C7", "Ã");
    urlChars.replace("%C8", "Ã");
    urlChars.replace("%C9", "Ã");
    urlChars.replace("%CA", "Ã");
    urlChars.replace("%CB", "Ã");
    urlChars.replace("%CC", "Ã");
    urlChars.replace("%CD", "Ã");
    urlChars.replace("%CE", "Ã");
    urlChars.replace("%CF", "Ã");
    urlChars.replace("%D0", "Ã");
    urlChars.replace("%D1", "Ã");
    urlChars.replace("%D2", "Ã");
    urlChars.replace("%D3", "Ã");
    urlChars.replace("%D4", "Ã");
    urlChars.replace("%D5", "Ã");
    urlChars.replace("%D6", "Ã");
    urlChars.replace("%D7", "Ã");
    urlChars.replace("%D8", "Ã");
    urlChars.replace("%D9", "Ã");
    urlChars.replace("%DA", "Ã");
    urlChars.replace("%DB", "Ã");
    urlChars.replace("%DC", "Ã");
    urlChars.replace("%DD", "Ã");
    urlChars.replace("%DE", "Ã");
    urlChars.replace("%DF", "Ã");
    urlChars.replace("%E0", "Ã");
    urlChars.replace("%E1", "Ã¡");
    urlChars.replace("%E2", "Ã¢");
    urlChars.replace("%E3", "Ã£");
    urlChars.replace("%E4", "Ã¤");
    urlChars.replace("%E5", "Ã¥");
    urlChars.replace("%E6", "Ã¦");
    urlChars.replace("%E7", "Ã§");
    urlChars.replace("%E8", "Ã¨");
    urlChars.replace("%E9", "Ã©");
    urlChars.replace("%EA", "Ãª");
    urlChars.replace("%EB", "Ã«");
    urlChars.replace("%EC", "Ã¬");
    urlChars.replace("%ED", "Ã­");
    urlChars.replace("%EE", "Ã®");
    urlChars.replace("%EF", "Ã¯");
    urlChars.replace("%F0", "Ã°");
    urlChars.replace("%F1", "Ã±");
    urlChars.replace("%F2", "Ã²");
    urlChars.replace("%F3", "Ã³");
    urlChars.replace("%F4", "Ã´");
    urlChars.replace("%F5", "Ãµ");
    urlChars.replace("%F6", "Ã¶");
    urlChars.replace("%F7", "Ã·");
    urlChars.replace("%F8", "Ã¸");
    urlChars.replace("%F9", "Ã¹");
    urlChars.replace("%FA", "Ãº");
    urlChars.replace("%FB", "Ã»");
    urlChars.replace("%FC", "Ã¼");
    urlChars.replace("%FD", "Ã½");
    urlChars.replace("%FE", "Ã¾");
    urlChars.replace("%FF", "Ã¿");

    return urlChars;
}

void showPartialUpdate(String room, String teacher, String lesson, String course)
{
    const char *name1 = "FreeSans18pt7b";
    const GFXfont *f1 = &FreeSans18pt7b;
    const char *name2 = "FreeSans12pt7b";
    const GFXfont *f2 = &FreeSans12pt7b;

    /**
 * Raum
 * */
    uint16_t box_x = 5;
    uint16_t box_y = 27;
    uint16_t box_w = 164;
    uint16_t box_h = 26;
    uint16_t cursor_y = box_y + 28;

    display.setRotation(135);
    display.setFont(f1);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(room);
    display.updateWindow(box_x, box_y, box_w, box_h, true);

    /**
 * Klasse
 * */
    box_x = 5;
    box_y = 89;
    box_w = 164;
    box_h = 26;
    cursor_y = box_y + 30;
    display.setFont(f1);

    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(course);
    display.updateWindow(box_x, box_y, box_w, box_h, true);

    /**
 * Lehrer
 * */

    box_x = 177;
    box_y = 27;
    box_w = 115;
    box_h = 26;
    cursor_y = box_y + 28;
    display.setFont(f2);

    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(teacher);
    display.updateWindow(box_x, box_y, box_w, box_h, true);

    /**
 * Unterricht
 * */

    box_x = 177;
    box_y = 89;
    box_w = 115;
    box_h = 26;
    cursor_y = box_y + 30;
    display.setFont(f2);

    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(lesson);
    display.updateWindow(box_x, box_y, box_w, box_h, true);
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(payload);
    showPartialUpdate(root["Raum"], root["TEACHER_ID"], root["Fach"], root["KNAME"]);
    //showPartialUpdate(root["Raum"],"TU","LF4",root["KNAME"]);
    Serial.print("go to sleep for (s) ");
    Serial.println(deepSpeeptime);
    int esp_err_t = esp_sleep_enable_timer_wakeup(deepSpeeptime * uS_TO_S_FACTOR);
    Serial.print("esp_err_t=");
    Serial.println(esp_err_t);
    /**
     * ESP_OK 0) on success
     * ESP_ERR_INVALID_ARG (0x102 ) if value is out of range (TBD)
     * */

    client.disconnect();
    delay(500);
    esp_deep_sleep_start();
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP32--Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str()))
        {
            client.subscribe(topic);
            Serial.print("connected and subscribe to:");
            Serial.println(topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void normalStartup()
{
    Serial.write("\r\nConnect to WLAN");

    // TODO: WPA2 enterprise magic starts here
    /*
    WiFi.disconnect(true);      
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ID, strlen(EAP_ID));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); 
    esp_wifi_sta_wpa2_ent_enable(&config);
    WiFi.begin(ssid);
    */
    // WPA2 enterprise magic ends here

    // TODO: Normale WLAN Verbindung
    WiFi.begin(ssid, password);

    Serial.println();
    Serial.println("Waiting for connection and IP Address from DHCP");
    int count = 0;
    while (WiFi.status() != WL_CONNECTED && count <= 20)
    {
        delay(1000);
        Serial.print(".");
        if (count % 2 == 0)
        {
            digitalWrite(LED_BUILTIN, LOW);
        }
        else
        {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        count++;
        if (count == 20)
        {
            Serial.println("Can't connect to WLAN, go to sleep!");
            if (bootCount == 1)
            {
                bootCount = 0;
            }
            esp_sleep_enable_timer_wakeup(5 * uS_TO_S_FACTOR);
            esp_deep_sleep_start();
        }
    }
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.write("\r\nIP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    digitalWrite(LED_BUILTIN, HIGH);

    timeClient.begin();
    timeClient.forceUpdate();
    Serial.print("Current time is ");
    Serial.println(timeClient.getFormattedTime());

    // Berechnen der Zeitspanne in s bis zum nächsten Aufwachen
    int h = timeClient.getHours();
    int m = timeClient.getMinutes();
    int l = sizeof(hUpdate) / sizeof(hUpdate[0]);
    Serial.print("Number of Timeslots=");
    Serial.println(l);
    bool found = false;
    for (int i = 0; i < l; i++)
    {
        Serial.print("i=");
        Serial.print(i);
        Serial.print(" h=");
        Serial.print(h);
        Serial.print(" m=");
        Serial.print(m);
        Serial.println(" ");
        if (hUpdate[i] == h)
        {
            if (mUpdate[i] > m)
            {
                Serial.print("Next Wakeup at h=");
                Serial.println(hUpdate[i]);
                Serial.print("Next Wakeup at m=");
                Serial.println(mUpdate[i]);
                deepSpeeptime = (hUpdate[i] - h) * 3600 + (mUpdate[i] - m) * 60;
                Serial.print("Set DeepSleep (s) to:");
                Serial.println(deepSpeeptime);
                found = true;
                break;
            }
        }
        if (hUpdate[i] > h)
        {
            Serial.print("Next Wakeup at h=");
            Serial.println(hUpdate[i]);
            Serial.print("Next Wakeup at m=");
            Serial.println(mUpdate[i]);
            deepSpeeptime = (hUpdate[i] - h) * 3600 + (mUpdate[i] - m) * 60;
            Serial.print("Set DeepSleep (s) to:");
            Serial.println(deepSpeeptime);
            found = true;
            break;
        }
    }
    if (!found)
    {
        Serial.println("Nächster Wakeup um 8:01");
        if (h < 8)
        {
            deepSpeeptime = (7 - h) * 3600 + (61 - m) * 60;
        }
        else
        {
            deepSpeeptime = ((23 - h) + 8) * 3600 + ((59 - m) + 1) * 60;
        }
        Serial.print("Set DeepSleep (s) to:");
        Serial.println(deepSpeeptime);
    }

    if (!client.connected())
    {
        reconnect();
        Serial.println("Connect to MQTT Server");
    }
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        client.loop();
        delay(500);
    }
    Serial.println("go to sleep without msg from MQTT Server");
    esp_sleep_enable_timer_wakeup(15 * 60 * uS_TO_S_FACTOR);
    client.disconnect();
    delay(500);
    esp_deep_sleep_start();
}

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);
    delay(500);
    if (bootCount==1) {
        delay(1000);
    }
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("setup");
    //Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));
    display.init();
    Serial.println("display Init");
    //display.drawExampleBitmap(gImage_splash, 0, 0, 128, 296, GxEPD_BLACK);
    //display.update();
    if (bootCount == 1)
    {
        if (digitalRead(BUTTON_PIN) == LOW)
        { // Check if button has been pressed
            display.drawExampleBitmap(ap, sizeof(ap), GxEPD::bm_default | GxEPD::bm_partial_update);
            display.drawExampleBitmap(ap, 0, 0, 128, 296, GxEPD_BLACK);
            display.update();
            Serial.println("Rebuild Image");
            while (digitalRead(BUTTON_PIN) == LOW)
                ; // Wait for button to be released
            Serial.println("Starte AP Mode");
             digitalWrite(LED_BUILTIN, HIGH);
            WiFi.mode(WIFI_AP);
            Serial.println("Starte WLAN Access Point");
            WiFi.softAP("ESP32Namensschild", "");
            Serial.write("\r\nStarte Server");
            server.begin();
        }
        else
        {
            display.drawExampleBitmap(gImage_gui, sizeof(gImage_gui), GxEPD::bm_default | GxEPD::bm_partial_update);
            display.drawExampleBitmap(gImage_gui, 0, 0, 128, 296, GxEPD_BLACK);
            display.update();
            Serial.println("Rebuild Image");
            if (String(topic).length() == 0)
            {
                Serial.println("Starte AP Mode");
                WiFi.mode(WIFI_AP);
                Serial.println("Starte WLAN Access Point");
                WiFi.softAP("ESP32Namensschild", "");
                Serial.write("\r\nStarte Server");
                server.begin();
            }
            else
            {
                normalStartup();
            }
        }
    }
    else
    {
        normalStartup();
    }
}
void loop()
{
    WiFiClient clientS = server.available();
    if (clientS)
    {
        Serial.write("\r\nnew Client:");
        String request = clientS.readStringUntil('\r');
        Serial.println(">" + request + "<");
        String req = request.substring(request.indexOf("/"), request.lastIndexOf(" "));
        Serial.println(req);
        String s = header_ok;
        s += contentTypeText;
        s += String("\r\n");
        s += htmlHead;
        String t = String(topic);
        if (req.indexOf("topic=") != -1)
        {
            t = req.substring(req.indexOf("topic=") + 6);
            t = urlDecode(t);
            t.toCharArray(topic, 1023, 0);
            s += "<h2>Set Topic to " + t + " and reboot</h2>";
            s += "</body></html>";
            clientS.print(s);
            clientS.flush();
            clientS.stop();
            bootCount = 0;
            esp_sleep_enable_timer_wakeup(1 * 10 * uS_TO_S_FACTOR);
            delay(500);
            esp_deep_sleep_start();
        }
        else
        {
            String form = htmlForm;
            form.replace("#topic#", t);
            s += form;
            s += "</body></html>";
            clientS.print(s);
            clientS.flush();
            clientS.stop();
        }
    }
}
