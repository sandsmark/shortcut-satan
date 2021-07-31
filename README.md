Shortcut Satan™
===============

![logo](/logo.jpg)


A global *hotkey daemon*.

Config
------

You define shortcuts and what they do in ~/.config/shortcut-satan.conf. The
format is the same as hkd, just without the weird -'s at the beginning of each
line.

If you run it with `-p` it will print the keys you press. Useful for creating
your config.


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
