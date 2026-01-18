# librobotcontrol (Kernel 6.12 / Bookworm Port)

> **Note:** This is a modern port of the original librobotcontrol.
> It updates the hardware abstraction layer to support **Linux Kernel 6.12+** (Debian Bookworm) by using `libgpiod` and `/dev/bone/` interfaces instead of deprecated Sysfs entries.

---

## Port Status & Features
This version fixes "Bus Errors" and "Segmentation Faults" experienced on modern Kernels (5.10 / 6.x) caused by removed kernel interfaces.

* **GPIO:** Replaced Sysfs (`/sys/class/gpio`) with **libgpiod**.
* **PWM/Encoders:** Updated to use stable `/dev/bone/` symlinks and `gpio-line-names`.
* **Validation:** Tested on **BeagleBoard.org Debian Bookworm Base Image (2025-09-05)** with Kernel `6.12.45-bone34`.

See [VALIDATION.md](VALIDATION.md) for a detailed list of working features.

### Hardware Configuration
To use this library, you should install the **RoboticsCape.dtbo** device tree overlay found in `device_tree/dtb-6.12/`.
1. **Overlay:** This overlay defines the specific Pin Names required by librobotcontrol.
2. **Device Paths:** The library relies on `/dev/bone/` symlinks (e.g. for PWM).

---

### Software Dependencies
Since this port links against the system libraries, you must install the development headers on your BeagleBone.
**Note:** This port was validated with **libgpiod v1.6.3**.

```bash
sudo apt update
sudo apt install build-essential libgpiod-dev git
```

### System Requirements & Image Compatibility
**Important for Non-BeagleBoard Images:**

This library relies on BeagleBoard-specific **udev rules** to create `/dev/bone/*` symlinks (e.g., `/dev/bone/pwm/1/a`).

* **Official Images:** Works out-of-the-box on BeagleBoard.org Debian images.
* **Custom/Vanilla Images:** If you are using a standard Debian or Yocto build, you must manually install the `bb-customizations` package or configure udev rules to map hardware paths to `/dev/bone/...`. Without this, the library will not find the hardware.

