# OLED Screensaver

## 🖤 Black Screensaver for OLED Displays

**OLED Screensaver** is a lightweight Windows application that blacks out the **primary display** to prevent **burn-in** on OLED screens.

The screensaver automatically activates after a user-defined period of **inactivity**:

* Inactivity is detected when the **mouse is not moving inside the primary monitor's area** or when the mouse is completely idle.

This is particularly useful in **multi-monitor setups**, where you want the primary display (often a laptop screen) to go black while you continue working on external monitors.

---

## ✨ Features

* ⚡ Full-screen, borderless black window to protect OLED displays.
* 🖱️ Automatic activation based on user inactivity or mouse moving away from the primary monitor.
* 🛑 System tray icon with:

  * Pause/Resume functionality.
  * Adjustable inactivity timeout.
  * Quick exit option.
* 🎯 Low system resource usage.

---

## 🛠️ Prerequisites

To compile the project in **Visual Studio (Windows Desktop Application in C++)**, you need:

* ✅ Visual Studio
  * Desktop Development with C++ workload


---

## 🔧 Usage Instructions

* The screensaver will activate when the mouse is inactive or outside the primary monitor for the specified timeout.
* Right-click the system tray icon to:

  * Pause the application
  * Change the inactivity threshold
  * Exit the program
* Click anywhere on the black window to immediately deactivate the screensaver.
* Press Esc to close the program (when the black screen window is focused).

---

## 📌 Notes

* The mouse cursor is automatically hidden when the black window is active.
* No conficts with Windows screensavers/lock mechanisms.
* Tested on Windows 11


## How to Contribute:
1. Fork this repository.
2. Create a new branch: `git checkout -b feature/my-feature`
3. Make your changes and commit: `git commit -m 'Add my feature'`
4. Push to your branch: `git push origin feature/my-feature`
5. Open a pull request.

---

## 🚧 Future Improvements
- 🖥️ Support for per-monitor inactivity thresholds.
- ⏸️ Support for hotkeys.

---

## 📃 License

This project is licensed under the MIT License.

---
