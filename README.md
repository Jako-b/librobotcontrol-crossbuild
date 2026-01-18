# librobotcontrol - Windows Cross-Compilation Bundle

This repository provides a complete environment to cross-compile **librobotcontrol** for the BeagleBone directly on Windows using Eclipse CDT and MSYS2.

It solves common issues with modern toolchains (GLIBC version mismatches) by **statically linking** core libraries.

## Key Features

* **No Linux VM required:** Builds directly on Windows.
* **Static Linking:** Includes `libgpiod.a` and links statically against GLIBC. The resulting binaries run on **any** BeagleBone Linux version (old Debian Buster/Bullseye or newer).
* **Modern Kernel Support:** Updated source code to use `/dev/bone/` paths and `libgpiod` instead of deprecated Sysfs GPIO.

---

## Port Status
This version is optimized for **Kernel 6.x** and **Debian 12**. 
It replaces deprecated Sysfs GPIO calls with modern `libgpiod` and uses `/dev/bone/` symlinks for hardware access.

Full test results can be found in [VALIDATION.md](./VALIDATION.md).

## 1. Prerequisites

### A) MSYS2
1.  Download from [msys2.org](https://www.msys2.org).
2.  Install and update:
    ```bash
    pacman -Syu
    pacman -S make
    ```
    *(Note: `make` is required inside MSYS2).*

### B) ARM GNU Toolchain
1.  Download the **Windows (mingw-w64-i686) hosted** toolchain: [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
2.  Extract it to a path without spaces, e.g., `C:/Toolchains/arm-gnu-toolchain`.

### C) Eclipse CDT
1.  Install "Eclipse IDE for C/C++ Developers".

---

## 2. Setup

1.  **Clone this repository.**
2.  **Edit `startEclipse.cmd`:**
    Open the file in a text editor and set the paths to your toolchain and Eclipse installation:
    ```batch
    set "TOOLCHAIN_DIR=C:/Toolchains/arm-gnu-toolchain"
    set "ECLIPSE_EXE=C:/Eclipse/eclipse/eclipse.exe"
    ```

---

## 3. Building

**Do not start Eclipse directly!** You must use the `startEclipse.cmd` script to set up the environment variables correctly.

1.  Double-click **`startEclipse.cmd`**.
2.  Eclipse will open.

### Import the Project
1.  **File** -> **Import** -> **C/C++** -> **Existing Code as Makefile Project**.
2.  Browse to the `librobotcontrol` folder inside this repository.
3.  Select **"Cross GCC"** (or ignore the toolchain selection).
4.  Click **Finish**.

### Configure the Build Target

1.  Right-click on the project -> **Properties**.
2.  Navigate to **C/C++ Build**.
3.  Click on the **Behavior** tab (on the right side).
4.  Change the text in the **Build (incremental build)** field:
    * From: `all`
    * To: `-f Makefile.cross all`
5.  Change the text in the **Clean** field:
    * From: `clean`
    * To: `-f Makefile.cross clean`
6.  Click **Apply and Close**.

### Build
1.  **Project** -> **Build Project**.
    * *Note: Eclipse executes `make -f Makefile.cross all`.*
2.  Check the console output. You should see the cross-compilation steps and the creation of static libraries.

---

## 4. Hardware Configuration (Device Tree)

To use the PWM channels (Motors/Servos) on modern kernels (Linux 5.10+), you must enable the correct Device Tree Overlay.

This repository includes a patched overlay in:
`librobotcontrol/device_tree/dtb-6.12.x/new_activated_pwm2/RoboticsCape.dtbo`

**Installation on BeagleBone:**
1.  Copy the `.dtbo` file to the BeagleBone.
2.  Edit `/boot/uEnv.txt` on the BeagleBone to load this overlay.

---

## 5. Technical Details

### Path Updates
The source code (`pwm.c`, `rc_find_pin.c`, `encoder_eqep.c`) has been updated to use the modern BeagleBone interface paths:
* Old: `/sys/class/gpio` (Deprecated)
* New: `/dev/bone/pwm/...` and `/dev/gpiochip...`