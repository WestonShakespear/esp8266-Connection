#include "ESP8266WiFi.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(5, 4, 2, 14, 12, 13);

WiFiServer server(80);

int lowestRSSI = -100;
int lowestRSSINum = 0;
int passLoca = 0;
int lowestLoca = -100;
int b = 0;

bool recognized = false;
bool nameSet = false;
bool ipSet = true;

String recogSSID[3] = {"SSID 1", "SSID 2", "SSID 3"};
String ssidPASS[3] = {"SSID 1 PSK", "SSID 2 PSK", "SSID 3 PSK"};

void startSOFT()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting Soft AP");

 // WiFi.softAP(ssidAP, passAP);

  
}

String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {
0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup()
{
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.clear();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  lcd.print("setup done");
  delay(100);

  lcd.clear();
  lcd.print("scan start");
  delay(100);

  int n = WiFi.scanNetworks();
  lcd.clear();
  lcd.print("scan done");
  delay(500);
  if (n == 0)
  {
    lcd.clear();
    lcd.print("no networks found");
    startSOFT();
  }
  else
  {
    lcd.clear();
    lcd.print(n);
    lcd.setCursor(0, 1);
    lcd.print("networks found");
    lcd.setCursor(0, 0);
    delay(100);
    for (int i = 0; i < n; ++i)
    {
      lcd.clear();
      lcd.print(WiFi.SSID(i));
      lcd.setCursor(15, 0);
      lcd.print((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      lcd.setCursor(0, 1);
      lcd.print("dB: ");
      lcd.setCursor(4, 1);
      lcd.print(WiFi.RSSI(i));
      lcd.setCursor(0, 0);

      for (int a = 0; a < 3; a++)
      {
        if (WiFi.SSID(i) == recogSSID[a])
        {
          lcd.setCursor(13, 1);
          lcd.print("Yes");
          lcd.setCursor(0, 0);
          recognized = true;
          passLoca = a;
        }
      }
      
      if (WiFi.RSSI(i) > lowestRSSI and recognized == true)
      {
        lowestRSSI = WiFi.RSSI(i);
        lowestRSSINum = i;
        lowestLoca = passLoca;
      }

      recognized = false;
      
      delay(250);
    }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lowest RSSI: ");
      lcd.setCursor(0, 1);
      lcd.print(lowestRSSI);
      delay(100);

      lcd.clear();
      lcd.print("Connecting to: ");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.SSID(lowestRSSINum));
      delay(250);

      //char* ssid = recogSSID[lowestLoca];
      //char* pass = ssidPASS[lowestLoca];

      if (lowestLoca == -100)
      {
        startSOFT();
      }

      String ssidSTR = recogSSID[lowestLoca];
      String passSTR = ssidPASS[lowestLoca];

      char ssid[100];
      char pass[100];

      ssidSTR.toCharArray(ssid, 100);
      passSTR.toCharArray(pass, 100);

      WiFi.begin(ssid, pass);

      lcd.clear();
      lcd.setCursor(0, 0);

      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        lcd.setCursor(b, 0);
        lcd.print(".");

        b += 1;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("IP Address:");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP());

      server.begin();
    
  }

}

void loop()
{

  if (nameSet)
  {
    
  }
  if (ipSet)
  {

  }
  
 WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  while(!client.available()){
    delay(1);
  }
  
  String req = client.readStringUntil('\r');
  String spl = req;
  client.flush();

  spl.remove(0, 5);
  spl = getValue(spl, ' ', 0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(spl);
  
  if (req.indexOf("/lcd/name") != -1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Weston");
    lcd.setCursor(0, 1);
    lcd.print("Shakespear");
    lcd.setCursor(0, 0);
  }
  else if (req.indexOf("/lcd/ip") != -1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP Address:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 0); 
  }
  else if (req.indexOf("/") != -1)
  {

  }
  
  client.flush();

  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n";
  s += "\r\n";
  s += "<!DOCTYPE HTML>\r\n";
  s += "<html>\r\n";
  s += "<head>\r\n";
  s += "<script type=\"text/javascript\" src=\"http://code.jquery.com/jquery-2.1.1.min.js\"></script>\r\n";
  s += "<script type=\"text/javascript\">\r\n";
  s += "alert(\"Welcome To The ESP8266 Interface Page!!\");\r\n";
  s += "function resetLink(ele)\r\n";
  s += "{\r\n";
  s += "var href=\"/\"+$(ele).val();\r\n";
  s += "$(\".ajax-link\").attr(\"href\", \"\"+href);\r\n";
  s += "}\r\n";
  s += "</script\r\n";
  s += "</head>\r\n";
  s += "<body>\r\n";
  s += "<div class=\"col-sm-6\"><h1>Display: </h1></div>\r\n";
  s += "<div class=\"col-sm-6\"><input type=\"text\" name=\"display_id\" id=\"display_id\" onchange=\"resetLink(this);\" clas$</div>\t\n";
  s += "<li><a class=\"ajax-link\" href=\"ajax/legal_notice.php?id=+ document.getElementById('display_id').value\">Send To ESP8266</a></li>\r\n";
  s += "</body>\r\n";
  s += "</html>\r\n";
  
  client.print(s);
  delay(1);

}


