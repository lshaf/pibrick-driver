from gpiozero import Button
from signal import pause
import time

# Create a Button object, connected to BCM pin 20
# pull_up=True means the pin is high (True) until connected to ground (False)
button = Button(20, pull_up=False)

def on_button_pressed():
    print(f"Button pressed at {time.strftime('%H:%M:%S')}")
    # Add your desired action here (e.g., turn on an LED, send an email)

def on_button_released():
    print(f"Button released at {time.strftime('%H:%M:%S')}")

# Assign the functions to the button's 'when_pressed' and 'when_released' events
button.when_pressed = on_button_pressed
button.when_released = on_button_released

print("Monitoring GPIO 20... Press Ctrl+C to exit.")

try:
    # 'pause()' keeps the script running indefinitely, waiting for events
    pause()
except KeyboardInterrupt:
    print("\nExiting program.")