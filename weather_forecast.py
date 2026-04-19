import os
import json
import csv
import requests
from dotenv import load_dotenv
import time
import datetime
from multiprocessing import shared_memory

# Load configuration from .env
load_dotenv()
API_KEY = os.getenv('OPENWEATHER_API_KEY')
CITY = os.getenv('CITY')
TELEGRAM_TOKEN = os.getenv('TELEGRAM_BOT_TOKEN')
CHAT_ID = os.getenv('TELEGRAM_CHAT_ID')

# Global Constants
STATUS_FILE = "../permission.json"
TEMP_FILE = "../permission_temp.json"
CHECK_INTERVAL = 3 * 60 * 60 # 3 hours
LOG_FILE = "../weather_observation_log.csv"
RAIN_SLOTS_THRESHOLD = 2  # Threshold for deciding watering based on rain forecast
SHM_NAME = "garden_person_presence"
MOISTURE_FILE = "../soil_moisture.json"
MOISTURE_THRESHOLD = 30  # Example threshold for dry soil

def send_telegram(text):
    """Send a notification message via Telegram Bot API."""
    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    payload = {"chat_id": CHAT_ID, "text": text}
    try:
        response = requests.post(url, json=payload, timeout=10)
        # Print results into the terminal
        if response.status_code == 200:
            print("Telegram notification sent successfully!")
        else:
            print(f"Failed to send: {response.text}")
    except Exception as e:
        print(f"Notification Error: {e}")

def make_decision() -> int:
    """
    Evaluates environmental factors to determine if watering is necessary.
    Returns:
        0: SKIP (Green LED)     - Sufficient moisture or rain detected/predicted.
        1: GO   (Red LED)       - Dry soil and no significant rain expected.
        2: TOO_MUCH  (Blue LED) - Sufficient moisture and more rain predicted.
        3: ERROR (Blue LED)     - System error or data missing, high frequency of blinking.
    """
    try:
        # 1. Fetch real-time soil moisture from the local sensor data
        with open(MOISTURE_FILE, 'r') as f:
            moisture_data = json.load(f)
            # Default to 100 (wet) to prevent accidental over-watering if file is corrupt
            cur_moisture = int(moisture_data.get("MOISTURE", 100))
        # 2. Analyze weather trends: past, present, and future forecast
        with open(LOG_FILE, 'r') as f:
            rows = list(csv.DictReader(f))
            if not rows:
                return 3 # Error: No historical data found
            # Integrate all rain factors into a single score
            rain_slots = 0
            # Weighted historical/current rain (Past & Present)
            if rows[-1]['weather'] == 'Rain':
                rain_slots += 3
            if len(rows) >= 2 and rows[-2]['weather'] == 'Rain':
                rain_slots += 2
            if len(rows) >= 3 and rows[-3]['weather'] == 'Rain':
                rain_slots += 1
            # Predicted rain intensity (Future)
            rain_slots += int(rows[-1]['rain_slots_48h'])
        # 3. Final Decision Logic
        #    Skip if accumulated rain score is high or soil is already moist
        if rain_slots > RAIN_SLOTS_THRESHOLD and cur_moisture > MOISTURE_THRESHOLD:
            return 2  # TOO_MUCH
        elif rain_slots > RAIN_SLOTS_THRESHOLD or cur_moisture > MOISTURE_THRESHOLD:
            return 0  # SKIP
        else:
            return 1  # GO
    except Exception as e:
        print(f"Decision Error: {e}")
        return 3  # ERROR

def save_to_log(data_row):
    """Append observation data to a local CSV file for future accuracy analysis."""
    file_exists = os.path.isfile(LOG_FILE)
    with open(LOG_FILE, 'a', newline='') as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(["timestamp", "wind_speed", "wind_gust", "weather", "temp", "humidity", "rain_slots_48h"])
        writer.writerow(data_row)
        # Flush the cache file to ensure the latest weather data is available
        f.flush()
        os.fsync(f.fileno())

