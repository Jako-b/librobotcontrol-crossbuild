### A. Windows Configuration (Host)

1.  **Configure Network Adapter:**
    * Open *Control Panel -> Network and Sharing Center -> Change adapter settings*.
    * Locate the adapter corresponding to the BeagleBone.
    * Right-click -> *Properties -> Internet Protocol Version 4 (TCP/IPv4)*.
    * Set the following static IP:
        * **IP address:** `192.168.7.1`
        * **Subnet mask:** `255.255.255.0`
        * **Default gateway:** *leave blank*
        * **DNS:** *leave blank*

2.  **Enable NAT (PowerShell as Administrator):**
    Run the following command in PowerShell:

    ```powershell
    New-NetNat -Name "BeagleBoneNAT" -InternalIPInterfaceAddressPrefix 192.168.7.0/24
    ```

### B. BeagleBone Configuration

Connect to the BeagleBone and perform the following steps:

1.  **Set Default Gateway:**

    ```bash
    sudo ip route add default via 192.168.7.1 dev usb0
    ```

2.  **Set DNS Server:**

    ```bash
    echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf
    ```

3.  **Test Connection:**

    ```bash
    ping -c 3 8.8.8.8
    ```

---

## Installing libgpiod

Ensure that the BeagleBone has internet access (see above), and then run:

```bash
sudo apt update
sudo apt install libgpiod2 libgpiod-dev
```