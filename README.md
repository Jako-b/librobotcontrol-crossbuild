# librobotcontrol – Cross-Build für Windows (MSYS2 + Eclipse)

Dieses Projekt ermöglicht das Cross-Kompilieren von **librobotcontrol** für den BeagleBone direkt unter Windows – ohne Linux-VM.

Ein neuer Nutzer muss lediglich:
1. MSYS2 installieren
2. ARM-Toolchain herunterladen
3. in `startEclipse.cmd` zwei Pfade anpassen  
4. `startEclipse.cmd` starten  
→ danach kann sofort in Eclipse gebaut werden.

---

## 1. Voraussetzungen

### MSYS2
Download: https://www.msys2.org

Wird benötigt für:
- `/usr/bin/bash`
- `make`
- `cygpath`

Nach Installation MSYS2 aktualisieren:

```bash
pacman -Syu
pacman -S make
```

---

### ARM GNU Toolchain (arm-none-linux-gnueabihf)

Download:  
https://developer.arm.com/downloads

Installation:  
Einfach in beliebigen Ordner entpacken, z. B.:

```
C:/toolchains/arm-gnu-toolchain/
```

---

### Eclipse CDT

Eclipse wird **nicht** mehr im Repository bereitgestellt.

Der Nutzer installiert Eclipse CDT selbst und trägt den Pfad zu `eclipse.exe` in  
`startEclipse.cmd` ein.

---

## 2. Toolchain- und Eclipse-Pfade setzen

In der Datei:

```
startEclipse.cmd
```

folgende zwei Variablen anpassen:

```
set "TOOLCHAIN_DIR=C:/.../arm-gnu-toolchain"
set "ECLIPSE_EXE=C:/.../eclipse.exe"
```

- nur vorwärts-Slashes `/`
- kein abschließender Slash

---

## 3. Eclipse starten

Unter Windows:

```
startEclipse.cmd
```

Wichtig:  
**Eclipse darf nicht direkt gestartet werden.**  
Nur der Start über `.cmd` setzt PATH, MSYS2 und die Toolchain korrekt.

---

## 4. Build in Eclipse

Der Ordner "librobotcontrol" muss importiert werden über 

Import → C/C++ → Existing Code as Makefile Project

Dann den Ordner "librobotcontrol" auswählen und bestätigen.


In Eclipse:

- Project → Clean
- Project → Build Project

Eclipse führt automatisch `make all` aus.  
Der Compiler wird durch `startEclipse.sh` automatisch auf die ARM-Toolchain gesetzt.

---

## 5. Repository-Struktur

```
librobotcontrol-crossbuild/
├── librobotcontrol/        # eigentliche Bibliothek
├── startEclipse.cmd        # Windows-Launcher
├── startEclipse.sh         # MSYS2-Wrapper
├── README.md
└── .gitignore
```

Der Nutzer bringt selbst mit:

- ARM Toolchain
- Eclipse CDT

Diese sind bewusst **nicht** im Repository enthalten.

---

## 6. .gitignore Hinweise

Dieses Repository speichert **nicht**:

- Toolchain
- Eclipse Installation
- Eclipse Workspace
- Build-Output
- temporäre Dateien