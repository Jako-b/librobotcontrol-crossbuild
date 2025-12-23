### A. Windows-Konfiguration (Host)

1.  **Netzwerkadapter konfigurieren:**
    * Öffnen Sie die *Systemsteuerung -> Netzwerk- und Freigabecenter -> Adaptereinstellungen ändern*.
    * Suchen Sie den Adapter, der dem BeagleBone entspricht.
    * Rechtsklick -> *Eigenschaften -> Internetprotokoll Version 4 (TCP/IPv4)*.
    * Folgende statische IP vergeben:
        * **IP-Adresse:** `192.168.7.1`
        * **Subnetzmaske:** `255.255.255.0`
        * **Standardgateway:** *leer lassen*
        * **DNS:** *leer lassen*

2.  **NAT aktivieren (PowerShell als Administrator):**
    Führen Sie folgenden Befehl in der PowerShell aus:

    ```powershell
    New-NetNat -Name "BeagleBoneNAT" -InternalIPInterfaceAddressPrefix 192.168.7.0/24
    ```

### B. BeagleBone-Konfiguration

Verbinden Sie sich auf den BeagleBone und führen Sie folgende Schritte aus:

1.  **Standard-Gateway setzen:**

    ```bash
    sudo ip route add default via 192.168.7.1 dev usb0
    ```

2.  **DNS-Server eintragen:**

    ```bash
    echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf
    ```

3.  **Verbindung testen:**

    ```bash
    ping -c 3 8.8.8.8
    ```

---

## 2. Installation von libgpiod

Stellen Sie sicher, dass der BeagleBone Internetzugriff hat (siehe oben), und führen Sie dann aus:

```bash
sudo apt update
sudo apt install libgpiod2 libgpiod-dev
```