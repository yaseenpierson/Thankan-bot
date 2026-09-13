# 🚰 Thankan-Bot: The Anti-Hydration Cup Robot 🏃💨

> *"You will never drink your water in peace again."*

[![Version](https://img.shields.io/badge/version-v1.0_Alpha-blue.svg)](https://github.com/yaseenpierson/Thankan-bot)
[![Hardware](https://img.shields.io/badge/microcontroller-ESP32-E7352C.svg)](https://www.espressif.com/)
[![Display](https://img.shields.io/badge/display-0.91%22_OLED_SSD1306-00979D.svg)](#)
[![Meme Level](https://img.shields.io/badge/meme%20level-100%25-orange.svg)](#)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

**Thankan-Bot** is an autonomous desktop cup-carrier robot designed with one chaotic mission: **preventing you from drinking your water**. The moment your hand approaches your cup, Thankan-Bot's 5-way IR sensory array triggers, changing its animated OLED eyes to panic or rage, blasting situational meme audio via its onboard DFPlayer Mini, and speeding away across your desk.

---

## 🎯 The Premise

Most robots aim to assist humans. **Thankan-Bot exists to deny your hydration.**

- 💧 You get thirsty.
- 🖐️ You reach for your cup.
- ⚡ 5x IR proximity sensors spot your hand approaching from any angle.
- 🤖 Thankan-Bot's animated OLED eyes shift from peaceful/happy to horrified or furious.
- 🔊 It cuts the calm background music, queues up screaming meme sounds, and sprints away in the opposite direction.

---

## ✨ Features

- 🏎️ **5-Way Tactical Evasion**: 5 strategically placed IR proximity sensors detect hands from the front, front-left, front-right, left, and right, calculating the safest escape path.
- 🤖 **Expressive OLED Eyes (Vector / Cozmo Style)**: Integrated 0.91" 128x64 SSD1306 OLED screen with animated robotic eyes featuring smooth scaling, idle breathing, blinking, and dynamic expressions:
  - **Happy / Idle**: Gentle eye breathing & alternating pulses with a smile.
  - **Surprised / Scared**: Eyes widen dramatically when danger is detected right in front.
  - **Angry**: Furrowed eyebrows when escaping flanked threats.
  - **Confused**: Unequal eye sizes with a question mark when startled from the sides.
  - **Sleepy, Focused, and Wink** modes.
- 🔊 **Dynamic DFPlayer Meme Audio Subsystem**:
  - **Rest Mode**: Loops chill resting background music (`003.mp3`, `007.mp3`).
  - **Evasion Mode**: Interrupts instantly on new stimuli to fire rotating evasion meme clips (`001.mp3`, `004.mp3`, `006.mp3`, `008.mp3`).
- ⚡ **Non-Blocking Evasion Loop**: State-machine architecture using `millis()` timing and rising-edge sensor detection so animations, motor movements, and audio play seamlessly without stutter.
- 💻 **Serial Debug & Test Console**: Switch expressions directly via Serial Monitor commands (`EXPR_HAPPY`, `EXPR_ANGRY`, `EXPR_SURPRISED`, etc.).

---

## 🧠 Evasion Matrix & Behavior

Thankan-Bot senses threat vectors and chooses its reaction in real time:

| Sensor Triggered | Threat Direction | OLED Expression | Motor Maneuver | Audio State |
|---|---|---|---|---|
| **None** | Clear / Rest | **Happy / Idle** | Stopped | Plays rest track (`003.mp3` / `007.mp3`) |
| **IR1** | Front | **Surprised** 😳 | Reverse (350 ms) | Next evasion meme |
| **IR2** | Front-Left | **Angry** 😠 | Reverse Right (400 ms) | Next evasion meme |
| **IR3** | Front-Right | **Angry** 😠 | Reverse Left (400 ms) | Next evasion meme |
| **IR4** | Left | **Confused** 🤨 | Tank Turn Right (450 ms) | Next evasion meme |
| **IR5** | Right | **Confused** 🤨 | Tank Turn Left (450 ms) | Next evasion meme |
| **Multiple** | Flanked / Cornered | **Angry / Surprised** | Evades toward side with least danger | Next evasion meme |

---

## 🛠️ Hardware & Pin Configuration (ESP32)

### 1. Motors (Dual H-Bridge Driver)
| Pin Name | ESP32 GPIO | Description |
|---|---|---|
| `IN1` | **GPIO 25** | Left Motor Forward |
| `IN2` | **GPIO 26** | Left Motor Reverse |
| `IN3` | **GPIO 27** | Right Motor Forward |
| `IN4` | **GPIO 14** | Right Motor Reverse |

### 2. IR Proximity Sensors (Active LOW)
| Sensor | ESP32 GPIO | Placement |
|---|---|---|
| `IR1` | **GPIO 32** | Front |
| `IR2` | **GPIO 33** | Front-Left |
| `IR3` | **GPIO 34** | Front-Right |
| `IR4` | **GPIO 35** | Left |
| `IR5` | **GPIO 13** | Right |

### 3. DFPlayer Mini Audio Module
| DFPlayer Pin | ESP32 GPIO | Description |
|---|---|---|
| `TX` | **GPIO 16 (RX2)** | Serial Data to ESP32 |
| `RX` | **GPIO 17 (TX2)** | Serial Data from ESP32 (use 1kΩ resistor in series) |
| `VCC` / `GND` | 5V / GND | Power |
| `SPK_1` / `SPK_2` | Speaker | 3W 8Ω Speaker |

### 4. OLED Display (0.91" 128x64 SSD1306 I2C)
| OLED Pin | ESP32 GPIO | Description |
|---|---|---|
| `SDA` | **GPIO 21** | I2C Data |
| `SCL` | **GPIO 22** | I2C Clock |
| `VCC` | 3.3V / 5V | Power |
| `GND` | GND | Ground |

---

## 📁 MicroSD Card Audio Track Setup

Format your MicroSD card to **FAT32** and place numbered MP3 files in the root or `/mp3/` directory:

```
MicroSD Card/
├── 001.mp3   <-- Evasion 1 (e.g., "Why are you running?")
├── 002.mp3   <-- Optional sound bite
├── 003.mp3   <-- Rest track A (chill elevator / waiting theme)
├── 004.mp3   <-- Evasion 2 (e.g., Vine boom / screaming)
├── 005.mp3   <-- Optional sound bite
├── 006.mp3   <-- Evasion 3 (e.g., Metal pipe falling / anime gasp)
├── 007.mp3   <-- Rest track B (peaceful tune)
└── 008.mp3   <-- Evasion 4 (e.g., Eurobeat drift theme)
```

---

## 💻 Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/yaseenpierson/Thankan-bot.git
cd Thankan-bot
```

### 2. Install Required Libraries
In the **Arduino IDE** (Library Manager) or **PlatformIO**, install:
- **Adafruit SSD1306**
- **Adafruit GFX Library**
- **DFRobotDFPlayerMini**

### 3. Upload to ESP32
1. Open [`Bot.ino`](Bot.ino) in Arduino IDE.
2. Under **Tools > Board**, select your ESP32 Dev Module (e.g., `DOIT ESP32 DEVKIT V1` or `ESP32 Dev Module`).
3. Select your COM port.
4. Click **Upload**.

---

## 🕹️ Serial Debugging & Expression Testing

Open the Arduino Serial Monitor at **115200 baud** to test eye expressions live:

- `EXPR_HAPPY` - Happy face with smile
- `EXPR_SURPRISED` - Wide shocked eyes
- `EXPR_ANGRY` - Angry eyes with eyebrows
- `EXPR_CONFUSED` - Asymmetric eyes with question mark
- `EXPR_SLEEPY` - Narrow relaxed eyes
- `EXPR_FOCUSED` - Narrow squinting eyes
- `EXPR_WINK` - Winking expression
- `EXPR_IDLE` - Default breathing & blinking
- `STATUS` - Print current active expression mode

---

## 🚀 Future Roadmap

- [ ] 🛑 **Cliff Detection**: Downward-pointing IR sensors to stop Thankan from driving off the edge of the desk.
- [ ] 🔄 **Erratic Spin Maneuvers**: Random 360° spin taunts when user hesitation is detected.
- [ ] 📱 **BLE / Web App Controller**: Trigger remote taunts and override driving from a phone browser.

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for details.
