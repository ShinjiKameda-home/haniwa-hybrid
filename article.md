# Smart HANIWA: Timeless yet Cutting-edge Irrigation and Scarecrow Hybrid System

## Motivation: Bridging the Past and Future – A New Approach to Japanese Irrigation

**"Which is the world's largest tomb?"**

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

![Packaged haniwa](images/20260316_packed_haniwa.png "I took the Haniwa out of its packaging. Unglazed clay is very fragile, but there were no noticeable cracks.")
![Standing haniwa](images/20260316_standing_haniwa.png "Haniwa could stand in my room.")
![Hole of haniwa](images/20260316_hole78p45_haniwa.png "A hole in the bottom of the Haniwa. Measuring it with calipers reveals its diameter to be 78.45mm.")

In Japan’s limited space, there is a distinct beauty in multipurpose design. Let’s breathe new life into an ancient form.

## Step: Integrating Weather Intelligence — The Brain of Haniwa
"Have you ever watered your garden, only to have it pour rain an hour later?" We've all been there. It’s a frustrating waste of resources and effort. To prevent this, I think it not enough even "Terracotta Irrigation" is adopted. my Haniwa needed more than just a soil moisture sensor; it needed the power of foresight.

I decided to integrate weather forecast data to create a "Smart Irrigation Advisor." But before the Haniwa could worry about water, it faced a more immediate challenge: The Hayama Wind. I discovered that when wind speeds exceed 9m/s, my "BirdWatcher" AI goes into a frenzy, mistaking swaying branches for birds. To save my system from this "storm of false positives," I suddenly needed to struggle developing a coordination system between my weather service and the AI monitor.

1. Invoking "Dr. Wadachi"
I named the weather service "DrWadachi", in honor of Kiyoo Wadachi, a pioneer in Japanese meteorology. (https://en.wikipedia.org/wiki/Kiyoo_Wadachi)

I read a book in my university days. Dr. Wadachi once said he pursued this field because "great discoveries could be made with nothing but observation data and human intellect." I feel a deep sympathy for this. In my university days, I used cutting-edge equipment costing a few 100 million yen (more than 1 million dollars). Today, I am pursuing the same essence of discovery using a used PC and a $5 Raspberry Pi Pico W.

Setup Process:
API Passport: I used the OpenWeatherMap API (5-Day / 3-Hour Forecast, https://openweathermap.org/). One smart request a day provides enough data to anticipate Hayama's weather while remaining resource-efficient.

**Obtaining the API Passport**
 - Register: Create a free account ( 1000 requests/day available ) at OpenWeather.org.
 - Activation: After verifying your email, navigate to the "My API Keys" tab.
 - Generate: A default key is usually provided, but you can create a dedicated one named Haniwa_Project.

2. The Logic: A Symbiosis of Mind and Action
The intelligence of this project is distributed across three distinct roles, creating a feedback loop between the cloud AI, the edge AI, and the physical interface.

A. Dr. Wadachi (The Astronomer / Cloud AI)
The Python service acts as the central analyst. It doesn't just fetch data; it interprets the environment:

Predictive Irrigation: It correlates real-time soil moisture data (sent from the Haniwa) with the 48-hour precipitation forecast.

Safety Oversight: It monitors wind speeds to decide if it's safe for the "BirdWatcher" to operate.

Directive Issuance: It writes a centralized JSON "Status Report" that dictates the behavior of the other components.

B. The BirdWatcher (The Lamplighter / Edge AI)
This component handles the high-intensity vision processing. It identifies visitors—not just birds, but also cats, dogs, and humans.

The "Lamplighter" Rest Logic: Too busy lamplighters need proper rests. When Dr. Wadachi reports winds >9m/s, the BirdWatcher enters a "Sheep-Counting" sleep. It checks the JSON status every 10 seconds, staying alert enough to wake up the moment the wind dies down, without wasting CPU cycles during the storm.

C. The Haniwa (The Fox / Human Interface)
The Haniwa is the bridge to the physical world. It stays quiet to save energy but becomes interactive when needed:

Human Presence Awareness & Security: When the BirdWatcher detects a "Person" nearby, it immediately notifies the Haniwa. This serves a dual purpose: it prepares the Haniwa for interaction and acts as a visual security deterrent.

**The Visual Signal:** Only when a person is present, the Haniwa's eyes glow.

 - For the Owner: It signals the irrigation status (e.g., glowing to suggest watering if no rain is forecast).

 - For an Intruder: The sudden illumination of the Haniwa’s eyes in a dark garden sends a clear message: "You are being monitored." It transforms an ancient clay figure into a silent, modern-day guardian of my home in Hayama.

## First Report Success!
The system is alive. The inaugural report indicated a wind speed of 9.32 m/s in Hayama—right at the threshold! Dr. Wadachi successfully signaled the BirdWatcher to take a nap. The project has officially moved from "code on a screen" to a "living, breathing monitor" that respects the natural environment of the Miura Peninsula.



## Disclaimer
<b> Personal Project:</b> This project is a personal hobby and technical experiment. It is not affiliated with, sponsored by, or endorsed by any of the organizations mentioned (e.g., Shibayama Haniwa Museum).

<b> Media Credits:</b> All photos of the Haniwa were taken by the author of the actual product purchased at the museum.

<b> Accuracy:</b> While I strive for technical and historical accuracy, I assume no responsibility for any issues or damages arising from the use of the information in this article.