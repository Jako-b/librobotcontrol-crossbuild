# Software Validation Results

This validation was performed to verify the functionality of the ported `librobotcontrol` library on modern Linux kernels.

### Test Environment
* **Hardware:** TI AM335x BeagleBone Green
* **Operating System:** BeagleBoard.org Debian Bookworm Base Image 2025-09-05
* **Kernel:** Linux 6.12.45-bone34 (PREEMPT, armv7l)
* **Library Version:** 1.0.5
* **Porting Method:** Native compilation with custom patches for `libgpiod` integration and `/dev/bone/` path compatibility.

---

## Results Overview
| Binary Name | Status | Notes |
| :--- | :--- | :--- |
| **rc_altitude** | ✅ PASS | Barometer & DMP initialized successfully. Altitude and velocity data received. |
| **rc_balance** | ❌ FAIL | ERROR opening pinmux driver. Fails due to deprecated Sysfs pinmuxing. |
| **rc_benchmark_algebra** | ✅ PASS | Performance test completed successfully. MFLOPS results displayed. |
| **rc_bind_dsm** | ❌ FAIL | ERROR opening pinmux driver. Pinmux helper not enabled for P9_11. |
| **rc_blink** | ❌ FAIL | Failed to open LED file handle. Path mismatch in `/sys/class/leds`. |
| **rc_calibrate_accel** | ⚠️ PARTIAL | Calibration routine started and sensors sampled successfully. |
| **rc_calibrate_dsm** | ⚠️ N/A | Missing hardware or permission denied. |
| **rc_calibrate_escs** | ❌ FAIL | Bus error (Unhandled fault) when running with sudo (PRU access conflict). |
| **rc_calibrate_gyro** | ✅ PASS | Gyro calibration routine finished successfully. |
| **rc_calibrate_mag** | ❌ FAIL | Retrieved all zeros from magnetometer. I2C compass connection failed. |
| **rc_check_battery** | ✅ PASS | Battery and DC Jack voltage values read correctly via ADC. |
| **rc_cpu** | ✅ PASS | Current frequency read and CPU governor change successful. |
| **rc_dsm_passthrough** | ❌ FAIL | ERROR opening pinmux driver. Sysfs pinmuxing not supported. |
| **rc_kill** | ⚠️ UNKNOWN | Reports status correctly, but process termination was not explicitly verified. |
| **rc_model** | ✅ PASS | Hardware model correctly detected (e.g., MODEL_BB_GREEN). |
| **rc_spi_loopback** | ❌ FAIL | `/dev/spidev` device not found. SPI not enabled in Device Tree. |
| **rc_test_adc** | ✅ PASS | Raw ADC values for all channels (0-3) and voltages provided. |
| **rc_test_algebra** | ✅ PASS | Matrix math (Inversion, LUP, QR) results displayed correctly. |
| **rc_test_bmp** | ✅ PASS | Barometer temperature and pressure values read successfully. |
| **rc_test_buttons** | ⚠️ SKIP | Program starts but does not register events. |
| **rc_test_complementary_filters** | ✅ PASS | Filter coefficients and complementary sums calculated correctly. |
| **rc_test_dmp** | ✅ PASS | IMU initialized via DMP. Accelerometer and orientation data received. |
| **rc_test_dmp_tap** | ✅ PASS | Tap detection operational. Vibration events registered. |
| **rc_test_drivers** | ⚠️ PARTIAL | PASSED: GPIO, PWM, EQEP, I2C, ADC. ERROR: PRU, UARTs, LEDs. |
| **rc_test_dsm** | ❌ FAIL | ERROR opening pinmux driver. Pinmux access via Sysfs blocked. |
| **rc_test_encoders** | ❌ FAIL | Bus error (Unhandled fault) when running with sudo (PRU access conflict). |
| **rc_test_encoders_eqep** | ✅ PASS | Hardware encoder positions (E1-E3) read via `/dev/bone/counter`. |
| **rc_test_encoders_pru** | ❌ FAIL | Bus error (Unhandled fault) when running with sudo. |
| **rc_test_escs** | ❌ FAIL | Bus error (Unhandled fault) when running with sudo. |
| **rc_test_filters** | ✅ PASS | Butterworth, Lowpass and Integrator filter math verified. |
| **rc_test_kalman** | ✅ PASS | Kalman filter state estimation (pos/vel) data displayed. |
| **rc_test_leds** | ❌ FAIL | Failed to open LED file handle. |
| **rc_test_matrix** | ✅ PASS | All matrix algebra operations (Determinant, Transpose, etc.) verified. |
| **rc_test_mavlink** | ✅ PASS | UDP heartbeat packets sent and received on localhost. |
| **rc_test_motors** | ✅ PASS | Critical: Duty cycle sent successfully. PWM paths via `/dev/bone/pwm` confirmed. |
| **rc_test_mpu** | ✅ PASS | Raw Accelerometer and Gyroscope data read correctly. |
| **rc_test_polynomial** | ✅ PASS | Polynomial math (Convolution, Derivatives) verified. |
| **rc_test_pthread** | ✅ PASS | Real-time thread creation and execution verified. |
| **rc_test_servos** | ❌ FAIL | Permission denied / Failed to map PRU shared memory. |
| **rc_test_time** | ✅ PASS | System time and nanosecond sleep functions verified. |
| **rc_test_vector** | ✅ PASS | Vector mathematics and norm calculations verified. |
| **rc_uart_loopback** | ⚠️ N/A | Requires hardware loopback (jumper) to verify data transfer. |
| **rc_version** | ✅ PASS | Correctly returns library version. |