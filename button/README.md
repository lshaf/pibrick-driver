# piBrick PocketCM5 - Button Service

The button monitor (`pibrickbtn`, the `monitor_keydown` loop) is part of the
unified **pibrick.service**, not a separate service:

- `build.sh` compiles `pibrickbtn.c` to `/usr/local/bin/pibrickbtn`.
- `pibrick.service` builds the kernel modules (`ExecStartPre=build.sh`) and then
  runs the button monitor (`ExecStart=/usr/local/bin/pibrickbtn`).

## Installation
Installed by the top-level `install.sh` (`sudo bash ./install.sh`). The action
scripts below are copied to `/etc/pibrick`.

## Customize
Modify the scripts in `/etc/pibrick`:
- `power-short.sh` — short press of the power button (default: toggle display)
- `user-short.sh` — short press of the user button (default: change brightness)
- `user-long.sh` — long press of the user button

A long press of the power button emits `KEY_POWER`.
