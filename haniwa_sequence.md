``` mermaid
sequenceDiagram
    participant S as garden_gateway.py
    participant C as haniwa_connector.cpp
    participant M as haniwa_main.cpp
    participant R as haniwa_monitor.cpp

    Note over C: Connect to garden_gateway.py
    S->>C: Measure moisture (TCP Request)
    C->>M: Transfer "Measure"
    M->>R: Kick haniwa_get_moisture()
    R-->>M: Response soil moisture data
    M->>C: Transfer soil moisture data
    C->>S: Return moisture (TCP Response)

    Note over S: Refer weather forecast and soil moisture
    S->>C: Send Decision Result
    C->>M: Transfer the "Decision"
    M->>R: Blinking LEDs
    Note over R: Notify the "Decision" to users
```

``` mermaid
sequenceDiagram
    participant H as Haniwa (Pico W)
    participant GG as garden_gateway.service
    participant SC as soil_condition.json
    participant BW as bird_watcher.service
    participant WF as weather_forecast.service
    participant PM as permission.json

    Note over H, SC: 1. Regular Soil Update (Every 15 mins)
    H->>GG: Send moisture data (TCP)
    GG->>SC: Update moisture value
    GG-->>H: Acknowledgement (ACK)

    Note over BW, PM: 2-3. Intelligence & Decision making
    BW->>WF: "Person" detected! Report encounter
    WF->>SC: Read recent soil moisture
    WF->>WF: Analyze weather_observation_log.csv
    WF->>WF: Check latest cached Forecast Data
    WF->>PM: Write Decision (CRITICAL / SAFE / WET_WARNING)

    Note over H, PM: 4-6. Feedback to Haniwa
    H->>GG: Request latest Permission (Next Poll)
    GG->>PM: Read permission status
    GG-->>H: Transfer "Decision Result"
    
    Note over H: Haniwa_main.cpp / monitor.cpp
    H->>H: Update Flags & Trigger LED (10s Blink)
    Note right of H: Red: Critical<br/>Green: Safe<br/>Blue: Warning/Error
```