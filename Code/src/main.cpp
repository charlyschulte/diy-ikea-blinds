// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl>
//byte MotorOutputPin1[NrOfMotorOutputs] = {24, 22, 25, 23};

// License: GPLv2
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <EEPROM.h>

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
#define MaxStepsToGo 31       //Die maximale umdrehung, die das relais von oben bis unten braucht
#define MotorRelaisPin 34     //DER RELAIS PIN
#define NrOfMotorOutputs 4    //Anzahl der Ausgänge zur Ansteuerung des Motors
#define NrOfPatternsOneStep 8 //Anzahl der Muster zur Ansteuerung eines Schritts
signed long next;
//Variabeln für Drehrichtung definieren
const byte MotorOff = 0;
const byte DirMotorRight = 1;
const byte DirMotorLeft = 2;
int incomingByte = 0;

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
IPAddress ip(172, 16, 0, 100);
IPAddress server(192, 168, 178, 51);

unsigned int ActStepNr = 0; //Aktueller Schritt
byte StepPattern[NrOfPatternsOneStep][NrOfMotorOutputs] =
    {
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 1},
};

//Motor Pins für Ausgänge 1-4 definieren
byte MotorOutputPin2[NrOfMotorOutputs] = {40, 38, 41, 39};
byte MotorOutputPin1[NrOfMotorOutputs] = {24, 22, 25, 23};
byte MotorOutputPin3[NrOfMotorOutputs] = {28, 26, 29, 27};

/////////////// Funktion für Motoransteuerung Anfang //////////////////
void MotorMove(byte MotorDir, unsigned int MotorSpeed, unsigned int StepsToGo, byte MotorOutputPin[NrOfMotorOutputs])
{

  //Motor Stop alle Ausgänge auf LOW schalten
  if (MotorDir == MotorOff)
  {
    for (byte i = 0; i < NrOfMotorOutputs; i++)
    {
      digitalWrite(MotorOutputPin[i], LOW);
    }
  }

  //Motor Rechtsdrehung
  else if (MotorDir == DirMotorRight)
  {
    for (int i = 0; i < StepsToGo; i++)
    {
      ActStepNr++;
      if (ActStepNr == NrOfPatternsOneStep)
        ActStepNr = 0;
      for (byte j = 0; j < NrOfMotorOutputs; j++)
      {
        digitalWrite(MotorOutputPin[j], StepPattern[ActStepNr][j]);
      }
      delay(MotorSpeed); //Verweilzeit um die Geschwindigkeit zu beeinflussen, für schnellere Ansteuereung ev. delayMicroseconds
    }
  }

  //Motor Linksdrehung
  else if (MotorDir == DirMotorLeft)
  {
    for (int i = 0; i < StepsToGo; i++)
    {
      if (ActStepNr == 0)
        ActStepNr = NrOfPatternsOneStep - 1;
      else
        ActStepNr -= 1;
      for (byte j = 0; j < NrOfMotorOutputs; j++)
      {
        digitalWrite(MotorOutputPin[j], StepPattern[ActStepNr][j]);
      }
      delay(MotorSpeed);
    }
  }
}
void MotorDirectionMove(int Integer, byte MotorOutputPin[NrOfMotorOutputs], int motorindex)
{
  Serial.print("Motor:");
  Serial.println(motorindex);
  if (EEPROM.read(motorindex) >= 0 && EEPROM.read(motorindex) <= 31 && Integer >= 0 && Integer <= 31)
  { //EEPROM WERT GESETZT
    if (EEPROM.read(motorindex) < Integer)
    { //RUNTERFAHREN
      Serial.print("TURN DOWN");
      Serial.print(Integer);
      MotorMove(DirMotorRight, 1, 2048 * (Integer - EEPROM.read(motorindex)), MotorOutputPin);
      MotorMove(MotorOff, 0, 0, MotorOutputPin);
      EEPROM.write(motorindex, Integer);
    }
    else if (EEPROM.read(motorindex) > Integer)
    { //HOCHFAHREN
      Serial.print("TURN UP");
      Serial.print(Integer);
      MotorMove(DirMotorLeft, 1, 2048 * (EEPROM.read(motorindex) - Integer), MotorOutputPin);
      MotorMove(MotorOff, 0, 0, MotorOutputPin);
      EEPROM.write(motorindex, Integer);
    }
    else
    { // WERTE GLEICH
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{

  for (int i = 0; i < length; i++)
  {
    Serial.println((char)payload[i]);
  }
  String s = String((char *)payload);
  if (String((char *)topic) == "Buero/Fenster/Blinds/all/save")
  {
    Serial.println("SAVED NEW VALUES");
    EEPROM.write(1, MaxStepsToGo);
    EEPROM.write(2, MaxStepsToGo);
    EEPROM.write(3, MaxStepsToGo);
  }
  int integer = s.toInt();
  if (String((char *)topic) == "Buero/Fenster/Blinds/1")
  {
    Serial.println("IT WORKS");
    digitalWrite(MotorRelaisPin, LOW);
    MotorDirectionMove(integer, MotorOutputPin1, 1);

    digitalWrite(MotorRelaisPin, HIGH);
  }
  if (String((char *)topic) == "Buero/Fenster/Blinds/2")
  {
    Serial.println("IT WORKS");
    digitalWrite(MotorRelaisPin, LOW);
    MotorDirectionMove(integer, MotorOutputPin2, 2);
    digitalWrite(MotorRelaisPin, HIGH);
  }
  if (String((char *)topic) == "Buero/Fenster/Blinds/3")
  {
    Serial.println("IT WORKS");
    digitalWrite(MotorRelaisPin, LOW);
    MotorDirectionMove(integer, MotorOutputPin3, 3);
    digitalWrite(MotorRelaisPin, HIGH);
  }
  if (String((char *)topic) == "Buero/Fenster/Blinds/all")
  {
    Serial.println("IT WORKS");
    digitalWrite(MotorRelaisPin, LOW);
    MotorDirectionMove(integer, MotorOutputPin1, 1);
    digitalWrite(MotorRelaisPin, HIGH);
    delay(1000);
    digitalWrite(MotorRelaisPin, LOW);
    MotorDirectionMove(integer, MotorOutputPin2, 2);
    digitalWrite(MotorRelaisPin, HIGH);
    delay(1000);
    digitalWrite(MotorRelaisPin, LOW);
    MotorDirectionMove(integer, MotorOutputPin3, 3);
    digitalWrite(MotorRelaisPin, HIGH);
  }

  Serial.println(topic);
}

EthernetClient ethClient;
PubSubClient client(ethClient);
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", "broker", "localpassword"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("Buero/Fenster/Blinds/3");
      client.subscribe("Buero/Fenster/Blinds/2");
      client.subscribe("Buero/Fenster/Blinds/1");
      client.subscribe("Buero/Fenster/Blinds/all");
      client.subscribe("Buero/Fenster/Blinds/all/save");
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
void setup()
{
  pinMode(MotorRelaisPin, OUTPUT);
  digitalWrite(MotorRelaisPin, HIGH);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.begin(115200);
  for (byte i = 0; i < NrOfMotorOutputs; i++)
  {
    pinMode((MotorOutputPin1[i]), OUTPUT);
    pinMode((MotorOutputPin2[i]), OUTPUT);
    pinMode((MotorOutputPin3[i]), OUTPUT);
    digitalWrite(MotorOutputPin1[i], LOW);
    digitalWrite(MotorOutputPin2[i], LOW);
    digitalWrite(MotorOutputPin3[i], LOW);
  }

  uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
