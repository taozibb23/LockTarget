# LockTarget - Visyal Aim assist for AimLabs
An automatic aiming assistant that detects blue targets in real time, tracks them with Kalman filter,and moves the mouse using PID control.
--------------------
## Features
- **Screen Capture**: via OBS Virtual Camera.
- **Blue color Detection**: with HSV threshold.
- **Kalman filter**: for smooth position estimation (two 1-D filters for X and Y).
- **PID control**: (P-only at the movement) for mouse movement.
- **Auto-click**: when the cursor stays within 10 pixels of the target for 5 consecutive frames.
- **Debug overlays** - trajectory trail, center cross, FPS counter.
---------
## Requirements
- **OS**: Windows 11/10 (OBS Visual Camera required)
- **Libraries**:
- OpenCV 4.x (prebuilt for VC14/VC15)
- Visual Studio 2022 (or any C++ compiler)
- OBS Studio >= (built-in virtual camera)
- **Hardware**: Normal PC ,2.5 GHz+, 8 GB RAM(for smooth screen capture)
----------
## Getting Started
### 1. Install and configure OBS
- Download OBS studio, install it, and start the visual camera from **Tools -> Visual Camera**.
- Add a **Display Capture** or **Window Capture** source (e.g.,AimLabs game window).
### 2. Build and run the project
---------------------------------------------

├── Armor_tz.cpp          # color detection, contour extraction, kalman filter, draw lock target
├── lock_tz.cpp           # lock and choose target, mouse control,auto-click
├── display_tz.cpp        # debugging display: trajectory trail, cross, FPS
├── main_tz.cpp           # main loop (schedule all modules)
├── tzbb.h                # header file (kalman1D, PID, Armor, ColorValue, Locktarget)
├── display.h             # Display class declarartion
├── test_coord.cpp        # cursor mapping test file (excluded from compilation)
└── README.md / README_EN.md

```bash
git clone https://github.com/taozibb23/LockTarget.git
cd LockTarget
