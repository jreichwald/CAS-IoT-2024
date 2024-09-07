# CAS-IoT-2024

## Architekurübersicht
```mermaid 
flowchart LR
    n2["LoRa Sensor"] --> n1["LoRa Gateway"]
    n3["LoRa Sensor"] --> n1
    n1 --> n4["ChirpStack"]
    n5["WLAN Sensor"] --> n8["MQTT"]
    n6["WLAN Sensor"] --> n8
    n4 --> n8
    n8 --> n11["Nodered"]
    n11 -- n9["Prometheus"]
    n11 --> n12
    n12["Influx"] --> n10
    n9 --> n10["Grafana"]
```

## ChirpStack Doku

### Initiale Einrichtung
1. Herunterladen des ChirpStack Servers und des MQTT Servers entsprechend der [Dokumentation von ChirpStack](https://www.chirpstack.io/docs/getting-started/docker.html) per git.
2. Starten des ChirpStack Servers und des MQTT Brokers mit docker compose, entsprechend der [Dokumentation](https://github.com/chirpstack/chirpstack-docker)
3. Die Konfiguration von ChirpStack wie folgt anpassen:
```
[integration]
  enabled=["mqtt"]

  [integration.mqtt]
    server="tcp://$MQTT_BROKER_HOST:1883/"
    username="generaluser"
    password=""
    json=true
```

### Gateway Anbinden 
* Mit dem WiFi des Gateways verbinden 
* Im Gateway die `global_conf.json` anpassen 
* ChirpStack Adresse (141.72.13.25) eintragen mit Up and Down Port 1700 
* In ChirpStack das Gateway hinzufügen mit der EUI
* In einer Application neue Devices mit ihren EUI anlegen 

## Sensoren
Alle Sensoren senden die Daten inklusive Metadaten im JSON Dateiformat. Hierbei werden folgende Sensordaten verwendet:
* Temperature   (key: t)
* Humidty       (key: h)
* Sound	        (key: s)
* Lightbarrier  (key: l)

Erwartetes Packet:
```
{"topic":"eu868/gateway/e45f01fdfe98a7bf/event/up","payload":[10,23,0,0,0,0,0,0,0,0,0,246,209,15,30,32,58,180,168,63,152,70,75,12,36,34,18,8,224,233,132,158,3,18,10,26,8,8,200,208,7,16,9,40,1,42,65,10,16,101,52,53,102,48,49,102,100,102,101,57,56,97,55,98,102,16,239,177,3,26,12,8,133,163,235,182,6,16,216,129,221,208,2,48,231,255,255,255,255,255,255,255,255,1,61,205,204,60,65,64,1,72,1,106,4,70,223,224,193,128,1,2],"qos":0,"retain":false,"_msgid":"ecd14cfb43995f81"}
```

## NodeRed
NodeRed ist das Anwendungsystem. Mit NodeRed wird ein Preprocessing durchgeführt, bevor Daten in die Datenbank eingespeist werden.
Weiterhin können über NodeRed komplizer
### Betrieb
Für den Betrieb wurde Docker (docker-compose) gewählt. Um die Installation von entsprechenden Erweiterungen für Influxdb und Prometheus zu gewährleisten, wurde ein eigens Dockerfile angelegt und das Image gebaut.
Die Daten werden persistent über Volumes gespeichert.

### Anbindung von Sensoren
Die Kommunikation erfolgt grundsätzlich über MQTT (mq.jreichwald.de:1887). 
 
