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
