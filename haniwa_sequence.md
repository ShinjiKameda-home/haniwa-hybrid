sequenceDiagram
    participant S as Garden Gateway
    participant C as haniwa_connector
    participant M as haniwa_main
    participant R as haniwa_monitor

    Note over C: サーバーからの指示を待機 (Listen)
    S->>C: 測定要求 (TCP Request)
    C->>M: 測定コマンドを伝達
    M->>R: haniwa_get_moisture()
    R-->>M: 土壌水分データ
    M->>C: データを受け渡し
    C->>S: 水分値を送信 (TCP Response)

    Note over S: 天気予報と水分値を照合
    S->>C: 判断結果 (潅水要否・LED制御指示)
    C->>M: 指示を伝達
    M->>R: LED点灯/消灯指示
    Note over R: LEDでユーザーに通知