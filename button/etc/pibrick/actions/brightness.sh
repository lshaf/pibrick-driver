#!/bin/bash

get_current_count() {
    cat /sys/class/backlight/pibrick-backlight/brightness
}

current_count=$(get_current_count)
new_brightness=$((current_count + 64))

if [ $new_brightness -gt 1023 ]; then
    new_brightness=0
fi

echo $new_brightness | sudo tee /sys/class/backlight/pibrick-backlight/brightness
