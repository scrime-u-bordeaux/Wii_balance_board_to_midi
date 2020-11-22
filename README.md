# Wii_balance_board_to_midi
hardware hack to ouput midiUSB from Wii Balance Board device
it use teensyLC microcontroller dev board and four hx711 load cell ADC boards
original button on Wii balance is use for tare, blue led also connected.
-send midiUSB pitchWheel on channel 1 for X move (4096 is center)
-send midiUSB pitchWheel on channel 2 for Y move (4096 is center)
-send midiUSB pitchWheel on channel 3 for total weight
![balance](../balance.JPG)
