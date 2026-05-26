#!/bin/bash

PINS=(27 18 22 24 10 25 9 8 11 7 5 16 6)

cleanup() {
    echo
    echo "Cleaning up..."

    for pin in "${PINS[@]}"; do
        pinctrl set "$pin" op dl
    done

    exit 0
}

trap cleanup INT TERM

# Configure all as outputs LOW
for pin in "${PINS[@]}"; do
    pinctrl set "$pin" op dl
done

echo "Cycling GPIOs..."

while true; do
    for pin in "${PINS[@]}"; do
        echo "GPIO $pin ON"

        # output high
        pinctrl set "$pin" op dh

        sleep 0.2

        echo "GPIO $pin OFF"

        # output low
        pinctrl set "$pin" op dl

        sleep 0.1
    done
done

