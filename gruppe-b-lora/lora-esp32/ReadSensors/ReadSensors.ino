// CAS-IoT-2024
// Software for reading sensor data via Heltec WiFi LoRa 32(V3).
// Read temperature and humidity from a DHT11. Send data over LoRa to our ChirpStack server.

#include "LoRaWan_APP.h"
#include "ArduinoJson.h"
#include "DHT.h"

/* OTAA para*/
uint8_t devEui[] = { 0xa8, 0xb4, 0x3a, 0x20, 0x1e, 0x0f, 0xd1, 0xf6 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x66, 0x01 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask*/
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;


/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;


#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
	/*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
	*appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
	*if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
	*if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
	*for example, if use REGION_CN470, 
	*the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
	*/
    float h = dht.readHumidity();
    Serial.print("Humidity: ");
    Serial.println(h);

    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.println(t);
   
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // JSON-Datenstruktur erstellen
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["t"] = t;
    jsonDoc["h"] = h;

    // JSON-Daten in einen String konvertieren
    String jsonData;
    serializeJson(jsonDoc, jsonData);

    // Ausgabe der JSON-Daten über die serielle Schnittstelle
    Serial.print("Sende JSON-Daten: ");
    Serial.println(jsonData);

    // JSON-String in Byte-Array konvertieren
    int jsonLength = jsonData.length();
    uint8_t payload[jsonLength];
    jsonData.getBytes(payload, jsonLength + 1);
    // Ausgabe des Byte-Arrays für Debug-Zwecke
    appDataSize = jsonLength;
    Serial.print("Byte-Array: ");
    for (int i = 0; i < jsonLength; i++) {
      Serial.print(payload[i], HEX);   // Ausgabe in Hexadezimal für bessere Lesbarkeit
      Serial.print(" ");
      appData[i] = payload[i];
    }
}

RTC_DATA_ATTR bool firstrun = true;

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

  dht.begin();

  if(firstrun)
  {
    LoRaWAN.displayMcuInit();
    firstrun = false;
  }

}

void loop()
{
	switch( deviceState )
	{
		case DEVICE_STATE_INIT:
		{
      #if(LORAWAN_DEVEUI_AUTO)
            LoRaWAN.generateDeveuiByChipID();
      #endif
			LoRaWAN.init(loraWanClass,loraWanRegion);
			//both set join DR and DR when ADR off 
			LoRaWAN.setDefaultDR(3);
			break;
		}
		case DEVICE_STATE_JOIN:
		{
			LoRaWAN.displayJoining();
			LoRaWAN.join();
			break;
		}
		case DEVICE_STATE_SEND:
		{
			LoRaWAN.displaySending();
			prepareTxFrame( appPort );
			LoRaWAN.send();
			deviceState = DEVICE_STATE_CYCLE;
			break;
		}
		case DEVICE_STATE_CYCLE:
		{
			// Schedule next packet transmission
			txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
			LoRaWAN.cycle(txDutyCycleTime);
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
			LoRaWAN.displayAck();
			LoRaWAN.sleep(loraWanClass);
			break;
		}
		default:
		{
			deviceState = DEVICE_STATE_INIT;
			break;
		}
	}
}