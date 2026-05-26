# Multi-Sensory Reaction Benchmarker (GD32VF103)

This project features a standalone embedded firmware system designed to benchmark and analyze human reaction times. By leveraging a 32-bit RISC-V architecture, the system coordinates hardware timers and peripheral drivers to capture precise time differences between automated stimuli triggers and physical user inputs.

The firmware cycles through three distinct, unpredictable stimuli types:
* **Visual:** Targeted LED illumination.
* **Acoustic:** Sound generation via a piezo buzzer.
* **Haptic:** Mechanical pulse feedback using a vibration motor.

Upon stimulus activation, the onboard hardware registers the time elapsed until the tactile button is pressed, outputting telemetry data for statistical visualization.

---

## Technical Objectives

* **Precision Timing:** Capture millisecond-level response latencies using hardware clocks.
* **Stochastic Isolation:** Deploy randomized delay intervals to prevent anticipatory user bias.
* **Data Logging:** Track sequential execution runs to build historical trend charts directly on the display module.

---

## System Workflow

1. **System Initialization:** Power-on self-test and hardware peripheral configuration.
2. **Idle Polling:** Display prompts user that the benchmarking pipeline is armed.
3. **Randomized Wait:** Non-deterministic sleep cycle to isolate pure reflexes.
4. **Stimulus Assert:** Firing of a random actuator (LED, buzzer, or vibrator).
5. **Interrupt Capture:** High-speed polling/interrupt capture of the tactile user switch.
6. **Telemetry Computation:** Calculating exact delta-time between trigger and capture.
7. **Local Output:** Displaying active metrics and updating the local data array.

---

## Hardware Blueprint

The physical testing rig integrates the following core hardware components:

* **Microcontroller:** GD32VF103 RISC-V 32-bit MCU
* **Display:** Graphical LCD Screen
* **Input:** Low-latency Tactile Push Button
* **Actuators:** Standard LED, Piezoelectric Buzzer, 5V DC Vibration Motor
* **Prototyping:** Breadboard, jumper wires, and 5V USB power delivery

---

## Firmware Architecture

The modular firmware is built in pure C, compiled against the RISC-V toolchain. It utilizes decoupled driver modules to optimize register operations:


| Module / Source File | Core Responsibility |
|---|---|
| `main.c` | Top-level state machine, randomized delay logic, and event orchestration. |
| `lcd.c` / `lcd.h` | Direct-to-display drawing routines and telemetry printing. |
| `pwm.c` / `pwm.h` | Pulse-Width Modulation registers configured for acoustic frequency control. |
| `vibrator.c` / `vibrator.h` | Low-level GPIO driver interface managing haptic motor activation states. |
| `oledfont.h` | Hexadecimal font arrays mapping alphanumeric characters to the LCD. |
| `Makefile` | Build automation script specifying optimization levels and compiler flags. |

---

## Architectural Block Diagram

The block diagram details the architectural topology and physical signal buses routing data between the core processor and peripheral devices.

<img width="346" height="458" alt="image" src="https://github.com" />

---

## Execution Flowchart

The flowchart traces the internal software loops, state validations, and hardware interrupt checkpoints from bootup to result calculation.

<img width="557" height="328" alt="image" src="https://github.com" />

---

## Schematic Circuit Diagram

The circuit schematic map provides precise hardware pinning, current-limiting configurations, and active power rails across the board layout.

<img width="579" height="310" alt="image" src="https://github.com" />

---

## Hardware Prototype

The physical assembly maps out the component layout and connection topology on the main testing board.

<img width="312" height="420" alt="Prototyp-2" src="https://github.com" />

---

## Operational Execution Guide

1. **Power Up:** Connect the MCU via USB to initiate the hardware power lines.
2. **Calibration Check:** Observe the LCD until the system signals a ready state.
3. **Reflex Input:** Monitor the device and hit the capture button immediately when any sensory trigger goes active.
4. **Data Review:** Evaluate the latency metric printed on the UI screen.
5. **Cycle Aggregation:** Run consecutive test iterations to accumulate reliable datasets.

---

## Data Analysis and Telemetry Outputs

Each completed reflex cycle yields an immediate millisecond metrics report on the LCD screen. By storing sequential array entries locally, the firmware compiles user trend logs. This indexed data formats a real-time historical distribution chart (histogram), allowing operators to analyze fatigue progression or contrast auditory response speeds against visual triggers.
