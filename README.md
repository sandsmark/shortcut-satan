Shortcut Satan™
===============

Like [Hotkey Daemon](https://github.com/gunboy001/hkd/), but better because it is written in C++:

![c++](/c++.jpg)

Config
------

You define shortcuts and what they do in ~/.config/shortcut-satan.conf. The
format is the same as hkd, just without the weird -'s at the beginning of each
line.


Example config
--------------

```
MUTE: pamixer -t
VOLUMEUP: pamixer -i 10
VOLUMEDOWN: pamixer -d 10
BRIGHTNESSUP: light -A 33
BRIGHTNESSDOWN: light -U 33

# Lock
WIN L: xset s activate
```
