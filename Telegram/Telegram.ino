#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#define WIFI_SSID "XXXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXX"
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // время обновления
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;
void setup()
{
  pinMode(18, OUTPUT);
  Serial.begin(115200);
  delay(512);
  Serial.println();
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}
void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    text.toLowerCase();
    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";
    if ((text == "/ledon") || (text == "ledon"))
    {
      digitalWrite(18, HIGH);
      bot.sendMessage(chat_id, "Светодиод включен", "");
    }
    if ((text == "/ledoff") || (text == "ledoff"))
    {
      digitalWrite(18, LOW);
      bot.sendMessage(chat_id, "Светодиод выключен", "");
    }
    if ((text == "/hall") || (text == "hall"))
    {
      float val = hallRead();
      bot.sendMessage(chat_id, String(val, 1), "");
    }

    if ((text == "/start") || (text == "start") || (text == "/help") || (text == "help"))
    {
      bot.sendMessage(chat_id, "Привет, " + from_name + "!", "");
      bot.sendMessage(chat_id, "Я контроллер Йотик 32. Я принимаю следующие команды:", "");
      bot.sendMessage(chat_id, "/ledon или /ledoff - включить/выключить светодиод", "");
      bot.sendMessage(chat_id, "/hall - считать значение с датчика Холла", "");
      bot.sendMessage(chat_id, "Команды можно писать без слэша слева и буквами любого регистра", "");
    }
  }
}

