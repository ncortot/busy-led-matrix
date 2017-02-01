import evdev
from evdev.util import categorize
import os

base_path = os.path.dirname(os.path.realpath(__file__))

device = evdev.InputDevice('/dev/input/event0')

keymap = {
    'KEY_KP1': 'images/open.png',
    'KEY_KP2': 'images/closed.png',
    'KEY_KP3': 'images/pacman.png',
    'KEY_KP4': 'images/street.png',
    'KEY_KP5': 'images/call.png',
    'KEY_KP7': 'images/beer.png',
    'KEY_KP8': 'images/vacation.png',
    'KEY_KP9': 'images/error.png',
}

for event in device.read_loop():
    if event.type == evdev.ecodes.EV_KEY:
        key = evdev.events.KeyEvent(event)
        if key.keycode in keymap and key.keystate == key.key_down:
            command = "( echo -ne '\\xff'; convert %s -rotate 90 -depth 1 rgba:-; echo -ne '\\xff' ) > /dev/ttyAMA0" % os.path.join(base_path, keymap[key.keycode])
            os.system(command)