def get_weather(force_report=False):
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
    wind_gust = current['wind'].get('gust', wind_speed)
    weather_main = current['weather'][0]['main']
    temp = current['main']['temp']
    humidity = current['main']['humidity']
    now = datetime.datetime.now()

    # Extract next weather parameters (after 3 hours)
    upcoming = data['list'][1]
    next_wind_speed = upcoming['wind'].get('speed', 0)
    next_wind_gust = upcoming['wind'].get('gust', next_wind_speed)

    # Count rain slots in the next 48 hours (16 slots of 3-hour intervals)
    rain_slots = sum(1 for slot in data['list'][:16] if 'Rain' in [w['main'] for w in slot['weather']])

    # --- ACTION 1: Log data to CSV for accuracy verification ---
    save_to_log([
        now.strftime('%Y-%m-%d %H:%M:%S'),
        wind_speed,
        wind_gust,
        weather_main,
        temp,
        humidity,
        rain_slots
    ])

    # Logic for System Permissions
    #   1. BirdWatching: Allowed if wind is calmed down for 3-6 hours
    curr_safe = (wind_speed <= 7.0) and (wind_gust <= 9.0)
    next_safe = (next_wind_speed <= 9.0) and (next_wind_gust <= 12.0)
    bird_perm = curr_safe and next_safe
    #   2. Watering: Allowed if make_decision() returns GO, otherwise SKIP or TOO_MUCH
    water_perm = make_decision() == 1

    # --- ACTION 2: Update permission.json for system-to-system integration ---
    status = {
        "wind_speed": wind_speed,
        "wind_gust": wind_gust,
        "rain_slots_48h": rain_slots,
        "birdwatching": bird_perm,
        "watering": water_perm,
        "updated_at": now.strftime('%Y-%m-%d %H:%M:%S')
    }
    # Atomic write to avoid file disruption and race conditions with other services
    with open(TEMP_FILE, 'w') as f:
        json.dump(status, f, indent=4)
    os.replace(TEMP_FILE, STATUS_FILE)

    # --- ACTION 3: Human Report via Telegram (Only at 6:30 AM JST, or just restarted) ---
    if now.hour == 6 or force_report:
        advice = "SKIP Watering" if not water_perm else "GO Watering"
        wind_alert = f"\nWARNING: Strong Wind or Gust!" if not bird_perm else ""

        message = (
            f"--- Weather Report ---\n"
            f"Status: {weather_main}\n"
            f"Temp: {temp}C / Humid: {humidity}%\n"
            f"Wind: {wind_speed}m/s / Gust: {wind_gust}m/s {wind_alert}\n"
            f"Watering: {advice}"
        )
        send_telegram(message)
    else:
        # Silent update for other time slots (9:30, 12:30, etc.)
        print(f"[{now.strftime('%H:%M')}] System updated & Logged (Silent mode).")

def main_loop():
    # Initialize shared memory for the Person detection
    try:
        shm = shared_memory.SharedMemory(name=SHM_NAME, create=True, size=1)
        shm.buf[0] = 0  # Initialize presence to 0 (no person)
        print(f"Shared memory '{SHM_NAME}' created by Weather Forecast.")
    except FileExistsError:
        shm = shared_memory.SharedMemory(name=SHM_NAME)
        print(f"Shared memory '{SHM_NAME}' already exists, reusing it.")

    # Send startup notification
    send_telegram("Mission Start: Monitoring the Weather...")
    # Force reporting for the first time getting weather data
    get_weather(force_report=True) 

    while True:
        now = datetime.datetime.now()

        # Make a decision of watering when a person is detected
        if shm.buf[0] == 1:
            is_present = bool(shm.buf[0])
            send_telegram(f"Person detected in the garden!: {is_present}")
            decision = make_decision()  # Make a decision for watering based on weather data and soil moisture
            ## send_led_command(decision)  # Send command to the LED system via garden_gateway.service
            # Send Telegram notification based on the decision
            if decision == 0:
                send_telegram("Watering: SKIP (Green LED)")
            elif decision == 1:
                send_telegram("Watering: GO (Red LED)")
            elif decision == 2:
                send_telegram("Watering: TOO_MUCH (Blue LED)")
            else:
                send_telegram("Error: Unable to make a decision")
            shm.buf[0] = 0              # Clear the flag after processing

        # Acquire weather data every 3 hours
        if now.minute == 30 and now.hour % 3 == 0:
            get_weather()
            # Sleep to prevent redundant execution within the same minute
            for _ in range(7):
                time.sleep(10)
        # Just check the clock every 2 seconds
        time.sleep(2)

if __name__ == "__main__":
    main_loop()