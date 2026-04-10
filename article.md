# The HANIWA Guardian: Timeless yet Cutting-edge Terracotta Irrigation and Smart Scarecrow Hybrid System

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

 In Japan’s limited space, there is a distinct beauty in multipurpose design. Let’s breathe new life into an ancient form.

![Packaged haniwa](images/20260316_packed_haniwa.png "The Haniwa in its packaging. Unglazed clay is very fragile, but there were no noticeable cracks.")
![Standing haniwa](images/20260316_standing_haniwa.png "Haniwa can stand in my room.")
![Hole of haniwa](images/20260316_hole78p45_haniwa.png "A hole at the bottom of the Haniwa. Its diameter is 78.45mm.")

## The Great Unification – Giving the Haniwa a Solid Foundation
I faced a very grounding reality: my ancient replica had a gaping hole in its bottom (78.45mm, to be precise). If this was going to be a "Terracotta Irrigation" system, it needed to hold water.

Unglazed clay is wonderfully porous, which is perfect for slow-release irrigation (think of it as a low-tech, ancient Olla pot). However, the replica's structural integrity was questionable for long-term outdoor use, and that bottom hole was a deal-breaker.

I needed a solution that was:
 - Watertight: To hold the reservoir.
 - Structural-integrity: To provide a stable wedge to the soil.
 - Hackster-style: Budget-friendly and readily available.

The 100-Yen Shop Miracle
Enter the Japanese 100-yen shop (our version of the dollar store). After scouring the gardening aisle with my calipers, I found the perfect candidate: a standard, unglazed terracotta pot (known as suyaki in Japan).

![Unglazed pots](images/20260327_unglazed_pots_214352.png "Unglazed pots priced as 100 yen per 2 pots.")

The dimensions were tantalizingly close. But in hardware hacking, "close" only counts in horseshoe and hand grenades. I needed a perfect, airtight fit.

**Precision Bonding**
To unite the 1500-year-old design with the $1 modern pot, I used a silicone sealant. After some try-and-error, the haniwa could have stopped wetting himself. 

I applied the resin to the rim of the 50-yen pot and carefully inserted it into the Haniwa's base. It was a friction fit that transitioned into a permanent, watertight seal. The result was a seamless, unified structure. The Haniwa now had a solid, watertight "bucket" built into its lower half. It looked just like a pre-historic "flame-style pottery" vessel. (https://www.japan.travel/japan-heritage/popular/flame-style-pottery-snow-niigata-shinano-jomon-jade)

![Attached pot as a wedge leg of Haniwa](images/20260328_attached_pot_as_a_wedge.jpg "Zero-leakage integrity achieved.")

## Integrating Weather Intelligence — The Brain of Haniwa
"Have you ever watered your garden, only to have it pour rain an hour later?" We've all been there. It’s a frustrating waste of resources and effort. To prevent this, I thought it not enough even "Terracotta Irrigation" is adopted. My Haniwa needed more than just a soil moisture sensor; it needed the power of foresight.I decided to integrate weather forecast data to create a "Smart Irrigation Advisor."

![SystemArchitecture](images/System_Architecture_260407.jpg "The schematic diagram of a system")

