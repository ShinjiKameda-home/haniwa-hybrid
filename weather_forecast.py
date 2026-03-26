import os
import json
import csv
import requests
from dotenv import load_dotenv
import time
import datetime

# Load configuration from .env
load_dotenv()
API_KEY = os.getenv('OPENWEATHER_API_KEY')
CITY = os.getenv('CITY')
TELEGRAM_TOKEN = os.getenv('TELEGRAM_BOT_TOKEN')
CHAT_ID = os.getenv('TELEGRAM_CHAT_ID')

# Global constants
STATUS_FILE = "../permission.json"
TEMP_FILE = "../permission_temp.json"
CHECK_INTERVAL = 3 * 60 * 60 # 3 hours
LOG_FILE = "weather_observation_log.csv"

# Local constants
last_execution_time = 0

def send_telegram(text):
    """Send a notification message via Telegram Bot API."""
    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    payload = {"chat_id": CHAT_ID, "text": text}
    requests.post(url, json=payload)

def save_to_log(data_row):
    """Append observation data to a local CSV file for future accuracy analysis."""
    file_exists = os.path.isfile(LOG_FILE)
    with open(LOG_FILE, 'a', newline='') as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(["timestamp", "wind_speed", "weather", "temp", "humidity", "rain_slots_48h"])
        writer.writerow(data_row)

def get_weather():
    """Fetch weather data, update system permissions, and notify user if it's morning."""
    url = f"https://api.openweathermap.org/data/2.5/forecast?q={CITY}&appid={API_KEY}&units=metric"
    try:
        response = requests.get(url, timeout=15)
        if response.status_code != 200:
            print(f"API Error: {response.status_code}")
            return
        data = response.json()
    except Exception as e:
        print(f"Connection Error: {e}")
        return

    # Extract current weather parameters
    current = data['list'][0]
    wind_speed = current['wind'].get('speed', 0)
    weather_main = current['weather'][0]['main']
    temp = current['main']['temp']
    humidity = current['main']['humidity']
    now = datetime.datetime.now()

    # Logic for System Permissions
    # 1. BirdWatching: Allowed if wind speed <= 8.0 m/s
    bird_perm = wind_speed <= 8.0
    # 2. Watering: Check rain slots in the next 48 hours (16 slots of 3-hour intervals)
    rain_slots = sum(1 for slot in data['list'][:16] if 'Rain' in [w['main'] for w in slot['weather']])
    water_perm = rain_slots < 3

    # --- ACTION 1: Log data to CSV for accuracy verification ---
    save_to_log([
        now.strftime('%Y-%m-%d %H:%M:%S'),
        wind_speed,
        weather_main,
        temp,
        humidity,
        rain_slots
    ])

    # --- ACTION 2: Update permission.json for system-to-system integration ---
    status = {
        "wind_speed": wind_speed,
        "rain_slots_48h": rain_slots,
        "birdwatching": bird_perm,
        "watering": water_perm,
        "updated_at": now.strftime('%Y-%m-%d %H:%M:%S')
    }
    # Atomic write to avoid race conditions with other services
    with open(TEMP_FILE, 'w') as f:
        json.dump(status, f, indent=4)
    os.replace(TEMP_FILE, STATUS_FILE)

    # --- ACTION 3: Human Report via Telegram (Only at 6:30 AM JST) ---
    if now.hour == 6:
        advice = "SKIP Watering" if not water_perm else "Proceed Watering"
        wind_alert = f"\n WARNING: Strong wind ({wind_speed}m/s)" if not bird_perm else ""

        message = (
            f"--- Dr. Wadachi Morning Report ---\n"
            f"Status: {weather_main}\n"
            f"Temp: {temp}C / Humid: {humidity}%\n"
            f"Wind: {wind_speed}m/s{wind_alert}\n"
            f"Watering: {advice}"
        )
        send_telegram(message)
    else:
        # Silent update for other time slots (9:30, 12:30, etc.)
        print(f"[{now.strftime('%H:%M')}] System updated & Logged (Silent mode).")

def main_loop():
    global last_execution_time
    while True:
        now = datetime.datetime.now()
        if now.minute == 30 and now.hour % 3 == 0:
            current_ts = time.time()
            if (current_ts - last_execution_time) > 3600:
                get_weather()
                last_execution_time = current_ts
        time.sleep(60)

if __name__ == "__main__":
    main_loop()