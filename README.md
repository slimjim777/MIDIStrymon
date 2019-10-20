# MIDI controller for the Strymon Timeline and Bigsky

An Arduino-based MIDI controller with MIDI thru and output to control
the Strymon Timeline and Bigsky. This four-button controller toggles
the Strymon's between various patches by sending Program Change messages
or a Control Change message (to engage or bypass the Timeline). Each button has
a notional on/off state, so four buttons are able to send eight different scenarios.

The MIDI thru merely passes through the input to the output, which has been used to pass on
MIDI clock to the devices.

The code was based on an Arduino Nano 33 IoT, but could easily be adapted for
other boards. With a few extra code mods it could detect double-press or long-press
scenarios, which allow even more patches to be accessed.