** Invoking "Dr. Wadachi" **
The brain of the Haniwa, this weather service was named "Dr. Wadachi" in honor of Kiyoo Wadachi, a pioneer in Japanese meteorology and the first Director-General of the Japan Meteorological Agency. (https://en.wikipedia.org/wiki/Kiyoo_Wadachi)

Since I remembered, I read a book in my university days. Dr. Wadachi once said he pursued this field because "great discoveries could be made with nothing but observation data and human intellect." I feel a deep sympathy for this. In my university days, I used cutting-edge equipment costing a few 100 million yen (more than 1 million dollars). Today, I am pursuing the same essence of discovery using a used $30 PC and a $5 Raspberry Pi Pico W.

Setup Process:
API Passport: I used the OpenWeatherMap API (5-Day / 3-Hour Forecast, https://openweathermap.org/). One smart request a day provides enough data to anticipate Hayama's weather while remaining resource-efficient.

**Obtaining the API Passport**
 - Register: Create a free account ( 1000 requests/day available ) at OpenWeather.org.
 - Activation: After verifying your email, navigate to the "My API Keys" tab.
 - Generate: A default key is usually provided, but you can create a dedicated one named Haniwa_Project.

The Logic: A Symbiosis of Mind and Action
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

Thank you for reading this far. As you may have noticed, I also love Antoine de Saint-Exupéry's "Le Petit Prince." In particular, I love the part where the astronomer suddenly gains attention from other scholars the moment he changes his clothes, the fact that the lamplighter is the only one the prince feels he could befriend (and as a reader, I wish he would take a break), and the important lesson the fox teaches us: the need for a regular, specific routine. I tried to incorporate all of these elements into this system, with the utmost respect for the great author.

However, before the Haniwa could worry about water, it faced a more immediate challenge: The Hayama Wind. I discovered that when wind speeds exceed 9m/s, my "BirdWatcher" AI goes into a frenzy, mistaking swaying branches for birds. To save my system from this "storm of false positives," I suddenly needed to struggle developing a coordination system between my weather service and the AI monitor.

## First Report Success!
The system is alive. The inaugural report indicated a wind speed of 9.32 m/s in Hayama—right at the threshold! Since I discovered that when wind speeds exceed 9m/s, my "BirdWatcher" AI goes into a frenzy, mistaking swaying branches for birds, I ask Dr. Wadachi  to signaled the BirdWatcher to take a nap. 
Therefore, the project has officially moved from "code on a screen" to a "living, breathing monitor" that respects the natural environment of the Miura Peninsula.


As a mechanical engineer, the concept of sneak paths was a revelation. Adding resistors wasn't just about current limiting; it was about defining the flow of digital intent, much like check valves in a hydraulic system.



///the next step will be written here///
!["schematic around Raspberry Pi Pico 2 W"]
///the next step will be written here///



## The Haniwa: A Philosophical Conclusion
I wish to complete this project with a single, guiding philosophy:

Technology is a treasure to those who seek its meaning, yet a shadow of fear to those who do not.

The concept is simple: By cross-referencing local soil moisture with sky weather forecasts, the Cloud-AI determines the necessity of watering. The moment a "person" is detected by its Edge-AI eye, the Haniwa statue’s gaze ignites.

Within this single movement, I have infused two distinct functions (souls):
 - For my family and me who understand the language of its blinking colors, it serves as a beacon of insight, guiding our hands to nurture the garden.
 - For an intruder who knows nothing of its meaning, it stands just as a strange guardian, a spectral warning that drives them away.

I believe this duality is my ultimate tribute to the ancient people who built the Kofun tumuli. Just as their monuments symbolized both a solemn mourning for the departed and a prayer for the safe seafaring of the living, I, too, strive to create systems that carry such depth and purpose.


## Disclaimer
**Personal Project:** This project is a personal hobby and technical experiment. It is not affiliated with, sponsored by, or endorsed by any of the organizations mentioned (e.g., Shibayama Haniwa Museum).

**Media Credits:** All photos of the Haniwa were taken by the author of the actual product purchased at the museum.

**Accuracy:** While I strive for technical and historical accuracy, I assume no responsibility for any issues or damages arising from the use of the information in this article.

**A Special Tribute:**
This project is deeply inspired by the spirit of Antoine de Saint-Exupéry. His stories have been the most cherished companions of my life. As you know, he was a cutting-edge pilot and a traditional writer. He taught us that "What is essential is invisible to the eye." In this garden in Hayama, through a clay statue and a few lines of code, I have tried to capture a small piece of that invisible essence. To the man who looked at the stars and had to draw sheep, and to the ancient people who looked at the ocean and saw hope — this is my humble gratitude.