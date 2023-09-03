#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <avr/wdt.h>


char ssid[] = "YOUR_SSID";
char pass[] = "YOUR_PASSWORD";

int status = WL_IDLE_STATUS;
static int MAX_RETRY_COUNT = 25;
WiFiClient client;
IPAddress server(1, 2, 3, 4); // your endpoint

String inputString;
boolean stringComplete = false;
char inChar;
int counter;
int commaPosition;
float ivt[50];

// Function to perform a software reset
void softwareReset() {
  wdt_enable(WDTO_15MS);  // Enable watchdog timer and set the timeout
  while (1) {}            // Wait for the watchdog timer to reset the board
}

void setup() {
  // 9600 baud
  Serial1.begin(9600);

  while (!Serial1) {
    ;  // Wait for serial port to connect
  }

  connectToWiFi();
}

void connectToWiFi() {
  // Attempt to connect to Wi-Fi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    if (status == WL_CONNECTED) {
      Serial.println("Connected to Wi-Fi!");
      printWiFiStatus();
    } else {
      Serial.println("Connection failed. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void printWiFiStatus() {
  // Print current Wi-Fi status
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

float stringToFloat(String input) {
  String stringTemp = input;
  char stfarray[stringTemp.length() + 1];
  stringTemp.toCharArray(stfarray, sizeof(stfarray));
  float stf = atof(stfarray);
  stringTemp = "";
  memset(stfarray, 0, sizeof stfarray);
  return stf;
}

void readSerialAndGenerateInputString() {
  while (Serial1.available() > 0) {
    inChar = Serial1.read();
    if (inChar != 32) {
      inputString += inChar;
    }
    delay(2);
  }
  inChar = '0';
  Serial1.println(inputString);
  stringComplete = true;
}

void postPayload(char *payload) {
  if (client.connect(server, 5000)) {
    int contentLength = strlen(payload);
    Serial.println("Posting payload to endpoint /flp");
    Serial.println(payload);
    client.println("POST /flp HTTP/1.1");
    client.println("Host: 1.2.3.4"); //your endpoint
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(contentLength);
    client.println();
    client.print(payload);
  } else {
    Serial.println("POST failed, couldn't connect to server!");
  }

  if (client.connected()) {
    client.stop();
  }
}

// TODO: Split this into functions
void createPayloadAndPOST() {
  commaPosition = inputString.indexOf(';');
  while (commaPosition >= 0) {
    commaPosition = inputString.indexOf(';');
    if (commaPosition != -1) {
      ivt[counter] = stringToFloat(inputString.substring(0, commaPosition));
      inputString = inputString.substring(commaPosition + 1, inputString.length());
    } else {
      if (inputString.length() > 0) {
        ivt[counter] = stringToFloat(inputString.substring(0, commaPosition));
      }
    }
    counter++;
  }
  counter = 0;
  inputString = "";
  commaPosition = 0;

  StaticJsonDocument<800> doc;

  doc["framledn"] = ivt[1];       //Framledning
  doc["ute"] = ivt[2];            //Ute
  doc["tappvarmvtopp"] = ivt[3];  //Tappvarmvatten Topp
  doc["varmevmitt"] = ivt[4];     //Varmvatten Mitt
  doc["varmevbotten"] = ivt[5];   //Värmevatten Botten
  doc["inne"] = ivt[6];           //Rumstemp
  doc["hetgas"] = ivt[7];         //Hetgas
  doc["egtemp"] = ivt[8];         //Extra Acc-Tank
  doc["tryckvakt"] = ivt[9];      //Tryckvakt
  doc["hogtryck"] = ivt[10];      //Högtryck
  doc["lagtryck"] = ivt[11];      //Lågtryck
  doc["egsemester"] = ivt[12];    //Semester aktiv?
  doc["kompressor"] = ivt[13];    //Kompressor aktiv
  doc["schuntoppen"] = ivt[14];   //SV1 Öppna
  doc["schuntstangd"] = ivt[15];  //SV1 Stäng
  doc["cirkpump"] = ivt[16];      //P1 Rad
  //doc["flakt"] = ivt[17]; //Fläkt
  doc["larm"] = ivt[18];  //Larm aktiv
  //doc["20"] = ivt[19]; //Extern P2
  //doc["21"] = ivt[20]; //LLT GT1
  //doc["22"] = ivt[21]; //LL GT1
  doc["bvframledning"] = ivt[22];  //BV GT1
  //doc["24"] = ivt[23]; //UL GT1
  //doc["25"] = ivt[24]; //LL GT3:2
  //doc["26"] = ivt[25]; //ULT GT3:2
  //doc["27"] = ivt[26]; //UL GT3:2
  //doc["28"] = ivt[27]; //LL GT3:3
  //doc["29"] = ivt[28]; //BV GT3:3
  doc["extravv1"] = ivt[29];  //SV3 BV Förskj
  doc["extravv2"] = ivt[30];  //Effekt ink vit VV behov
  doc["extravv3"] = ivt[31];  //Tillskotstimer VV behov
  //doc["33"] = ivt[32];   //Tappv prio
  doc["elpatron"] = ivt[33];  //Tillskott i %/10
  //doc["35"] = ivt[34]; //Tillskott RAD
  doc["extravv4"] = ivt[35];  //Tillskott Tillägg
  //doc["37"] = ivt[36]; //Default SV2 Open

  // Debug to serial terminal
  //serializeJsonPretty(doc, Serial1):

  char buffer[800];

  serializeJson(doc, buffer);

  memset(ivt, 0, sizeof ivt);

  stringComplete = false;

  Serial.println(buffer);

  postPayload(buffer);
}

void debugSetup() {
  stringComplete = true;
  // A serial data string, we can use it to run tests.
  inputString = "360;315;33;478;445;447;-512;774;-512;0;0;0;0;1;0;0;1;1;0;0;296;311;326;341;495;520;545;301;326;351;435;105;26;0;0;0;0";
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    static int retryCount = 0;

    Serial.println("Wi-Fi connection lost. Reconnecting...");
    connectToWiFi();

    // Error handling for repeated reconnection failures
    if (WiFi.status() != WL_CONNECTED) {
      retryCount++;
      if (retryCount >= MAX_RETRY_COUNT) {
        Serial.println("Max reconnection attempts reached. Restarting...");
        softwareReset();  // Restart the Arduino
      }
    } else {
      retryCount = 0;  // Reset retry count upon successful reconnection
    }
  }

  if (Serial1.available() > 0) {
    readSerialAndGenerateInputString();
  }

  if (stringComplete) {
    delay(100);
    createPayloadAndPOST();
    delay(500);
  }
}
