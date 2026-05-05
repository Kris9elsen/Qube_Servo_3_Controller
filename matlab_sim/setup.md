# QUBE Servo 3 — MATLAB & Simulink Setup Guide (QLabs Virtual)

## 1. What You Need Installed

### Install order matters — do it in this sequence:

1. **MATLAB R2022a or newer** with the following toolboxes:
   - Simulink
   - Simulink Coder
   - MATLAB Coder
   - Control System Toolbox (recommended)

2. **QUARC 2024 SP1 or newer** (from Quanser)
   - QLabs (Quanser Interactive Labs) is bundled into QUARC if your license supports it
   - Install MATLAB **before** QUARC — otherwise QUARC won't detect it

3. **Quanser Interactive Labs (QLabs)** application
   - Required to run the virtual QUBE Servo 3 simulation

> If QUARC was installed before MATLAB, uninstall QUARC and reinstall it after MATLAB is set up.

---

## 2. Post-Install MATLAB Setup

After installing, run this in the MATLAB Command Window:

```matlab
cd('C:\Program Files\Quanser\QUARC\quarc')
quarc_setup
```

Then **fully close and reopen MATLAB**. A **QUARC tab** should appear in the Simulink ribbon.

### Verify your setup:
```matlab
ver                  % should list QUARC and Simulink Coder
qc_get_license()     % should return your license info
mex -setup C         % should confirm MinGW or MSVC compiler
```

---

## 3. License Note

If your license file is named `qlabs_quarc_home_license.qlic`, you have a **QLabs Home license**. This means:

- `qc_get_target_types()` returns empty — no code generation targets
- The QUARC tab may not appear in Simulink
- You **cannot** use QUARC External mode (Monitor & Tune with code generation)

**This is fine.** QLabs virtual experiments work in **normal Simulink simulation mode**. You just need to enable `Active during normal simulation` on your HIL blocks (see Section 5).

---

## 4. Connecting to the Virtual QUBE Servo 3

### TCP Port Numbers (from QLabs Hybrid Configuration Guide)

| Virtual Product | HIL Board Type | HIL Port |
|---|---|---|
| QUBE Servo 3 with Disc | `qube_servo3_usb` | `18922` |
| QUBE Servo 3 with Pendulum | `qube_servo3_usb` | `18923` |

### HIL Initialize Block Settings

In your Simulink model, open the **HIL Initialize** block and set:

| Field | Value |
|---|---|
| Board type | `qube_servo3_usb` |
| Board identifier | `0@tcpip://localhost:18922` (disc) or `0@tcpip://localhost:18923` (pendulum) |

### Run Order

1. Open **QLabs** and launch the correct workspace (Disc or Pendulum)
2. Run your **Simulink model** (normal simulation play button)
3. The model connects to QLabs over TCP automatically

---

## 5. HIL Block Configuration

All HIL blocks need **"Active during normal simulation"** checked to work without QUARC External mode.

### HIL Initialize
- **Analog Outputs tab** → set channel `0`
- **Digital Outputs tab** → set channel `0`
- **Encoder Inputs tab** → set channels `0 1`

### HIL Write
- **Analog channel 0** → motor voltage (±15V max, use ±10V in practice)
- **Digital channel 0** → amplifier enable (must be `1` to move motor)

### HIL Read
- **Encoder channel 0** (`e0`) → motor/arm angle
- **Encoder channel 1** (`e1`) → pendulum angle

---
