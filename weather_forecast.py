import os
import json
import requests
from dotenv import load_dotenv

# Load configuration from .env
load_dotenv()
API_KEY = os.getenv('OPENWEATHER_API_KEY')
CITY = os.getenv('CITY')
TELEGRAM_TOKEN = os.getenv('TELEGRAM_BOT_TOKEN')
CHAT_ID = os.getenv('TELEGRAM_CHAT_ID')
STATUS_FILE = "../permission.json"
TEMP_FILE = "../permission_temp.json"

def send_telegram(text):
    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    payload = {"chat_id": CHAT_ID, "text": text}
    requests.post(url, json=payload)

def get_weather():
    # Fetch 5-day/3-hour forecast
    url = f"https://api.openweathermap.org/data/2.5/forecast?q={CITY}&appid={API_KEY}&units=metric"
    response = requests.get(url)
    
    if response.status_code != 200:
        print(f"Error: {response.status_code}")
        return

    data = response.json()

    # System Permissions (System-to-System Logic)
    current_wind = data['list'][0]['wind'].get('speed', 0)
    bird_perm = current_wind <= 8.0  # Safe if wind <= 8m/s

    # Check rain slots (3-hour intervals) in the next 48 hours (16 slots)
    rain_slots = sum(1 for slot in data['list'][:16] if 'Rain' in [w['main'] for w in slot['weather']])
    water_perm = rain_slots < 3  # Stop watering if 2 or more rain slots predicted

    status = {
        "wind_speed": current_wind,
        "rain_slots_48h": rain_slots,
        "birdwatching": bird_perm,
        "watering": water_perm
    }

    # Atomic write to avoid race conditions
    # TEMP_FILE and STATUS_FILE are needed to be in the same directory
    with open(TEMP_FILE, 'w') as f:
        json.dump(status, f, indent=4)
    os.replace(TEMP_FILE, STATUS_FILE)

    # Human Report (Telegram Notification)
    advice = "SKIP Watering" if not water_perm else "Proceed Watering"
    wind_warn = " [HIGH WIND]" if not bird_perm else ""

    message = (
        f"--- Dr. Wadachi Report ---\n"
        f"City: {CITY}\n"
        f"Wind: {current_wind}m/s{wind_warn}\n"
        f"Rain Slots (48h): {rain_slots}\n"
        f"Advice: {advice}"
    )
    
    # Extract data from the latest forecast (next 3 hours)
    current = data['list'][0]
    weather_main = current['weather'][0]['main']
    temp = current['main']['temp']
    humidity = current['main']['humidity']
    wind_speed = current['wind'].get('speed', 0)
    
    # Simple wind alert logic (threshold: 8m/s)
    wind_alert = ""
    if wind_speed > 8.0:
        wind_alert = f"\n WARNING: Strong wind ({wind_speed}m/s)"

    # Build the English message
    message = (
        f"Weather Report at [{CITY}]\n"
        f"Status: {weather_main}\n"
        f"Temp: {temp}C\n"
        f"Humidity: {humidity}%\n"
        f"Wind: {wind_speed}m/s"
        f"{wind_alert}"
    )
    
    send_telegram(message)

if __name__ == "__main__":
    get_weather()