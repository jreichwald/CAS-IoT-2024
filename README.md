# CAS-IoT-2024

## Architekurübersicht
```mermaid 
flowchart LR
    n2["Sensor"] --> n1["LoRa"]
    n3["Sensor"] --> n1
    n1 --> n4["ChirpStack"]
    n5["Sensor"] --> n7["WLAN"]
    n6["Sensor"] --> n7
    n7 --> n8["MQTT"]
    n4 --> n8
    n8 --> n9["Prometheus to matt + Influx"]

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
