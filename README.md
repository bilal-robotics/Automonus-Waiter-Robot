# 🤖 Line Following Robot — Table 2

An Arduino-based **PID Line Following Robot** with dual-pattern navigation, OLED animated eyes, buzzer feedback, and autonomous junction-based mission control.

---

## 📸 Features

- ✅ **PID Control** for smooth and accurate line following
- ✅ **3 IR Sensors** (Left, Center, Right) for line detection
- ✅ **Dual Pattern Mode** — automatically switches navigation pattern after completing mission 1
- ✅ **Junction Counter** — detects and counts intersections to navigate a predefined path
- ✅ **OLED Display** — animated blinking eyes using SSD1306 128x64
- ✅ **Buzzer Feedback** — beeps on startup, mission complete, and final stop
- ✅ **L298N Motor Driver** — controls two DC motors independently

---

## 🛠️ Hardware Required

| Component | Quantity |
|---|---|
| Arduino Uno / Nano | 1 |
| IR Sensor Module | 3 |
| L298N Motor Driver | 1 |
| DC Gear Motors | 2 |
| SSD1306 OLED Display (128x64, I2C) | 1 |
| Buzzer (Active) | 1 |
| Battery / Power Supply | 1 |
| Chassis + Wheels | 1 set |

---

## 🔌 Pin Configuration

### IR Sensors
| Sensor | Arduino Pin |
|---|---|
| IR Left | A0 |
| IR Center | A1 |
| IR Right | A2 |

### L298N Motor Driver
| Signal | Arduino Pin |
|---|---|
| ENA (Right Motor PWM) | 9 |
| IN1 | 2 |
| IN2 | 3 |
| ENB (Left Motor PWM) | 10 |
| IN3 | 4 |
| IN4 | 5 |

### Other
| Component | Arduino Pin |
|---|---|
| Buzzer | 12 |
| OLED SDA | A4 |
| OLED SCL | A5 |

---

## 📚 Libraries Required

Install these via **Arduino Library Manager**:

- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `Wire` *(built-in)*

---

## ⚙️ PID Tuning Parameters

```cpp
float Kp = 20.0;   // Proportional
float Ki = 0.01;   // Integral
float Kd = 60.0;   // Derivative
int baseSpeed = 55; // Base motor speed (0–255)
int steerTrim = 3;  // Steering correction offset
```

Adjust `Kp`, `Ki`, `Kd` based on your track and motor speed for optimal performance.

---

## 🗺️ Navigation Logic

### Pattern Mode 1
| Junction | Action |
|---|---|
| 1, 3 | Turn Left |
| 2 | Go Straight |
| 5 | Stop → U-Turn → Switch to Pattern 2 |

### Pattern Mode 2
| Junction | Action |
|---|---|
| 1, 2, 3 | Turn Left |
| 4, 5 | Go Straight |
| 5 | Final Stop → Mission Complete |

> The robot automatically transitions from **Pattern 1 → Pattern 2** after reaching the first final destination (junction 5).

---

## 🚀 How to Upload

1. Clone or download this repository
2. Open `tabel2.ino` in Arduino IDE
3. Install required libraries
4. Select your board: **Tools → Board → Arduino Uno**
5. Select the correct port: **Tools → Port**
6. Click **Upload**

---

## 🔊 Buzzer Behavior

| Event | Beep |
|---|---|
| Startup | 2 short beeps |
| Mission 1 Complete | 1 medium beep (500ms) |
| All Missions Complete | 1 long beep (2000ms) |

---

## 📺 OLED Display

The OLED shows **animated blinking eyes** throughout operation, toggling every 500ms between open and closed state — giving the robot a lively personality! 👀

---

## 📁 File Structure

```
📦 line-following-robot/
 ┣ 📄 tabel2.ino      ← Main Arduino sketch
 ┗ 📄 README.md       ← This file
```

---

## 🧑‍💻 Author

> Project: **Table 2 Line Following Robot**  
> Platform: Arduino  
> Language: C++ (Arduino Framework)

---

## 📝 License

This project is open-source. Feel free to modify and use for educational purposes.
