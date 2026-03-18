# Smart HANIWA: Timeless yet Cutting-edge Irrigation and Scarecrow Hybrid System

## Motivation: Bridging the Past and Future – A New Approach to Japanese Irrigation

<b>"Which is the world's largest tomb?"</b>

Many would point to the Great Pyramid of Giza. I have immense respect for the pyramids and hope to visit them one day, but in terms of surface area, there is another contender in Japan: the Daisenryo Kofun, in Sakai City, Osaka Prefecture. While it is widely believed to be the final resting place of Emperor Nintoku, it remains one of history's greatest mysteries.

Official Website: Daisenryo Kofun at Osaka Convention & Tourism Bureau

https://osaka-info.jp/en/spot/tomb-emperor-nintoku-daisen-kofun/

Because it is a sacred site and a World Heritage monument, excavation is strictly prohibited. We may not know the truth for generations—not until non-destructive inspection technologies advance enough to see inside without disturbing its peace.

I grew up in a small town right next to this "silent giant." (Of course, I've also been to this cemetery that's like a forest.) It was a place where life moved slowly; in spring, tractors would crawl along the national highway, often creating long traffic jams. But, nobody honks their horns in frustration. Everyone in town shared a deep respect for the farmers. We’d see the tractors turning into the fields, scattering bits of mud on the asphalt, and we’d naturally think—or even call out—"Good work today!" or "Keep it up!"

Living in an environment where people naturally accepted and respected one another, alongside a massive, technologically untouchable mystery, inspired me. It led me to explore the intersection of ancient history, human warmth, and modern hardware.

There is an intriguing theory that the haniwa (terracotta clay figure) placed in Kofuns were not merely funeral offerings. The story suggests that these burial mounds, positioned along the coastline, functioned as a kind of lighthouse, with the hollow bodies of the haniwa emitting light to guide sailors home. This suggests the existence of a benevolent ruler who wished to protect people even after death.

I recently visited a cultural heritage site, the Shibayama Haniwa Museum in Chiba, Japan. This museum is located near the Shibayama Kofun clusters and serves as a vital center for preserving the history of these ancient figures.

Official Website: Shibayama Haniwa Museum

https://www.haniwakan.com/

(Note: The website is in Japanese, but you can easily translate it using your browser's translation feature.)

I brought home a Haniwa replica. It's more than a souvenir　for me — it seemed the perfect vessel for the next cutting-edge project, bridging the hearts and prayers of people from 1,500 years ago with modern IoT.

Vision: The Haniwa Guardian of the Potager
I’m transforming this ancient guardian into a hybrid of "Terracotta Irrigation" and "Active Scarecrow" for my small garden.

 - Terracotta Irrigation: Utilizing the porous nature of clay for automatic, low-tech irrigation. Combined with a Raspberry Pi Pico W, it will monitor soil moisture and manage water levels in the body itself. It won't just water the plants; it’ll notify me via Telegram when it needs a refill.

 - Active Scarecrow: Based on my previous project where I repurposed security cameras for bird watching, it will be usable to trigger glowing LED on Haniwa's eyes and a buzzer to keep crows or cats away.

In Japan’s limited space, there is a distinct beauty in multipurpose design. Let’s breathe new life into an ancient form.



## Step: Integrating Weather Intelligence
To give my Haniwa the power of foresight, I integrated the OpenWeatherMap API. Here is how I set up the "Brain" of the project:

https://openweathermap.org/

1. Obtaining the API Passport
 - Register: Create a free account ( <1000 requests available ) at OpenWeather.org.
 - Activation: After verifying your email, navigate to the "My API Keys" tab.
 - Generate: A default key is usually provided, but you can create a dedicated one named Haniwa_Project.

Note: It can take up to a few hours for a new key to become active globally.

Note: Optimized Data Retrieval
To minimize network traffic and power consumption, I designed the "DrWadachi" service to fetch a single comprehensive forecast once a day. Since the OpenWeatherMap "5 Day / 3 Hour Forecast" provides detailed predictions for the next 120 hours, one smart request is all my Haniwa needs to anticipate the weather in Miura. It’s a perfect balance between staying informed and being resource-efficient.

Note: Accessing the Credentials
After the email verification, navigate to the "My API keys" section under your profile menu. Here, you will find your unique 32-character alphanumeric key. Think of this as the digital tuning frequency for your "DrWadachi" weather station.

2. Secure Configuration (Environment Variables)
To keep my observation site private and my keys secure, I used a .env file. This is crucial for preventing sensitive information from being pushed to a public GitHub repository.

3. Developing the "DrWadachi" Service (Python)
I wrote a Python script named weather_forecast.py on my Home Server. This script acts as the "DrWadachi" intelligence:

It uses the requests library to fetch JSON data from OpenWeatherMap using the hidden GPS coordinates.

It parses the 3-hour forecast to determine the irrigation needs.

It sends a formal report to my smartphone via the Telegram Bot API.


## Disclaimer
<b> Personal Project:</b> This project is a personal hobby and technical experiment. It is not affiliated with, sponsored by, or endorsed by any of the organizations mentioned (e.g., Shibayama Haniwa Museum).

<b> Media Credits:</b> All photos of the Haniwa were taken by the author of the actual product purchased at the museum.

<b> Accuracy:</b> While I strive for technical and historical accuracy, I assume no responsibility for any issues or damages arising from the use of the information in this article.