# Street-Management-System-Vehicle-Speed-Tracking-and-Smart-Street-Lighting
This project elucidates the implementation of Smart Street Management System, which includes an automated Smart Street Lighting System followed by implementation of an innovative Vehicle Speed Detection system. 

### Smart Street Lighting
It involves lights that adapt to surrounding illumination and operate by motion detection. The circuit is based on light intensity detection, under low illumination, the resistance of the LDR falls allowing current to flow through the circuit and whenever motion is detected, the LEDs light up.

### Vehicle Speed Detection
This project presents a system, developed for over-speeding detection and alerting offenders as well as authorities automatically. In this work, it has been proved that automation provides efficient performance compared to a human handled system.

### Dashboard
![dashboard](https://github.com/saifsafsf/Street-Management-System-Vehicle-Speed-Tracking-and-Smart-Street-Lighting/assets/73883918/348bbb14-daa9-4b47-87c7-b34e86eb5c4f)

### How to Run
- Upload simluation folders on Wokwi
- Replace `{{ API_KEY }}` and `{{ CHANNEL_NUM }}` in `./{{ SIMULATION_NAME }}/sketch.ino` with your credentials from ThingSpeak
- Run simulations
- Run `./scripts/generate_data.py` (if `./data/street_data.csv` does not exist)
- Run `./scripts/lights_insights.py` (if `./data/lights_insights.csv` does not exist)
- View `Street Mgmt System Dashboard using.pbix` Power BI

### Future Work
Data is generated using python scripts trying to simulate the situation. This can be replaced by:
- Using simulations to send the data to ThingSpeak
- Connecting Power BI directly to ThingSpeak using its API
- Cleaning the data & extracting the insights in Power BI
- Finally, visualizing the data in Power BI
