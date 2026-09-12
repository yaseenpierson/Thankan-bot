# 🚰 Useless-Bot: The Anti-Hydration Cup Robot 🏃💨

> *"You will never drink water in peace again."*

[![Version](https://img.shields.io/badge/version-v1.0_Alpha-blue.svg)](https://github.com/MaxonXOXO/Useless-bot)
[![Hardware](https://img.shields.io/badge/platform-Arduino-00979D.svg)](https://www.arduino.cc/)
[![Meme Level](https://img.shields.io/badge/meme%20level-100%25-orange.svg)](#)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A motorized desktop companion with a singular, chaotic mission: **preventing you from drinking your water**. Whenever you reach for your cup, Useless-Bot detects your hand, executes high-speed evasive maneuvers across your desk, and blasts meme sound effects and voice clips from its onboard SD card module.

---

## 🎯 The Purpose

Most robots aim to make human lives easier. **Useless-Bot exists to bring sheer chaos to your hydration routine.**

- You get thirsty.
- You reach out to grab your cup.
- The bot senses your attempt.
- **It books it across your desk while screaming meme audio.**

---

## ✨ Features

- 🏎️ **Autonomous Tactical Evasion**: Senses approaching hands and speeds away in random or calculated evasive trajectories.
- 🔊 **Dynamic SD-Card Meme Audio**: Plays situational sound bites (e.g., *"Why are you running?"*, panic screams, metal pipe falling, Eurobeat drift themes) while in active escape mode.
- 🥛 **Integrated Cup Carrier**: Built to hold your cup/beverage while actively refusing to let you touch it.
- ⚡ **Lightweight & Agile**: Responsive motor drive designed for quick desk maneuvers.

---

## 🛠️ Hardware Stack

| Component | Description |
|---|---|
| **Microcontroller** | Arduino (Nano / Uno) or ESP32 |
| **Audio Module** | DFPlayer Mini / MicroSD Card Module + Speaker |
| **Storage** | MicroSD Card formatted with sound effects & meme audio |
| **Sensors** | Ultrasonic (HC-SR04) / IR Proximity / ToF Sensor for hand detection |
| **Motor Driver** | Dual H-Bridge Motor Driver (TB6612FNG / L298N / L9110S) |
| **Motors** | Micro N20 DC Gear Motors with high-grip rubber wheels |
| **Power** | 7.4V 2S LiPo / 18650 Battery pack + Voltage Regulator |

---

## 🚀 Future Roadmap

- [ ] 🤖 **Expressive OLED Eyes**: Vector & Cozmo-inspired animated robotic eyes via I2C OLED display (SSD1306) showing panic, anger, and smugness.
- [ ] 🛑 **Desk Edge Detection**: Downward-facing IR cliff sensors so it doesn't commit desk suicide while fleeing.
- [ ] 🔄 **Advanced Evasion Maneuvers**: Fake-out reverses, drift turns, and erratic zig-zag escapes.
- [ ] 📱 **Remote Taunt Mode**: BLE / WiFi companion control for manual triggers.

---

## 📁 SD Card Audio Setup

Place formatted `.mp3` or `.wav` audio files on the root or designated folder of your FAT32 formatted MicroSD card:

```
SD_CARD/
├── mp3/
│   ├── 0001_why_are_you_running.mp3
│   ├── 0002_run_vine_boom.mp3
│   ├── 0003_gas_gas_gas.mp3
│   ├── 0004_cant_touch_this.mp3
│   └── 0005_screaming_cowboy.mp3
```

---

## 💻 Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/MaxonXOXO/Useless-bot.git
cd Useless-bot
```

### 2. Open in Arduino IDE
1. Open [`Bot.ino`](Bot.ino) in the **Arduino IDE**.
2. Install necessary libraries via the Library Manager:
   - `DFRobotDFPlayerMini` (or your SD/Audio library)
   - `SoftwareSerial` (if using Arduino Nano/Uno for DFPlayer)
   - `Wire` & `Adafruit_SSD1306` (for future OLED eye integration)
3. Select your board and COM port, then click **Upload**.

---

## 🤝 Contributing

Got funny audio clip ideas or smoother evasion algorithms? Pull requests and issues are welcome!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/MemeEvasion`)
3. Commit your Changes (`git commit -m "Add new drift evasion logic"`)
4. Push to the Branch (`git push origin feature/MemeEvasion`)
5. Open a Pull Request

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.
