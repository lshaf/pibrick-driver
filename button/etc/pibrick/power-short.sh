#!/bin/bash

# Emit KEY_POWER (brings up the desktop shutdown options) through the
# pibrickbtn uinput device created by the button service.
dev=""
for d in /sys/class/input/event*; do
    if [ "$(cat "$d/device/name" 2>/dev/null)" = "pibrickbtn" ]; then
        dev="/dev/input/$(basename "$d")"
        break
    fi
done

if [ -n "$dev" ]; then
    evemu-event "$dev" --type EV_KEY --code KEY_POWER --value 1 --sync
    evemu-event "$dev" --type EV_KEY --code KEY_POWER --value 0 --sync
fi
