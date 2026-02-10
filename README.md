# \# Smart Physio System (IoT TENS \& Patient Monitoring)

# 

# A wearable IoT medical device designed for automated physiotherapy and patient safety monitoring. This system detects muscle fatigue and posture using sensors, syncs data to the cloud in real-time, and automatically triggers a TENS (Transcutaneous Electrical Nerve Stimulation) therapy unit when needed.

# 

# \## 👨‍💻 Developer Information

# \*\*Developer:\*\* Harsh Talap  

# \*\*Project Type:\*\* IoT Medical Prototype  

# \*\*Date:\*\* February 2026  

# 

# ---

# 

# \## 📌 Project Overview

# The Smart Physio System assists patients in rehabilitation by monitoring muscle activity and body posture.

# \- \*\*Problem:\*\* Patients often over-exert muscles or maintain poor posture without realizing it.

# \- \*\*Solution:\*\* This device tracks muscle strain and tilt angles. If fatigue is detected, it automatically activates a TENS unit to relax the muscle. All data is logged to a mobile app for doctors to review.

# 

# \### Key Features

# \- \*\*Auto-TENS Therapy:\*\* Automatically triggers muscle stimulation (via TIP122 Driver) when the Flex sensor detects fatigue.

# \- \*\*Fall \& Posture Detection:\*\* Uses MPU6050 Gyroscope to detect unsafe body angles (Rolling/Falling).

# \- \*\*Real-Time Cloud Sync:\*\* ESP32 pushes live data to \*\*Google Firebase Realtime Database\*\*.

# \- \*\*Android App Dashboard:\*\* A custom app (`com.example.flexa`) displays "Safe" (Green) or "Unsafe" (Red) status instantly.

# \- \*\*Safety Engineering:\*\* Includes \*\*Brownout Protection\*\* and \*\*Battery Optimization\*\* algorithms to run safely on a 9V battery.

# 

# ---

# 

# \## 🛠️ Hardware \& Bill of Materials

# \*Based on the project cost sheet.\*

# 

# | Component | Specification | Function |

# | :--- | :--- | :--- |

# | \*\*Microcontroller\*\* | ESP32-WROOM-32 | Main Brain (WiFi + Logic) |

# | \*\*Muscle Sensor\*\* | Flex Sensor (2.2 Inch) | Detects muscle bending \& fatigue |

# | \*\*Posture Sensor\*\* | MPU6050 | 6-Axis Accelerometer \& Gyroscope |

# | \*\*TENS Transformer\*\* | 9-0-9 Step-Up (500mA) | Generates high-voltage therapeutic pulses |

# | \*\*Driver Transistor\*\* | TIP122 (Darlington) | High-current switching for the transformer |

# | \*\*Isolation\*\* | PC817 Optocouplers | Protects ESP32 from high-voltage spikes |

# | \*\*Power\*\* | Hi-Watt 9V Battery | Portable power source |

# | \*\*Regulation\*\* | LM2596 / Buck Converter | Steps down 9V to 5V for ESP32 |

# 

# ---

# 

# \## 📂 Project Structure

# \- \*\*`/App`\*\*: Android Studio Project (Java/XML) for the patient monitoring app.

# \- \*\*`/Firmware`\*\*: Arduino IDE code (`.ino`) for ESP32 logic and sensor fusion.

# \- \*\*`/Circuit`\*\*: Schematics and wiring diagrams.

# \- \*\*`/Documentation`\*\*: Cost sheets (`components.pdf`) and project reports.

# 

# ---

# 

# \## 🚀 How to Run

# 

# \### 1. Firmware Setup (ESP32)

# 1\. Open `Firmware/Sketch\_for\_esp/Sketch\_for\_esp.ino` in Arduino IDE.

# 2\. Install required libraries: `Firebase\_ESP\_Client`, `Adafruit\_MPU6050`, `Wire`.

# 3\. \*\*Important:\*\* Update your WiFi credentials and Firebase API Key in the code.

# 4\. Upload to the \*\*DOIT ESP32 DEVKIT V1\*\* board.

# 

# \### 2. Android App Setup

# 1\. Open the `/App` folder in \*\*Android Studio\*\*.

# 2\. Sync the project with Gradle files.

# 3\. Ensure your `google-services.json` is placed in the `app/` folder.

# 4\. Build and Run on a physical Android device or Emulator.

# 

# ---

# 

# \## ⚠️ Safety Warnings

# \- \*\*High Voltage:\*\* The TENS unit generates high-voltage pulses. Do not touch the electrode output wires directly while the device is active.

# \- \*\*Battery Safety:\*\* If using a standard 9V battery, the firmware automatically limits WiFi power to prevent voltage drops (Brownout).

