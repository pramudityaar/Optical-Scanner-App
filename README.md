# 🔬 1D Automated Optical Scanner & Data Acquisition System

[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-blue.svg)](https://github.com/pramudityaar/Optical-Scanner-App)
[![C++](https://img.shields.io/badge/Language-C%2B%2B17-00599C.svg?logo=c%2B%2B)](https://isocpp.org/)
[![GUI](https://img.shields.io/badge/GUI-Dear%20ImGui%20%7C%20ImPlot-orange.svg)](https://github.com/ocornut/imgui)
[![Baudrate](https://img.shields.io/badge/Baudrate-921600%20bps-brightgreen.svg)]()
[![Hardware](https://img.shields.io/badge/Hardware-ESP32%20%7C%20TMC2209%20%7C%20ADS1115-red.svg)]()

A high-precision desktop instrumentation and real-time data acquisition (DAQ) application engineered for automated 1D optical beam profiling, spatial intensity measurements, and photodetector frequency response analysis.

Designed according to **Technical Reference Manual (TRM) v2.0**, this software interfaces seamlessly with dual-core ESP32 firmware to perform micrometer-level linear scans and high-frequency sensor reading.

---

## 📸 Key Highlights & Features

- **Industrial Gray UI Architecture:** Designed with strict industrial standards, high-contrast readability, and a strict 3-word labeling rule for minimalist laboratory workflows.
- **Dynamic COM Port Enumerator:** Native Windows API scanning (`QueryDosDeviceA`) to automatically detect active serial devices without manual guesswork.
- **High-Speed Serial Communication:** High-throughput streaming at **921,600 bps** using a low-overhead, deterministic ASCII command protocol.
- **Multi-Mode Visualization:**
  - **Linear Scan Mode:** Real-time spatial curve profiling ($Position \text{ [mm]} \text{ vs } Intensity \text{ [mV]}$) with dynamic progress feedback.
  - **Frequency Domain Mode:** Real-time analog time-domain tracking and digital square-wave state visualization using `PlotStairs` (oscilloscope-style).
- **Embedded Peak Detection & Frequency Extraction:** Built-in threshold-crossing algorithm to automatically compute instantaneous frequency ($f = 1/\Delta t$) during data acquisition.
- **Native Windows CSV Export:** Native `GetSaveFileName` dialog integration with automated scientific metadata logging (timestamp, gain modes, calibration factors, and structured dataset columns).

---

## 🛠️ Hardware Specification

| Subsystem | Component | Specifications |
| :--- | :--- | :--- |
| **Master Controller** | WEMOS LOLIN D32 | ESP32 Dual-Core @ 240 MHz, FreeRTOS |
| **Motion Actuator** | NEMA 17 + TMC2209 | SilentStepStick, SFU1204 Ball Screw (800 steps/mm) |
| **Analog Detection** | Photodiode + Micrometer Slit | ADS1115 (16-bit $\Delta\Sigma$ ADC via I2C) |
| **Digital Detection** | Fast Photogate Logic | Direct GPIO interruption / logic state recording |

---

## 📡 Serial Communication Protocol (TRM v2.0)

All commands are transmitted without bloatware text at **921,600 bps, 8N1**:

| Command | Action / Meaning | Lolin D32 Reply |
| :---: | :--- | :--- |
| `H` | Execute homing sequence | `H,0` $\rightarrow$ `H,1` |
| `G` | Start 1D linear scan | `SL` $\rightarrow$ `[Stream Data]` $\rightarrow$ `EL` |
| `A` | Start analog time-domain read | `SA` $\rightarrow$ `[Stream Data]` $\rightarrow$ `EA` |
| `W` | Start digital logic state read | `SD` $\rightarrow$ `[Stream Data]` $\rightarrow$ `ED` |
| `M0` / `M1` | Set ADC Gain (0: Photovoltaic, 1: Photodiode) | `M,0` / `M,1` |
| `P<val>` | Move to absolute position (e.g., `P25.5`) | `P,<val>` |
| `C,<steps>,<speed>` | Configure motor calibration (e.g., `C,800,15`) | `C,OK` |
| `?` | Query current stage position | `P,<position>` |
| `X` | Emergency Stop (E-Stop) | `X` |

---

## 🚀 Quick Start (Pre-built Binaries)

For laboratory operators and evaluators who do not have a C++ build environment installed:

1. Navigate to the **[Releases](https://github.com/pramudityaar/Optical-Scanner-App/releases)** page.
2. Download the latest `ScannerApp-v2.0-win64.zip`.
3. Extract the ZIP archive anywhere on your Windows PC.
4. Connect the **Lolin D32** scanner via USB.
5. Launch `ScannerApp.exe`.
6. Click **Refresh**, choose the detected COM port, and press **Connect**.

---

## 💻 Building from Source

### Prerequisites

- **Toolchain:** MSYS2 with `ucrt64` environment (`gcc`, `g++`, `make`, `cmake`).
- **Graphics & Windowing:** OpenGL3, GLFW3.

### Build Steps

1. Open your **MSYS2 UCRT64** terminal and clone the repository:
   ```bash
   git clone [https://github.com/pramudityaar/Optical-Scanner-App.git](https://github.com/pramudityaar/Optical-Scanner-App.git)
   cd Optical-Scanner-App
