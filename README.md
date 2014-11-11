cpphip8
=======

The customary emulator writing rite of passage, apparently. Chip-8 emulator in C++ with SDL

## Building
Standard cmake procedure:
```
mkdir build
cd build
cmake ..
make
```

And Bob's your uncle!

## Controls
Keypad is emulated like this:
```
Chip8     => QWERTZ
---------    ---------
|1|2|3|C|    |1|2|3|4|
|4|5|6|D|    |Q|W|E|R|
|7|8|9|E|    |A|S|D|F|
|A|0|B|F|    |Y|X|C|V|
---------    ---------
```

### Controlling the emulation
|Key | Function |
| --- | -------- |
| TAB | Reset the emulator |
| Space | Pause the emulator |
| Esc | Exit |
